#include "mbed.h"
#include "platform/mbed_thread.h"
#include "stdint.h"
#define LM75_REG_TEMP (0x00) // Temperature Register
#define LM75_REG_CONF (0x01) // Configuration Register
#define LM75_ADDR     (0x90) // LM75 address

#define LM75_REG_TOS (0x03) // TOS Register
#define LM75_REG_THYST (0x02) // THYST Register

DigitalOut red(LED2);

I2C i2c(I2C_SDA, I2C_SCL);
InterruptIn lm75_int(D7);
Serial pc(SERIAL_TX, SERIAL_RX);
int16_t i16; // This variable needs to be 16 bits wide for the TOS and THYST conversion to work
char data_write[3];
char data_read[3];

float tos=28;
float thyst=26;

// Ticker ticker;
// float tick_interval = 1;
float temperature_log[60] {};

void interrupt(){
// attach an alarm signal
}

// void tick(void){
//     // Read Temperature Register
//     float temp;
//     data_write[0] = LM75_REG_TEMP;
//     i2c.write(LM75_ADDR, data_write, 1, 1); // no stop
//     i2c.read(LM75_ADDR, data_read, 2, 0);
//     // Calculate temperature value in Celcius
//     int16_t i16 = (data_read[0] << 8) | data_read[1];
//     // Read data as twos complement integer so sign is correct
//     temp = i16 / 256.0;
//     if(temperature_log[59] == NULL){ // check last position first
//         for(int i=0; i<59; i++){
//             if(temperature_log[i] == NULL){
//                 temperature_log[i] = temp;
//                 break;
//             }
//         }
//     }
//     else{
//         for(int i=0; i<59; i++){
//             temperature_log[i] = temperature_log[i+1];
//         }
//         temperature_log[59] = temp;
//     }
//}

int main()
{
    //ticker.attach(tick, tick_interval);
    // configure with comparator mode
    data_write[0] = LM75_REG_CONF;
    data_write[1] = 0x00;
    int status = i2c.write(LM75_ADDR, data_write, 2, 0);
    if(status !=0){  // If ERROR
        while(1){
            red = !red;
            wait(0.2);
        }
    }
    // SET TOS REGISTER (CHECK THIS CODE)
    data_write[0]=LM75_REG_TOS;
    i16 = (int16_t)(tos*256) & 0xFF80;
    data_write[1]=(i16 >> 8) & 0xff;
    data_write[2]=i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);
    // SET THYST REGISTER (CHECK THIS CODE) 
    data_write[0]=LM75_REG_THYST;
    i16 = (int16_t)(thyst*256) & 0xFF80;
    data_write[1]=(i16 >> 8) & 0xff;
    data_write[2]=i16 & 0xff;
    i2c.write(LM75_ADDR, data_write, 3, 0);
    // Attaches Interrupt
    lm75_int.rise(&interrupt);
    // Read temperature and create log of last minute of data
    while(1){
        float temp;
        data_write[0] = LM75_REG_TEMP;
        i2c.write(LM75_ADDR, data_write, 1, 1); // no stop
        i2c.read(LM75_ADDR, data_read, 2, 0);
        // Calculate temperature value in Celcius
        int16_t i16 = (data_read[0] << 8) | data_read[1];
        // Read data as twos complement integer so sign is correct
        temp = i16 / 256.0;
        pc.printf("Temperature = %.3f\r\n",temp);
        if(temperature_log[59] == NULL){ // check last position first
            for(int i=0; i<59; i++){
                if(temperature_log[i] == NULL){
                    temperature_log[i] = temp;
                    break;
                }
            }
        }
        else {
            for(int i=0; i<59; i++){ temperature_log[i] = temperature_log[i+1]; }
            temperature_log[59] = temp;
        }
        wait(1.0);
    }  
}


/// Task
// 1. record a temperature value every second in an array that will 
//    contain the last minute of data (older data is replaced by 
//    new data once the array is full). An timer interrupt must be 
//    used to trigger the recording of the value.
// 2. if the temperature goes above a threshold value of 28 degree 
//    Celsius, get the temperature sensor to trigger an interrupt 
//    that will get the LEDs on the microcontroller to flash a visual 
//    alarm signal (for you to imagine), stop the recording, and 
//    transmit the last minute of data to your computer by 
//    USB serial communication. Values should be transmitted as text 
//    (i.e. printed on the serial port), one value per line. 
//    The alarm signal should continue until the micro-controller is 
//    rebooted.
// 3. (Optional) optional: if you are keen, you may try to capture 
//    and plot the temperature data on your computer using a python 
//    script.

// I overcomplicated this by using a timer for my temperature readings...
// As I discovered the hard way, you're not supposed to do communication calls within an interrupt.
// ie. you can't use i2c in an interrupt
// so i rewrote code that really didn't need rewritten...
// wait no it literally says in the question so i've now reverted it for no reason...