/*
 * i2c_clock_test.c
 *
 *  Created on: Feb 26, 2026
 *      Author: amaxw
 *
 *      This file will be used to test if I successfully:
 *      (1) Get 100 kHz output on SCL of the I2C line
 *      (2) f_i2cClk = 8 MHz
 *      (3) t_SCLL = 5 us
 *      (4) t_SCLH = 4 us
 *      (5) t_SDADEL = 500ns
 *      (6) t_SCLDEL = 1250ns
 *
 *      I will start by verfiying goal (1). If this is not 100kHz I will have to analyze some other clock sources to debug why I am not getting 100 kHz
 *      Once I successfully get SCL = 100 kHz I will complete tests 2 - 6.
 *
 *      To easily verify clocking and registers thus far, I will initialize all required settings, then start a transfer to a fake slave. This way I
 *      able to read the clock frequency and validate all clocking values and ensure that all configs are being set properly before actual data transfer.
 *      I included tests for phases 0,1, and 2 which is more than clock testing. I validated my whole TX function here. WIll be making another file for further testing
 *
 */

#include <string.h> //  For memset()
#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"
#include "stm32f334xx_i2c_driver.h"

void delay(void){

	for (int i = 0; i < 250000; i++);
}


int main(){
	// PHASE 0 TEST START !!!!
	/*1. Configure GPIO pins accordingly for SCL and SDA*/
	GPIO_Handle_t GPIO_SCL;
	GPIO_Handle_t GPIO_SDA;
	I2C_Handle_t I2C1_Hanlde;

	/*memset is used to ensure the block of memory that the entire struct is stored in does not contain garbage values.
	 * If not every value in the struct is assigned a value, then memset guarantees that the value is 0 and not a random garbage value that may cause unpredicted behavior
	 * for struct properties that are not used.
	 * Use when you want a clean state for a structure
	 * ADD THIS TO C MASTER NOTES SOMEWHERE (PROB C STANDARD LIBRARY)
	 * */
	memset(&GPIO_SCL, 0, sizeof(GPIO_SCL));
	memset(&GPIO_SDA, 0, sizeof(GPIO_SDA));


	// SCL (PB6)
	GPIO_SCL.pGPIOx = GPIOB;
	GPIO_SCL.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_SCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIO_SCL.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	GPIO_SCL.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
	GPIO_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; // // Dont need pull-up because RTC module already pulls line high
	GPIO_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GPIO_SCL);


	// SDA (PB7)
	GPIO_SDA.pGPIOx = GPIOB;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
	GPIO_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; // Dont need pull-up because RTC module already pulls line high
	GPIO_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GPIO_SDA);



	/*2. Select I2C clock as HSI */
	I2C_Clock_Choice(HSI);

	/*3. Enable periperhal clock*/
	I2C_Peri_ClockControl(ENABLE);
	// PHASE 0 TEST END !!!!

	// PHASE 1 TEST START !!!!
	I2C1_Hanlde.pI2C1 = I2C1;

	// Set freq and mode then pass to I2C_clock config
	I2C1_Hanlde.I2C_PinConfig.I2C_FREQ = I2C_FREQ_8MHZ;
	I2C1_Hanlde.I2C_PinConfig.I2C_MODE = STANDARD_100K;
	I2C_Clock_Config(&I2C1_Hanlde);

	// Set analog/digital filtering and clock stretching in init() function
	I2C1_Hanlde.I2C_PinConfig.I2C_ANLG_FILTER_CTRL = ANLG_FILTER_EN;
	I2C1_Hanlde.I2C_PinConfig.I2C_DIGI_FILTER_CTRL = DIGI_FILTER_DI;
	I2C1_Hanlde.I2C_PinConfig.I2C_CLK_STRETCH = ENABLE;
	I2C_Init(&I2C1_Hanlde);

	// Set all configs required by config_master()
	I2C1_Hanlde.I2C_PinConfig.I2C_ADDR_MODE = ADDR_MODE_7BIT;
	I2C1_Hanlde.I2C_PinConfig.I2C_SLAVE_ADDR = 0x68; // Addr of I2C module
	I2C1_Hanlde.I2C_PinConfig.I2C_TRANSFER_DIR = WRITE;
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 2;
	I2C1_Hanlde.I2C_PinConfig.I2C_AUTOEND_EN = ENABLE;
//	I2C_Config_Master(&I2C1_Hanlde);

	// Enable I2C peripheral using I2C_Peri_Control(). Must com
	I2C_Peri_Control(I2C1, ENABLE);

//	while(1){
//		// Generate start bit
//		I2C_Clear_Flags(I2C1);
//		I2C_Config_Master(&I2C1_Hanlde);
//		I2C_Start(I2C1);
//		delay();
//
//	}

	// PHASE 1 TEST END !!!!


	// PHASE 2 TEST START !!!!

	/*Requirements:
	 * (1): Change slave address to: 0x68 *** DONE ***
	 * (2): Set NBYTES appropriately *** DONE ***
	 * (3): Determine what data should be and then initialize it *** DONE ***
	 * (4): Call I2C_Master_TX() then observe logic analyzer
	 * (5): Handle error states by simply using an if-else branch where one condition is I2C_OK and everything else is the errors
//	 *
	 * For test 1 data will be:
	 * Byte 1: Register to write to (= 0x07)
	 * Byte 2: Write data to said register (= 0b0001 0000) Should just enable SQW and output 1 Hz
	 *
//	 * */

	I2C_Config_Master(&I2C1_Hanlde);


	uint8_t data[2]; // Need to figure out what data I need to put here; Might change declaration
	data[0] = 0x07;
	data[1] = 0x10; // 0b 0001 0000

	I2C_Status_t status = I2C_Master_TX(&I2C1_Hanlde, data); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (status != I2C_OK){
	    while(1);
	}

	// PHASE 2 TEST END !!!!
}





