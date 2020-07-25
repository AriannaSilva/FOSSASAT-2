/* Project: FossaSat-2 Flight Software
   Author: Team Steiner
   Module: ads_determination
   File: solar_determination.c
   04/30/20

   This file drives the calculation of the solar ephemerides from lux and current data
*/

/***************** Headers ********************/
#include "../ADCS/adcs.h"

/*************** Main function *****************/
void ADS_Solar_Determination(const ADCS_CALC_TYPE luxData[ADCS_NUM_PANELS], ADCS_CALC_TYPE solarEph[ADCS_NUM_AXES]) {
  // Constants definitions
  ADCS_CALC_TYPE panelUnitVector[ADCS_NUM_PANELS][ADCS_NUM_AXES] = ADCS_PANEL_UNIT_VECTOR; // Unitary vector pointing to the solar panels in pseudo-inverse form

  // Maximum obtainable power/lux array
  ADCS_CALC_TYPE maxPower[ADCS_NUM_PANELS];
  maxPower[0] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_LIGHT_PANEL_Y + 2*(sizeof(float)));
  maxPower[1] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_POWER_XA) + 2*(sizeof(float));
  maxPower[2] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_POWER_ZA + 2*(sizeof(float)));
  maxPower[3] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_POWER_Y + 2*(sizeof(float)));
  maxPower[4] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_POWER_XB + 2*(sizeof(float)));
  maxPower[5] = (ADCS_CALC_TYPE)PersistentStorage_Get<float>(FLASH_STATS_POWER_ZB + 2*(sizeof(float)));

  // Main calculation
  for(uint8_t i = 0; i < ADCS_NUM_AXES; i++) {
    solarEph[i] = 0;
    for(uint8_t j = 0; j < ADCS_NUM_PANELS; j++) {
      solarEph[i] += panelUnitVector[i][j]*(luxData[j]*(1.0/maxPower[j]));
    }
  }

  // Normalize the solar ephemerides
  ADCS_CALC_TYPE solarEphNorm = ADCS_Add_Tolerance(ADCS_VectorNorm(solarEph),0);

  for(uint8_t i = 0; i < ADCS_NUM_AXES; i++) {
    solarEph *= (1.0/solarEphNorm);
  }
}
