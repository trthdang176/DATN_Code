#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <SD.h>
#include <SPI.h>

// Default WiFi credentials (can be changed via 'w' command)
String current_wifi_ssid = "BIN BO";
String current_wifi_password = "17062031809207";
#define mqtt_server "192.168.100.171" 

#define RXD2 16
#define TXD2 17

#define CS_PIN   22
#define MOSI_PIN 23
#define SCK_PIN  18
#define MISO_PIN 19

// EEPROM addresses for WiFi credentials
#define EEPROM_SIZE 512
#define SSID_ADDR 0
#define SSID_LEN_ADDR 100
#define PASS_ADDR 110
#define PASS_LEN_ADDR 210

// Task priorities
#define WIFI_TASK_PRIORITY 2
#define UART_TASK_PRIORITY 3
#define MQTT_TASK_PRIORITY 2

// Queue and semaphore handles
QueueHandle_t uartReceiveQueue;
QueueHandle_t uartSendQueue;
SemaphoreHandle_t wifiSemaphore;
SemaphoreHandle_t mqttSemaphore;

WiFiClient espClient;
PubSubClient client(espClient);

// Global variables
String data_history_topic;
String Dataprogram_topic;
String data_device_topic;
String status_topic;
String checkconnect_topic;
String MAC_Address;

HardwareSerial Serial_stm32(2);

// SD Card pin definitions (adjust according to your wiring)
#define SD_CS_PIN 5

// History data management
int currentReadIndex = -1;  // -1 means start from newest
int totalHistoryCount = 0;

// UART buffer for handling split messages
String uartBuffer = "";
unsigned long lastUartTime = 0;
#define UART_TIMEOUT_MS 300  // 300ms timeout for message completion

// Structure for UART data
struct UartData {
  char command;
  String data;
};

// Structure for data to send to STM32
struct UartSendData {
  int command_type;  // 0 = WiFi status, 1 = Program data, 2 = History data response
  String data;
};

// WiFi and connection status
volatile bool wifiConnected = false;
volatile bool mqttConnected = false;
volatile bool lastWifiState = false;
volatile bool wifiCredentialsChanged = false;  // Flag to interrupt connection attempts

// Program data storage
String programData[5] = {"", "", "", "", ""};

void setup() {
  Serial.begin(115200);
  Serial_stm32.begin(115200, SERIAL_8N1, RXD2, TXD2);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Load WiFi credentials from EEPROM
  loadWiFiCredentials();
  
  // Initialize MAC address and topics
  MAC_Address = WiFi.macAddress();
  MAC_Address.replace(":", "");
  
  data_history_topic = MAC_Address + "/" + "History";
  Dataprogram_topic = MAC_Address + "/" + "DataProgram";
  data_device_topic = MAC_Address + "/" + "StatusDevice";
  status_topic = MAC_Address + "/" + "status";
  checkconnect_topic = MAC_Address + "/" + "checkconnect";
  
  // Create queues and semaphores
  uartReceiveQueue = xQueueCreate(50, sizeof(UartData));  // Increased queue size
  uartSendQueue = xQueueCreate(20, sizeof(UartSendData));
  wifiSemaphore = xSemaphoreCreateBinary();
  mqttSemaphore = xSemaphoreCreateBinary();
  
  // Set MQTT server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(
    wifiTask,           
    "WiFi Task",        
    4096,               
    NULL,               
    WIFI_TASK_PRIORITY, 
    NULL,               
    0                   
  );
  
  xTaskCreatePinnedToCore(
    uartTask,           
    "UART Task",        
    4096,               
    NULL,               
    UART_TASK_PRIORITY, 
    NULL,               
    1                   
  );
  
  xTaskCreatePinnedToCore(
    mqttTask,           
    "MQTT Task",        
    4096,               
    NULL,               
    MQTT_TASK_PRIORITY, 
    NULL,               
    0                   
  );
  
  Serial.println("FreeRTOS tasks created successfully!");

  // Initialize SD card
  if (!SD.begin(CS_PIN)) {
    Serial.println("SD Card initialization failed!");
  } else {
    Serial.println("SD Card initialized successfully");
  }
  
  // Initialize history count
  countHistoryRecords();
  
  Serial.println("Total history records: " + String(totalHistoryCount));
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// Load WiFi credentials from EEPROM
void loadWiFiCredentials() {
  int ssidLen = EEPROM.read(SSID_LEN_ADDR);
  int passLen = EEPROM.read(PASS_LEN_ADDR);
  
  if (ssidLen > 0 && ssidLen < 50) {  // Reasonable SSID length
    current_wifi_ssid = "";
    for (int i = 0; i < ssidLen; i++) {
      current_wifi_ssid += char(EEPROM.read(SSID_ADDR + i));
    }
  }
  
  if (passLen > 0 && passLen < 50) {  // Reasonable password length
    current_wifi_password = "";
    for (int i = 0; i < passLen; i++) {
      current_wifi_password += char(EEPROM.read(PASS_ADDR + i));
    }
  }
  
  Serial.println("Loaded WiFi credentials - SSID: " + current_wifi_ssid);
}

// Save WiFi credentials to EEPROM
void saveWiFiCredentials(String ssid, String password) {
  // Save SSID
  EEPROM.write(SSID_LEN_ADDR, ssid.length());
  for (int i = 0; i < ssid.length(); i++) {
    EEPROM.write(SSID_ADDR + i, ssid[i]);
  }
  
  // Save Password
  EEPROM.write(PASS_LEN_ADDR, password.length());
  for (int i = 0; i < password.length(); i++) {
    EEPROM.write(PASS_ADDR + i, password[i]);
  }
  
  EEPROM.commit();
  Serial.println("WiFi credentials saved to EEPROM");
}

// Count total history records in file
void countHistoryRecords() {
  totalHistoryCount = 0;
  
  File file = SD.open("/hisdata.txt", FILE_READ);
  if (!file) {
    Serial.println("Failed to open hisdata.txt for counting");
    return;
  }
  
  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.length() > 0) {
      totalHistoryCount++;
    }
  }
  file.close();
  
  // Reset read index to newest
  currentReadIndex = -1;
}

// Store history data to SD card
void storeHistoryData(String data) {
  String timestamp = String(millis()); // You can use RTC for real timestamp
  String record = timestamp + "," + data;
  
  File file = SD.open("/hisdata.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open hisdata.txt for writing");
    return;
  }
  
  file.println(record);
  file.close();
  totalHistoryCount++;
  
  Serial.println("History stored: " + record);
}

// Read history data from SD card (5 records at a time)
String readHistoryData(bool readNext) {
  File file = SD.open("/hisdata.txt", FILE_READ);
  if (!file) {
    Serial.println("Failed to open hisdata.txt for reading");
    return "NO_DATA";
  }
  
  // Read all lines into an array
  String lines[totalHistoryCount];
  int lineCount = 0;
  
  while (file.available() && lineCount < totalHistoryCount) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      lines[lineCount] = line;
      lineCount++;
    }
  }
  file.close();
  
  if (lineCount == 0) {
    return "NO_DATA";
  }
  
  // Update totalHistoryCount to actual count
  totalHistoryCount = lineCount;
  
  // Determine starting index
  if (currentReadIndex == -1) {
    // Start from newest (last 5 records)
    currentReadIndex = totalHistoryCount;
  }
  
  int startIdx, endIdx;
  
  if (readNext) {
    // Read older data (decrease index)
    endIdx = currentReadIndex - 1;
    startIdx = max(0, endIdx - 4);
    currentReadIndex = startIdx;
  } else {
    // Read newer data (increase index)
    startIdx = currentReadIndex;
    endIdx = min(totalHistoryCount - 1, startIdx + 4);
    currentReadIndex = endIdx + 1;
  }
  
  // Build result string (newest first)
  String result = "";
  for (int i = max(0, totalHistoryCount - 1 - endIdx); i <= min(totalHistoryCount - 1, totalHistoryCount - 1 - startIdx); i++) {
    if (i >= 0 && i < totalHistoryCount) {
      result += lines[totalHistoryCount - 1 - i] + "|";
    }
  }
  
  if (result.length() > 0) {
    result = result.substring(0, result.length() - 1); // Remove last "|"
  }
  
  return result.length() > 0 ? result : "NO_MORE_DATA";
}

// WiFi task with dynamic credentials and interruptible connection
void wifiTask(void *parameter) {
  Serial.println("WiFi Task started");
  
  while (true) {
    bool currentWifiState = (WiFi.status() == WL_CONNECTED);
    
    if (!currentWifiState || wifiCredentialsChanged) {
      if (wifiCredentialsChanged) {
        Serial.println("WiFi credentials changed - reconnecting...");
        WiFi.disconnect();
        vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for disconnect
        wifiCredentialsChanged = false;
      }
      
      wifiConnected = false;
      mqttConnected = false;
      
      Serial.println("Connecting to WiFi: " + current_wifi_ssid);
      WiFi.begin(current_wifi_ssid.c_str(), current_wifi_password.c_str());
      
      // Non-blocking connection attempt with interrupt capability
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 40 && !wifiCredentialsChanged) {
        vTaskDelay(pdMS_TO_TICKS(250)); // Shorter delay for more responsive interruption
        Serial.print(".");
        attempts++;
        
        // Check every few attempts if credentials changed
        if (attempts % 4 == 0 && wifiCredentialsChanged) {
          Serial.println("\nConnection interrupted - new credentials received");
          break;
        }
      }
      
      currentWifiState = (WiFi.status() == WL_CONNECTED);
      
      if (currentWifiState && !wifiCredentialsChanged) {
        wifiConnected = true;
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        xSemaphoreGive(wifiSemaphore);
      } else if (!wifiCredentialsChanged) {
        Serial.println("\nWiFi connection failed, retrying...");
      }
    } else {
      if (!wifiConnected) {
        wifiConnected = true;
        xSemaphoreGive(wifiSemaphore);
      }
    }
    
    // Send WiFi status changes to STM32
    if (currentWifiState != lastWifiState) {
      Serial.println("WiFi state changed: " + String(currentWifiState ? "Connected" : "Disconnected"));
      
      UartSendData statusData;
      statusData.command_type = 0;
      statusData.data = String(currentWifiState ? 1 : 0);
      
      Serial.println("Attempting to queue WiFi status: " + statusData.data);
      if (xQueueSend(uartSendQueue, &statusData, pdMS_TO_TICKS(500)) == pdTRUE) {
        Serial.println("WiFi status queued for STM32: " + statusData.data);
      } else {
        Serial.println("Failed to queue WiFi status for STM32 - queue full");
        // Force send immediately if queue is full
        String messageToSend = String(statusData.command_type) + "," + statusData.data;
        Serial_stm32.println(messageToSend);
        Serial.println("WiFi status sent directly to STM32: " + messageToSend);
      }
      
      lastWifiState = currentWifiState;
    }
    
    vTaskDelay(pdMS_TO_TICKS(2000)); // Reduced delay for more responsiveness
  }
}

// Enhanced UART task with message buffering
void uartTask(void *parameter) {
  Serial.println("UART Task started");
  UartData uartData;
  UartSendData sendData;
  
  while (true) {
    // RECEIVE from STM32 with buffering
    if (Serial_stm32.available()) {
      String newData = Serial_stm32.readString();
      unsigned long currentTime = millis();
      
      // If too much time has passed, treat as new message
      if (currentTime - lastUartTime > UART_TIMEOUT_MS) {
        uartBuffer = "";
      }
      
      uartBuffer += newData;
      lastUartTime = currentTime;
      
      // Check if message is complete (ends with newline)
      int newlinePos = uartBuffer.indexOf('\n');
      if (newlinePos != -1) {
        String completeMessage = uartBuffer.substring(0, newlinePos);
        completeMessage.trim();
        
        // Remove processed part from buffer
        uartBuffer = uartBuffer.substring(newlinePos + 1);
        
        if (completeMessage.length() > 0) {
          Serial.println("Complete message from STM32: " + completeMessage);
          
          uartData.command = completeMessage.charAt(0);
          uartData.data = completeMessage.substring(1);
          
          if (xQueueSend(uartReceiveQueue, &uartData, pdMS_TO_TICKS(10)) != pdTRUE) {
            Serial.println("UART queue full - dropping message: " + completeMessage);
          }
        }
      }
    } else {
      // Check for timeout on incomplete message
      unsigned long currentTime = millis();
      if (uartBuffer.length() > 0 && currentTime - lastUartTime > UART_TIMEOUT_MS) {
        Serial.println("UART timeout - processing incomplete message: " + uartBuffer);
        
        String timeoutMessage = uartBuffer;
        timeoutMessage.trim();
        uartBuffer = "";
        
        if (timeoutMessage.length() > 0) {
          uartData.command = timeoutMessage.charAt(0);
          uartData.data = timeoutMessage.substring(1);
          
          if (xQueueSend(uartReceiveQueue, &uartData, pdMS_TO_TICKS(10)) != pdTRUE) {
            Serial.println("UART queue full - dropping timeout message: " + timeoutMessage);
          }
        }
      }
    }
    
    // SEND to STM32
    if (xQueueReceive(uartSendQueue, &sendData, pdMS_TO_TICKS(10)) == pdTRUE) {
      String messageToSend = String(sendData.command_type) + "," + sendData.data;
      Serial_stm32.println(messageToSend);
      Serial.println("Sent to STM32: " + messageToSend);
      
      // Add small delay to ensure message is sent
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void mqttTask(void *parameter) {
  Serial.println("MQTT Task started");
  UartData receivedData;
  
  while (true) {
    // ALWAYS process UART commands first, even without WiFi connection
    if (xQueueReceive(uartReceiveQueue, &receivedData, pdMS_TO_TICKS(50)) == pdTRUE) {
      processUartCommand(receivedData.command, receivedData.data);
    }
    
    // Handle MQTT connection only when WiFi is available
    if (wifiConnected) {
      if (!client.connected()) {
        reconnectMQTT();
      }
      
      if (client.connected()) {
        client.loop();
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(50));  // Faster processing
  }
}

// Enhanced processUartCommand with WiFi config and history reading
void processUartCommand(char command, String data) {
  String topic;
  String payload;
  
  switch (command) {
    case 'H': // History data
    case 'h':
      // Store to SD card first
      storeHistoryData(data);
      
      topic = data_history_topic;
      payload = data;
      Serial.println("Publishing history data: " + payload);
      break;
      
    case 'W': // WiFi configuration change
    case 'w':
      {
        int commaPos = data.indexOf(',');
        if (commaPos > 0) {
          String newSSID = data.substring(0, commaPos);
          String newPassword = data.substring(commaPos + 1);
          
          if (newSSID.length() > 0 && newPassword.length() > 0) {
            current_wifi_ssid = newSSID;
            current_wifi_password = newPassword;
            
            // Save to EEPROM
            saveWiFiCredentials(newSSID, newPassword);
            
            // Set flag to interrupt current connection and force reconnection
            wifiCredentialsChanged = true;
            
            Serial.println("WiFi credentials updated - SSID: " + newSSID);
            
          } else {
            Serial.println("Invalid WiFi credentials format");
            
          }
        } 
        return; 
      }
    case 'R': // Read history data
    case 'r':
      {
        bool readNext = (data == "next" || data == "NEXT");
        String historyData = readHistoryData(readNext);
        
        // Send history data back to STM32
        UartSendData historyResponse;
        historyResponse.command_type = 2; // 2 = History data response
        historyResponse.data = historyData;
        
        if (xQueueSend(uartSendQueue, &historyResponse, pdMS_TO_TICKS(500)) == pdTRUE) {
          Serial.println("History data sent to STM32: " + historyData);
        } else {
          Serial.println("Failed to queue history data for STM32");
        }
        return; // Don't publish to MQTT
      }
      
    case 'P': // Program data
    case 'p':
      if (data.length() > 0) {
        int firstComma = data.indexOf(',');
        if (firstComma > 0) {
          String program_num = data.substring(0, firstComma);
          topic = Dataprogram_topic + "/" + program_num;
          payload = data.substring(firstComma + 1);
          Serial.println("Publishing program data: " + payload);
        }
      }
      break;
      
    case 'D': // Device status
    case 'd':
      if (data.length() > 0) {
        int firstComma = data.indexOf(',');
        if (firstComma > 0) {
          String device_num = data.substring(0, firstComma);
          topic = data_device_topic + "/" + device_num;
          payload = data.substring(firstComma + 1);
          Serial.println("Publishing device status: " + payload);
        }
      }
      break;
      
    default:
      Serial.println("Unknown command: " + String(command));
      return;
  }
  
  // Publish to MQTT
  if (topic.length() > 0 && payload.length() > 0) {
    if (client.publish(topic.c_str(), payload.c_str(), true)) {
      Serial.println("Successfully published to: " + topic);
    } else {
      Serial.println("Failed to publish to: " + topic);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);
  
  String topicStr = String(topic);
  
  if (topicStr == checkconnect_topic) {
    Serial.println("WiFi status check requested");
    String statusResponse = wifiConnected ? "connected" : "disconnected";
    client.publish(status_topic.c_str(), statusResponse.c_str());
    Serial.println("WiFi status published: " + statusResponse);
    return;
  }
  
  if (topicStr.startsWith(Dataprogram_topic + "/")) {
    int lastSlash = topicStr.lastIndexOf('/');
    if (lastSlash > 0) {
      String programNumStr = topicStr.substring(lastSlash + 1);
      int programNum = programNumStr.toInt();
      
      if (programNum >= 1 && programNum <= 4 && !programNumStr.equals("request")) {
        if (programData[programNum] != messageTemp) {
          programData[programNum] = messageTemp;
          
          Serial.println("Program " + String(programNum) + " data changed: " + messageTemp);
          
          UartSendData programUpdate;
          programUpdate.command_type = 1;
          programUpdate.data = String(programNum) + "," + messageTemp;
          
          if (xQueueSend(uartSendQueue, &programUpdate, pdMS_TO_TICKS(500)) == pdTRUE) {
            Serial.println("Program update queued for STM32: " + programUpdate.data);
          } else {
            Serial.println("Failed to queue program update for STM32");
          }
        }
      }
    }
  }
}

void reconnectMQTT() {
  while (!client.connected() && wifiConnected) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP32Client-" + MAC_Address;
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttConnected = true;
      
      client.subscribe(checkconnect_topic.c_str());
      Serial.println("Subscribed to: " + checkconnect_topic);
      
      for (int i = 1; i <= 4; i++) {
        String programTopic = Dataprogram_topic + "/" + String(i);
        client.subscribe(programTopic.c_str());
        Serial.println("Subscribed to: " + programTopic);
      }
      
      for (int i = 1; i <= 4; i++) {
        String requestTopic = Dataprogram_topic + "/" + String(i) + "/request";
        client.subscribe(requestTopic.c_str());
        Serial.println("Subscribed to: " + requestTopic);
      }
      
      xSemaphoreGive(mqttSemaphore);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}