/*
 * servo_test.c
 * Writing a program to turn a servo motor x degrees.
 *
 * This code will:
 * Continuously turn a servo motor 10 degrees until it is at 180 deg and then it will turn back to 0.
 *
 *
 * START BY TRYING TO JUST TURN MOTOR IN MAIN THEN DO IT IN INTERRUPT
 *
 *  Created on: Dec 1, 2025
 *      Author: amaxw
 */

#include <string.h>
#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"
#include "stm32f334xx_pwm_driver.h"

void delay(void){

	for (int i = 0; i < 250000; i++);
}

void EXTI9_5_IRQHandler(void){
		delay();
		GPIO_IRQHandling(GPIO_PIN_NO_8);
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_12);
	}

void set_servo_angle(PWM_Handle_t* pwm_timX_chX, uint8_t angle){
	if (angle > 180){
		angle = 180; // Caps at 180 so everything functions as expected
	}
	uint32_t pulse_length = 500 + ((uint32_t)angle * 2000u) / 180u;
	pwm_timX_chX->pTIMx->CCR1 = pulse_length;
}



int main (void){
/* ********************* LED HANDLE CODE ********************* */
	// Start by initializing the GPIO_Handle_t and PWM_Handle_t structs
	GPIO_Handle_t GPIOLed;
	GPIO_Handle_t GPIOPwm;
	PWM_Handle_t pwm_tim2_ch1;

	memset(&pwm_tim2_ch1, 0, sizeof(pwm_tim2_ch1)); // Learn what this really does
	memset(&GPIOLed, 0, sizeof(GPIOLed));
	memset(&GPIOPwm, 0, sizeof(GPIOPwm));

	// Set pin to PA12 so GPIOA and PinNumber = 12
	GPIOLed.pGPIOx = GPIOA;
	GPIOLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	// Choose speed, output as the mode, output type
	GPIOLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIOLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH;
	GPIOLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP; 		// Push-Pull Configuration
	GPIOLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIOLed);

	// Set pin to PA0 so GPIOA and PinNumber = 0
	GPIOPwm.pGPIOx = GPIOA;
	GPIOPwm.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	// Choose speed, mode, output type
	GPIOPwm.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIOPwm.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH;
	GPIOPwm.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	GPIOPwm.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GPIOPwm.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALT_FN_1;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIOPwm);



	// INPUT CLOCK FREQUENCY IS 8 MHz
	// Calculate what PSC should be to get 60 Hz.
//	uint32_t psc_val = 79;
//	uint32_t arr_val = 1666;

	// Calculate what PSC should be to get 400 Hz.
//	uint32_t psc_val = 79;
//	uint32_t arr_val = 249;

	// Calculate what PSC should be to get 10 KHz.
//	uint32_t psc_val = 79;
//	uint32_t arr_val = 9;

	// Calculate what PSC should be to get 50 Hz. Doing 50 Hz as this is the standard frame period for servo motors
	uint32_t psc_val = 7;
	uint32_t arr_val = 19999;



	// Configure PWM settings and set PWM (TIM2) clock
	pwm_tim2_ch1.pTIMx = TIM2;
	pwm_tim2_ch1.PWM_PinConfig.PWM_Cnt_Dir = UPCOUNTING;
	pwm_tim2_ch1.PWM_PinConfig.PWM_PSC = psc_val;
	pwm_tim2_ch1.PWM_PinConfig.PWM_ARR = arr_val;
	pwm_tim2_ch1.PWM_PinConfig.PWM_Mode = TIM_OP_MODE_1;
	pwm_tim2_ch1.PWM_PinConfig.PWM_Preload = TIM_PRELOAD_EN;
	pwm_tim2_ch1.PWM_PinConfig.PWM_Op_Pol = TIM_POL_ACT_HI;
	pwm_tim2_ch1.PWM_PinConfig.PWM_Chn_Op_Mode = TIM_CCXS_CHN_OUT;

	pwm_PeriClockControl(ENABLE, TIM_SELECT_2);
	pwm_init(&pwm_tim2_ch1);


	// ******* TODO: WILL NEED TO FIGURE OUT BEST TIME TO CALL THIS AND THE STOP ONE AS WELL ********
	pwm_start(&pwm_tim2_ch1);



	while(1){
		for (uint32_t angle = 0; angle <= 180; angle+= 10){
			set_servo_angle(&pwm_tim2_ch1, angle);
			delay();
		}

		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_12);

		for (uint32_t angle = 180; angle > 0; angle-= 10){
			set_servo_angle(&pwm_tim2_ch1, angle);
			delay();
		}

		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_12);

	}



}

//
//void EXTI9_5_IRQHandler(void){
//	// When button pressed then this interrupt is called
//		delay();
//		GPIO_IRQHandling(GPIO_PIN_NO_8);
//		// STEP 3: Call turn motor func in IRQ_Handler
//		turn_motor(deg);
//		for (uint8_t i; i++; i < 10){
//			delay();
//		}
//	}


