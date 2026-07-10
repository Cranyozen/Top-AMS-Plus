# 在 Linux 上配置内置 USB_JTAG 调试器

## 配置 udev 规则

```bash
wget https://raw.githubusercontent.com/espressif/openocd-esp32/refs/heads/master/contrib/60-openocd.rules
sudo cp 60-openocd.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
```

## 将用户添加到组

```bash
sudo groupadd plugdev
sudo usermod -aG plugdev $USER
```
