#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

INA219DataStruct returnINA219Data() {
  INA219DataStruct result;
  result.shuntvoltage = ina219.getShuntVoltage_mV();
  result.busvoltage = ina219.getBusVoltage_V();
  result.current_mA = ina219.getCurrent_mA();
  result.power_mW = ina219.getPower_mW();
  result.loadvoltage = result.busvoltage + (result.shuntvoltage / 1000);

  result.voltage_V = result.loadvoltage;
  result.current_A = result.current_mA / 1000;
  result.power_W = result.voltage_V * result.current_A;
  return result;
}

bool startINA219() {
  if (!ina219.begin()) {
    Serial.println(F("INA219 initialization failed"));
    return false;
  }

  ina219.setCalibration_32V_2A();
  return true;
}
