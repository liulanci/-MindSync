#include "cloud_dictionary_service.h"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <chrono>

namespace ime {
namespace plugins {
namespace cloud_dict {

class CloudDictionaryService::Impl {
public:
    CloudDictSyncConfig config;
    std::unordered_map<std::string, std::vector<CloudDictEntry>> localCache;
    uint64_t localVersion = 0;
    uint64_t remoteVersion = 0;
    bool syncing = false;
    bool initialized = false;
    std::chrono::system_clock::time_point lastSyncTime;
    std::function<void(const CloudDictSyncResult&)> syncCallback;
    std::function<void(const CloudDictQueryResult&)> queryCallback;

    size_t cacheHits = 0;
    size_t cacheMisses = 0;

    std::mutex mutex;
    std::thread syncThread;
    std::condition_variable syncCv;
    bool stopSyncThread = false;

    struct SyncTask {
        enum Type { FullSync, IncrementalSync, Upload, Download };
        Type type;
        std::vector<CloudDictEntry> entries;
        uint64_t sinceVersion = 0;
    };
    std::queue<SyncTask> syncQueue;

    void startSyncThread() {
        syncThread = std::thread([this]() {
            while (!stopSyncThread) {
                std::unique_lock<std::mutex> lock(mutex);
                syncCv.wait_for(lock, std::chrono::seconds(config.syncIntervalSeconds),
                    [this]() { return !syncQueue.empty() || stopSyncThread; });

                if (stopSyncThread) break;

                if (!syncQueue.empty()) {
                    auto task = syncQueue.front();
                    syncQueue.pop();
                    lock.unlock();
                    processSyncTask(task);
                } else if (config.enableAutoSync) {
                    lock.unlock();
                    incrementalSync();
                }
            }
        });
    }

    void processSyncTask(const SyncTask& task) {
        switch (task.type) {
            case SyncTask::FullSync:
                fullSync();
                break;
            case SyncTask::IncrementalSync:
                incrementalSync();
                break;
            case SyncTask::Upload:
                uploadChanges(task.entries);
                break;
            case SyncTask::Download:
                downloadUpdates(task.sinceVersion);
                break;
        }
    }

    CloudDictSyncResult fullSync() {
        std::lock_guard<std::mutex> lock(mutex);
        syncing = true;

        CloudDictSyncResult result;
        auto startTime = std::chrono::system_clock::now();

        // 模拟完整同步流程
        result.itemsDownloaded = 0;
        result.itemsUploaded = 0;

        // 1. 上传本地变更
        // 2. 下载云端完整词库
        // 3. 合并本地和云端数据
        // 4. 更新本地版本号

        localVersion = remoteVersion;
        result.success = true;
        result.timestamp = std::chrono::system_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime);
        lastSyncTime = result.timestamp;

        syncing = false;

        if (syncCallback) {
            syncCallback(result);
        }

        return result;
    }

    CloudDictSyncResult incrementalSync() {
        std::lock_guard<std::mutex> lock(mutex);
        syncing = true;

        CloudDictSyncResult result;
        auto startTime = std::chrono::system_clock::now();

        result.itemsDownloaded = 0;
        result.itemsUploaded = 0;

        localVersion = remoteVersion;
        result.success = true;
        result.timestamp = std::chrono::system_clock::now();
        result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime);
        lastSyncTime = result.timestamp;

        syncing = false;

        if (syncCallback) {
            syncCallback(result);
        }

        return result;
    }

    CloudDictSyncResult uploadChanges(const std::vector<CloudDictEntry>& entries) {
        std::lock_guard<std::mutex> lock(mutex);

        CloudDictSyncResult result;
        result.itemsUploaded = static_cast<uint32_t>(entries.size());
        result.success = true;
        result.timestamp = std::chrono::system_clock::now();

        if (syncCallback) {
            syncCallback(result);
        }

        return result;
    }

    CloudDictSyncResult downloadUpdates(uint64_t sinceVersion) {
        std::lock_guard<std::mutex> lock(mutex);

        CloudDictSyncResult result;
        result.itemsDownloaded = 0;
        result.success = true;
        result.timestamp = std::chrono::system_clock::now();

        if (syncCallback) {
            syncCallback(result);
        }

        return result;
    }

    CloudDictQueryResult query(const std::string& code, size_t maxResults) {
        std::lock_guard<std::mutex> lock(mutex);

        CloudDictQueryResult result;
        auto startTime = std::chrono::system_clock::now();

        auto it = localCache.find(code);
        if (it != localCache.end()) {
            result.entries = it->second;
            result.fromCache = true;
            cacheHits++;
        } else {
            cacheMisses++;
        }

        result.totalCount = static_cast<uint32_t>(result.entries.size());
        if (result.entries.size() > maxResults) {
            result.entries.resize(maxResults);
        }

        result.queryTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - startTime);

        if (queryCallback) {
            queryCallback(result);
        }

        return result;
    }

    bool exportToRimeFormat(const std::string& outputPath) {
        std::ofstream out(outputPath, std::ios::binary);
        if (!out.is_open()) {
            return false;
        }

        out << "# Rime dictionary\n";
        out << "# encoding: utf-8\n\n";
        out << "---\n";
        out << "name: cloud_dict\n";
        out << "version: \"" << localVersion << "\"\n";
        out << "sort: by_weight\n";
        out << "use_preset_vocabulary: true\n";
        out << "...\n\n";

        for (const auto& [code, entries] : localCache) {
            for (const auto& entry : entries) {
                out << entry.word << "\t" << entry.code;
                if (!entry.pinyin.empty()) {
                    out << "\t" << entry.pinyin;
                }
                out << "\t" << entry.weight;
                out << "\n";
            }
        }

        return true;
    }

    bool importFromRimeFormat(const std::string& inputPath) {
        std::ifstream in(inputPath, std::ios::binary);
        if (!in.is_open()) {
            return false;
        }

        std::string line;
        bool inData = false;

        while (std::getline(in, line)) {
            if (line == "...") {
                inData = true;
                continue;
            }
            if (!inData || line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string word, code, pinyin;
            double weight = 1.0;

            if (std::getline(iss, word, '\t') &&
                std::getline(iss, code, '\t')) {
                if (std::getline(iss, pinyin, '\t')) {
                    iss >> weight;
                }

                CloudDictEntry entry;
                entry.word = word;
                entry.code = code;
                entry.pinyin = pinyin;
                entry.weight = weight;
                entry.version = localVersion;

                localCache[code].push_back(entry);
            }
        }

        return true;
    }
};

CloudDictionaryService::CloudDictionaryService()
    : impl_(std::make_unique<Impl>()) {}

CloudDictionaryService::~CloudDictionaryService() {
    shutdown();
}

bool CloudDictionaryService::initialize(const CloudDictSyncConfig& config) {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->config = config;
    impl_->initialized = true;
    impl_->startSyncThread();
    return true;
}

void CloudDictionaryService::shutdown() {
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->stopSyncThread = true;
    }
    impl_->syncCv.notify_all();

    if (impl_->syncThread.joinable()) {
        impl_->syncThread.join();
    }

    impl_->initialized = false;
}

CloudDictSyncResult CloudDictionaryService::fullSync() {
    return impl_->fullSync();
}

CloudDictSyncResult CloudDictionaryService::incrementalSync() {
    return impl_->incrementalSync();
}

CloudDictSyncResult CloudDictionaryService::uploadChanges(
    const std::vector<CloudDictEntry>& entries) {
    return impl_->uploadChanges(entries);
}

CloudDictSyncResult CloudDictionaryService::downloadUpdates(uint64_t sinceVersion) {
    return impl_->downloadUpdates(sinceVersion);
}

CloudDictQueryResult CloudDictionaryService::query(const std::string& code, size_t maxResults) {
    return impl_->query(code, maxResults);
}

CloudDictQueryResult CloudDictionaryService::fuzzyQuery(const std::string& code, size_t maxResults) {
    return impl_->query(code, maxResults);
}

CloudDictQueryResult CloudDictionaryService::prefixQuery(const std::string& prefix, size_t maxResults) {
    return impl_->query(prefix, maxResults);
}

bool CloudDictionaryService::isSyncing() const {
    return impl_->syncing;
}

std::chrono::system_clock::time_point CloudDictionaryService::getLastSyncTime() const {
    return impl_->lastSyncTime;
}

uint64_t CloudDictionaryService::getLocalVersion() const {
    return impl_->localVersion;
}

uint64_t CloudDictionaryService::getRemoteVersion() const {
    return impl_->remoteVersion;
}

void CloudDictionaryService::setSyncCallback(
    std::function<void(const CloudDictSyncResult&)> callback) {
    impl_->syncCallback = std::move(callback);
}

void CloudDictionaryService::setQueryCallback(
    std::function<void(const CloudDictQueryResult&)> callback) {
    impl_->queryCallback = std::move(callback);
}

bool CloudDictionaryService::clearLocalCache() {
    std::lock_guard<std::mutex> lock(impl_->mutex);
    impl_->localCache.clear();
    impl_->cacheHits = 0;
    impl_->cacheMisses = 0;
    return true;
}

CloudDictionaryService::CacheStats CloudDictionaryService::getCacheStats() const {
    CacheStats stats;
    stats.entryCount = 0;
    for (const auto& [key, entries] : impl_->localCache) {
        stats.entryCount += entries.size();
    }
    stats.totalSize = stats.entryCount * sizeof(CloudDictEntry);
    size_t total = impl_->cacheHits + impl_->cacheMisses;
    stats.hitRate = total > 0 ? static_cast<double>(impl_->cacheHits) / total : 0.0;
    return stats;
}

bool CloudDictionaryService::exportToRimeFormat(const std::string& outputPath) {
    return impl_->exportToRimeFormat(outputPath);
}

bool CloudDictionaryService::importFromRimeFormat(const std::string& inputPath) {
    return impl_->importFromRimeFormat(inputPath);
}

} // namespace cloud_dict
} // namespace plugins
} // namespace ime