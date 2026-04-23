#include <gtest/gtest.h>
#include "ime/core/engine.h"
#include "ime/core/input_context.h"
#include "ime/core/candidate.h"
#include "ime/core/config.h"
#include "ime/core/key_event.h"
#include "ime/core/dictionary.h"
#include "ime/core/plugin_manager.h"

using namespace ime::core;

class EngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = createEngine();
    }

    void TearDown() override {
        if (engine_) {
            engine_->shutdown();
        }
    }

    std::shared_ptr<Engine> engine_;
};

TEST_F(EngineTest, CreateEngine) {
    ASSERT_NE(engine_, nullptr);
}

TEST_F(EngineTest, InitializeEngine) {
    auto config = createConfig();
    config->setString("engine.dataPath", "./test_data");
    config->setString("engine.userPath", "./test_user");
    config->setString("engine.pluginPath", "./test_plugins");

    EXPECT_TRUE(engine_->initialize(*config));
}

TEST_F(EngineTest, GetEngineStatus) {
    auto status = engine_->getStatus();
    EXPECT_FALSE(status.initialized);
}

TEST_F(EngineTest, ShutdownEngine) {
    engine_->shutdown();
    auto status = engine_->getStatus();
    EXPECT_FALSE(status.initialized);
}

class InputContextTest : public ::testing::Test {
protected:
    InputContext context_;
};

TEST_F(InputContextTest, InitialState) {
    EXPECT_TRUE(context_.getInputText().empty());
    EXPECT_EQ(context_.getCursorPosition(), 0u);
    EXPECT_EQ(context_.getContextId(), 0u);
}

TEST_F(InputContextTest, SetInputText) {
    context_.setInputText("nihao");
    EXPECT_EQ(context_.getInputText(), "nihao");
    EXPECT_EQ(context_.getCursorPosition(), 5u);
}

TEST_F(InputContextTest, AppendInputText) {
    context_.setInputText("ni");
    context_.appendInputText("hao");
    EXPECT_EQ(context_.getInputText(), "nihao");
}

TEST_F(InputContextTest, ClearInputText) {
    context_.setInputText("test");
    context_.clearInputText();
    EXPECT_TRUE(context_.getInputText().empty());
    EXPECT_EQ(context_.getCursorPosition(), 0u);
}

TEST_F(InputContextTest, SetCursorPosition) {
    context_.setInputText("hello");
    context_.setCursorPosition(3);
    EXPECT_EQ(context_.getCursorPosition(), 3u);
}

TEST_F(InputContextTest, SetSelection) {
    context_.setInputText("hello");
    context_.setSelection(1, 4);
    EXPECT_EQ(context_.getSelectionStart(), 1u);
    EXPECT_EQ(context_.getSelectionEnd(), 4u);
    EXPECT_TRUE(context_.hasSelection());
}

TEST_F(InputContextTest, InputMode) {
    context_.setInputMode(InputMode::Chinese);
    EXPECT_EQ(context_.getInputMode(), InputMode::Chinese);

    context_.setInputMode(InputMode::English);
    EXPECT_EQ(context_.getInputMode(), InputMode::English);
}

TEST_F(InputContextTest, KeyEventHistory) {
    KeyEvent event;
    event.keyCode = 65;
    event.isKeyDown = true;
    context_.addKeyEvent(event);

    auto history = context_.getKeyEventHistory();
    EXPECT_EQ(history.size(), 1u);
    EXPECT_EQ(history[0].keyCode, 65u);

    auto lastKey = context_.getLastKeyEvent();
    ASSERT_NE(lastKey, nullptr);
    EXPECT_EQ(lastKey->keyCode, 65u);
}

TEST_F(InputContextTest, Reset) {
    context_.setInputText("test");
    context_.setInputMode(InputMode::English);
    context_.reset();

    EXPECT_TRUE(context_.getInputText().empty());
    EXPECT_EQ(context_.getInputMode(), InputMode::Chinese);
}

class CandidateTest : public ::testing::Test {
protected:
    CandidateList list_{5};
};

TEST_F(CandidateTest, AddCandidate) {
    Candidate c;
    c.text = "你";
    c.code = "ni";
    c.weight = 1.0;
    list_.add(c);

    EXPECT_EQ(list_.size(), 1u);
    EXPECT_FALSE(list_.empty());
}

TEST_F(CandidateTest, AddMultipleCandidates) {
    std::vector<Candidate> candidates;
    for (int i = 0; i < 10; ++i) {
        Candidate c;
        c.text = "词" + std::to_string(i);
        c.code = "ci" + std::to_string(i);
        c.weight = 1.0 - i * 0.1;
        candidates.push_back(c);
    }
    list_.addAll(candidates);

    EXPECT_EQ(list_.size(), 10u);
}

TEST_F(CandidateTest, Pagination) {
    for (int i = 0; i < 12; ++i) {
        Candidate c;
        c.text = "词" + std::to_string(i);
        c.weight = 1.0;
        list_.add(c);
    }

    EXPECT_EQ(list_.getTotalPages(), 3u);
    EXPECT_EQ(list_.getCurrentPageIndex(), 0u);

    auto page = list_.getCurrentPage();
    EXPECT_EQ(page.size(), 5u);

    list_.nextPage();
    EXPECT_EQ(list_.getCurrentPageIndex(), 1u);

    list_.prevPage();
    EXPECT_EQ(list_.getCurrentPageIndex(), 0u);
}

TEST_F(CandidateTest, SortCandidates) {
    Candidate c1;
    c1.text = "低";
    c1.weight = 0.5;
    Candidate c2;
    c2.text = "高";
    c2.weight = 1.0;

    list_.add(c1);
    list_.add(c2);
    list_.sort();

    auto all = list_.getAll();
    EXPECT_EQ(all[0].text, "高");
    EXPECT_EQ(all[1].text, "低");
}

TEST_F(CandidateTest, ClearCandidates) {
    Candidate c;
    c.text = "测试";
    list_.add(c);

    list_.clear();
    EXPECT_TRUE(list_.empty());
    EXPECT_EQ(list_.size(), 0u);
}

class ConfigTest : public ::testing::Test {
protected:
    std::shared_ptr<Config> config_;
};

TEST_F(ConfigTest, SetAndGetString) {
    config_ = createConfig();
    config_->setString("test.key", "value");
    EXPECT_EQ(config_->getString("test.key"), "value");
    EXPECT_EQ(config_->getString("nonexistent", "default"), "default");
}

TEST_F(ConfigTest, SetAndGetInt) {
    config_ = createConfig();
    config_->setInt("test.int", 42);
    EXPECT_EQ(config_->getInt("test.int"), 42);
    EXPECT_EQ(config_->getInt("nonexistent", 0), 0);
}

TEST_F(ConfigTest, SetAndGetBool) {
    config_ = createConfig();
    config_->setBool("test.bool", true);
    EXPECT_TRUE(config_->getBool("test.bool"));
    EXPECT_FALSE(config_->getBool("nonexistent", false));
}

TEST_F(ConfigTest, SetAndGetDouble) {
    config_ = createConfig();
    config_->setDouble("test.double", 3.14);
    EXPECT_DOUBLE_EQ(config_->getDouble("test.double"), 3.14);
}

TEST_F(ConfigTest, HasKey) {
    config_ = createConfig();
    config_->setString("existing.key", "value");
    EXPECT_TRUE(config_->hasKey("existing.key"));
    EXPECT_FALSE(config_->hasKey("nonexistent.key"));
}

TEST_F(ConfigTest, RemoveKey) {
    config_ = createConfig();
    config_->setString("to.remove", "value");
    EXPECT_TRUE(config_->removeKey("to.remove"));
    EXPECT_FALSE(config_->hasKey("to.remove"));
}

class KeyEventTest : public ::testing::Test {};

TEST_F(KeyEventTest, KeyCombination) {
    KeyEvent event;
    event.keyCode = KeyCodes::KEY_A;
    event.ctrl = true;
    event.shift = true;

    EXPECT_TRUE(event.isCombination());
    EXPECT_TRUE(event.isAlphaKey());
}

TEST_F(KeyEventTest, KeyTypes) {
    KeyEvent funcKey;
    funcKey.keyCode = KeyCodes::F1;
    EXPECT_TRUE(funcKey.isFunctionKey());

    KeyEvent navKey;
    navKey.keyCode = KeyCodes::UP;
    EXPECT_TRUE(navKey.isNavigationKey());

    KeyEvent editKey;
    editKey.keyCode = KeyCodes::BACKSPACE;
    EXPECT_TRUE(editKey.isEditingKey());

    KeyEvent numKey;
    numKey.keyCode = KeyCodes::KEY_5;
    EXPECT_TRUE(numKey.isNumericKey());
}

class EngineConfigTest : public ::testing::Test {};

TEST_F(EngineConfigTest, DefaultConfig) {
    auto config = createDefaultEngineConfig();

    EXPECT_EQ(config.candidatePageSize, 5u);
    EXPECT_TRUE(config.showComment);
    EXPECT_TRUE(config.showPinyin);
    EXPECT_FALSE(config.fuzzyPinyin);
    EXPECT_FALSE(config.autoSelect);
    EXPECT_TRUE(config.inlinePreedit);
    EXPECT_FALSE(config.enableCloudSync);
    EXPECT_TRUE(config.enableLearning);
    EXPECT_TRUE(config.enableSecurity);
}

TEST_F(EngineConfigTest, ConfigConversion) {
    auto config = createConfig();
    config->setString("engine.dataPath", "/custom/data");
    config->setInt("engine.candidatePageSize", 10);
    config->setBool("engine.fuzzyPinyin", true);

    auto engineConfig = EngineConfig::fromConfig(*config);

    EXPECT_EQ(engineConfig.dataPath, "/custom/data");
    EXPECT_EQ(engineConfig.candidatePageSize, 10u);
    EXPECT_TRUE(engineConfig.fuzzyPinyin);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}