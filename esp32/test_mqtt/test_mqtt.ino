#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#define wifi_ssid "IoTVision_2.4GHz"
#define wifi_password "iotvision@2022"
// #define mqtt_server "192.168.100.171" 
#define mqtt_server "192.168.0.126" 

#define RXD2 16
#define TXD2 17

// Task priorities
#define WIFI_TASK_PRIORITY 2
#define UART_TASK_PRIORITY 3
#define MQTT_TASK_PRIORITY 2

// Queue and semaphore handles
QueueHandle_t uartQueue;
SemaphoreHandle_t wifiSemaphore;
SemaphoreHandle_t mqttSemaphore;

WiFiClient espClient;
PubSubClient client(espClient);

// Global variables
String data_history_topic;
String Dataprogram_topic;
String data_device_topic;
String MAC_Address;

HardwareSerial Serial_stm32(2);

// Structure for UART data
struct UartData {
  char command;
  String data;
};

// WiFi status
volatile bool wifiConnected = false;
volatile bool mqttConnected = false;

void setup() {
  Serial.begin(115200);
  Serial_stm32.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  // Initialize MAC address and topics
  MAC_Address = WiFi.macAddress();
  MAC_Address.replace(":", "");
  
  data_history_topic = MAC_Address + "/" + "History";
  Dataprogram_topic = MAC_Address + "/" + "DataProgram";
  data_device_topic = MAC_Address + "/" + "StatusDevice";
  
  // Create queue and semaphores
  uartQueue = xQueueCreate(10, sizeof(UartData));
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

// WiFi monitoring task
void wifiTask(void *parameter) {
  Serial.println("WiFi Task started");
  
  // Main task loop - runs forever
  while (true) {
    
    // CHECK 1: Is WiFi currently disconnected?
    if (WiFi.status() != WL_CONNECTED) {
      // Set flags to indicate we're disconnected
      wifiConnected = false;
      mqttConnected = false;  // MQTT can't work without WiFi
      
      Serial.println("Connecting to WiFi...");
      WiFi.begin(wifi_ssid, wifi_password);  // Start connection attempt
      
      // INNER LOOP: Wait for connection with timeout protection
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        vTaskDelay(pdMS_TO_TICKS(500));  // Wait 500ms (FreeRTOS delay)
        Serial.print(".");               // Show progress dots
        attempts++;                      // Count attempts (20 x 500ms = 10 seconds max)
      }
      
      // CHECK 2: Did we successfully connect?
      if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // SIGNAL other tasks that WiFi is ready
        xSemaphoreGive(wifiSemaphore);  // Release semaphore to wake up MQTT task
        
      } else {
        Serial.println("\nWiFi connection failed, retrying...");
        // Will retry in next loop iteration after 5 second delay
      }
      
    } else {
      // WiFi is connected, but check if our flag is outdated
      if (!wifiConnected) {
        wifiConnected = true;
        xSemaphoreGive(wifiSemaphore);  // Signal that WiFi is available
      }
    }
    
    // Wait 5 seconds before checking WiFi status again
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

// UART receiving task
void uartTask(void *parameter) {
  // Serial.println("UART Task started");
  UartData uartData;
  
  while (true) {
    if (Serial_stm32.available()) {
      String message = Serial_stm32.readStringUntil('\n');
      message.trim(); // Remove whitespace
      
      if (message.length() > 0) {
        Serial.println("Received from STM32: " + message);
        
        // Parse command (first character) and data
        uartData.command = message.charAt(0);
        uartData.data = message.substring(1);
        
        // Send to MQTT task
        if (xQueueSend(uartQueue, &uartData, pdMS_TO_TICKS(100)) != pdTRUE) {
          Serial.println("Failed to send data to queue");
        }
      }
    }
    
    vTaskDelay(pdMS_TO_TICKS(10)); // Small delay to prevent watchdog issues
  }
}

// MQTT publishing task
void mqttTask(void *parameter) {
  // Serial.println("MQTT Task started");
  UartData receivedData;
  
  while (true) {
    // Wait for WiFi connection
    if (xSemaphoreTake(wifiSemaphore, pdMS_TO_TICKS(1000)) == pdTRUE || wifiConnected) {
      
      // Check MQTT connection
      if (!client.connected()) {
        reconnectMQTT();
      }
      
      if (client.connected()) {
        client.loop();
        
        // Process UART data
        if (xQueueReceive(uartQueue, &receivedData, pdMS_TO_TICKS(100)) == pdTRUE) {
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
      // Parse program data: program_number,ic_name,param1,param2
      // Example: 1,74123,2,1
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
      // Parse device data: device_number,status,program_number,ic_number
      // Example: 1,running,2,3
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

// MQTT callback function
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);
}

// MQTT reconnection function
void reconnectMQTT() {
  while (!client.connected() && wifiConnected) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP32Client-" + MAC_Address;
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      mqttConnected = true;
      
      // Subscribe to topics if needed
      client.subscribe((data_history_topic + "/command").c_str());
      client.subscribe((Dataprogram_topic + "/command").c_str());
      client.subscribe((data_device_topic + "/command").c_str());
      
      xSemaphoreGive(mqttSemaphore);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
}