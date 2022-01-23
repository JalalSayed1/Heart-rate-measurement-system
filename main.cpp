#include "mbed.h"
//#include "platform/mbed_thread.h"
#include "TextLCD.h"

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

// Din address values from the data sheet of FRDM-KL25Z:
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

/* 
Write to the maxim via SPI
args register and the column data
*/
void write_to_max( int reg, int col){ //! reg means row (maybe)
    Load_Data = LOW;            // begin
    Din.write(reg);  // specify register
    Din.write(col);  // put data
    Load_Data = HIGH;           // make sure data is loaded (on rising edge of LOAD/CS)
}

//writes 8 bytes to the display  
// char  pattern_star[8] = { 0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04, 0x00};
void pattern_to_display(char *testdata){
    int cdata; // data to be written to one of the rows
    for(int idx = 0; idx <= 7; idx++) { // idx of the row
        cdata = testdata[idx]; 
        write_to_max(idx+1,cdata);
    }
}

void setup_dot_matrix (){
    // initiation of the max 7219
    // SPI setup: 8 bits, mode 0
    Din.format(8, 0);
  
    Din.frequency(100000); //down to 100khx easier to scope
      

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
    
    // max and min voltages we can get from the op-amps:
    double MAX_VOLTAGE = 3.3;
    double MIN_VOLTAGE = 0.0;

    // data to be written to the last column:
    // left shift all values (throwing the first elt out), update last elt to the new signal value, send to display it 
    char  data_to_write[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    
    int output_value;
    
    clear(); // to clear display before start

    while (true) {
        
        setup_dot_matrix(); //setup matrix

        // output value in percent:
        output_value = (int) signal/MAX_VOLTAGE*100

        // led_num = which led to turn on on the last column:
        char led_num;
        switch(output_value){
            // each row ~ 13%
            case 0 ... 13: led_num = 0x0 ; break; // at row 0
            case 14 ... 26: led_num = 0x1 ; break; // at row 1
            case 27 ... 39: led_num = 0x2 ; break; // at row 2
            case 40 ... 53: led_num = 0x3 ; break; // at row 3
            case 54 ... 66: led_num = 0x4 ; break; // at row 4
            case 67 ... 79: led_num = 0x5 ; break; // at row 5
            case 80 ... 92: led_num = 0x6 ; break; // at row 6
            case 93 ... 100: led_num = 0x7 ; break; // at row 7
        }
        // left shift all values:
        for(int i=1; i<8; i++){
            data_to_write[i-1] = data_to_write[i];
        }
        // then update the last element only:
        data_to_write[7] = led_num;


        // update the display:
        pattern_to_display(data_to_write);
        wait_ms(500); // ms
        
        // clear(); // to clear display

        // for(int i=0; i<8; i++){
        //     switch(output_value){
        //         // each row ~ 13%
        //         case 0 ... 13: reg = 0 ; break; // row 0
        //         case 14 ... 26: reg = 1 ; break; // row 1
        //         case 27 ... 39: reg = 2 ; break; // row 2
        //         case 40 ... 53: reg = 3 ; break; // row 3
        //         case 54 ... 66: reg = 4 ; break; // row 4
        //         case 67 ... 79: reg = 5 ; break; // row 5
        //         case 80 ... 92: reg = 6 ; break; // row 6
        //         case 93 ... 100: reg = 7 ; break; // row 7
                
        //         default:
        //             Din << "BAD VALUE";
        //             break;
                
        //     }
        //     thread_sleep_for(1000); //0.5s
        // }
    }
}