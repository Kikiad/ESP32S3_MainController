#define TINY_GSM_MODEM_SIM7600
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// ----------------------    LTE SIM7600 Variable
#include <TinyGsmClient.h>
#include <PubSubClient.h>

#define PROTOCOL SERIAL_8N1
#define SerialAT Serial2
#define RXD1 21
#define TXD1 20

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

bool err_mqtt = false;
const String cn_str     = "HD78339";
const String sn_str     = "33724";

const String broker_str = "ppa-ba.net";
const String user_mqtt  = "device-client";
const String pass_mqtt  = "xPXgCxPtsj2DmmGz";
unsigned long last_millis_publish = 0;
unsigned long last_reconnect = 0;

// ----------------------    GPS Variable
#include <TimeLib.h>
#include <TinyGPSPlus.h>
#define SECS_PER_HOUR 3600

SoftwareSerial GPS_Serial(6, 7);
TinyGPSPlus gps;
time_t prevDisplay = 0;
String timeZone_str = "+7:00";
String dateTimeGps_str;
String dateGps_str;
String timeGps_str;
String lat_str;
String lng_str;
String alt_str;
String spd_str;
String hdg_str;
String sat_str;

// -------------------      Radar Sensor Variable
uint8_t RX_BUF[64] = {0};
uint8_t RX_count = 0;
uint8_t RX_temp;
uint16_t range;
bool stat_call_radar = false;
String distance_str = "";

// -------------------      Voltage Sensor Variable
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 s_volt;
String v_sensor_str = "";

// -------------------      Motor Driver Variable
#include <FS_MX1508.h>
#define in1 16
#define in2 15
#define in3 5
#define in4 4

MX1508 motorA(in1, in2);
MX1508 motorB(in3, in4);

int pwm_motorA = 0;
bool m1_maju = false;
bool m1_mundur = false;
bool m1_stop = false;
bool m1_brake = false;
bool m2_maju = false;
bool m2_mundur = false;
bool m2_brake = false;
bool m2_stop = false;

// --------------------       SD Card & SPIFFS variable
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"
#define SD_Pin 10
bool err_sd = false;
bool err_spiffs = false;
String model_chip = "";
unsigned long last_log_live = 0;

//  -------------------      Serial Monitor Proses(Recive & Trans)
String data_serial;
unsigned long lastMillis = 0;
bool debug_main_proces = true;

void handle_main(void *parameter) {
  for (;;) {
    handle_GPS();
    handle_voltage_sensor();
    handle_debug_print();
    handle_serial_monitor();
    handle_driver_motor();
    handle_radar_sensor();
    handle_logging_data();
  }
}

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(9600);
  Serial1.begin(256000, SERIAL_8N1, 18, 17);
  Serial.println();
  setup_motor_driver();
  setup_ina();
  setup_sd_spiffs();
  setup_sim_and_mqtt();

  xTaskCreatePinnedToCore(handle_main, "Main_Handle", 4096, NULL, 2, NULL, 1);
}

void loop() {
    handle_sim_mqtt();
}
