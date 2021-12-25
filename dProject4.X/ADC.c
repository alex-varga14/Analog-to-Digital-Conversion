/* 
 * File:   ADC.c
 * Author: Alex School
 *
 * Created on November 4, 2021, 2:20 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <xc.h>
#include "ADC.h"
#include "UART2.h"
#include "math.h"

//GLOBAL VARIABLES
uint8_t ADC_ARRAY[1000];
uint8_t BUFflg = 0;
uint8_t ADCflg = 0;
uint16_t ADC_1;
uint16_t ADC_2;
uint16_t ADC_3;
uint16_t ADC_4;
uint16_t ADC_5;
uint16_t ADC_6;
uint16_t ADC_7;
uint16_t ADC_8;
uint16_t ADC_9;
uint16_t ADC_10;
uint16_t ADC_11;
uint16_t ADC_12;
uint16_t ADC_13;
uint16_t ADC_14;
uint16_t ADC_15;
uint16_t ADC_16;

void ADCinit(void)
{
    AD1CON1bits.ADON =0; //Turn off ADC, turn on before sampling in do_ADC()
    AD1CON1bits.ADSIDL =0; //Continue adc operation in idle mode
    AD1CON1bits.FORM = 0b00; //0b00: Int, 01: Signed Int, 10: Fractional, 11: Signed fract
    AD1CON1bits.SSRC = 0b111; //Start ADC conversion time set my SAMC has passed
    AD1CON1bits.ASAM = 0; //Next sampling begins when SAMP bit is set
    AD1CON1bits.SAMP = 0; //set this bit in ADC routine to start sampling
    
    //Sampling time and ADC clock select
    AD1CON2bits.VCFG = 0b000; //Vref += AVDD   Vref -=AVSS
    AD1CON2bits.CSCNA = 0; //Do not scan inputs; use channel selected by CHOSA bits
    AD1CON2bits.SMPI = 0b0111; //Any ADC interrupts occur at completion of 8 sample/covert seq
    AD1CON2bits.BUFM = 1; //Buffer configured as 2 8 word buffer
    AD1CON2bits.ALTS = 0; //Always use MUXA input MUX settings
    AD1CON3bits.ADRC = 0; //0: Use system clk 1: ADC uses internal RC clock for idle or sleep mode
    AD1CON3bits.SAMC = 0b01111; //Sample time = 15*TAD = 15*1/125e3 = 0.124ms
    AD1CON3bits.ADCS = 0b000; //ADCS conversion clock select bits 64 Tcy. Ignored if using internal AD RC clock
    
    //TAD = (2/8MHz) * 64 = 0.016ms
    //SAMC = 15*0.016 ms = 0.24ms
    //1 conversion in 12 TADs : conv = 0.192 ms
    //full conversion = ~0.6 ms
    //250 Hz = 4ms period
    
    AD1CHSbits.CH0NB = 0; //CHO MUXB negative input is VR-
    AD1CHSbits.CH0SB = 0b0101; //Positive input is AN5/RA3/pin8 for MUXB
    AD1CHSbits.CH0NA = 0; //CHO MUXA negative input is VR-
    AD1CHSbits.CH0SA = 0b0101; //Positive input is AN5/RA3/pin8 for MUXA
    
    
    //IO PORT SELECTION
    TRISAbits.TRISA3 = 1; //set pin8/RA3/AN5 as input
    AD1PCFG = 0xFFFF; //set all bits as digital
    AD1PCFGbits.PCFG5 =0; //set only pin8/RA3/AN5 as analog input for ADC
    AD1CSSL = 0; //Input scan disabled, 0x0000 is default state
    
    //INT bit setup
    IPC3bits.AD1IP = 7;
    
    AD1CON1bits.ADON = 1; //Turn on ADC module
    
    return;
}


//ADC conversion subroutine
//Returns single conversion 10bit ADC value (ADCvalue) in unsigned int form

void do_ADC(void)
{
    //unsigned int ADCvalue = 0;
    double k = 0.25; //conversion factor (to fit in uint8_t array)
    double temp = 0.00;
    
    IFS0bits.AD1IF = 0; //clear ADC interrupt
    AD1CON1bits.ASAM = 1; //start auto sampling
    
    

    
    while(!IFS0bits.AD1IF){};   // conversion done?
    Delay_ms(6); //ensure 1ms has passed
    AD1CON1bits.ASAM = 0;       // if yes, stop sample/convert
    
    
    if (AD1CON2bits.BUFS == 1){
        temp = ADC1BUF0*k;
        ADC_1 = temp;
        
        temp = ADC1BUF1*k;
        ADC_2 = temp;
        
        temp = ADC1BUF2*k;
        ADC_3 = temp;
        
        temp = ADC1BUF3*k;
        ADC_4 = temp;
        
        temp = ADC1BUF4*k;
        ADC_5 = temp;
        
        temp = ADC1BUF5*k;
        ADC_6 = temp;
        
        temp = ADC1BUF6*k;
        ADC_7 = temp;
        
        temp = ADC1BUF7*k;
        ADC_8 = temp;
    }
    else{
        temp = ADC1BUF8*k;
        ADC_9 = temp;
        
        temp = ADC1BUF9*k;
        ADC_10 = temp;
        
        temp = ADC1BUFA*k;
        ADC_11 = temp;
        
        temp = ADC1BUFB*k;
        ADC_12 = temp;
        
        temp = ADC1BUFC*k;
        ADC_13 = temp;
        
        temp = ADC1BUFD*k;
        ADC_14 = temp;
        
        temp = ADC1BUFE*k;
        ADC_15 = temp;
        
        temp = ADC1BUFF*k;
        ADC_16 = temp;
    }
    
    /*
    AD1CON1bits.SAMP = 1; //start sampling, conversion starts automatically after SSRC and SAMC settings
    
    while(AD1CON1bits.DONE == 0){}
    
    ADCvalue = ADC1BUF0;
    */
    //AD1CON1bits.SAMP = 0; //stop sampling
    return;
}

//Does ADC on AN5 and displays ADCBUF0 value and proportional number of markers on Terminal
void DispADC(void)
{
    uint32_t total = 0;
    //Delay_ms(1); //wait 1 ms before sampling
    int ctr = 0;
    
    while (ctr < 1000){
        do_ADC();
        if (AD1CON2bits.BUFS == 1){
            ADC_ARRAY[ctr] = ADC_1;
            ctr++;
            ADC_ARRAY[ctr] = ADC_2;
            ctr++;
            ADC_ARRAY[ctr] = ADC_3;
            ctr++;
            ADC_ARRAY[ctr] = ADC_4;
            ctr++;
            ADC_ARRAY[ctr] = ADC_5;
            ctr++;
            ADC_ARRAY[ctr] = ADC_6;
            ctr++;
            ADC_ARRAY[ctr] = ADC_7;
            ctr++;
            ADC_ARRAY[ctr] = ADC_8;
            ctr++;
        }
        else{
            ADC_ARRAY[ctr] = ADC_9;
            ctr++;
            ADC_ARRAY[ctr] = ADC_10;
            ctr++;
            ADC_ARRAY[ctr] = ADC_11;
            ctr++;
            ADC_ARRAY[ctr] = ADC_12;
            ctr++;
            ADC_ARRAY[ctr] = ADC_13;
            ctr++;
            ADC_ARRAY[ctr] = ADC_14;
            ctr++;
            ADC_ARRAY[ctr] = ADC_15;
            ctr++;
            ADC_ARRAY[ctr] = ADC_16;
            ctr++;
        }
    }
    
    ctr = 0;
    while (ctr < 1000){
        total += ADC_ARRAY[ctr];
        ctr++;
    }
    /*
    while (i < 1000) {
        adcbuf = do_ADC(); //does ADC conversion on AN5/IO8
//        Delay_ms(5);
        total += adcbuf;
        //        Delay_ms(5);
        //Disp2Dec(adcbuf);
        i++;
    }   
    */
    
    double avg = total/1000;
 
    int numHyphens = (int)ceil(avg * (0.08)); // multiply by (20/255) to convert

    int j;
    for(j = 0; j < numHyphens; j++) {
        XmitUART2('-', 1);
    }
    Disp2Hex( (int)(avg*4) );// *4 since we had to /4 earlier to fit into uint8_t
    //XmitUART2('\n', 1);
    Disp2String("                                      ");
    XmitUART2('\r', 1);
    return;   
}

//ADC Interrupt subroutine

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
    IFS0bits.AD1IF = 0; //clear the ADC1 interrupt flag
}




