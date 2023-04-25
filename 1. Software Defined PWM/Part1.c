/*
 *
 *
 *  Created on: 4 22, 2023
 *      Author: Jay Gianduso
 *
 *
 */

#include <msp430.h>

// Initialize the LEDs
void initLEDs() {

  P1DIR |= BIT0;    // Pin 1.0 Output
  P1OUT &= ~BIT0;   // Cleared pin
  P6DIR |= BIT6;    // Pin 6.6 Output
  P6OUT &= ~BIT6;   // Cleared pin

}

//  Initialize the buttons
void initButton() {

  P4DIR &= ~BIT1;   // pin 4.1 input
  P4OUT |= BIT1;
  P4REN |= BIT1;    // pull up
  P4IE  |= BIT1;    // interrupt
  P4IES &= ~BIT1;

  P2DIR &= ~BIT3;   // pin 2.3 input
  P2OUT |= BIT3;
  P2REN |= BIT3;    // pull up
  P2IES &= ~BIT3;   // interrupt
  P2IE |= BIT3;

}

// Setup the timers for PWM
void initTimers() {

  // Initialize Timer 0
  TB0CTL = TBSSEL__SMCLK | MC__UP | TBIE;   // TimerB0 SMCLK | Up-Mode | Interrupt Enabled
  TB0CCTL1 |= CCIE; // Enable capture/compare interrupt
  TB0CCR0 = 1000;   // Sets the clock to 1KHz frequency
  TB0CCR1 = 500;    // Sets the duty cycle to 50%

  // Initialize Timer 1
  TB1CTL = TBSSEL__SMCLK | MC__UP | TBIE;   // TimerB1 SMCLK | Up-Mode | Interrupt Enabled
  TB1CCTL1 |= CCIE; // Enable capture/compare interrupt
  TB1CCR0 = 1000;   // Sets the click to 1KHz frequency
  TB1CCR1 = 500;    // Sets the duty cycle to 50%

}

void main() {

  WDTCTL = WDTPW | WDTHOLD; // Watchdog timer disable

  initLEDs();   // Initialize the LEDs
  initButton(); // Initialize the Buttons
  initTimers(); // Setup the timers for PWM

  PM5CTL0 &= ~LOCKLPM5;

  __bis_SR_register(LPM0_bits + GIE);   // Low-power mode
  __no_operation();
}

// Button 2.3 interrupt (Red LED)
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR() {

  P2IFG &= ~BIT3;   // Clear interrupt flag

  if (TB0CCR1 >= 1000)   // Check if at 100% duty cycle
    TB0CCR1 = 1;    // Set duty cycle to 0%

  else
    TB0CCR1 += 100; // Increase duty cycle by 10%

}

// Button 4.1 interrupt (Green LED)
#pragma vector=PORT4_VECTOR
__interrupt void P4_ISR() {

  P4IFG &= ~BIT1;   // Clear interrupt flag

  if (TB1CCR1 >= 1000)  // Check if at 100% duty cycle
    TB1CCR1 = 1;    // Set duty cycle to 0%
  else
    TB1CCR1 += 100; // Increase duty cycle by 10%

}

// PWM 0 interrupt
#pragma vector=TIMER0_B1_VECTOR
__interrupt void T0_ISR() {

  switch (__even_in_range(TB0IV, TB0IV_TBIFG)) {    // Check if rising or falling edge

    case TB0IV_NONE:
      break;

    case TB0IV_TBCCR1:
      P1OUT &= ~BIT0;   // Turn LED off if falling edge
      break;

    case TB0IV_TBCCR2:
      break;

    case TB0IV_TBIFG:
      P1OUT |= BIT0;    // Turn LED on if rising edge
      break;

    default:
      break;

  }

}

// PWM 1 interrupt
#pragma vector=TIMER1_B1_VECTOR
__interrupt void T1_ISR() {

  switch (__even_in_range(TB1IV, TB1IV_TBIFG)) {    // Check if rising or falling edge

    case TB1IV_NONE:
      break;

    case TB1IV_TBCCR1:
      P6OUT &= ~BIT6;   // Turn LED off if falling edge
      break;

    case TB1IV_TBCCR2:
      break;

    case TB1IV_TBIFG:
      P6OUT |= BIT6;    // Turn LED on if rising edge
      break;

    default:
      break;

  }

}
