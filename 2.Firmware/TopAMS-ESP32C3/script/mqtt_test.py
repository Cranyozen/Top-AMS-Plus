#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import json
import logging
import os
import ssl
import time
from datetime import datetime
from typing import Optional, Callable, Dict, Any

import paho.mqtt.client as mqtt

# 常量定义
BAMBU_MQTT_DEFAULT_USER = "bblp"
BAMBU_MQTT_DEFAULT_PORT = 8883
BAMBU_MQTT_TOPIC_BASE = "device"
BAMBU_MQTT_TOPIC_REPORT = "report"

# MQTT状态枚举
class BambuMQTTStatus:
    DISCONNECTED = 0
    CONNECTING = 1
    CONNECTED = 2
    ERROR = 3

class BambuStatus:
    """Bambu打印机状态数据类"""
    def __init__(self):
        self.nozzle_temper: float = 0.0
        self.bed_temper: float = 0.0
        self.wifi_signal: str = ""
        self.command: str = ""
        self.msg: int = 0
        self.sequence_id: str = ""
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "nozzle_temper": self.nozzle_temper,
            "bed_temper": self.bed_temper,
            "wifi_signal": self.wifi_signal,
            "command": self.command,
            "msg": self.msg,
            "sequence_id": self.sequence_id
        }

class BambuMQTT:
    """Bambu MQTT客户端类"""
    
    def __init__(self, ip: str, password: str, serial: str, 
                 info_callback: Optional[Callable] = None,
                 data_save_path: str = "mqtt_data"):
        self.ip = ip
        self.password = password
        self.serial = serial
        self.info_callback = info_callback
        self.data_save_path = data_save_path
        
        self.client: Optional[mqtt.Client] = None
        self.status = BambuStatus()
        self.mqtt_status = BambuMQTTStatus.DISCONNECTED
        
        # 设置日志
        logging.basicConfig(level=logging.INFO)
        self.logger = logging.getLogger("BambuMQTT")
        
        # 创建数据保存目录
        os.makedirs(data_save_path, exist_ok=True)
        
        self.logger.info(f"BambuMQTT initialized: ip={ip}, serial={serial}")
    
    def _save_mqtt_data(self, topic: str, payload: str, timestamp: str):
        """保存MQTT数据包到文件"""
        try:
            # 创建数据记录
            data_record = {
                "timestamp": timestamp,
                "topic": topic,
                "payload": payload,
                "serial": self.serial
            }
            
            # 生成文件名（按日期分组）
            date_str = datetime.now().strftime("%Y-%m-%d")
            filename = f"mqtt_data_{self.serial}_{date_str}.jsonl"
            filepath = os.path.join(self.data_save_path, filename)
            
            # 追加写入文件
            with open(filepath, 'a', encoding='utf-8') as f:
                f.write(json.dumps(data_record, ensure_ascii=False) + '\n')
            
            self.logger.debug(f"Data saved to {filepath}")
            
        except Exception as e:
            self.logger.error(f"Failed to save MQTT data: {e}")
    
    def _parse_json_data(self, payload: str) -> bool:
        """解析JSON数据并更新状态"""
        try:
            data = json.loads(payload)
            
            # 解析print字段
            # if "print" in data:
            #     print_data = data["print"]
                
            #     if "nozzle_temper" in print_data:
            #         self.status.nozzle_temper = float(print_data["nozzle_temper"])
                
            #     if "bed_temper" in print_data:
            #         self.status.bed_temper = float(print_data["bed_temper"])
                
            #     if "wifi_signal" in print_data:
            #         self.status.wifi_signal = str(print_data["wifi_signal"])
                
            #     if "command" in print_data:
            #         self.status.command = str(print_data["command"])
                
            #     if "msg" in print_data:
            #         self.status.msg = int(print_data["msg"])
                
            #     if "sequence_id" in print_data:
            #         self.status.sequence_id = str(print_data["sequence_id"])
                
                # self.logger.info(f"Nozzle Temperature: {self.status.nozzle_temper:.2f}")
                # self.logger.info(f"Bed Temperature: {self.status.bed_temper:.2f}")
                # self.logger.info(f"WiFi Signal: {self.status.wifi_signal}")
                
                # # 调用回调函数
                # if self.info_callback:
                #     self.info_callback(self.status)
                
                # return True
            
        except json.JSONDecodeError as e:
            self.logger.error(f"Failed to parse JSON data: {e}")
        except Exception as e:
            self.logger.error(f"Error processing data: {e}")
        
        return False
    
    def _on_connect(self, client, userdata, flags, rc):
        """连接回调函数"""
        if rc == 0:
            self.logger.info("MQTT_EVENT_CONNECTED")
            self.mqtt_status = BambuMQTTStatus.CONNECTED
            
            # 订阅主题
            topic = f"{BAMBU_MQTT_TOPIC_BASE}/{self.serial}/{BAMBU_MQTT_TOPIC_REPORT}"
            self.logger.info(f"Subscribing to topic: {topic}")
            
            result, mid = client.subscribe(topic, qos=1)
            if result == mqtt.MQTT_ERR_SUCCESS:
                self.logger.info(f"Subscribed to topic successfully, msg_id={mid}")
            else:
                self.logger.error(f"Failed to subscribe to topic: {topic}")
        else:
            self.logger.error(f"Connection failed with code {rc}")
            self.mqtt_status = BambuMQTTStatus.ERROR
    
    def _on_disconnect(self, client, userdata, rc):
        """断开连接回调函数"""
        self.logger.info("MQTT_EVENT_DISCONNECTED")
        self.mqtt_status = BambuMQTTStatus.DISCONNECTED
    
    def _on_message(self, client, userdata, msg):
        """消息接收回调函数"""
        timestamp = datetime.now().isoformat()
        topic = msg.topic
        payload = msg.payload.decode('utf-8')
        
        self.logger.info(f"MQTT_EVENT_DATA")
        self.logger.info(f"Received data on topic: {topic}")
        
        if payload:
            self.logger.info(f"Data: {payload}")
            
            # 保存数据包
            self._save_mqtt_data(topic, payload, timestamp)
            
            # 解析数据
            self._parse_json_data(payload)
        else:
            self.logger.info("No data received")
    
    def _on_log(self, client, userdata, level, buf):
        """日志回调函数"""
        self.logger.debug(f"MQTT Log: {buf}")
    
    def start(self):
        """启动MQTT客户端"""
        try:
            # 创建MQTT客户端
            self.client = mqtt.Client()
            
            # 设置回调函数
            self.client.on_connect = self._on_connect
            self.client.on_disconnect = self._on_disconnect
            self.client.on_message = self._on_message
            self.client.on_log = self._on_log
            
            # 设置用户名密码
            self.client.username_pw_set(BAMBU_MQTT_DEFAULT_USER, self.password)
            
            # 配置TLS/SSL
            context = ssl.create_default_context()
            context.check_hostname = False
            context.verify_mode = ssl.CERT_NONE
            self.client.tls_set_context(context)
            
            # 设置keepalive
            self.client.keepalive = 120
            
            # 连接到broker
            broker_uri = f"{self.ip}:{BAMBU_MQTT_DEFAULT_PORT}"
            self.logger.info(f"Connecting to MQTT broker at {broker_uri}")
            
            self.mqtt_status = BambuMQTTStatus.CONNECTING
            self.client.connect(self.ip, BAMBU_MQTT_DEFAULT_PORT, 60)
            
            # 启动网络循环
            self.client.loop_start()
            
            self.logger.info("BambuMQTT client started")
            
        except Exception as e:
            self.logger.error(f"Failed to start MQTT client: {e}")
            self.mqtt_status = BambuMQTTStatus.ERROR
    
    def stop(self):
        """停止MQTT客户端"""
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
            self.client = None
            self.mqtt_status = BambuMQTTStatus.DISCONNECTED
            self.logger.info("BambuMQTT client stopped")
    
    def publish_message(self, message: str) -> bool:
        """发布消息"""
        if not self.client:
            self.logger.error("MQTT client not initialized")
            return False
        
        topic = f"{BAMBU_MQTT_TOPIC_BASE}/{self.serial}/{BAMBU_MQTT_TOPIC_REPORT}"
        
        try:
            result = self.client.publish(topic, message, qos=1)
            
            if result.rc == mqtt.MQTT_ERR_SUCCESS:
                self.logger.info(f"Message published successfully: {message}")
                return True
            else:
                self.logger.error(f"Failed to publish message: {message}")
                return False
                
        except Exception as e:
            self.logger.error(f"Error publishing message: {e}")
            return False
    
    def get_status(self) -> BambuStatus:
        """获取当前状态"""
        return self.status
    
    def is_connected(self) -> bool:
        """检查是否已连接"""
        return self.mqtt_status == BambuMQTTStatus.CONNECTED

# 使用示例
def status_callback(status: BambuStatus):
    """状态更新回调函数"""
    print(f"Status updated: {status.to_dict()}")

def main():
    """主函数示例"""
    # 配置参数
    PRINTER_IP = "192.168.1.199"  # 替换为实际的打印机IP
    PRINTER_PASSWORD = "56154859"  # 替换为实际密码
    PRINTER_SERIAL = "03919D530105226"  # 替换为实际序列号
    
    # 创建MQTT客户端
    mqtt_client = BambuMQTT(
        ip=PRINTER_IP,
        password=PRINTER_PASSWORD,
        serial=PRINTER_SERIAL,
        info_callback=status_callback,
        data_save_path="./mqtt_data_logs"
    )
    
    try:
        # 启动客户端
        mqtt_client.start()
        
        # 等待连接
        while not mqtt_client.is_connected():
            time.sleep(1)
        
        print("Connected to Bambu printer!")

        mqtt_client.publish_message('{ "pushing": {"sequence_id": "0", "command": "start"}}')
        
        # 保持运行
        while True:
            time.sleep(5)
            # 可以在这里添加其他逻辑
            mqtt_client.publish_message('{"pushing": {"sequence_id": "0", "command": "pushall"}}')
            
    except KeyboardInterrupt:
        print("\nShutting down...")
    finally:
        mqtt_client.stop()

if __name__ == "__main__":
    main()