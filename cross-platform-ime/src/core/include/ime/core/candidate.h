#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cstdint>
#include <chrono>

namespace ime {
namespace core {

/**
 * @brief 候选词
 */
struct Candidate {
    std::string text;           // 候选文本
    std::string code;           // 输入编码
    std::string comment;        // 注释/提示
    double weight = 0.0;        // 权重/频率
    uint32_t order = 0;         // 排序序号
    std::string source;         // 来源词库
    std::string type;           // 类型 (word/phrase/symbol/emoji)
    bool isUserWord = false;    // 是否为用户词
    bool isPinned = false;      // 是否置顶
    std::chrono::system_clock::time_point lastUsed; // 最后使用时间
    uint64_t useCount = 0;      // 使用次数

    bool operator<(const Candidate& other) const {
        if (isPinned != other.isPinned) return isPinned;
        if (weight != other.weight) return weight > other.weight;
        return order < other.order;
    }

    bool operator==(const Candidate& other) const {
        return text == other.text && code == other.code;
    }
};

/**
 * @brief 候选词列表
 */
class CandidateList {
public:
    CandidateList() = default;
    explicit CandidateList(size_t pageSize);

    /**
     * @brief 添加候选词
     */
    void add(const Candidate& candidate);

    /**
     * @brief 批量添加候选词
     */
    void addAll(const std::vector<Candidate>& candidates);

    /**
     * @brief 获取当前页候选词
     */
    std::vector<Candidate> getCurrentPage() const;

    /**
     * @brief 获取指定页候选词
     */
    std::vector<Candidate> getPage(size_t page) const;

    /**
     * @brief 获取所有候选词
     */
    const std::vector<Candidate>& getAll() const;

    /**
     * @brief 获取指定索引的候选词
     */
    const Candidate& get(size_t index) const;

    /**
     * @brief 选中指定索引的候选词
     */
    const Candidate& select(size_t index);

    /**
     * @brief 翻页
     */
    bool nextPage();

    /**
     * @brief 翻页
     */
    bool prevPage();

    /**
     * @brief 跳转到指定页
     */
    bool gotoPage(size_t page);

    /**
     * @brief 获取当前页码
     */
    size_t getCurrentPageIndex() const;

    /**
     * @brief 获取总页数
     */
    size_t getTotalPages() const;

    /**
     * @brief 获取候选词总数
     */
    size_t size() const;

    /**
     * @brief 是否为空
     */
    bool empty() const;

    /**
     * @brief 清空候选词列表
     */
    void clear();

    /**
     * @brief 设置每页大小
     */
    void setPageSize(size_t size);

    /**
     * @brief 获取每页大小
     */
    size_t getPageSize() const;

    /**
     * @brief 排序候选词
     */
    void sort();

    /**
     * @brief 去重
     */
    void deduplicate();

    /**
     * @brief 获取选中的候选词
     */
    const Candidate* getSelected() const;

    /**
     * @brief 获取选中索引
     */
    int getSelectedIndex() const;

private:
    std::vector<Candidate> candidates_;
    size_t pageSize_ = 5;
    size_t currentPage_ = 0;
    int selectedIndex_ = -1;
};

} // namespace core
} // namespace ime