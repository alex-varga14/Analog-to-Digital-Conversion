/* 
 * File:   IOs.c
 * Author: Dominic Vandekerkhove, Alex Varga, Carter Fuchs 
 *
 * Created on October 7, 2021, 1:54 PM
 */
#include <xc.h>
#include <p24F16KA101.h>
#include "UART2.h"

//global variables
int ButtonPressedFlag;

//function declarations
void IOinit();
void IOcheck();
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void);

void IOinit() {
    AD1PCFG = 0xFFFF;
    
    TRISAbits.TRISA2 = 1;   //set PB1 as input
    CNPU2bits.CN30PUE = 1;  //pull up PB1
    CNEN2bits.CN30IE = 1;   //enable CN30 interrupt
    
    TRISBbits.TRISB4 = 1;   //set PB2 as input
    CNPU1bits.CN0PUE = 1;   //pull up PB2
    CNEN1bits.CN1IE = 1;   //enable CN1 interrupt
    
    TRISAbits.TRISA4 = 1;   //set PB3 as input
    CNPU1bits.CN1PUE = 1;   //pull up PB3
    CNEN1bits.CN0IE = 1;   //enable CN0 interrupt
    
    TRISBbits.TRISB8 = 0;   //set LED pin as output
    
    IPC4bits.CNIP = 5;      //set CN interrupt priority
    IFS1bits.CNIF = 0;      //clears the CN interrupt flag
    IEC1bits.CNIE = 1;      //enable the CN interrupt (general)
    
}

// PB1 pressed: 1 sec intervals ON and OFF
// PB2 pressed: 2 sec intervals ON and OFF
// PB3 pressed: 3 sec intervals ON and OFF
// two or more push buttons pressed: LED stays ON
// no PB pressed: LED stays OFF
void IOcheck() {
    if(ButtonPressedFlag) {
        int i;
        for(i = 0; i < 10; i++) {
            //Delay_ms(1000);
            DispADC();
        }
        ButtonPressedFlag = 0;
    }   
}


void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void) {
    IFS0bits.T2IF = 0; //clear T2 interrupt flag
    return;
}

void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt (void)
{
    if(IFS1bits.CNIF == 1)
    {   
        if (PORTAbits.RA2 == 0 || PORTBbits.RB4 == 0 || PORTAbits.RA4 == 0)
         ButtonPressedFlag = 1;
    }
    
    IFS1bits.CNIF = 0; // clear IF flag
    Nop();
    
    return;
}