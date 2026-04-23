#include "rime_adapter_plugin.h"
#include <rime_api.h>
#include <algorithm>
#include <cstring>

namespace ime {
namespace plugins {
namespace rime {

RimeAdapterPlugin::RimeAdapterPlugin() {
    info_.id = "rime-adapter";
    info_.name = "Rime Input Method Adapter";
    info_.version = "1.0.0";
    info_.description = "Adapts Rime (中州韻) input method engine as a plugin";
    info_.author = "Cross-Platform IME Team";
    info_.license = "MIT";
    info_.type = ime::core::PluginType::Engine;
}

RimeAdapterPlugin::~RimeAdapterPlugin() {
    shutdown();
}

const ime::core::PluginInfo& RimeAdapterPlugin::getInfo() const {
    return info_;
}

bool RimeAdapterPlugin::initialize(const ime::core::Config& config) {
    if (initialized_) {
        return true;
    }

    userDataDir_ = config.getString("rime.userDataDir", "./user/rime");
    sharedDataDir_ = config.getString("rime.sharedDataDir", "./data/rime");

    if (!initializeRime()) {
        return false;
    }

    initialized_ = true;
    return true;
}

bool RimeAdapterPlugin::processInput(ime::core::InputContext& context) {
    if (!initialized_ || !enabled_) {
        return false;
    }

    auto* lastKey = context.getLastKeyEvent();
    if (!lastKey) {
        return false;
    }

    int mask = 0;
    if (lastKey->ctrl) mask |= kControlMask;
    if (lastKey->shift) mask |= kShiftMask;
    if (lastKey->alt) mask |= kAltMask;

    return processRimeKey(static_cast<int>(lastKey->keyCode), mask);
}

std::vector<ime::core::Candidate> RimeAdapterPlugin::filterCandidates(
    const std::vector<ime::core::Candidate>& candidates,
    const ime::core::InputContext& context) {
    return candidates;
}

void RimeAdapterPlugin::shutdown() {
    if (!initialized_) {
        return;
    }

    cleanupRime();
    initialized_ = false;
}

bool RimeAdapterPlugin::isInitialized() const {
    return initialized_;
}

bool RimeAdapterPlugin::isEnabled() const override {
    return enabled_;
}

void RimeAdapterPlugin::setEnabled(bool enabled) {
    enabled_ = enabled;
}

std::vector<ime::core::Candidate> RimeAdapterPlugin::getCandidates(
    const ime::core::InputContext& context) {
    if (!initialized_ || !rimeApi_) {
        return {};
    }

    return convertRimeCandidates();
}

bool RimeAdapterPlugin::commitCandidate(const ime::core::Candidate& candidate,
    ime::core::InputContext& context) {
    if (!initialized_ || !rimeApi_) {
        return false;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    RimeCommit commit;
    if (api->get_commit(rimeInstance_, &commit)) {
        if (commit.text) {
            context.setInputText(commit.text);
        }
    }

    return true;
}

std::string RimeAdapterPlugin::getSchemaId() const {
    return currentSchemaId_;
}

std::string RimeAdapterPlugin::getSchemaName() const {
    return currentSchemaName_;
}

bool RimeAdapterPlugin::setUserDataDir(const std::string& path) {
    userDataDir_ = path;
    return true;
}

bool RimeAdapterPlugin::setSharedDataDir(const std::string& path) {
    sharedDataDir_ = path;
    return true;
}

bool RimeAdapterPlugin::switchSchema(const std::string& schemaId) {
    if (!initialized_ || !rimeApi_) {
        return false;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    if (api->select_schema(rimeInstance_, schemaId.c_str())) {
        currentSchemaId_ = schemaId;
        return true;
    }
    return false;
}

std::vector<std::string> RimeAdapterPlugin::getAvailableSchemas() const {
    std::vector<std::string> schemas;
    if (!initialized_ || !rimeApi_) {
        return schemas;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    RimeSchemaList list;
    if (api->get_schema_list(rimeInstance_, &list)) {
        for (size_t i = 0; i < list.size; ++i) {
            schemas.push_back(list.list[i].id);
        }
        api->free_schema_list(&list);
    }

    return schemas;
}

bool RimeAdapterPlugin::deploy() {
    if (!rimeApi_) {
        return false;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    api->start_maintenance(rimeInstance_);
    return api->is_maintenance_finished(rimeInstance_);
}

bool RimeAdapterPlugin::syncUserData() {
    if (!rimeApi_) {
        return false;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    api->sync_user_data(rimeInstance_);
    return true;
}

void* RimeAdapterPlugin::getRimeInstance() const {
    return rimeInstance_;
}

bool RimeAdapterPlugin::initializeRime() {
    auto* api = rime_get_api();
    if (!api) {
        return false;
    }

    rimeApi_ = api;

    RimeSetup setup;
    setup.shared_data_dir = sharedDataDir_.c_str();
    setup.user_data_dir = userDataDir_.c_str();
    setup.log_dir = (userDataDir_ + "/logs").c_str();

    api->setup(&setup);

    rimeInstance_ = api->create_session();
    if (!rimeInstance_) {
        rimeApi_ = nullptr;
        return false;
    }

    return true;
}

void RimeAdapterPlugin::cleanupRime() {
    if (rimeApi_ && rimeInstance_) {
        auto* api = static_cast<RimeApi*>(rimeApi_);
        api->destroy_session(rimeInstance_);
        rimeInstance_ = nullptr;
    }
    rimeApi_ = nullptr;
}

std::vector<ime::core::Candidate> RimeAdapterPlugin::convertRimeCandidates() {
    std::vector<ime::core::Candidate> candidates;

    if (!rimeApi_ || !rimeInstance_) {
        return candidates;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    RimeMenu menu;
    if (!api->get_menu(rimeInstance_, &menu)) {
        return candidates;
    }

    for (size_t i = 0; i < menu.page_size && i < menu.page_size; ++i) {
        RimeCandidate* rimeCandidate = &menu.candidates[i];
        ime::core::Candidate candidate;
        candidate.text = rimeCandidate->text;
        candidate.comment = rimeCandidate->comment ? rimeCandidate->comment : "";
        candidate.order = static_cast<uint32_t>(i);
        candidate.source = "rime";
        candidates.push_back(candidate);
    }

    return candidates;
}

bool RimeAdapterPlugin::processRimeKey(int keycode, int mask) {
    if (!rimeApi_ || !rimeInstance_) {
        return false;
    }

    auto* api = static_cast<RimeApi*>(rimeApi_);
    return api->process_key(rimeInstance_, keycode, mask);
}

} // namespace rime
} // namespace plugins
} // namespace ime