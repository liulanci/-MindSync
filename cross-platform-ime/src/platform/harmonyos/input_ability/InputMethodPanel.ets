/**
 * HarmonyOS输入法Ability
 * 支持HarmonyOS 2.0及更高版本
 * 基于InputMethodAbility框架实现
 */
import inputMethodClient from '@ohos.inputMethodClient';
import inputMethodEngine from '@ohos.inputMethodEngine';
import window from '@ohos.window';
import { BusinessError } from '@ohos.base';

interface CandidateItem {
  text: string;
  code: string;
  weight: number;
}

interface ImeConfig {
  dataPath: string;
  userPath: string;
  pluginPath: string;
  candidatePageSize: number;
  enableCloudSync: boolean;
  syncServer: string;
}

@Entry
@Component
struct InputMethodPanel {
  @State candidates: CandidateItem[] = [];
  @State selectedIndex: number = 0;
  @State currentInput: string = '';
  @State isChineseMode: boolean = true;
  @State panelHeight: number = 280;

  private inputMethodAbility: inputMethodEngine.InputMethodAbility | null = null;
  private keyboardController: inputMethodEngine.KeyboardController | null = null;
  private textInputClient: inputMethodEngine.InputDataClient | null = null;
  private engine: object | null = null;

  aboutToAppear() {
    this.initInputMethod();
    this.initEngine();
  }

  private initInputMethod() {
    try {
      this.inputMethodAbility = inputMethodEngine.getInputMethodAbility();
      this.keyboardController = this.inputMethodAbility.createKeyboardController();

      this.inputMethodAbility.on('inputStart', (keyboardController, textInputClient) => {
        this.keyboardController = keyboardController;
        this.textInputClient = textInputClient;
      });

      this.inputMethodAbility.on('inputStop', () => {
        this.cleanup();
      });
    } catch (err) {
      const error = err as BusinessError;
      console.error(`InputMethod init failed: ${error.code} - ${error.message}`);
    }
  }

  private initEngine() {
    // 通过NAPI调用C++核心引擎
    // this.engine = nativeImeCreateEngine();
  }

  build() {
    Column() {
      // 候选词区域
      this.CandidateArea()

      // 工具栏
      this.Toolbar()

      // 键盘区域
      this.KeyboardArea()
    }
    .width('100%')
    .height(this.panelHeight)
    .backgroundColor('#F5F5F5')
  }

  @Builder
  CandidateArea() {
    Scroll() {
      Row() {
        ForEach(this.candidates, (candidate: CandidateItem, index?: number) => {
          Text(candidate.text)
            .fontSize(16)
            .fontColor(index === this.selectedIndex ? '#FFFFFF' : '#333333')
            .backgroundColor(index === this.selectedIndex ? '#007DFF' : '#FFFFFF')
            .borderRadius(4)
            .padding({ left: 12, right: 12, top: 6, bottom: 6 })
            .margin({ right: 8 })
            .onClick(() => {
              if (index !== undefined) {
                this.selectCandidate(index);
              }
            })
        }, (candidate: CandidateItem, index?: number) => `${index}_${candidate.text}`)
      }
      .padding({ left: 8, right: 8, top: 4, bottom: 4 })
    }
    .scrollable(ScrollDirection.Horizontal)
    .scrollBar(BarState.Off)
    .width('100%')
    .height(44)
    .visibility(this.candidates.length > 0 ? Visibility.Visible : Visibility.None)
  }

  @Builder
  Toolbar() {
    Row() {
      Button(this.isChineseMode ? '中' : '英')
        .fontSize(14)
        .fontColor('#333333')
        .backgroundColor('#E0E0E0')
        .borderRadius(4)
        .width(40)
        .height(32)
        .onClick(() => this.toggleInputMode())

      Button('空格')
        .fontSize(14)
        .fontColor('#333333')
        .backgroundColor('#E0E0E0')
        .borderRadius(4)
        .layoutWeight(1)
        .height(32)
        .onClick(() => this.insertSpace())

      Button('⌫')
        .fontSize(14)
        .fontColor('#333333')
        .backgroundColor('#E0E0E0')
        .borderRadius(4)
        .width(40)
        .height(32)
        .onClick(() => this.deleteBackward())

      Button('换行')
        .fontSize(14)
        .fontColor('#333333')
        .backgroundColor('#E0E0E0')
        .borderRadius(4)
        .width(48)
        .height(32)
        .onClick(() => this.insertNewline())
    }
    .width('100%')
    .height(40)
    .padding({ left: 4, right: 4 })
    .justifyContent(FlexAlign.SpaceBetween)
  }

  @Builder
  KeyboardArea() {
    Grid() {
      ForEach(this.getKeyboardKeys(), (key: string) => {
        GridItem() {
          Text(key)
            .fontSize(20)
            .fontColor('#333333')
            .textAlign(TextAlign.Center)
            .backgroundColor('#FFFFFF')
            .borderRadius(4)
            .width('100%')
            .height(40)
            .onClick(() => this.handleKeyPress(key))
        }
        .padding(2)
      }, (key: string) => key)
    }
    .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr 1fr')
    .rowsGap(4)
    .columnsGap(4)
    .width('100%')
    .padding({ left: 4, right: 4 })
  }

  private getKeyboardKeys(): string[] {
    if (this.isChineseMode) {
      return ['q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
              'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '',
              'z', 'x', 'c', 'v', 'b', 'n', 'm', '', '', ''];
    }
    return ['1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
            'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
            'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ''];
  }

  private handleKeyPress(key: string) {
    if (key === '') return;

    if (this.isChineseMode) {
      this.currentInput += key;
      this.updateCandidates();
    } else {
      this.commitText(key);
    }
  }

  private updateCandidates() {
    if (this.currentInput.isEmpty()) {
      this.candidates = [];
      return;
    }

    // 通过NAPI调用核心引擎获取候选词
    // const results = nativeImeGetCandidates(this.engine, this.currentInput);
    // this.candidates = results;
    // 临时模拟
    this.candidates = [
      { text: '你', code: 'ni', weight: 1.0 },
      { text: '尼', code: 'ni', weight: 0.8 },
      { text: '泥', code: 'ni', weight: 0.6 }
    ];
  }

  private selectCandidate(index: number) {
    if (index < 0 || index >= this.candidates.length) return;

    const candidate = this.candidates[index];
    this.commitText(candidate.text);
    this.currentInput = '';
    this.candidates = [];
  }

  private commitText(text: string) {
    if (this.textInputClient) {
      try {
        this.textInputClient.insertText(text);
      } catch (err) {
        const error = err as BusinessError;
        console.error(`Commit text failed: ${error.code}`);
      }
    }
  }

  private insertSpace() {
    if (this.currentInput.length > 0) {
      this.selectCandidate(0);
    } else {
      this.commitText(' ');
    }
  }

  private deleteBackward() {
    if (this.currentInput.length > 0) {
      this.currentInput = this.currentInput.slice(0, -1);
      this.updateCandidates();
    } else if (this.textInputClient) {
      try {
        this.textInputClient.deleteForward(1);
      } catch (err) {
        const error = err as BusinessError;
        console.error(`Delete failed: ${error.code}`);
      }
    }
  }

  private insertNewline() {
    this.commitText('\n');
  }

  private toggleInputMode() {
    this.isChineseMode = !this.isChineseMode;
    if (!this.isChineseMode) {
      this.currentInput = '';
      this.candidates = [];
    }
  }

  private cleanup() {
    this.currentInput = '';
    this.candidates = [];
    this.selectedIndex = 0;
  }
}