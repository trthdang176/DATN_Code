#include <WiFi.h> 
#include <Wire.h>
#include <PubSubClient.h>

#define wifi_ssid "BIN BO"
#define wifi_password "17062031809207"
#define mqtt_server "192.168.100.171"

#define test_topic "test/data"

#define RXD2 18
#define TXD2 19

WiFiClient espClient;
PubSubClient client(espClient);

uint8_t data;
long lastMsg = 0;

HardwareSerial Serial_stm32(2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial_stm32.begin(115200,SERIAL_8N1,RXD2,TXD2);

}

void setup_wifi () {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print("."); }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  // if (!client.connected()) {
  // reconnect();
  // }
  // client.loop();

  // long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   data = random(0,100);
  //   Serial.print("data: ");
  //   Serial.println(data);
  //   client.publish(test_topic, String(data).c_str(),true);
  // }

  if (Serial_stm32.available()) {
    // Read data and display it
    String message = Serial_stm32.readStringUntil('\n');
    Serial.println("Received: " + message);
  }

}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



