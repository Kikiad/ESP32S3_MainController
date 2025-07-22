void handle_serial_monitor() {
  while (Serial.available()) {
    data_serial = Serial.readString();
    data_serial.trim();
    data_serial.toUpperCase();

    if (data_serial.indexOf("KALIBRASI") >= 0) {
      Serial.println("Setting Distance Calibration...");
      stat_call_radar = true;
      String call_value_str = data_serial.substring(data_serial.indexOf("KALIBRASI=") + 10);
      int call_value_int = call_value_str.toInt();
      delay(10);
      Serial.println("Kalibrasi_value: " + String(call_value_int));
      handle_kalibrasi_radar_sensor(call_value_int);
    }

    if (data_serial.indexOf("IN_PWM") >= 0) {
      Serial.println("Setting PWM....");
      String pwm_str = data_serial.substring(data_serial.indexOf("IN_PWM=") + 7);
      int pwm_int = pwm_str.toInt();
      if (pwm_int <= 255 and pwm_int >= 10) {
        pwm_motorA = pwm_int;
      } else Serial.println("Masukan Salah/Data Invalid"); Serial.println();
    }
    
    if (data_serial == "RESTART") {
      Serial.print("ESP_Restart.");
      delay(500);
      Serial.print(".");
      delay(500);
      Serial.println(".");
      delay(500);
      Serial.println(".");
      ESP.restart();
    }

    if (data_serial == "M1_MAJU") {
      Serial.println("Starting Motor Forward Rotation");
      Serial.println();
      m1_maju = true;
      m1_mundur = false;
      m1_stop = false;
      m1_brake = false;
    }

    if (data_serial == "M1_MUNDUR") {
      Serial.println("Starting Motor Reverse Rotation");
      Serial.println();
      m1_mundur = true;
      m1_maju = false;
      m1_stop = false;
      m1_brake = false;
    }

    if (data_serial == "M1_STOP") {
      Serial.println("Stop Motor Rotation");
      Serial.println();
      m1_stop = true;
      m1_maju = false;
      m1_mundur = false;
      m1_brake = false;
    }

    if (data_serial == "M1_BRAKE") {
      Serial.println("Brake Motor Rotation");
      Serial.println();
      m1_brake = true;
      m1_stop = false  ;
      m1_maju = false;
      m1_mundur = false;
    }
    
    if (data_serial == "M2_MAJU") {
      Serial.println("Starting Motor Forward Rotation");
      Serial.println();
      m2_maju = true;
      m2_mundur = false;
      m2_stop = false;
      m2_brake = false;
    }

    if (data_serial == "M2_MUNDUR") {
      Serial.println("Starting Motor Reverse Rotation");
      Serial.println();
      m2_mundur = true;
      m2_maju = false;
      m2_stop = false;
      m2_brake = false;
    }

    if (data_serial == "M2_STOP") {
      Serial.println("Stop Motor Rotation");
      Serial.println();
      m2_stop = true;
      m2_maju = false;
      m2_mundur = false;
      m2_brake = false;
    }

    if (data_serial == "M2_BRAKE") {
      Serial.println("Brake Motor Rotation");
      Serial.println();
      m2_brake = true;
      m2_stop = false  ;
      m2_maju = false;
      m2_mundur = false;
    }
  }
}

void handle_debug_print(void) {
  if (debug_main_proces) {
    if (!stat_call_radar) {
      if (millis() - lastMillis > 500) {
        lastMillis = millis();
        Serial.println("=================== Debug_Main_Process ======================");
        Serial.println("Distance_radar: " + distance_str);
        Serial.println("Datetime_GPS: " + dateTimeGps_str);
        Serial.println("Latitude_GPS: " + lat_str);
        Serial.println("Longitude_GPS: " + lng_str);
        Serial.println("Voltage_S: " + v_sensor_str);
        Serial.println("pwm_motor: " + String(pwm_motorA));
        Serial.println("json_message: " + message_json());
        Serial.println("=============================================================");
        Serial.println();
      }
    }
  }
}
