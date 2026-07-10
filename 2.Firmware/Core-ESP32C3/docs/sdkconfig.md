# sdkconfig 配置

## 启用 SoftAP

```
CONFIG_ESP_WIFI_SOFTAP_SUPPORT
```

# 编辑 Partition Table

- Partition Table: `Custom partition table CSV`
- Custom partition CSV file: `partition_table.csv`
- Offset of partition table: `0x8000`
- Generate an MD5 checksum for the partition table: `Checked`

# 设置 flash size

- flash size: `4MB`

# 跳过 MQTT TLS 证书验证

跳过服务器验证：该选项并不安全，仅供测试使用。在 ESP-TLS menuconfig 中启用 `CONFIG_ESP_TLS_INSECURE` 和 `CONFIG_ESP_TLS_SKIP_SERVER_CERT_VERIFY` 可启用该选项，此时，若未在 esp_tls_cfg_t 结构体选择其他服务器验证选项，ESP-TLS 将默认跳过服务器验证。