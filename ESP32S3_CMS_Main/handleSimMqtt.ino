void setup_sim_and_mqtt() {
  SerialAT.begin(115200, PROTOCOL, RXD1, TXD1);

  Serial.println(F("\nInitializing modem..."));
  String modemInfo = modem.getModemInfo();
  Serial.print(F("Modem Info: "));
  Serial.println(modemInfo);
}


void handle_sim_mqtt() {
  if (!mqtt.connected()) {
    mqtt_connect();
  } else {
    mqtt.loop();
    handle_publish_mqtt();
  }
}

boolean send_command(String command, int timeout) {
  SerialAT.println(command.c_str());
  while (millis() < timeout) {
    if (SerialAT.available() > 0) {
      String message = SerialAT.readString();
      message.trim();
      Serial.println(message);
      if (message.indexOf("OK") >= 0) return true;
      else if (message.indexOf("ERROR") >= 0) return false;
      else return false;
    }
  }
}

int get_signal_quality() {
  int rssi = modem.getSignalQuality();
  int dBm = -113 + 2 * rssi; // Convert RSSI to dBm
  return dBm;
}

void mqtt_connect(void) {
  if (!modem.gprsConnect("internet", "", "")) {
    Serial.println("Not Connect in GPRS, Check Your SIM");
    handle_no_sim();
  } else {
    mqtt.setServer((char*) broker_str.c_str(), 1883);
    mqtt.setCallback(handle_callback);

    Serial.println("Connecting to Broker MQTT");
    int count_mqtt = 0;
    while (!mqtt.connect((cn_str + sn_str).c_str(), user_mqtt.c_str(), pass_mqtt.c_str()) and count_mqtt < 3) {
      count_mqtt++;
      if (count_mqtt >= 3) {
        Serial.println("Reset SIM");
        SerialAT.println("AT+CRESET");
      }
    }

    if (!mqtt.connected()) {
      Serial.println("Failed Connect to broker");
    } else {
      err_mqtt = false;
      Serial.println("Connected to Broker");
      String sub_topic = "equipment/operated/" + sn_str;
      mqtt.subscribe(sub_topic.c_str());
      Serial.println("Subscribe Topic: " + sub_topic);
    }
  }
}

void handle_no_sim() {
  SerialAT.println("AT+CRESET");
  while (millis() < 10000) {
    if (SerialAT.available() > 0) {
      String message = SerialAT.readString();
      message.trim();
      Serial.println(message);
      if (message.indexOf("OK") >= 0) {
        if (send_command("AT+NETOPEN?", 2000)) Serial.println("Sim Ready Kembali");
      }
      else if (message.indexOf("ERROR") >= 0) Serial.println("Reset Error");
      else Serial.println("terdeteksi error");
    }
  }
}

void mqtt_pub(String topic, String message) {
  if (!mqtt.publish(topic.c_str(), message.c_str())) {
    Serial.println("MQTT Failed Publish");
  } else Serial.println("MQTT Publish Done...");
}

void handle_publish_mqtt(void) {
  if (!err_mqtt) {
    if (millis() - last_millis_publish > 3000) {
      Serial.println("============================ Publish Data MQTT  ======================");
      String payload = message_json();
      if (payload != "null") {
        mqtt_pub("testing/project/" + sn_str, payload);
      } else Serial.println("Invalid Parameter Json");
      Serial.println("======================================================================");
      last_millis_publish = millis();
    }
  }
}

void handle_callback(char* topic, byte * message, unsigned int length) {
  Serial.println("=============================== Callback ==============================");
  Serial.print("[MQTT] Message Sub: ");
  Serial.print(topic);
  Serial.print("\" : ");
  String message_callback = "";
  for (int i = 0; i < length; i++) {
    Serial.print(char(message[i]));
    message_callback += char(message[i]);
  }
  Serial.println();
  Serial.print("[MQTT] Length: ");
  Serial.println(length);
  Serial.println("======================================================================");
}
