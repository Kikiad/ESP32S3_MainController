void setup_sd_spiffs() {
  Serial.println("[SD] Setup processing...");
  if (!SD.begin(SD_Pin)) {
    Serial.println("[SD] Setup Failed. please check your SD_Card");
    err_sd = true;
  } else {
    err_sd = false;
    Serial.println("[SD] Setup Success");
  }

  Serial.println("[SPIFFS] Setup processing...");
  if (!SPIFFS.begin(true)) {
    Serial.println("[SPIFFS] Setup Failed. Please check your ESP");
    err_spiffs = true;
  } else {
    err_spiffs = false;
    Serial.println("[SPIFFS] Setup Success");
  }

  model_chip = ESP.getChipModel();
}

void handle_logging_data() {
  if (millis() - last_log_live > 3000) {
    Serial.println("================= Logging Data Location ================");
    if (err_sd or err_spiffs) {
      handle_err_sd_spiffs();
    } else log_live_data();
    Serial.println("========================================================");
    Serial.println();
    last_log_live = millis();
  }
}

void handle_err_sd_spiffs() {
  if (err_sd) {
    SPI.begin();
    Serial.println("[SD] Setup processing...");
    if (!SD.begin(SD_Pin)) {
      Serial.println("[SD] Setup Failed. please check your SD_Card");
    } else err_sd = false;
  }

  if (err_spiffs) {
    SPI.begin();
    Serial.println("[SPIFFS] Setup processing...");
    if (!SPIFFS.begin(true)) {
      Serial.println("[SPIFFS] Setup Failed. please check your ESP");
    } else err_spiffs = false;
  }
}

void log_live_data() {
  if (!err_sd and !err_spiffs and message_json() != "null" and message_json() != "") {
    String x_file_name;
    String file_name;
    if (dateGps_str != "" and dateGps_str.length() > 1) {
      x_file_name = "/L_" + dateGps_str + "_" + model_chip + ".csv";
      file_name = read_file(SPIFFS, "/file_name.txt");
      file_name.trim();

      if (file_name != x_file_name) {
        Serial.println("Creating new File . . . .");
        if (!write_file(SPIFFS, "/file_name.txt", x_file_name.c_str())) {
          err_spiffs = true;
        }
        if (!write_file(SD, x_file_name.c_str(), "id, date, time, rdr_sen, volt_sen, lat_val, lng_val, hdg_val, alt_val")) {
          err_sd = true;
        }
      } else Serial.println("[Log] File Name Exists . . . .");

      file_name = read_file(SPIFFS, "/file_name.txt");
      file_name.trim();
      
      File file = SD.open(file_name.c_str());
      if (!file) {
        if (!write_file(SD, file_name.c_str(), "id, date, time, rdr_sen, volt_sen, lat_val, lng_val, hdg_val, alt_val")) {
          err_sd = true;
        }
      }
      file.close();

      String x_add_file = model_chip + "," + dateGps_str + "," + timeGps_str + "," + distance_str + "," +
                          v_sensor_str + "," + lat_str + "," + lng_str + "," + hdg_str + "," + alt_str;

      if (!append_file(SD, file_name.c_str(), x_add_file.c_str())) {
        err_sd = true;
      } else Serial.println("[Logging] Success");
    }
  } else Serial.println("[Log] Parameter Invalid");
}


bool write_file(fs::FS & fs, const char * path, const char * message) {
  String fs_name = "Unknown FS";

#ifdef ESP32
  if (&fs == &SPIFFS) {
    fs_name = "SPIFFS";
  } else if (&fs == &SD) {
    fs_name = "SD";
  }
#endif

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("[" + fs_name + "] Write failed.");
    file.close();
    return false;
  } else {
    file.println(message);
    Serial.println("[" + fs_name + "] Message writed.");
    file.close();
    return true;
  }
}

bool append_file(fs::FS & fs, const char * path, const char * message) {
  String fs_name = "Unknown FS";

#ifdef ESP32
  if (&fs == &SPIFFS) {
    fs_name = "SPIFFS";
  } else if (&fs == &SD) {
    fs_name = "SD";
  }
#endif

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("[" + fs_name + "] Failed to open file for appending");
    file.close();
    return false;
  } else {
    file.println(message);
    Serial.println("[" + fs_name + "] Message appended");
    file.close();
    return true;
  }
}

String read_file(fs::FS & fs, const char * path) {
  File file = fs.open(path, "r");
  if (!file || file.isDirectory()) {
    return String();
  }
  String fileContent;
  while (file.available()) {
    fileContent += String((char)file.read());
  }
  file.close();
  return fileContent;
}
