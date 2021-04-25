#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <PubSubClient.h>
#include <Wire.h>

/** Set this flag to 1 if you want to see debug messages.*/
#define DEBUG                       1

/** Local router connected to Internet SSID.*/
#define LOCAL_ROUTER_SSID           "It burns when IP"

/***/
#define LOCAL_ROUTER_PASSWORD       "nakacisenamoj"

/** Hive MQ server where topics will be published.*/
#define MQTT_SERVER                 "broker.mqttdashboard.com"

/** Access point SSID of base station.*/
#define AP_SSID                     "BASE_STATION"

/** Access point password of base station.*/
#define AP_PASS                     "teorijazavere"

/**WiFi channel.*/
#define WIFI_CHANNEL                1

/** Maximum possible number of connected devices to base station.*/
#define MAX_CONNECTED               7

/** Port which base station will listen for CH`s or nodes.*/
#define BROADCAST_PORT              50000

#define TOPIC                       "Message from node/s: "