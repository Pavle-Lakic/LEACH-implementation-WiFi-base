/** @file base_functions.cpp
 *  @brief
 *  
 *  This file represents LEACH implementation for base station.
 *  Base station listens to UDP broadcast port for stations, and
 *  sends it to local UDP port.
 *  
 *  @author Pavle Lakic
 *  @bug For some reason base does WDT reset sometimes,
 * although yield() on each function call. If any of stations
 * send at aproximatly same UDP messages, base station might not
 * receive it.
 */

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
  yield();
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

    IPAddress local = WiFi.localIP();
    IPAddress send(local[0], local[1], local[2], 255);
    Udp.beginPacket(send, 50000);
    Udp.write(packetBuffer);
    Udp.endPacket();

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

  //client.setServer(mqtt_server, 1883);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(LOCAL_ROUTER_SSID, LOCAL_ROUTER_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

  WiFi.softAPConfig(IPAddress(apIP), IPAddress(apIP), subnet);

  WiFi.softAP(AP_SSID, AP_PASS, WIFI_CHANNEL, false, MAX_CONNECTED);
  
#if DEBUG
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println();
#endif

  Udp.begin(BROADCAST_PORT);
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);
}
void loop() {

    wait_for_packet();

}