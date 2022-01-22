#include "mbed.h"
//#include "platform/mbed_thread.h"
#include "TextLCD.h"
#include <thread>

/*
TREAD #1:
Get analogue input from the op-amps (for small amount of time eg. 1Hz = 1000ms)
convert it to digital (ADC)

NB:
top row of the display is the max voltage we can we from input (what is the max value?)
lowest row is the min voltage

THREAD #2:
loop over the rows of the display fast (>=50Hz) and set the correct row to GND and the correct column to +ve voltage
8 rows covers 100% => each row = 12.5%

*/

// Inputs:
// ADC - signal from op-amps
AnalogIn signal();
// Outputs:
// to display pins
//DigitalOut gpo(D0);
//DigitalOut led(LED_RED);

int main(){
    // array of 0's and 1'. 0 mean this row has not voltage supply, 1 means +ve voltage:
    int columns[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}};
    
    // counter to tell us at what column are we on. starts at 0 up to 7 then stops incrementing:
    int counter = 0;
    
    // max and min voltages we can get from the op-amps:
    double MAX_VOLTAGE = 3.3;
    double MIN_VOLTAGE = 0;
    
    int output_value;
    
    while (true) {
        
        // output value in percent:
        output_value = (int) signal/MAX_VOLTAGE*100
        
        for(int i=0; i<8; i++){
            switch(output_value){
                // each row = 13%
                case 0 ... 13: display = ; break; // row 0
                case 14 ... 26: display = ; break; // row 1
                case 27 ... 39: display = ; break; // row 2
                case 40 ... 53: display = ; break; // row 3
                case 54 ... 66: display = ; break; // row 4
                case 67 ... 79: display = ; break; // row 5
                case 80 ... 92: display = ; break; // row 6
                case 93 ... 100: display = ; break; // row 7
                
                default:
                    cout << "BAD VALUE";
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