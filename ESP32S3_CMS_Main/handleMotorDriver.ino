void setup_motor_driver(){
    motorA.motorBrake();   
    motorB.motorBrake(); 
}

void handle_driver_motor() {
  if (m1_maju) {
      motorA.motorGo(pwm_motorA);
  }

  if (m1_mundur){
      motorA.motorGo(-pwm_motorA);
  }

  if (m1_stop){
    motorA.motorStop(); 
  }

  if (m1_brake){
    motorA.motorBrake(); 
  }
  
  if (m2_maju) {
      motorB.motorGo(pwm_motorA);
  }

  if (m2_mundur){
      motorB.motorGo(-pwm_motorA);
  }

  if (m2_stop){
    motorB.motorStop(); 
  }

  if (m2_brake){
    motorB.motorBrake(); 
  }
}
