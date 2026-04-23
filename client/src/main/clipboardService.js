const { clipboard, nativeImage } = require('electron');
const { EventEmitter } = require('events');

class ClipboardService extends EventEmitter {
    constructor() {
        super();
        this.isMonitoring = false;
        this.lastContent = '';
        this.lastImage = null;
        this.pollInterval = 1000; // 1秒轮询间隔
        this.intervalId = null;
        this.injectionEnabled = true;
    }

    startMonitoring() {
        if (this.isMonitoring) {
            return;
        }

        this.isMonitoring = true;
        this.lastContent = clipboard.readText();
        this.lastImage = clipboard.readImage();

        this.intervalId = setInterval(() => {
            this.checkClipboardChanges();
        }, this.pollInterval);

        // 剪贴板监控服务已启动
    }

    stopMonitoring() {
        if (!this.isMonitoring) {
            return;
        }

        this.isMonitoring = false;
        if (this.intervalId) {
            clearInterval(this.intervalId);
            this.intervalId = null;
        }

        // 剪贴板监控服务已停止
    }

    checkClipboardChanges() {
        // 检查文本变化
        const currentText = clipboard.readText();
        if (currentText && currentText !== this.lastContent) {
            this.lastContent = currentText;
            this.emit('text-changed', currentText);
            // 检测到剪贴板文本变化
        }

        // 检查图片变化
        const currentImage = clipboard.readImage();
        if (!currentImage.isEmpty() && !this.imagesEqual(currentImage, this.lastImage)) {
            this.lastImage = currentImage;
            const imageData = currentImage.toDataURL();
            this.emit('image-changed', imageData);
            // 检测到剪贴板图片变化
        }
    }

    imagesEqual(img1, img2) {
        if (!img1 || !img2) return false;
        if (img1.isEmpty() || img2.isEmpty()) return false;
        
        try {
            const data1 = img1.toDataURL();
            const data2 = img2.toDataURL();
            return data1 === data2;
        } catch (error) {
            return false;
        }
    }

    injectText(text) {
        if (!this.injectionEnabled) {
            return false;
        }

        try {
            clipboard.writeText(text);
            this.lastContent = text;
            this.emit('text-injected', text);
            // 文本已注入剪贴板
            return true;
        } catch (error) {
            console.error('文本注入失败:', error);
            return false;
        }
    }

    injectImage(imageData) {
        if (!this.injectionEnabled) {
            return false;
        }

        try {
            const image = nativeImage.createFromDataURL(imageData);
            clipboard.writeImage(image);
            this.lastImage = image;
            this.emit('image-injected', imageData);
            // 图片已注入剪贴板
            return true;
        } catch (error) {
            console.error('图片注入失败:', error);
            return false;
        }
    }

    enableInjection() {
        this.injectionEnabled = true;
        // 剪贴板注入功能已启用
    }

    disableInjection() {
        this.injectionEnabled = false;
        // 剪贴板注入功能已禁用
    }

    getStatus() {
        return {
            isMonitoring: this.isMonitoring,
            injectionEnabled: this.injectionEnabled,
            lastContent: this.lastContent ? this.lastContent.substring(0, 100) + '...' : null,
            hasImage: !!(this.lastImage && !this.lastImage.isEmpty())
        };
    }
}

module.exports = ClipboardService;