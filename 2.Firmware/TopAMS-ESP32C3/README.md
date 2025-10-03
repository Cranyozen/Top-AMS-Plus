# TopAMS ESP32 C3  固件

[CN](README.md) | [EN](README_en.md)

## 介绍

这是 TopAMS-Plus ESP32 C3 的固件，用于控制 TopAMS（为 Bambulab A1 或更多 3D 打印机设计）。

## 特性

- 基于 ESP32 C3 (Super Mini)，支持 Wi-Fi 和蓝牙连接
- 强大的兼容性和可推展能力，尽可能适配多的硬件 (TODO)
- 使用 ESP-IDF 框架开发，尽量不依赖第三方库（区别于原版 TopAMS 依赖 Arduino, ArduinoJson）
- 支持更灵活的通道配置
- 支持更完善的配套软件
- 支持 OTA 固件升级 (TODO)
- 支持更多传感器和外设 (TODO)

## 开发环境

- ESP-IDF 版本: v5.3.2 (推荐使用相同版本，避免 sdkconfig 不兼容)
- OS: Linux 6.16.8-arch3-1 (KDE Plasma 6.4.5)
- 其他开发环境理论上兼容，主要是看 **ESP-IDF 版本**

## 使用

### 使用发布的固件

1. 从 [releases 页面]() 下载最新固件
2. 使用 idf.py 或 flash 工具将固件刷入 ESP32 C3

### 构建固件

1. 克隆代码库

2. 安装 ESP-IDF 环境
   请参阅 [ESP-IDF 文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation) 以获取有关如何设置 ESP-IDF 环境的说明。

3. 配置项目 (使用 ESP-IDF v5.3.2 理论上可跳过此步骤)
  * 打开项目配置菜单 (`idf.py menuconfig`)
  * 按照 [docs/sdk_config.md](docs/sdk_config.md) 中的说明进行配置

3. 构建项目
   * 运行 `idf.py build` 来编译项目

4. 刷写固件
   * 将 ESP32 C3 连接到计算机
   * 运行 `idf.py -p (PORT) flash` 来上传固件

## TODO


## 参考

- [ESP32-C3 Technical Reference Manual CN](https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_cn.pdf)
- [ESP32-C3 Datasheet CN](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_cn.pdf)