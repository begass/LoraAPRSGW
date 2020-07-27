
#include "TempPressHum.h"

//////////////////////////////////////////////
//C Functions needed to get function pointers right for bme280.h
int fd_Temp;
float dTemp=0;
float dPressure=0;
float dHuminity=0;
struct bme280_dev dev;
int8_t rslt = BME280_OK;
char sI2C_Dev[15]="/dev/i2c-1";

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  write(fd_Temp, &reg_addr,1);
  read(fd_Temp, data, len);
  return 0;
}

void user_delay_ms(uint32_t period)
{
  usleep(period*1000);
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = (int8_t *)malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);
  write(fd_Temp, buf, len +1);
  free(buf);
  return 0;
}
//////////////////////////////////////////////



int8_t setupBME280(){
        //Test BME280
        if ((fd_Temp = open(sI2C_Dev, O_RDWR)) < 0) {
             printf("Failed to open the i2c bus %s \n", sI2C_Dev);
             return 1;
        }
        if (ioctl(fd_Temp, I2C_SLAVE, 0x76) < 0) {
            printf("Failed to acquire bus access and/or talk to slave.\n");
            return 1;
        }else{
            printf("Open i2s %s success.\n", sI2C_Dev);
        }
        dev.dev_id = BME280_I2C_ADDR_PRIM;
        dev.intf = BME280_I2C_INTF;
        //printf("Testpoint ....1\n");  
        dev.read = user_i2c_read;
        dev.write = user_i2c_write;
        dev.delay_ms = user_delay_ms;
        rslt = bme280_init(&dev);
        printf("setupBME280 finished\n");
        return 0; 
}

int8_t readBME280(){
              uint8_t settings_sel;
              struct bme280_data comp_data;

              /* Recommended mode of operation: Indoor navigation */
              dev.settings.osr_h = BME280_OVERSAMPLING_1X;
              dev.settings.osr_p = BME280_OVERSAMPLING_16X;
              dev.settings.osr_t = BME280_OVERSAMPLING_2X;
              dev.settings.filter = BME280_FILTER_COEFF_16;

              settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

              rslt = bme280_set_sensor_settings(settings_sel, &dev);
              rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
              //Wait for the measurement to complete and print data @25Hz 
              dev.delay_ms(40);
              rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);
              
              dTemp=comp_data.temperature;
              dPressure=comp_data.pressure;
              dHuminity=comp_data.humidity;
/*
              printf("Temperature, Pressure, Humidity\r\n");              
    #ifdef BME280_FLOAT_ENABLE
              //printf("temp %0.2f, p %0.2f, hum %0.2f\r\n",comp_data.temperature, comp_data.pressure, comp_data.humidity);
              printf("temp %0.2f, p %0.2f, hum %0.2f\r\n",dTemp, dPressure, dHuminity);
    #else
              printf("temp %ld, p %ld, hum %ld\r\n",comp_data.temperature, comp_data.pressure, comp_data.humidity);
    #endif
*/                
              return 0;
}

float getTemp(){
    return dTemp;
}

float getPressure(){
    return dPressure;
}
    
float getHuminity(){
    return dHuminity;
}
