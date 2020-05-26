#include "PersistentStorage.h"

uint32_t CRC32_Get(uint8_t* buff, size_t len, uint32_t initial) {
  uint32_t crc = initial;
  while (len--) {
    crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buff) & 255];
    buff++;
  }
  
  return crc;
}

bool PersistentStorage_Check_CRC(uint8_t* buff, uint32_t crcPos) {
  // check CRC of the current page
  uint32_t currCrc = 0;
  memcpy(&currCrc, buff + crcPos, sizeof(uint32_t));
  uint32_t realCrc = CRC32_Get(buff, crcPos);
  if(currCrc != realCrc) {
    // memory error happened between last check and now, increment the counter
    uint32_t errCounter = 0;
    memcpy(&errCounter, buff + crcPos + sizeof(uint32_t), sizeof(uint32_t));
    errCounter++;
    memcpy(buff + crcPos + sizeof(uint32_t), &errCounter, sizeof(uint32_t));

    // set the new CRC
    memcpy(buff + crcPos, &realCrc, sizeof(uint32_t));
    return(false);
  }
  return(true);
}

void PersistentStorage_Update_Stats(uint8_t flags) {
  // read stats page
  uint8_t statsBuffer[FLASH_EXT_PAGE_SIZE];
  PersistentStorage_Read(FLASH_STATS, statsBuffer, FLASH_EXT_PAGE_SIZE);
  
  if(flags & STATS_FLAGS_TEMPERATURES) {
    // temperatures
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_PANEL_Y, (int16_t)(Sensors_Temperature_Read(tempSensorPanelY) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_TOP, (int16_t)(Sensors_Temperature_Read(tempSensorTop) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_BOTTOM, (int16_t)(Sensors_Temperature_Read(tempSensorBottom) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_BATTERY, (int16_t)(Sensors_Temperature_Read(tempSensorBattery) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_SEC_BATTERY, (int16_t)(Sensors_Temperature_Read(tempSensorSecBattery) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_TEMP_MCU, (int16_t)(Sensors_Temperature_Read(tempSensorMCU) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER)));
  }

  if(flags & STATS_FLAGS_CURRENTS) {
    // currents
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_XA, (int16_t)(Sensors_Current_Read(currSensorXA) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_XB, (int16_t)(Sensors_Current_Read(currSensorXB) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_ZA, (int16_t)(Sensors_Current_Read(currSensorZA) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_ZB, (int16_t)(Sensors_Current_Read(currSensorZB) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_Y, (int16_t)(Sensors_Current_Read(currSensorY) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_CURR_MPPT, (int16_t)(Sensors_Current_Read(currSensorMPPT) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER)));
  }

  if(flags & STATS_FLAGS_VOLTAGES) {
    // voltages
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_XA, (uint8_t)(Sensors_Current_ReadVoltage(currSensorXA) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_XB, (uint8_t)(Sensors_Current_ReadVoltage(currSensorXB) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_ZA, (uint8_t)(Sensors_Current_ReadVoltage(currSensorZA) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_ZB, (uint8_t)(Sensors_Current_ReadVoltage(currSensorZB) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_Y, (uint8_t)(Sensors_Current_ReadVoltage(currSensorY) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_VOLT_MPPT, (uint8_t)(Sensors_Current_ReadVoltage(currSensorMPPT) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER)));
  }

  if(flags & STATS_FLAGS_LIGHT) {
    // lights
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_LIGHT_PANEL_Y, Sensors_Read_Light(lightSensorPanelY));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_LIGHT_TOP, Sensors_Read_Light(lightSensorTop));
  }

  if(flags & STATS_FLAGS_IMU) {
    // IMU
    Sensors_IMU_Update();
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_GYRO_X, imu.calcGyro(imu.gx));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_GYRO_Y, imu.calcGyro(imu.gy));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_GYRO_Z, imu.calcGyro(imu.gz));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_ACCEL_X, imu.calcAccel(imu.ax));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_ACCEL_Y, imu.calcAccel(imu.ay));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_ACCEL_Z, imu.calcAccel(imu.az));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_MAG_X, imu.calcMag(imu.mx));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_MAG_Y, imu.calcMag(imu.my));
    PersistentStorage_Update_Stat(statsBuffer, FLASH_STATS_MAG_Z, imu.calcMag(imu.mz));
  }

  // write updated page
  PersistentStorage_Write(FLASH_STATS, statsBuffer, FLASH_EXT_PAGE_SIZE);

  FOSSASAT_DEBUG_PRINTLN(F("Stats:"));
  FOSSASAT_DEBUG_PRINT_FLASH(FLASH_STATS, FLASH_EXT_PAGE_SIZE);
}

void PersistentStorage_Reset_Stats() {
  // build a completely stats page
  uint8_t statsPage[FLASH_EXT_PAGE_SIZE];

  // set everything to 0 by default
  memset(statsPage, 0, FLASH_EXT_PAGE_SIZE);

  // get minimum and maximum values for all used data types
  int16_t intMax = 0x7FFF;
  int16_t intVal = 0;
  int16_t intMin = 0xFFFF;
  uint8_t byteMax = 0xFF;
  uint8_t byteVal = 0;
  uint8_t byteMin = 0x00;
  uint32_t floatMaxBits = 0x7F7FFFFF;
  uint32_t floatMinBits = 0xFF7FFFFF;
  float floatMax = 0;
  float floatVal = 0;
  float floatMin = 0;
  memcpy(&floatMax, &floatMaxBits, sizeof(uint32_t));
  memcpy(&floatMin, &floatMinBits, sizeof(uint32_t));

  // set temperatures
  memcpy(statsPage + (FLASH_STATS_TEMP_PANEL_Y - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_TEMP_TOP - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_TEMP_BOTTOM - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_TEMP_BATTERY - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_TEMP_SEC_BATTERY - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_TEMP_MCU - FLASH_STATS), &intMax, sizeof(intMax));

  intVal = Sensors_Temperature_Read(tempSensorPanelY) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_PANEL_Y - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Temperature_Read(tempSensorTop) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_TOP - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Temperature_Read(tempSensorBottom) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_BOTTOM - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Temperature_Read(tempSensorBattery) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_BATTERY - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Temperature_Read(tempSensorSecBattery) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_SEC_BATTERY - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Temperature_Read(tempSensorMCU) * (TEMPERATURE_UNIT / TEMPERATURE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_TEMP_MCU - FLASH_STATS) + sizeof(intVal), &intMax, sizeof(intMax));
  
  memcpy(statsPage + (FLASH_STATS_TEMP_PANEL_Y - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_TEMP_TOP - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_TEMP_BOTTOM - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_TEMP_BATTERY - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_TEMP_SEC_BATTERY - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_TEMP_MCU - FLASH_STATS) + 2*sizeof(intVal), &intMax, sizeof(intMax));

  // set currents
  memcpy(statsPage + (FLASH_STATS_CURR_XA - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_CURR_XB - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_CURR_ZA - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_CURR_ZB - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_CURR_Y - FLASH_STATS), &intMax, sizeof(intMax));
  memcpy(statsPage + (FLASH_STATS_CURR_MPPT - FLASH_STATS), &intMax, sizeof(intMax));

  intVal = Sensors_Current_Read(currSensorXA) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_XA - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Current_Read(currSensorXB) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_XB - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Current_Read(currSensorZA) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_ZA - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Current_Read(currSensorZB) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_ZB - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Current_Read(currSensorY) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_Y - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  intVal = Sensors_Current_Read(currSensorMPPT) * ((CURRENT_UNIT / 1000) / CURRENT_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_MPPT - FLASH_STATS) + sizeof(intVal), &intVal, sizeof(intVal));
  
  memcpy(statsPage + (FLASH_STATS_CURR_XA - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_CURR_XB - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_CURR_ZA - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_CURR_ZB - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_CURR_Y - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));
  memcpy(statsPage + (FLASH_STATS_CURR_MPPT - FLASH_STATS) + 2*sizeof(intMin), &intMin, sizeof(intMin));

  // set voltages
  memcpy(statsPage + (FLASH_STATS_VOLT_XA - FLASH_STATS), &byteMax, sizeof(byteMax));
  memcpy(statsPage + (FLASH_STATS_VOLT_XB - FLASH_STATS), &byteMax, sizeof(byteMax));
  memcpy(statsPage + (FLASH_STATS_VOLT_ZA - FLASH_STATS), &byteMax, sizeof(byteMax));
  memcpy(statsPage + (FLASH_STATS_VOLT_ZB - FLASH_STATS), &byteMax, sizeof(byteMax));
  memcpy(statsPage + (FLASH_STATS_VOLT_Y - FLASH_STATS), &byteMax, sizeof(byteMax));
  memcpy(statsPage + (FLASH_STATS_VOLT_MPPT - FLASH_STATS), &byteMax, sizeof(byteMax));

  byteVal = Sensors_Current_ReadVoltage(currSensorXA) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_CURR_XA - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  byteVal = Sensors_Current_ReadVoltage(currSensorXB) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_VOLT_XB - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  byteVal = Sensors_Current_ReadVoltage(currSensorZA) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_VOLT_ZA - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  byteVal = Sensors_Current_ReadVoltage(currSensorZB) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_VOLT_ZB - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  byteVal = Sensors_Current_ReadVoltage(currSensorY) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_VOLT_Y - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  byteVal = Sensors_Current_ReadVoltage(currSensorMPPT) * (VOLTAGE_UNIT / VOLTAGE_MULTIPLIER);
  memcpy(statsPage + (FLASH_STATS_VOLT_Y - FLASH_STATS) + sizeof(byteVal), &byteVal, sizeof(byteVal));
  
  memcpy(statsPage + (FLASH_STATS_VOLT_XA - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));
  memcpy(statsPage + (FLASH_STATS_VOLT_XB - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));
  memcpy(statsPage + (FLASH_STATS_VOLT_ZA - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));
  memcpy(statsPage + (FLASH_STATS_VOLT_ZB - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));
  memcpy(statsPage + (FLASH_STATS_VOLT_Y - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));
  memcpy(statsPage + (FLASH_STATS_VOLT_MPPT - FLASH_STATS) + 2*sizeof(byteMin), &byteMin, sizeof(byteMin));

  // set light sensors
  memcpy(statsPage + (FLASH_STATS_LIGHT_PANEL_Y - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_LIGHT_TOP - FLASH_STATS), &floatMax, sizeof(floatMax));

  floatVal = Sensors_Read_Light(lightSensorPanelY);
  memcpy(statsPage + (FLASH_STATS_LIGHT_PANEL_Y - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = Sensors_Read_Light(lightSensorTop);
  memcpy(statsPage + (FLASH_STATS_LIGHT_TOP - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  
  memcpy(statsPage + (FLASH_STATS_LIGHT_PANEL_Y - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_LIGHT_TOP - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));

  // set IMU
  memcpy(statsPage + (FLASH_STATS_GYRO_X - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_GYRO_Y - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_GYRO_Z - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_ACCEL_X - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_ACCEL_Y - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_ACCEL_Z - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_MAG_X - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_MAG_Y - FLASH_STATS), &floatMax, sizeof(floatMax));
  memcpy(statsPage + (FLASH_STATS_MAG_Z - FLASH_STATS), &floatMax, sizeof(floatMax));
  
  Sensors_IMU_Update();
  floatVal = imu.calcGyro(imu.gx);
  memcpy(statsPage + (FLASH_STATS_GYRO_X - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcGyro(imu.gy);
  memcpy(statsPage + (FLASH_STATS_GYRO_Y - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcGyro(imu.gz);
  memcpy(statsPage + (FLASH_STATS_GYRO_Z - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcAccel(imu.ax);
  memcpy(statsPage + (FLASH_STATS_ACCEL_X - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcAccel(imu.ay);
  memcpy(statsPage + (FLASH_STATS_ACCEL_Y - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcAccel(imu.az);
  memcpy(statsPage + (FLASH_STATS_ACCEL_Z - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcMag(imu.mx);
  memcpy(statsPage + (FLASH_STATS_MAG_X - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcMag(imu.my);
  memcpy(statsPage + (FLASH_STATS_MAG_Y - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  floatVal = imu.calcMag(imu.mz);
  memcpy(statsPage + (FLASH_STATS_MAG_Z - FLASH_STATS) + sizeof(floatVal), &floatVal, sizeof(floatVal));
  
  memcpy(statsPage + (FLASH_STATS_GYRO_X - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_GYRO_Y - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_GYRO_Z - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_ACCEL_X - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_ACCEL_Y - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_ACCEL_Z - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_MAG_X - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_MAG_Y - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));
  memcpy(statsPage + (FLASH_STATS_MAG_Z - FLASH_STATS) + 2*sizeof(floatMin), &floatMin, sizeof(floatMin));

  // write all at once
  PersistentStorage_Write(FLASH_STATS, statsPage, FLASH_EXT_PAGE_SIZE);
}

void PersistentStorage_Increment_Counter(uint16_t addr) {
  uint16_t counter = 0;
  memcpy(&counter, systemInfoBuffer + addr, sizeof(uint16_t));
  counter++;
  PersistentStorage_Set(addr, counter);
}

void PersistentStorage_Increment_Frame_Counter(bool valid) {
  uint16_t addr = FLASH_LORA_VALID_COUNTER;
  if(currentModem == MODEM_LORA) {
    if(!valid) {
      addr += 2;
    }
  } else {
    if(valid) {
      addr += 4;
    } else {
      addr += 6;
    }
  }

  PersistentStorage_Increment_Counter(addr);
}

void PersistentStorage_Get_Callsign(char* buff, uint8_t len) {
  memcpy((uint8_t*)buff, systemInfoBuffer + FLASH_CALLSIGN, len);
  buff[len] = '\0';
}

void PersistentStorage_Set_Callsign(char* newCallsign) {
  // get length of the new callsign
  uint8_t newCallsignLen = (uint8_t)strlen(newCallsign);

  // check new callsign length
  if (newCallsignLen > MAX_STRING_LENGTH) {
    FOSSASAT_DEBUG_PRINTLN(F("New callsign too long!"));
    return;
  }

  // update callsign entries
  systemInfoBuffer[FLASH_CALLSIGN_LEN] = newCallsignLen;
  memcpy(systemInfoBuffer + FLASH_CALLSIGN, newCallsign, newCallsignLen);
}

uint32_t PersistentStorage_Get_Image_Len(uint8_t slot) {
  uint8_t buff[4];
  uint32_t addr = FLASH_IMAGE_LENGTHS_1;
  if(slot >= 64) {
    addr = FLASH_IMAGE_LENGTHS_2;
    slot -= 64;
  }
  PersistentStorage_Read(addr + slot*sizeof(uint32_t), buff, sizeof(uint32_t));
  uint32_t len;
  memcpy(&len, buff, sizeof(uint32_t));
  return(len);
}

void PersistentStorage_Set_Image_Len(uint8_t slot, uint32_t len) {
  // read the correct page
  uint8_t buff[FLASH_EXT_PAGE_SIZE];
  uint32_t addr = FLASH_IMAGE_LENGTHS_1;
  if(slot >= 64) {
    addr = FLASH_IMAGE_LENGTHS_2;
    slot -= 64;
  }
  PersistentStorage_Read(addr, buff, FLASH_EXT_PAGE_SIZE);
  
  // update value
  memcpy(buff + slot*sizeof(uint32_t), &len, sizeof(uint32_t));

  // write it back in (will automatically erase the sector)
  PersistentStorage_Write(addr, buff, FLASH_EXT_PAGE_SIZE);
}

void PersistentStorage_Set_Buffer(uint8_t addr, uint8_t* buff, size_t len) {
  // check address is in system info
  if(addr > (FLASH_SYSTEM_INFO_LEN - 1)) {
    return;
  }
  
  // read the current system info page
  uint8_t currSysInfoPage[FLASH_SYSTEM_INFO_LEN];
  PersistentStorage_Read(FLASH_SYSTEM_INFO_START, currSysInfoPage, FLASH_SYSTEM_INFO_LEN);

  // get current memory error counter
  uint32_t errCounter = 0;
  memcpy(&errCounter, currSysInfoPage + FLASH_MEMORY_ERROR_COUNTER, sizeof(uint32_t));

  // check CRC of the current page
  uint32_t currCrc = 0;
  memcpy(&currCrc, currSysInfoPage + FLASH_SYSTEM_INFO_CRC, sizeof(uint32_t));
  if(currCrc != CRC32_Get(currSysInfoPage, FLASH_SYSTEM_INFO_CRC)) {
    // memory error happened between last write and now, increment the counter
    FOSSASAT_DEBUG_PRINTLN(F("System info CRC check failed!"));
    errCounter++;
  }

  // check if we need to update
  uint8_t newSysInfoPage[FLASH_SYSTEM_INFO_LEN];
  memcpy(newSysInfoPage, currSysInfoPage, FLASH_SYSTEM_INFO_LEN);
  memcpy(newSysInfoPage + addr, buff, len);
  if(memcmp(currSysInfoPage, newSysInfoPage, FLASH_SYSTEM_INFO_LEN) == 0) {
    // the value is already there, no need to write
    return;
  }

  // update CRC
  uint32_t crc = CRC32_Get(newSysInfoPage, FLASH_SYSTEM_INFO_CRC);
  memcpy(newSysInfoPage + FLASH_SYSTEM_INFO_CRC, &crc, sizeof(uint32_t));

  // update memory error counter
  memcpy(newSysInfoPage + FLASH_MEMORY_ERROR_COUNTER, &errCounter, sizeof(uint32_t));

  // we need to update
  PersistentStorage_Write(FLASH_SYSTEM_INFO_START, newSysInfoPage, FLASH_SYSTEM_INFO_LEN);
}

void PersistentStorage_Reset_System_Info() {
  // set everything to 0 by default
  memset(systemInfoBuffer, 0, FLASH_SYSTEM_INFO_LEN);

  // set non-zero defaults

  // set default transmission configuration
  systemInfoBuffer[FLASH_TRANSMISSIONS_ENABLED] = 1;

  // set default callsign length
  systemInfoBuffer[FLASH_CALLSIGN_LEN] = strlen(CALLSIGN_DEFAULT);

  // set default callsign
  memcpy(systemInfoBuffer + FLASH_CALLSIGN, CALLSIGN_DEFAULT, strlen(CALLSIGN_DEFAULT));

  // set default receive windows
  systemInfoBuffer[FLASH_FSK_RECEIVE_LEN] = FSK_RECEIVE_WINDOW_LENGTH;
  systemInfoBuffer[FLASH_LORA_RECEIVE_LEN] = LORA_RECEIVE_WINDOW_LENGTH;

  // set default RTC epoch
  rtc.setDate(RTC_WEEKDAY, RTC_DAY, RTC_MONTH, RTC_YEAR);
  rtc.setTime(RTC_HOURS, RTC_MINUTES, RTC_SECONDS);
  systemInfoBuffer[FLASH_RTC_EPOCH] = rtc.getEpoch();

  // set default low power mode configuration
  systemInfoBuffer[FLASH_LOW_POWER_MODE_ENABLED] = 1;

  // set default voltage limits
  int16_t voltageLimit = DEPLOYMENT_BATTERY_VOLTAGE_LIMIT;
  memcpy(systemInfoBuffer + FLASH_DEPLOYMENT_BATTERY_VOLTAGE_LIMIT, &voltageLimit, sizeof(int16_t));
  voltageLimit = HEATER_BATTERY_VOLTAGE_LIMIT;
  memcpy(systemInfoBuffer + FLASH_HEATER_BATTERY_VOLTAGE_LIMIT, &voltageLimit, sizeof(int16_t));
  voltageLimit = BATTERY_CW_BEEP_VOLTAGE_LIMIT;
  memcpy(systemInfoBuffer + FLASH_BATTERY_CW_BEEP_VOLTAGE_LIMIT, &voltageLimit, sizeof(int16_t));
  voltageLimit = LOW_POWER_MODE_VOLTAGE_LIMIT;
  memcpy(systemInfoBuffer + FLASH_LOW_POWER_MODE_VOLTAGE_LIMIT, &voltageLimit, sizeof(int16_t));

  // set default temperature limits
  float tempLimit = BATTERY_HEATER_TEMP_LIMIT;
  memcpy(systemInfoBuffer + FLASH_BATTERY_HEATER_TEMP_LIMIT, &tempLimit, sizeof(float));
  tempLimit = MPPT_TEMP_LIMIT;
  memcpy(systemInfoBuffer + FLASH_MPPT_TEMP_LIMIT, &tempLimit, sizeof(float));

  // set default heater duty cycle
  uint8_t dutyCycle = BATTERY_HEATER_DUTY_CYCLE;
  memcpy(systemInfoBuffer + FLASH_BATTERY_HEATER_DUTY_CYCLE, &dutyCycle, sizeof(uint8_t));

  // set default MPPT temperature switch mode
  systemInfoBuffer[FLASH_MPPT_TEMP_SWITCH_ENABLED] = 1;

  // set default statistics transmission
  systemInfoBuffer[FLASH_AUTO_STATISTICS] = 1;

  // set default TLE
  uint8_t b = Navigation_Get_EpochYear(TLE_LINE_1);
  memcpy(systemInfoBuffer + FLASH_TLE_EPOCH_YEAR, &b, sizeof(uint8_t));
  double d = Navigation_Get_EpochDay(TLE_LINE_1);
  memcpy(systemInfoBuffer + FLASH_TLE_EPOCH_DAY, &d, sizeof(double));
  d = Navigation_Get_BallisticCoeff(TLE_LINE_1);
  memcpy(systemInfoBuffer + FLASH_TLE_BALLISTIC_COEFF, &d, sizeof(double));
  d = Navigation_Get_MeanMotion2nd(TLE_LINE_1);
  memcpy(systemInfoBuffer + FLASH_TLE_MEAN_MOTION_2ND, &d, sizeof(double));
  d = Navigation_Get_DragTerm(TLE_LINE_1);
  memcpy(systemInfoBuffer + FLASH_TLE_DRAG_TERM, &d, sizeof(double));
  d = Navigation_Get_Inclination(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_INCLINATION, &d, sizeof(double));
  d = Navigation_Get_RightAscension(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_RIGHT_ASCENTION, &d, sizeof(double));
  d = Navigation_Get_Eccentricity(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_ECCENTRICITY, &d, sizeof(double));
  d = Navigation_Get_PerigeeArgument(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_PERIGEE_ARGUMENT, &d, sizeof(double));
  d = Navigation_Get_MeanAnomaly(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_MEAN_ANOMALY, &d, sizeof(double));
  d = Navigation_Get_MeanMotion(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_MEAN_MOTION, &d, sizeof(double));
  uint32_t ul = Navigation_Get_RevolutionNumber(TLE_LINE_2);
  memcpy(systemInfoBuffer + FLASH_TLE_REVOLUTION_NUMBER, &ul, sizeof(uint32_t));

  // set default latest NMEA address
  uint32_t lastNmea = FLASH_NMEA_LOG_START;
  memcpy(systemInfoBuffer + FLASH_NMEA_LOG_LATEST_ENTRY, &lastNmea, sizeof(uint32_t));

  // set default latest NMEA fix
  uint32_t lastNmeaFix = 0;
  memcpy(systemInfoBuffer + FLASH_NMEA_LOG_LATEST_FIX, &lastNmeaFix, sizeof(uint32_t));

  // set default sleep intervals
  uint8_t numIntervals = DEFAULT_NUMBER_OF_SLEEP_INTERVALS;
  memcpy(systemInfoBuffer + FLASH_NUM_SLEEP_INTERVALS, &numIntervals, sizeof(uint8_t));
  uint8_t intervalSize = sizeof(int16_t) + sizeof(uint16_t);
  int16_t voltages[] = DEFAULT_SLEEP_INTERVAL_VOLTAGES;
  uint16_t lengths[] = DEFAULT_SLEEP_INTERVAL_LENGTHS;
  for(uint8_t i = 0; i < numIntervals; i++) {
    int16_t v = voltages[i];
    memcpy(systemInfoBuffer + FLASH_SLEEP_INTERVALS + i*intervalSize, &v, sizeof(int16_t));
    uint16_t l = lengths[i];
    memcpy(systemInfoBuffer + FLASH_SLEEP_INTERVALS + sizeof(int16_t) + i*intervalSize, &l, sizeof(uint16_t));
  }

  // set CRC
  uint32_t crc = CRC32_Get(systemInfoBuffer, FLASH_SYSTEM_INFO_CRC);
  memcpy(systemInfoBuffer + FLASH_SYSTEM_INFO_CRC, &crc, sizeof(uint32_t));

  // write the default system info
  PersistentStorage_Write(FLASH_SYSTEM_INFO_START, systemInfoBuffer, FLASH_SYSTEM_INFO_LEN);
}

uint8_t PersistentStorage_Get_Message(uint16_t slotNum, uint8_t* buff) {
  // read the message slot
  uint8_t messageBuff[MAX_STRING_LENGTH];
  PersistentStorage_Read(FLASH_STORE_AND_FORWARD_START + slotNum*MAX_STRING_LENGTH, messageBuff, MAX_STRING_LENGTH);

  // get message length
  uint8_t messageLen = messageBuff[sizeof(uint32_t)];

  // copy the message without length and ID
  if(messageLen < MAX_STRING_LENGTH) {
    memcpy(buff, messageBuff + sizeof(uint32_t) + sizeof(uint8_t), messageLen);
  }

  return(messageLen);
}

void PersistentStorage_Set_Message(uint16_t slotNum, uint8_t* buff, uint8_t len) {
  // read the current sector
  uint8_t sectorBuff[FLASH_SECTOR_SIZE];
  uint32_t addr = FLASH_STORE_AND_FORWARD_START + slotNum*MAX_STRING_LENGTH;
  PersistentStorage_Read(addr & 0xFFFFF000, sectorBuff, FLASH_SECTOR_SIZE);

  // update buffer
  memcpy(sectorBuff + ((slotNum*MAX_STRING_LENGTH) & 0x00000FFF), buff, len);

  // write updated buffer
  PersistentStorage_Write(addr & 0xFFFFF000, sectorBuff, FLASH_SECTOR_SIZE);
  FOSSASAT_DEBUG_PRINT_FLASH(addr, FLASH_EXT_PAGE_SIZE);
}

void PersistentStorage_Read(uint32_t addr, uint8_t* buff, size_t len) {
  uint8_t cmdBuff[] = {MX25L51245G_CMD_READ, (uint8_t)((addr >> 24) & 0xFF), (uint8_t)((addr >> 16) & 0xFF), (uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF)};
  PersistentStorage_SPItransaction(cmdBuff, 5, false, buff, len);
}

// counter to display the number of writes to external flash
#ifdef FOSSASAT_DEBUG
uint32_t writeCtr = 0;
#endif

void PersistentStorage_Write(uint32_t addr, uint8_t* buff, size_t len, bool autoErase) {
  FOSSASAT_DEBUG_PRINT(F("Write to Flash #"));
  FOSSASAT_DEBUG_PRINT(writeCtr++);
  FOSSASAT_DEBUG_PRINT(F(", address 0x"));
  FOSSASAT_DEBUG_PRINT(addr, HEX);
  
  // erase requested sector
  if(autoErase) {
    FOSSASAT_DEBUG_PRINT(F(", with sector erase"));
    PersistentStorage_SectorErase(addr);
  }
  FOSSASAT_DEBUG_PRINTLN();

  // set WEL bit again
  PersistentStorage_WaitForWriteEnable();

  // check if all bytes are in the same page
  uint32_t addrInPage = (addr & 0xFF) + len;
  if(addrInPage <= FLASH_EXT_PAGE_SIZE) {
    // all bytes are in the same page, write it
    uint8_t cmdBuff[] = {MX25L51245G_CMD_PP, (uint8_t)((addr >> 24) & 0xFF), (uint8_t)((addr >> 16) & 0xFF), (uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF)};
    PersistentStorage_SPItransaction(cmdBuff, 5, true, buff, len);
  } else {
    // some bytes are in the following page

    // get the number of bytes in the first page
    size_t firstPageLen = FLASH_EXT_PAGE_SIZE - (addr & 0xFF);

    // write the first page
    uint32_t newAddr = addr;
    uint8_t cmdBuff[] = {MX25L51245G_CMD_PP, (uint8_t)((newAddr >> 24) & 0xFF), (uint8_t)((newAddr >> 16) & 0xFF), (uint8_t)((newAddr >> 8) & 0xFF), (uint8_t)(newAddr & 0xFF)};
    PersistentStorage_SPItransaction(cmdBuff, 5, true, buff, firstPageLen);

    // wait until page is written
    PersistentStorage_WaitForWriteInProgress();

    // set WEL bit again
    PersistentStorage_WaitForWriteEnable();

    // write the remainder
    newAddr = (addr & 0xFFFFFF00) + FLASH_EXT_PAGE_SIZE;
    cmdBuff[1] = (uint8_t)((newAddr >> 24) & 0xFF);
    cmdBuff[2] = (uint8_t)((newAddr >> 16) & 0xFF);
    cmdBuff[3] = (uint8_t)((newAddr >> 8) & 0xFF);
    cmdBuff[4] = (uint8_t)(newAddr & 0xFF);
    PersistentStorage_SPItransaction(cmdBuff, 5, true, buff + firstPageLen, len - firstPageLen);
  }
  
  // wait until page is written
  PersistentStorage_WaitForWriteInProgress();
}

void PersistentStorage_SectorErase(uint32_t addr) {
  // set WEL bit
  PersistentStorage_WaitForWriteEnable();

  // erase required sector
  uint8_t cmdBuf[] = {MX25L51245G_CMD_SE, (uint8_t)((addr >> 24) & 0xFF), (uint8_t)((addr >> 16) & 0xFF), (uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF)};
  PersistentStorage_SPItransaction(cmdBuf, 5, false, NULL, 0);

  // wait until sector is erased
  PersistentStorage_WaitForWriteInProgress(1000);
}

void PersistentStorage_64kBlockErase(uint32_t addr) {
  // set WEL bit
  PersistentStorage_WaitForWriteEnable();

  // erase required sector
  uint8_t cmdBuf[] = {MX25L51245G_CMD_BE, (uint8_t)((addr >> 24) & 0xFF), (uint8_t)((addr >> 16) & 0xFF), (uint8_t)((addr >> 8) & 0xFF), (uint8_t)(addr & 0xFF)};
  PersistentStorage_SPItransaction(cmdBuf, 5, false, NULL, 0);

  // wait until sector is erased
  PersistentStorage_WaitForWriteInProgress(3000);
}

void PersistentStorage_WriteEnable() {
  PersistentStorage_SPItransaction(MX25L51245G_CMD_WREN);
}

void PersistentStorage_WriteDisable() {
  PersistentStorage_SPItransaction(MX25L51245G_CMD_WRDI);
}

// cppcheck-suppress unusedFunction
uint8_t PersistentStorage_ReadManufacturerID() {
  uint8_t cmdBuf[] = {MX25L51245G_CMD_REMS, 0x00, 0x00, 0x00};
  uint8_t buf[2];
  PersistentStorage_SPItransaction(cmdBuf, 4, false, buf, 2);
  return(buf[0]);
}

uint8_t PersistentStorage_ReadStatusRegister() {
  uint8_t buf[1];
  PersistentStorage_SPItransaction(MX25L51245G_CMD_RDSR, false, buf, 1);
  return(buf[0]);
}

// cppcheck-suppress unusedFunction
uint8_t PersistentStorage_ReadConfigRegister() {
  uint8_t buf[1];
  PersistentStorage_SPItransaction(MX25L51245G_CMD_RDCR, false, buf, 1);
  return(buf[0]);
}

// cppcheck-suppress unusedFunction
uint8_t PersistentStorage_ReadSecurityRegister() {
  uint8_t buf[1];
  PersistentStorage_SPItransaction(MX25L51245G_CMD_RDSCUR, false, buf, 1);
  return(buf[0]);
}

void PersistentStorage_Enter4ByteMode() {
  PersistentStorage_SPItransaction(MX25L51245G_CMD_EN4B);
}

// cppcheck-suppress unusedFunction
void PersistentStorage_Exit4ByteMode() {
  PersistentStorage_SPItransaction(MX25L51245G_CMD_EX4B);
}

void PersistentStorage_Reset() {
  pinMode(FLASH_RESET, OUTPUT);
  digitalWrite(FLASH_RESET, LOW);
  delayMicroseconds(100);
  pinMode(FLASH_RESET, INPUT);
}

// cppcheck-suppress unusedFunction
void PersistentStorage_WriteStatusRegister(uint8_t sr, uint8_t cr) {
  uint8_t buf[] = {sr, cr};
  PersistentStorage_WaitForWriteEnable();
  PersistentStorage_SPItransaction(MX25L51245G_CMD_WRSR, true, buf, 2);
  PersistentStorage_WriteDisable();
}

bool PersistentStorage_WaitForWriteEnable(uint32_t timeout) {
  // start the timer
  uint32_t start = millis();

  // repeat until WEL bit is set
  while(!(PersistentStorage_ReadStatusRegister() & MX25L51245G_SR_WEL)) {
    PersistentStorage_WriteEnable();
    delayMicroseconds(10);

    // check timeout
    if(millis() - start >= timeout) {
      return(false);
    }
  }
  return(true);
}

bool PersistentStorage_WaitForWriteInProgress(uint32_t timeout) {
  // start the timer
  uint32_t start = millis();

  // repeat as long as WIP bit is set
  while(PersistentStorage_ReadStatusRegister() & MX25L51245G_SR_WIP) {
    delayMicroseconds(10);

    // check timeout
    if(millis() - start >= timeout) {
      return(false);
    }
  }
  return(true);
}

void PersistentStorage_SPItransaction(uint8_t cmd, bool write, uint8_t* data, size_t numBytes) {
  uint8_t cmdBuf[] = {cmd};
  PersistentStorage_SPItransaction(cmdBuf, 1, write, data, numBytes);
}

void PersistentStorage_SPItransaction(uint8_t* cmd, uint8_t cmdLen, bool write, uint8_t* data, size_t numBytes) {
  digitalWrite(FLASH_CS, LOW);
  FlashSPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));

  // send command
  for(uint8_t n = 0; n < cmdLen; n++) {
    // send byte
    FlashSPI.transfer(cmd[n]);
  }

  // send data
  if(write) {
    for(size_t n = 0; n < numBytes; n++) {
      // send byte
      FlashSPI.transfer(data[n]);
    }

  } else {
    for(size_t n = 0; n < numBytes; n++) {
      data[n] = FlashSPI.transfer(MX25L51245G_CMD_NOP);
    }
  }

  FlashSPI.endTransaction();
  digitalWrite(FLASH_CS, HIGH);
}
