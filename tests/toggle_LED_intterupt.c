/*
 * 005button_interrupt.c
 *
 *  Created on: Oct 30, 2025
 *      Author: amaxw
 */



#define LOW					0
#define BTN_PRESSED 		LOW

#include <string.h>
#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"

void delay(void){

	for (int i = 0; i < 250000; i++);
}



int main (void){
/* ********************* LED HANDLE CODE ********************* */
	// Start by initializing the GPIO_Handle_t structs
	GPIO_Handle_t GpioLed, GPIOBtn;
	memset(&GpioLed, 0, sizeof(GpioLed));
	memset(&GPIOBtn, 0, sizeof(GPIOBtn));

	// Set pin to PA12 so GPIOA and PinNumber = 12
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	// Choose speed, output as the mode, output type
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH;
	GpioLed.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_PP; 		// Push-Pull Configuration
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GpioLed);



/* ********************* BUTTON HANDLE CODE ********************* */
	GPIOBtn.pGPIOx = GPIOC;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	// Choose speed, output as the mode, output type
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // WORKING
//	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; WHEN USING NO PU/PD AFTER I PRESS THE BUTTON, LED FLASHES AND IS SWITCHING FROM HIGH TO LOW FOR ABOUT A SECOND.ASK GPT/READ EMBEDDED TEXTBOOK ON WHY


	GPIO_PeriClockControl(GPIOC, ENABLE);
	GPIO_Init(&GPIOBtn);

	// IRQ Config
	// Using Pin 8 so need exti 8 active
	GPIO_IRQ_priorityConfig(IRQ_NO_EXTI9_5, NVIC_IRQ_PRIO15);
	GPIO_IRQ_Interrupt_Config(IRQ_NO_EXTI9_5, ENABLE);

	while(1);


//	while(1){
//		// If button is pressed, then light should activate
//		// (1): Read input from button using GPIO_ReadFromInputPin(...)
//		if (!(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_8))){		// ! (not) needed because PC13 is default pull up so always high. I want this to activate when button pressed which causes ground connection (O/LOW)
//			// (2): If LOW, then toggle
//			delay();
//			GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_12);
//		}
//	}
}

void EXTI9_5_IRQHandler(void){
		delay();
		GPIO_IRQHandling(GPIO_PIN_NO_8);
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_12);
	}
