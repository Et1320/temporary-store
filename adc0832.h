#include <reg52.H>
 
 //ADC0832µÄÒý½Å
sbit ADCS =P2^2;   //ADC0832 chip seclect
sbit ADDI =P2^0;   //ADC0832 k in
sbit ADDO =P2^0;   //ADC0832 k out
sbit ADCLK =P2^1;  //ADC0832 clock signal
unsigned char Adc0832(unsigned char channel)  ;
