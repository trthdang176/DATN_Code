#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// #define wifi_ssid "IoTVision_2.4GHz"
// #define wifi_password "iotvision@2022"
// #define mqtt_server "192.168.0.126" 

#define wifi_ssid "BIN BO"
#define wifi_password "17062031809207"
#define mqtt_server "192.168.100.171" 

#define RXD2 16
#define TXD2 17

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

// Structure for UART data
struct UartData {
  char command;
  String data;
};

// Structure for data to send to STM32
struct UartSendData {
  int command_type;  // 0 = WiFi status, 1 = Program data
  String data;
};

// WiFi and connection status
volatile bool wifiConnected = false;
volatile bool mqttConnected = false;
volatile bool lastWifiState = false;  // Track previous WiFi state for change detection

// Program data storage (to detect changes)
String programData[5] = {"", "", "", "", ""};  // Index 1-4 for programs

void setup() {
  Serial.begin(115200);
  Serial_stm32.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  // Initialize MAC address and topics
  MAC_Address = WiFi.macAddress();
  MAC_Address.replace(":", "");
  
  data_history_topic = MAC_Address + "/" + "History";
  Dataprogram_topic = MAC_Address + "/" + "DataProgram";
  data_device_topic = MAC_Address + "/" + "StatusDevice";
  status_topic = MAC_Address + "/" + "status";
  checkconnect_topic = MAC_Address + "/" + "checkconnect";
  
  // Create queues and semaphores
  uartReceiveQueue = xQueueCreate(10, sizeof(UartData));
  uartSendQueue = xQueueCreate(10, sizeof(UartSendData));
  wifiSemaphore = xSemaphoreCreateBinary();
  mqttSemaphore = xSemaphoreCreateBinary();
  
  // Set MQTT server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(
    wifiTask,           // Task function
    "WiFi Task",        // Task name
    4096,               // Stack size
    NULL,               // Parameters
    WIFI_TASK_PRIORITY, // Priority
    NULL,               // Task handle
    0                   // Core ID
  );
  
  xTaskCreatePinnedToCore(
    uartTask,           // Task function
    "UART Task",        // Task name
    4096,               // Stack size
    NULL,               // Parameters
    UART_TASK_PRIORITY, // Priority
    NULL,               // Task handle
    1                   // Core ID
  );
  
  xTaskCreatePinnedToCore(
    mqttTask,           // Task function
    "MQTT Task",        // Task name
    4096,               // Stack size
    NULL,               // Parameters
    MQTT_TASK_PRIORITY, // Priority
    NULL,               // Task handle
    0                   // Core ID
  );
  
  Serial.println("FreeRTOS tasks created successfully!");
}

void loop() {
  // Empty - all work is done in tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}

// WiFi monitoring task with status change detection
void wifiTask(void *parameter) {
  Serial.println("WiFi Task started");
  
  while (true) {
    bool currentWifiState = (WiFi.status() == WL_CONNECTED);
    
    // CHECK 1: Is WiFi currently disconnected?
    if (!currentWifiState) {
      wifiConnected = false;
      mqttConnected = false;
      
      Serial.println("Connecting to WiFi...");
      WiFi.begin(wifi_ssid, wifi_password);
      
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        vTaskDelay(pdMS_TO_TICKS(500));
        Serial.print(".");
        attempts++;
      }
      
      // Update current state after connection attempt
      currentWifiState = (WiFi.status() == WL_CONNECTED);
      
      if (currentWifiState) {
        wifiConnected = true;
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        xSemaphoreGive(wifiSemaphore);
      } else {
        Serial.println("\nWiFi connection failed, retrying...");
      }
    } else {
      // WiFi is connected
      if (!wifiConnected) {
        wifiConnected = true;
        xSemaphoreGive(wifiSemaphore);
      }
    }
    
    // CHECK FOR WIFI STATE CHANGE - Send update to STM32 only when state changes
    if (currentWifiState != lastWifiState) {
      Serial.println("WiFi state changed: " + String(currentWifiState ? "Connected" : "Disconnected"));
      
      // Send WiFi status to STM32 via UART queue
      UartSendData statusData;
      statusData.command_type = 0;  // 0 = WiFi status
      statusData.data = String(currentWifiState ? 1 : 0);  // 1 = connected, 0 = disconnected
      
      if (xQueueSend(uartSendQueue, &statusData, pdMS_TO_TICKS(100)) == pdTRUE) {
        Serial.println("WiFi status queued for STM32: " + statusData.data);
      } else {
        Serial.println("Failed to queue WiFi status for STM32");
      }
      
      lastWifiState = currentWifiState;  // Update last state
    }
    
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// UART task - handles both receiving and sending
void uartTask(void *parameter) {
  Serial.println("UART Task started");
  UartData uartData;
  UartSendData sendData;
  
  while (true) {
    // RECEIVE from STM32
    if (Serial_stm32.available()) {
      String message = Serial_stm32.readStringUntil('\n');
      message.trim();
      
      if (message.length() > 0) {
        Serial.println("Received from STM32: " + message);
        
        uartData.command = message.charAt(0);
        uartData.data = message.substring(1);
        
        if (xQueueSend(uartReceiveQueue, &uartData, pdMS_TO_TICKS(100)) != pdTRUE) {
          Serial.println("Failed to send received data to queue");
        }
      }
    }
    
    // SEND to STM32
    if (xQueueReceive(uartSendQueue, &sendData, pdMS_TO_TICKS(10)) == pdTRUE) {
      String messageToSend = String(sendData.command_type) + "," + sendData.data;
      Serial_stm32.println(messageToSend);
      Serial.println("Sent to STM32: " + messageToSend);
    }
    
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

// MQTT task with enhanced callback handling
void mqttTask(void *parameter) {
  Serial.println("MQTT Task started");
  UartData receivedData;
  
  while (true) {
    if (xSemaphoreTake(wifiSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE || wifiConnected) {
      
      if (!client.connected()) {
        reconnectMQTT();
      }
      
      if (client.connected()) {
        client.loop();
        
        // Process UART data from STM32
        if (xQueueReceive(uartReceiveQueue, &receivedData, pdMS_TO_TICKS(100)) == pdTRUE) {
          processUartCommand(receivedData.command, receivedData.data);
        }
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// Process UART commands and publish to appropriate MQTT topics
void processUartCommand(char command, String data) {
  String topic;
  String payload;
  
  switch (command) {
    case 'H': // History data
    case 'h':
      topic = data_history_topic;
      payload = data;
      Serial.println("Publishing history data: " + payload);
      break;
      
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

// MQTT callback function - handles incoming messages and sends updates to STM32
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
  
  // Handle WiFi status check requests
  if (topicStr == checkconnect_topic) {
    Serial.println("WiFi status check requested");
    
    // Respond with current WiFi status
    String statusResponse = wifiConnected ? "connected" : "disconnected";
    client.publish(status_topic.c_str(), statusResponse.c_str());
    Serial.println("WiFi status published: " + statusResponse);
    return;
  }
  
  // Handle DataProgram updates from Python application
  if (topicStr.startsWith(Dataprogram_topic + "/")) {
    // Extract program number from topic
    int lastSlash = topicStr.lastIndexOf('/');
    if (lastSlash > 0) {
      String programNumStr = topicStr.substring(lastSlash + 1);
      int programNum = programNumStr.toInt();
      
      // Check if this is a valid program number (1-4) and not a request
      if (programNum >= 1 && programNum <= 4 && !programNumStr.equals("request")) {
        // Check if program data has changed
        if (programData[programNum] != messageTemp) {
          programData[programNum] = messageTemp;  // Store new data
          
          Serial.println("Program " + String(programNum) + " data changed: " + messageTemp);
          
          // Send updated program data to STM32
          UartSendData programUpdate;
          programUpdate.command_type = 1;  // 1 = Program data
          programUpdate.data = String(programNum) + "," + messageTemp;  // Format: program_num,data
          
          if (xQueueSend(uartSendQueue, &programUpdate, pdMS_TO_TICKS(100)) == pdTRUE) {
            Serial.println("Program update queued for STM32: " + programUpdate.data);
          } else {
            Serial.println("Failed to queue program update for STM32");
          }
        }
      }
      // Handle program data requests
      else if (programNumStr.equals("request")) {
        // This is a request for program data - we could respond with stored data if needed
        Serial.println("Program data request received for program " + programNumStr);
      }
    }
  }
}

// MQTT reconnection function
void reconnectMQTT() {
  while (!client.connected() && wifiConnected) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP32Client-" + MAC_Address;
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttConnected = true;
      
      // Subscribe to important topics
      client.subscribe(checkconnect_topic.c_str());
      Serial.println("Subscribed to: " + checkconnect_topic);
      
      // Subscribe to all DataProgram topics for receiving updates from Python
      for (int i = 1; i <= 4; i++) {
        String programTopic = Dataprogram_topic + "/" + String(i);
        client.subscribe(programTopic.c_str());
        Serial.println("Subscribed to: " + programTopic);
      }
      
      // Subscribe to program requests
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