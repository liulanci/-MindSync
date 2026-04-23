#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <chrono>
#include <memory>
#include <functional>

namespace ime {
namespace security {

/**
 * @brief 安全审计事件类型
 */
enum class AuditEventType {
    InputDataAccess,        // 输入数据访问
    DictionaryAccess,       // 词库访问
    PluginLoad,             // 插件加载
    PluginUnload,           // 插件卸载
    ConfigChange,           // 配置变更
    SyncOperation,          // 同步操作
    AuthSuccess,            // 认证成功
    AuthFailure,            // 认证失败
    DataExport,             // 数据导出
    DataImport,             // 数据导入
    SecurityViolation,      // 安全违规
    EncryptionOperation,    // 加密操作
    KeyRotation,            // 密钥轮换
    SandboxViolation        // 沙箱违规
};

/**
 * @brief 安全审计事件
 */
struct AuditEvent {
    AuditEventType type;
    std::string source;
    std::string description;
    std::string userId;
    std::string deviceId;
    std::string details;
    std::chrono::system_clock::time_point timestamp;
    int severity; // 1-5, 5最高
};

/**
 * @brief 安全配置
 */
struct SecurityConfig {
    bool enableAuditLogging = true;
    bool enableDataEncryption = true;
    bool enableSandbox = true;
    bool enableInputSanitization = true;
    bool enableRateLimiting = true;
    bool enableTamperDetection = true;

    std::string encryptionAlgorithm = "AES-256-GCM";
    std::string keyDerivationFunction = "PBKDF2";
    uint32_t keyDerivationIterations = 100000;

    uint32_t maxInputLength = 1024;
    uint32_t maxDictionarySize = 1000000;
    uint32_t maxPluginCount = 50;
    uint32_t rateLimitPerSecond = 100;

    std::string auditLogPath = "./logs/security";
    uint32_t auditLogRetentionDays = 90;
    size_t maxAuditLogSize = 100 * 1024 * 1024; // 100MB
};

/**
 * @brief 安全审计器
 */
class SecurityAuditor {
public:
    virtual ~SecurityAuditor() = default;

    virtual bool initialize(const SecurityConfig& config) = 0;
    virtual void logEvent(const AuditEvent& event) = 0;
    virtual std::vector<AuditEvent> queryEvents(
        AuditEventType type,
        std::chrono::system_clock::time_point from,
        std::chrono::system_clock::time_point to) = 0;
    virtual bool exportAuditLog(const std::string& outputPath) = 0;
    virtual void shutdown() = 0;
};

/**
 * @brief 数据加密器
 */
class DataEncryptor {
public:
    virtual ~DataEncryptor() = default;

    virtual bool initialize(const std::string& masterKey) = 0;
    virtual std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> decrypt(const std::vector<uint8_t>& encryptedData) = 0;
    virtual std::string encryptString(const std::string& text) = 0;
    virtual std::string decryptString(const std::string& encryptedText) = 0;
    virtual bool rotateKey() = 0;
    virtual void shutdown() = 0;
};

/**
 * @brief 输入验证器
 */
class InputValidator {
public:
    virtual ~InputValidator() = default;

    virtual std::string sanitizeInput(const std::string& input) = 0;
    virtual bool validateInput(const std::string& input, size_t maxLength) = 0;
    virtual bool validateFilePath(const std::string& path) = 0;
    virtual bool validatePluginPath(const std::string& path) = 0;
    virtual bool validateConfigKey(const std::string& key) = 0;
};

/**
 * @brief 插件沙箱
 */
class PluginSandbox {
public:
    virtual ~PluginSandbox() = default;

    virtual bool initialize() = 0;
    virtual bool executeInSandbox(std::function<void()> operation,
        const std::string& pluginId) = 0;
    virtual bool checkPermission(const std::string& pluginId,
        const std::string& permission) = 0;
    virtual bool grantPermission(const std::string& pluginId,
        const std::string& permission) = 0;
    virtual bool revokePermission(const std::string& pluginId,
        const std::string& permission) = 0;
    virtual void shutdown() = 0;
};

/**
 * @brief 安全管理器
 */
class SecurityManager {
public:
    virtual ~SecurityManager() = default;

    virtual bool initialize(const SecurityConfig& config) = 0;
    virtual std::shared_ptr<SecurityAuditor> getAuditor() = 0;
    virtual std::shared_ptr<DataEncryptor> getEncryptor() = 0;
    virtual std::shared_ptr<InputValidator> getValidator() = 0;
    virtual std::shared_ptr<PluginSandbox> getSandbox() = 0;
    virtual const SecurityConfig& getConfig() const = 0;
    virtual void shutdown() = 0;
};

/**
 * @brief 创建安全管理器
 */
std::shared_ptr<SecurityManager> createSecurityManager();

} // namespace security
} // namespace ime