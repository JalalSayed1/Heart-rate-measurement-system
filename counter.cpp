#include "mbed.h"
/*
example of driving maxim chip for Glasgow Uni Projects.

Dr J.J.Trinder 2013,14
jon.trinder@glasgow.ac.uk

 
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

SPI max72_spi(PTD2, NC, PTD1);
DigitalOut load(PTD0); //will provide the load signal

int i,j = 0;

char  pattern_diagonal[8] = { 0x01, 0x2,0x4,0x08,0x10,0x20,0x40,0x80};
char  pattern_square[8] = { 0xff, 0x81,0x81,0x81,0x81,0x81,0x81,0xff};  
char  pattern_star[8] = { 0x04, 0x15, 0x0e, 0x1f, 0x0e, 0x15, 0x04, 0x00};

char right_num[10][8] = {{0x7, 0x5, 0x5, 0x5, 0x7, 0, 0, 0},{0x1, 0x1, 0x1, 0x1, 0x1, 0, 0, 0},
                    {0x7, 0x1, 0x7, 0x4, 0x7, 0, 0, 0},{0x7, 0x1, 0x3, 0x1, 0x7, 0, 0, 0},
                    {0x5, 0x5, 0x7, 0x1, 0x1, 0, 0, 0},{0x7, 0x4, 0x7, 0x1, 0x7, 0, 0, 0},
                    {0x7, 0x4, 0x7, 0x5, 0x7, 0, 0, 0},{0x7, 0x1, 0x1, 0x1, 0x1, 0, 0, 0},
                    {0x7, 0x5, 0x7, 0x5, 0x7, 0, 0, 0},{0x7, 0x5, 0x7, 0x1, 0x7, 0, 0, 0}};
                    
char left_num[10][8] = {{0x70, 0x50, 0x50, 0x50, 0x70, 0, 0, 0},{0x10, 0x10, 0x10, 0x10, 0x10, 0, 0, 0},
                    {0x70, 0x10, 0x70, 0x40, 0x70, 0, 0, 0},{0x70, 0x10, 0x30, 0x10, 0x70, 0, 0, 0},
                    {0x50, 0x50, 0x70, 0x10, 0x10, 0, 0, 0},{0x70, 0x40, 0x70, 0x10, 0x70, 0, 0, 0},
                    {0x70, 0x40, 0x70, 0x50, 0x70, 0, 0, 0},{0x70, 0x10, 0x10, 0x10, 0x10, 0, 0, 0},
                    {0x70, 0x50, 0x70, 0x50, 0x70, 0, 0, 0},{0x70, 0x50, 0x70, 0x10, 0x70, 0, 0, 0}};
/*
Write to the maxim via SPI
args register and the column data
*/
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


int main()
{
    setup_dot_matrix ();      /* setup matrix */
    while(1){
    //da_star();
    for (j=0; j<=9; j++){
        for (i=0; i<=9; i++) {
            for (int k=0; k<100; k++){
                pattern_to_display(right_num[i]);
                pattern_to_display(left_num[j]);
                }
            }
        }
    clear();

    }
}