/*
 *
 *
 *  Created on: April 24, 2023
 *      Author: Jay Gianduso
 *
 */
#include <msp430.h>


char led_state = 0;

int red_duty = 0;
int green_duty = 999;
int blue_duty = 999;


void led_setup()
{

    // Configure output pins for the LEDs
    P6DIR |= BIT0 | BIT1 | BIT2;    // P6.0, 6.1, 6.2 set as output
    P6OUT &= ~(BIT0 | BIT1 | BIT2); // Clear register
    P6SEL0 |= BIT0 | BIT1 | BIT2;   // Select peripheral function
    P6SEL1 &= ~(BIT0 | BIT1 | BIT2);
    P6IE |= BIT0 | BIT1 | BIT2;     // Enable interrupt

}

void timer_B3_setup()
{

    // Initialize Timer B3 for PWM output
    TB3CCR0 = 1000 - 1;
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;
    TB3CCTL1 = OUTMOD_7;
    TB3CCTL2 = OUTMOD_7;
    TB3CCTL3 = OUTMOD_7;

    // Set initial duty cycles for the LEDs
    TB3CCR1 = red_duty;
    TB3CCR2 = green_duty;
    TB3CCR3 = blue_duty;

}

void timer_B0_setup()
{

    // Initialize Timer B0 for LED color cycling
    TB0CCTL0 = CCIE;
    TB0CCR0 = 1;
    TB0CTL = TBSSEL_1 | MC_2 | ID_3 | TBCLR | TBIE;

}

int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // Disable watchdog timer

    led_setup();
    timer_B3_setup();
    timer_B0_setup();

    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM0_bits | GIE); // Enable low power mode

    __no_operation();   // Do nothing

}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void)
{
    // Update the duty cycles for the LEDs based on the current state
    switch(led_state)
    {
        case 0:  // Red to orange
            red_duty = 0;
            blue_duty = 999;
            green_duty--;

            if(green_duty == 0)
                led_state = 1;
            break;

        case 1: // Orange to green
            blue_duty = 999;
            green_duty = 0;
            red_duty++;

            if(red_duty == 999)
                led_state = 2;
            break;

        case 2: // Green to cyan
            red_duty = 999;
            green_duty = 0;
            blue_duty--;

            if(blue_duty == 0)
                led_state = 3;
            break;

        case 3: // Cyan to blue
            red_duty = 999;
            blue_duty = 0;
            green_duty++;

            if(green_duty == 999)
                led_state = 0;
            break;

        default:
            red_duty = 0;                    // Red duty cycle is at 0%
            green_duty = 999;                // Green duty cycle is at 100%
            blue_duty = 999;                 // Blue duty cycle is at 100%
            led_state = 0;               // Move to first case
    }
}

#pragma vector = PORT6_VECTOR
__interrupt void Port6_ISR(void)
{

    if (P6IFG & BIT0) // If interrupt flag is set for P6.0 (RED LED)
    {

        P6IFG &= ~BIT0; // Clear interrupt flag

        if (TB3CCR1 == 0) // If the RED LED is off
            TB3CCR1 = 1000 - 1; // Turn on RED LED

        else // If the RED LED is on
            TB3CCR1 = 0; // Turn off RED LED

    }

    else if (P6IFG & BIT1) // If interrupt flag is set for P6.1 (GREEN LED)
    {

        P6IFG &= ~BIT1; // Clear interrupt flag

        if (TB3CCR2 == 0) // If the GREEN LED is off
            TB3CCR2 = 1000 - 1; // Turn on GREEN LED

        else // If the GREEN LED is on
            TB3CCR2 = 0; // Turn off GREEN LED

    }

    else if (P6IFG & BIT2) // If interrupt flag is set for P6.2 (BLUE LED)
    {

        P6IFG &= ~BIT2; // Clear interrupt flag

        if (TB3CCR3 == 0) // If the BLUE LED is off
            TB3CCR3 = 1000 - 1; // Turn on BLUE LED

        else // If the BLUE LED is on
            TB3CCR3 = 0; // Turn off BLUE LED

    }
}
