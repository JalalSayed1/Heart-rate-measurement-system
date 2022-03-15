#include "mbed.h"
//#include "platform/mbed_thread.h"

/*
* Code breakdown:

Get analogue input from the op-amps (for small amount of time eg. 1Hz = 1000ms)
convert it to digital (ADC)

NB:
top row of the display is the max voltage we can we from input (what is the max value?)
lowest row is the min voltage

loop over the rows of the display fast (>=50Hz) and set the correct row to GND and the correct column to +ve voltage
we will always update the last column and shift old column values to the left
8 rows covers 100% => each row = 12.5%

*/

// Din address values from the data sheet of FRDM-KL25Z:
#define max7219_reg_noop 0x00
#define max7219_reg_digit0 0x01
#define max7219_reg_digit1 0x02
#define max7219_reg_digit2 0x03
#define max7219_reg_digit3 0x04
#define max7219_reg_digit4 0x05
#define max7219_reg_digit5 0x06
#define max7219_reg_digit6 0x07
#define max7219_reg_digit7 0x08
#define max7219_reg_decodeMode 0x09
#define max7219_reg_intensity 0x0a
#define max7219_reg_scanLimit 0x0b
#define max7219_reg_shutdown 0x0c
#define max7219_reg_displayTest 0x0f

#define LOW 0
#define HIGH 1

// max and min voltages we can get from the op-amps:
const double MAX_VOLTAGE = 3.3;
const double MIN_VOLTAGE = 0.0;

//* Inputs:
// ADC - signal from op-amps.
// AnalogIn PTB 0-3 and PTC 1-2:
AnalogIn Din(PTB0);

//* Outputs:
// display driver clock.
// PwmOut PTB 0-3, PTC 1-2, PTD 0-5, PTA 13, PTC 9-8, PTA 1-2 / 4-5 / 12
PwmOut pwm(PTB1);
// display driver active-low Load data pin:
DigitalOut Load_Data(PTA4);
// SCK, MISO, MOSI - Data-in line to the display diver.
// data is loaded into the internal 16-bit shift regs on CLK rising edge:
SPI Dout(PTD2, NC, PTD1);

/*
Write to the matrix via SPI
args register and the column data
*/
void write_to_max(int reg, int col)
{                     //! reg means row (maybe)
    Load_Data = LOW;  // begin
    Dout.write(reg);  // specify register
    Dout.write(col);  // put data
    Load_Data = HIGH; // make sure data is loaded (on rising edge of LOAD/CS)
}

// writes 8 bytes to the display
//  char  pattern_star[8] = { 0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04, 0x00};
void pattern_to_display(char *testdata)
{
    int cdata; // data to be written to one of the rows
    for (int idx = 0; idx <= 7; idx++)
    { // idx of the row
        cdata = testdata[idx];
        write_to_max(idx + 1, cdata);
    }
}

void setup_dot_matrix()
{
    // initiation of the max 7219
    // SPI setup: 8 bits, mode 0
    Dout.format(8, 0);

    Dout.frequency(100000); // down to 100khx easier to scope

    write_to_max(max7219_reg_scanLimit, 0x07);
    write_to_max(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
    write_to_max(max7219_reg_shutdown, 0x01);    // not in shutdown mode
    write_to_max(max7219_reg_displayTest, 0x00); // no display test
    for (int e = 1; e <= 8; e++)
    { // empty registers, turn all LEDs off
        write_to_max(e, 0);
    }
    // maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
    write_to_max(max7219_reg_intensity, 0x08);
}

void clear()
{
    for (int e = 1; e <= 8; e++)
    { // empty registers, turn all LEDs off
        write_to_max(e, 0);
    }
}

unsigned int filter(int prevSingal, int currectSignal)
{
    unsigned int outputSignal;
    int alpha = 0.2;
    // equation:
    outputSignal = alpha * currectSignal + (1 + alpha) * prevSingal;

    return outputSignal;
}

/**
 * @brief average = the sum of samples / size
 *
 * @param ptr
 * @param size
 * @return unsigned int
 */
unsigned int rollingAverage(int *ptr, int size)
{
    unsigned int average = 0;
    for (int i = 0; i < size; i++)
    {
        average += *(ptr + i);
    }
    return (unsigned int)average / size;
}

int Normalise(int signal)
{
    // char is 8 bits long:
    char result = (char)signal;
    // shift by 4 places:
    result = result >> 4;
    // force the MSB to be 0 because we only need 3 bit number as our matrix is has 7 rows.
    // int 7 = 0111 so AND bitwise operator will only set the MSB to 0 and leave the rest unchanged:
    result = result & 7;
    // return int for the sake of write_to_max as it uses int args:
    return (int)result;
}

Ticker interruptTimer;
unsigned int signal;
unsigned int prevSignal = 0;
// a row in matrix, a value from 1-7
int row;
const int numOfSamples = 5; // = array length
int samples[numOfSamples];
// (*) means give access - dereference.
// (&) means reference to variable and returns the address of that variable.
// dataPtr points to place where data can be read, will start at the second elt so we can process the signal using 2 values (first and second values):
int *dataPtr = &samples[1];
// samplePtr points to place where next data can be written:
int *samplePtr = &samples[0];
int numOfDataSampled = 0;

void performSampling()
{
    signal = Din;
    *samplePtr = signal;
    numOfDataSampled++;
    samplePtr = samplePtr + 1;
    wait_ms(500);
}

int main()
{
    // data to be written to the last column:
    // left shift all values (throwing the first elt out), update last elt to the new signal value, send to display it
    // char data_to_write[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

    // char sampleBuffer;
    // int outputValue;

    // clear(); // to clear display before start

    setup_dot_matrix();

    while (true)
    {

        interruptTimer.attach(&performSampling, 0.1);

        if (numOfDataSampled == numOfSamples)
        {
            //* process data:
            signal = *dataPtr;
            prevSignal = *(dataPtr - 1);
            // filter the signal and output it:
            signal = filter(prevSignal, signal);
            signal = rollingAverage(&samples[0], numOfSamples);
            row = Normalise(signal);

            //* update display:
            write_to_max(row, 1);

            // reset values:
            numOfDataSampled = 0;
            samplePtr = &samples[0];
        }

        // if this is the first signal we get:
        // if (prevSignal == 0)
        // {
        //     prevSignal = signal;
        //     *samplePtr = signal;
        //     samplePtr = samplePtr + 1;
        //     // we do not have enough samples so do not go any further that here:
        //     continue;
        // }

        // if (*samplePtr ==)
        // col 1 means last col as 1 = 0000 0001 in binary
        // TODO make a for loop and update the col by col by using the old samples:

        // output value in percent:
        // outputValue = (int) signal / MAX_VOLTAGE * 100;

        // led_num = which led to turn on on the last column:
        // char row;

        // switch (outputValue){
        // // each row ~ 13% of MAX_VOLTAGE
        // case 0 ... 13:
        //     row = 0x0;
        //     break; // at row 0
        // case 14 ... 26:
        //     row = 0x1;
        //     break; // at row 1
        // case 27 ... 39:
        //     row = 0x2;
        //     break; // at row 2
        // case 40 ... 53:
        //     row = 0x3;
        //     break; // at row 3
        // case 54 ... 66:
        //     row = 0x4;
        //     break; // at row 4
        // case 67 ... 79:
        //     row = 0x5;
        //     break; // at row 5
        // case 80 ... 92:
        //     row = 0x6;
        //     break; // at row 6
        // case 93 ... 100:
        //     row = 0x7;
        //     break; // at row 7
        // }
        // left shift all values:
        // for (int i = 1; i < 8; i++)
        // {
        //     data_to_write[i - 1] = data_to_write[i];
        // }
        // then update the last element only:
        // data_to_write[7] = row;

        // update the display:
        // pattern_to_display(data_to_write);
        // wait_ms(500); // ms

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