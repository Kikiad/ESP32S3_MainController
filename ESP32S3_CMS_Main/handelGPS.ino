void handle_GPS() {
  const int UTC_offset = 7;
  while (GPS_Serial.available()) {
    if (gps.encode(GPS_Serial.read())) {
      int Year = gps.date.year();
      byte Month = gps.date.month();
      byte Day = gps.date.day();
      byte Hour = gps.time.hour();
      byte Minute = gps.time.minute();
      byte Second = gps.time.second();
      setTime(Hour, Minute, Second, Day, Month, Year);
      adjustTime(UTC_offset * SECS_PER_HOUR);
    }
  }

  // -- Delete this section if not displaying time ------- //
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) {
      prevDisplay = now();
      if (gps.location.isUpdated()) {
        if (gps.location.isValid()) {
          float latitude = gps.location.lat();
          lat_str = String(latitude, 6);
          float longitude = gps.location.lng();
          lng_str = String(longitude, 6);
        }
        if (gps.altitude.isValid()) {
          float altitude = gps.altitude.meters();
          alt_str = String(altitude);
        }
        if (gps.speed.isValid()) {
          float speed = gps.speed.kmph();
          spd_str = String(speed);
        }
        if (gps.course.isValid()) {
          float heading = gps.course.deg();
          hdg_str = String(heading);
        }
        if (gps.satellites.isValid()) {
          float satelite = gps.satellites.value();
          sat_str = String(satelite);
        }
      }

      if (year() >= 2023 and gps.date.isValid() and gps.time.isValid()) {
        char datex[10];
        sprintf(datex, "%02d-%02d-%02d", year(), month(), day());
        dateGps_str = String(datex);
        char timex[10];
        sprintf(timex, "%02d:%02d:%02d", hour(), minute(), second());
        timeGps_str = String(timex);

        dateTimeGps_str = dateGps_str + " " + timeGps_str;
      }
    }
  }
}
