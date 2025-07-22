void handle_radar_sensor() {
  while (Serial1.available()) {
    RX_temp = Serial1.read();
    RX_BUF[RX_count++] = RX_temp;

    if (RX_count >= 5) {
      if (RX_BUF[0] == 0xAA && RX_BUF[RX_count - 1] == 0x55) {
        range = (RX_BUF[2] << 8) | RX_BUF[1];
        uint8_t gesture = RX_BUF[3];
        float distanceInMeters = range / 100.0;
        distance_str = String(distanceInMeters);
        memset(RX_BUF, 0x00, sizeof(RX_BUF));
        RX_count = 0;
      } else {
        if (RX_count >= sizeof(RX_BUF)) {
          memset(RX_BUF, 0x00, sizeof(RX_BUF));
          RX_count = 0;
        }
      }
    }
  }
}

void handle_kalibrasi_radar_sensor(int call) {
  Serial.println("Setting Distance Calibration...");
  setDistanceCalibration(call);
}

void sendCommand(uint8_t *command, size_t length) {
  Serial1.write(command, length);
}

void enableConfiguration() {
  uint8_t enableCommand[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
  sendCommand(enableCommand, sizeof(enableCommand));
  delay(100);
}

void endConfiguration() {
  uint8_t endCommand[] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};
  sendCommand(endCommand, sizeof(endCommand));
  delay(100);
}

void setDistanceCalibration(int32_t calibrationValue) {
  // Step 1: Enable Configuration
  enableConfiguration();
  delay(100);
  Serial.print("Enable Configuration ACK: ");
  bool enableAckDetected = false;

  // Check for Enable ACK (0xFF 0x01)
  while (Serial1.available()) {
    uint8_t byte = Serial1.read();
    if (byte == 0xFD && Serial1.read() == 0xFC) {
      enableAckDetected = true;
      Serial.printf("0x%02X ", byte);
    } else if (enableAckDetected) {
      Serial.printf("0x%02X ", byte);
      if (byte == 0x01) break;
    }
  }
  Serial.println();

  if (!enableAckDetected) {
    Serial.println("Enable Configuration ACK not received.");
    return;  // Exit if Enable ACK not received
  }

  // Step 2: Send Distance Calibration Command
  uint8_t calibrationCommand[] = {
    0xFD, 0xFC, 0xFB, 0xFA,  // Frame Header
    0x08, 0x00,              // Intra-frame Data Length (8 bytes follow)
    0x72, 0x00,              // Command Word for Distance Calibration
    0x00, 0x00,              // Distance Calibration Parameter Number
    (uint8_t)(calibrationValue & 0xFF),         // Calibration Value byte 1
    (uint8_t)((calibrationValue >> 8) & 0xFF),  // Calibration Value byte 2
    (uint8_t)((calibrationValue >> 16) & 0xFF), // Calibration Value byte 3
    (uint8_t)((calibrationValue >> 24) & 0xFF), // Calibration Value byte 4
    0x04, 0x03, 0x02, 0x01   // End of Frame
  };

  sendCommand(calibrationCommand, sizeof(calibrationCommand));
  delay(200);

  // Step 3: Read and confirm Distance Calibration Response
  Serial.print("Distance Calibration ACK: ");
  bool calibrationAckDetected = false;

  while (Serial1.available()) {
    uint8_t byte = Serial1.read();
    Serial.printf("0x%02X ", byte);
    if (byte == 0x01) {  // Confirm ACK end
      calibrationAckDetected = true;
      break;
    }
  }
  Serial.println();

  if (calibrationAckDetected) {
    Serial.println("Distance Calibration Command successful.");
  } else {
    Serial.println("Distance Calibration ACK not received.");
  }

  // Step 4: End Configuration
  endConfiguration();
  delay(100);
  Serial.println("Configuration Ended.");
  delay(1000);
  stat_call_radar = false;
}
