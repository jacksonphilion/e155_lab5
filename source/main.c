/****************************************************
// main.c

Jackson Philion, Oct.23.2024, jphilion@g.hmc.edu
This is the File Header. Very descriptive.

NOTE: This code was supported by an in-class tutorial on Interrupts, written
by Professor Joshua Brake, which may be found in the class github repository
here: https://github.com/HMC-E155/tutorial-interrupts/

****************************************************/

#include "lib/main.h"


/* THIS IS SOMETHING THAT I FOUND WHICH MAY NOT MATTER::::
// Set the rising trigger on EXTI_RTSR1, bit RTx for EXTIx (EXTI_RTSR1_RT) */

////////////  CREATE GLOBAL VARIABLES  /////////////////////////////////

int direction = 0;
double counterGrab = 0;
double RPM = 0;
uint8_t A = 0;
uint8_t B = 0;


///////////////////  MAIN LOOP  ////////////////////////////////////////

int main(void){

    //:::*::::::*::::::*::::*::  INITIALIZATION  :::*::::::*::::::*::::::*::

    // Set the SYSCLK to 20 MHz through the PLL (changed the PLL function)
    configureClock();

    ////////////  Configure GPIO pins as inputs  ////////////////////////

    gpioEnable(GPIO_PORT_A);
    pinMode(A_PIN, GPIO_INPUT);
    gpioEnable(GPIO_PORT_B);
    pinMode(B_PIN, GPIO_INPUT);

    ////////////  Initialize the Counting and Frame Timer  //////////////

    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
    initTIM_micro(COUNTING_TIM);  // Initialize TIM2, gets 1micros from prescaler
    initTIM_milli(FRAME_TIM);     // Initialize TIM6, gets 1millis from prescaler

    ////////////  Configure the A and B interrupts  /////////////////////

    // Enable SYSCFG clock domain in RCC: RCC_APB2ENR SYSCFGEN [0] to 1
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    // Configure EXTICR to grab PA6 and PB4
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI6, 0b000); // Select PA6
    SYSCFG->EXTICR[2] |= _VAL2FLD(SYSCFG_EXTICR2_EXTI4, 0b001); // Select PB4
    // Enable interrupts globally
    __enable_irq();         
    // 1. Configure mask bit
    EXTI->IMR1 |= (1 << gpioPinOffset(A_PIN));
    EXTI->IMR1 |= (1 << gpioPinOffset(B_PIN));
    // 2. Enable rising edge trigger
    EXTI->RTSR1 |= (1 << gpioPinOffset(A_PIN));
    EXTI->RTSR1 |= (1 << gpioPinOffset(B_PIN));
    // 3. Enable falling edge trigger
    EXTI->FTSR1 |= (1 << gpioPinOffset(A_PIN));
    EXTI->FTSR1 |= (1 << gpioPinOffset(B_PIN));
    // 4. Turn on EXTI interrupt in NVIC_ISER
    NVIC->ISER[0] |= (1 << EXTI4_IRQn);         // Turn on EXTI4 Interrupt, in pos 10
    NVIC->ISER[0] |= (1 << EXTI9_5_IRQn);       // Turn on for EXTI9_5
    
    //:::*::::::*::::::*::::*::  END INITIALIZE  ::::*:::::*::::::*::::::*::





    //:::*::::::*::::::*::::*::  MAIN PRINT LOOP  ::::*:::::*::::::*:::::*::

    while (1) {
        delay_millis(FRAME_TIM, FRAME_MS);                      // Generate delay of FRAME_MS (in main.h)
        if (counterGrab) {RPM = 500000/counterGrab; }           // Compute Stored Counter Value into RPM
        else {RPM=0;}                                           // If counterGrab is 0, RPM gets 0
        if (!RPM) {printf("Motor is stopped, 0 RPM\n");}
        if (direction==1) {
            printf("Motor spins at: %f, AAwise\n", RPM); }      // AA dir Print Statement
        else if (direction==2) {
            printf("Motor spins at: %f, BBwise\n", RPM); }      // BB dir Print Statement
        else {
            printf("Motor has not yet started\n"); }            // Undefined Print Statement
        counterGrab = 0;                                        // Reset the counterGrab so that 0 logs next
    }
    //:::*:::::::*:::::::*:::::*::  END MAIN  ::::*::::::*:::::::*:::::::*::
}


// EXTI[9:5] Interrupt on PA6
void EXTI9_5IRQHandler(void){
    A = digitalRead(A_PIN);         // Read the new value of A
    // If an A interrupt and we have A=1 and B=1, then special:
    if ((A==1)&(B==1)) {
        counterGrab = COUNTING_TIM->CNT;   // Log the Count level
        direction = 1;                  // Log the Direction
        COUNTING_TIM->EGR |= 0b1;          // Reset the Counter
    }
}

// EXTI4 Interrupt on PB4
void EXTI4IRQHandler(void){
    B = digitalRead(B_PIN);         // Read the new value of B
    // If an B interrupt and we have A=1 and B=1, then special:
    if ((A==1)&(B==1)) {
        counterGrab = COUNTING_TIM->CNT;   // Log the Count level
        direction = 2;                  // Log the Direction
        COUNTING_TIM->EGR |= 0b1;          // Reset the Counter
    }
}

