/*
 * SoftwarePWMExample.c
 *
 *  Created on: Feb 18, 2023
 *      Author: Russell Trafford
 *
 *      This example controls the LED connected to Pin 1.0 by PWM. You can change
the DutyCycle Global variable to change the brightness of the LED. You should vary
this to see how the brightness can change.
 *      You can also change this in the Variables/Expressions tab in the debugger
to experiment with it as well.
 */
#include <msp430.h>
unsigned short DutyCycle = 12000;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT
    // Configure GPIO
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    // Configure Timer_A
    TB0CTL = TBSSEL_2 | MC_2 | TBCLR | TBIE;      // SMCLK, continuous mode, clear
TBR, enable interrupt
    TB0CCTL1 |= CCIE;                             // Enable TB0 CCR1 Interrupt
    TB0CCR1 = DutyCycle;                          // Set CCR1 to the value to set
the duty cycle
    __bis_SR_register(LPM3_bits | GIE);           // Enter LPM3, enable interrupts
    __no_operation();                             // For debugger
}
// Timer0_B3 Interrupt Vector (TBIV) handler
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(TB0IV,TB0IV_TBIFG))
    {
        case TB0IV_NONE:
            break;                               // No interrupt
        case TB0IV_TBCCR1:
            P1OUT &= ~BIT0;
            break;                               // CCR1 Set the pin to a 0
        case TB0IV_TBCCR2:
            break;                               // CCR2 not used
        case TB0IV_TBIFG:
            P1OUT |= BIT0;                       // overflow Set the pin to a 1
            break;
        default:
            break;
    }
