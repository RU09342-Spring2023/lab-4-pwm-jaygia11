/*
 *
 *
 *  Created on: April 24, 2023
 *      Author: Jay Gianduso
 *
 */

#include <msp430.h>

void gpioInit(){

    // Servo pin setup
    P2DIR |= BIT0;      // P2.0 set as output
    P2OUT &= ~BIT0;     // Clear register
    P2SEL0 |= BIT0;     // Set peripheral function
    P2SEL1 &= ~BIT0;    // Set peripheral function

    // Button 2.3 setup
    P2DIR &= ~BIT3;     // P2.3 set as input
    P2OUT |= BIT3;      // P2.3 set to 1
    P2REN |= BIT3;      // enable pull up
    P2IES &= ~BIT3;     // Low --> High edge
    P2IE |= BIT3;       // interrupt enabled

    // Button 4.1 setup
    P4DIR &= ~BIT1;     // P4.1 set to input
    P4OUT |= BIT1;      // P4.1 set to 1
    P4REN |= BIT1;      // enable pull-up
    P4IES &= ~BIT1;     // Low --> High edge
    P4IE |= BIT1;       // interrupt enabled

}


void TimerB1Setup(){

    // Set up TB3
    TB1CTL = TBSSEL_2 | MC__UP | TBCLR | TBIE;    // SMCLK | up mode | clear TBR | interrupt enabled
    TB1CCTL0 = OUTMOD_7;                          // CCR1 reset/set
    TB1CCTL1 |= CCIE;                             // Enable Interrupt
    TB1CCR0 = 20000;                              // set frequency to 50Hz
    TB1CCR1 = 1500;                               // set duty cycle to 15%

}


int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // Stop WDT


    gpioInit();
    TimerB1Setup();

    PM5CTL0 &= ~LOCKLPM5;   // Disable the GPIO power-on default high-impedance mode

    __bis_SR_register(LPM0_bits + GIE);          // Enter LPM0 w/ interrupt

}

// Left button interrupt
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{

    P4IFG &= ~BIT1;         // Clear P4.1 interrupt flag

    if(TB1CCR1 < 2000)      // Set maximum position to 5% duty cycle
        TB1CCR1 += 100;     // Increase the duty cycle by 1% to move the servo right
    else
        TB1CCR1 = TB1CCR1;  // do nothing if at maximum position

}

// Right button interrupt
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{

    P2IFG &= ~BIT3;         // Clear P2.3 interrupt flag

    if(TB1CCR1 > 1000)      // set minimum position at 5% duty cycle
        TB1CCR1 -= 100;     // Decrease the duty cycle by 1% to move the servo left
    else
        TB1CCR1 = TB1CCR1;  // do nothing if at minimum position

}

// TimerB1 interrupt
#pragma vector = TIMER3_B1_VECTOR
__interrupt void Timer3_B1_ISR(void)
{

    switch(__even_in_range(TB1IV,TB1IV_TBIFG))
    {
        case  TB1IV_TBCCR1:     // Rising edge interrupt
            P2OUT &= ~BIT0;     // Clear pin 2.0
            break;
        default:                // Falling edge interrupt
            break;
    }

}
