/*
 * dim_LED_PWM.c
 *This code will simply dim an LED continuously using PWM signals
 *
 *
 *  Created on: Nov 5, 2025
 *      Author: amaxw
 */

#include <string.h>
#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"
#include "stm32f334xx_pwm_driver.h"

void delay(void){

	for (int i = 0; i < 2500; i++);
}

int main (void){
/* ********************* LED HANDLE CODE ********************* */
	// Start by initializing the GPIO_Handle_t and PWM_Handle_t structs
	GPIO_Handle_t GpioLed;
	PWM_Handle_t pwm_tim2_ch1;

	memset(&pwm_tim2_ch1, 0, sizeof(pwm_tim2_ch1)); // Learn what this really does
	memset(&GpioLed, 0, sizeof(GpioLed));

	// Set pin to PA0 so GPIOA and PinNumber = 0
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_0;
	// Choose speed, mode, output type
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	GpioLed.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALT_FN_1;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GpioLed);

	// INPUT CLOCK FREQUENCY IS 8 MHz
	// Calculate what PSC should be to get 60 Hz.
	uint32_t psc_val = 79;

	uint32_t arr_val = 1666;

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

	// Start outputting PWM signal
	pwm_start(&pwm_tim2_ch1);

	int32_t brightness = 1;
	int32_t stepSize = 1;



	while(1){
		// Once max reached stepSize switches to negative and decreases brightness (dims) vice versa for when min is reached
		if ((brightness >= 1666) || (brightness <= 0)){
			stepSize = -stepSize;
		}

		brightness += stepSize;
		pwm_tim2_ch1.pTIMx->CCR1 = brightness;


		delay();
	}
}



