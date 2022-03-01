#include "mbed.h"

AnalogIn signalIn(PTB1);
AnalogOut SignalOut(PTE30);

unsigned int i;

int main(){
    i=0;
    while(1) {
        i = Ain.read_u16();
        Aout.write_u16(i);
    }
} 
