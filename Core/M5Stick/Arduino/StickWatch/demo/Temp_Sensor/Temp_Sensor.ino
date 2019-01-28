
#include "rom/ets_sys.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

// https://github.com/espressif/esp-idf/blob/master/components/esp32/test/test_tsens.c

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
}

int read_temp() {
  SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 3, SENS_FORCE_XPD_SAR_S);
  SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
  CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
  CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
  SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);
  SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
  ets_delay_us(100);
  SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
  ets_delay_us(5);
  int res = GET_PERI_REG_BITS2(SENS_SAR_SLAVE_ADDR3_REG, SENS_TSENS_OUT, SENS_TSENS_OUT_S);
  printf("res=%d\n", res);
  return res;
}

void loop() {
  int temp_farenheit = read_temp();
  float temp_celsius = ( temp_farenheit - 32 ) / 1.8;
  Serial.print("Temp onBoard ");
  Serial.print(temp_farenheit);
  Serial.print("°F ");
  Serial.print(temp_celsius);
  Serial.println("°C");
  delay(2000);
}
