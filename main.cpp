#include "mbed.h"
//#include "platform/mbed_thread.h"
#include "TextLCD.h"

//! to del later if not used
#include <iostream>
#include <thread>


/*
* Code breakdown:

TREAD #1:
Get analogue input from the op-amps (for small amount of time eg. 1Hz = 1000ms)
convert it to digital (ADC)

NB:
top row of the display is the max voltage we can we from input (what is the max value?)
lowest row is the min voltage

THREAD #2:
loop over the rows of the display fast (>=50Hz) and set the correct row to GND and the correct column to +ve voltage
we will always update the last column and shift old column values to the left 
8 rows covers 100% => each row = 12.5%

*/


#define max7219_reg_noop         0x00
#define max7219_reg_digit0       0x01
#define max7219_reg_digit1       0x02
#define max7219_reg_digit2       0x03
#define max7219_reg_digit3       0x04
#define max7219_reg_digit4       0x05
#define max7219_reg_digit5       0x06
#define max7219_reg_digit6       0x07
#define max7219_reg_digit7       0x08
#define max7219_reg_decodeMode   0x09
#define max7219_reg_intensity    0x0a
#define max7219_reg_scanLimit    0x0b
#define max7219_reg_shutdown     0x0c
#define max7219_reg_displayTest  0x0f

#define LOW 0
#define HIGH 1

// Inputs:
// ADC - signal from op-amps
AnalogIn signal(A0);
// Outputs:
PwmOut pwm(D0); // display driver clock
DigitalOut Load_Data(D1); // display driver active-low Load data pin
// SCK, MISO, MOSI - Data in line to the display diver.
// data is loaded into the internal 16-bit shift regs on CLK rising edge
//? SPI max72_spi(PTD2, NC, PTD1); in the code example
SPI Din(D13,D12,D11); 

//DigitalOut gpo(D0);
//DigitalOut led(LED_RED);


/* 
Write to the maxim via SPI
args register and the column data
*/
void write_to_max( int reg, int col){ //! reg means row (maybe)
    load = LOW;            // begin
    max72_spi.write(reg);  // specify register
    max72_spi.write(col);  // put data
    load = HIGH;           // make sure data is loaded (on rising edge of LOAD/CS)
}

//writes 8 bytes to the display  
void pattern_to_display(char *testdata){
    int cdata; // data to be written to one of the rows
    for(int idx = 0; idx <= 7; idx++) {
        cdata = testdata[idx]; 
        write_to_max(idx+1,cdata);
    }
}

void setup_dot_matrix ()
{
    // initiation of the max 7219
    // SPI setup: 8 bits, mode 0
    max72_spi.format(8, 0);
  
    max72_spi.frequency(100000); //down to 100khx easier to scope
      

    write_to_max(max7219_reg_scanLimit, 0x07);
    write_to_max(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
    write_to_max(max7219_reg_shutdown, 0x01);    // not in shutdown mode
    write_to_max(max7219_reg_displayTest, 0x00); // no display test
    for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
    // maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
    write_to_max(max7219_reg_intensity,  0x08);     
 
}

void clear(){
     for (int e=1; e<=8; e++) {    // empty registers, turn all LEDs off
        write_to_max(e,0);
    }
}














int main(){

    // Din line must be deselected before setting up the spi pins
    Load_Data = 1;

    // .format(bits, Clock polarity and phase mode (0 - 3))
    Din.format(16,3);
    Din.frequency(50); // in hz

    Load_Data = 0; // active

    // array of 0's and 1'. 0 mean this row has not voltage supply, 1 means +ve voltage:
    // int columns[8][8] = {
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0},
    // {0,0,0,0,0,0,0,0}};
    
    
    // max and min voltages we can get from the op-amps:
    double MAX_VOLTAGE = 3.3;
    double MIN_VOLTAGE = 0.0;
    
    int output_value;
    
    while (true) {
        
        // output value in percent:
        output_value = (int) signal/MAX_VOLTAGE*100
        



        for(int i=0; i<8; i++){
            switch(output_value){
                // each row ~ 13%
                case 0 ... 13: Din = ; break; // row 0
                case 14 ... 26: Din = ; break; // row 1
                case 27 ... 39: Din = ; break; // row 2
                case 40 ... 53: Din = ; break; // row 3
                case 54 ... 66: Din = ; break; // row 4
                case 67 ... 79: Din = ; break; // row 5
                case 80 ... 92: Din = ; break; // row 6
                case 93 ... 100: Din = ; break; // row 7
                
                default:
                    Din << "BAD VALUE";
                    break;
                
            }
            thread_sleep_for(1000); //0.5s
        }
        
        //for (int i=0; i<8; i++){ // num of display rows
//            
//            }
        
        thread_sleep_for(1000);
        
        //gpo = !gpo; // toggle pin
        //led = !led; // toggle led
        //wait(0.2f);
    }
}