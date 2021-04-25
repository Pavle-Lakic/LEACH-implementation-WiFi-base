#include <Arduino.h>
#include "includes.h"

#define IP(a,b,c,d) (uint32_t)(a | (b << 8) | (c << 16) | (d << 24))

WiFiUDP Udp;
WiFiClient espClient;
PubSubClient client(espClient);
uint32_t apIP = IP(192,168,4,1);
IPAddress subnet(255,255,255,0);

const char* router_ssid = LOCAL_ROUTER_SSID;
const char* password = LOCAL_ROUTER_PASSWORD;
const char* mqtt_server = MQTT_SERVER;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1];

String clientId = "BaseStation-";
void wait_for_packet(void)
{
  int packetSize = Udp.parsePacket();

  if (packetSize) {

#if DEBUG
    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
                  packetSize,
                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
                  ESP.getFreeHeap());
#endif

    int n = Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    packetBuffer[n] = '\0';

#if DEBUG
    Serial.println("Contents:");
    Serial.println(packetBuffer);
#endif

    client.publish(TOPIC, packetBuffer);

  }
}

void reconnect(void) 
{
  while (!client.connected()) {

#if DEBUG
    Serial.print("Attempting MQTT connection...");
#endif

    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      digitalWrite(LED_BUILTIN, LOW);

#if DEBUG
      Serial.println("connected");
#endif
 
    } 
    else {

      digitalWrite(LED_BUILTIN, HIGH);

#if DEBUG
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif

      delay(1000);
    }
  }
}

void setup() {

#if DEBUG
  Serial.begin(115200);
#endif

  client.setServer(mqtt_server, 1883);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(LOCAL_ROUTER_SSID, LOCAL_ROUTER_PASSWORD);
  WiFi.softAPConfig(IPAddress(apIP), IPAddress(apIP), subnet);

  WiFi.softAP(AP_SSID, AP_PASS, WIFI_CHANNEL, false, MAX_CONNECTED);
  
#if DEBUG
  Serial.print("AP IP address: ");
  Serial.print(WiFi.softAPIP());
  Serial.println();
#endif

  Udp.begin(BROADCAST_PORT);
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    else {
        wait_for_packet();
    }
    client.loop();
}