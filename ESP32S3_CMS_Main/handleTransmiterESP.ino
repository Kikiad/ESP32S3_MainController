String message_json(void) {
  if (dateTimeGps_str != "" and lat_str != "" and lat_str != 0) {
    DynamicJsonDocument buff(512);
    JsonObject message = buff.to<JsonObject>();
    message["dateTime"] = dateTimeGps_str;
    message["lat"] = atof(lat_str.c_str());
    message["lng"] = atof(lng_str.c_str());
    message["hdg"] = atof(hdg_str.c_str());
    message["alt"] = atof(alt_str.c_str());
    message["sat"] = atof(sat_str.c_str());
    message["spd"] = atof(spd_str.c_str());
    message["sv"]  = atof(v_sensor_str.c_str());
    message["sr"]  = atof(distance_str.c_str());
    String json_str = "";
    serializeJson(message, json_str);
    return json_str;
  } else return "null";
}
