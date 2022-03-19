#include "mbed.h"

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

SPI max72_spi(PTD2, NC, PTD1);
DigitalOut load(PTD0); //will provide the load signal
DigitalOut gpo(PTD6);
AnalogIn Ain(PTB0);
AnalogOut Aout(PTE30);
Timer Talwayson;

unsigned int current, j, seg;
char row[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

unsigned int high = 65000;
unsigned int low = 0;

void write_to_max( int reg, int col)
{
    load = LOW;            // begin
    max72_spi.write(reg);  // specify register
    max72_spi.write(col);  // put data
    load = HIGH;           // make sure data is loaded (on rising edge of LOAD/CS)
}

//writes 8 bytes to the display  
void pattern_to_display(char *testdata){
    int cdata; 
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
     
  
  
       max72_spi.frequency(100000); //down to 100khx easier to scope ;-)
      

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

void shift(char *data) {
    gpo = 1;
    for (int j=0;j<7;j++) {
        data[j] = data[j+1];
        } 
    data[7] = 0;
    gpo = 0;
    }
    


int main()
{
    setup_dot_matrix ();      /* setup matrix */
    
    while(1){
        gpo=1;
        current=Ain.read_u16();
        seg = high/8;
        
        if (current < seg) {
            shift(row);
            row[7] = 0x1;
            pattern_to_display(row);
            }
        else if (current < (seg * 2)) {
            shift(row);
            row[7] = 0x2;
            pattern_to_display(row);
            }
        else if (current < (seg * 3)) {
            shift(row);
            row[7] = 0x4;
            pattern_to_display(row);
            }
        else if (current < (seg * 4)) {
            shift(row);
            row[7] = 0x8;
            pattern_to_display(row);
            }
        else if (current < (seg * 5)) {
            shift(row);
            row[7] = 0x10;
            pattern_to_display(row);
            }
        else if (current < (seg * 6)) {
            shift(row);
            row[7] = 0x20;
            pattern_to_display(row);
            }
        else if (current < (seg * 7)) {
            shift(row);
            row[7] = 0x40;
            pattern_to_display(row);
            }
        else if (current < (seg * 8)) {
            shift(row);
            row[7] = 0x80;
            pattern_to_display(row);
            }
        else {
            char row[] = {0, 0x7e, 0x46, 0x4a, 0x52, 0x62, 0x7e, 0x0};
            pattern_to_display(row);
            }
            
        Aout.write_u16(current);
        wait_ms(50);
    }
}