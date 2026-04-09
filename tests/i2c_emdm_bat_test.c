/*
 * i2c_emdm_bat_test.c
 *
 *  Created on: Mar 29, 2026
 *      Author: amaxw
 *
 *      Will be testing if i can read the battery life of the battery connected to the EMDM board over I2C.
 */


#include <string.h> //  For memset()
#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"
#include "stm32f334xx_i2c_driver.h"

void delay(void){

	for (int i = 0; i < 2500000; i++);
}


int main(){
	/* Configure GPIO pins accordingly for SCL and SDA*/
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
	GPIO_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // // Dont need pull-up because RTC module already pulls line high
	GPIO_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GPIO_SCL);


	// SDA (PB7)
	GPIO_SDA.pGPIOx = GPIOB;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	GPIO_SDA.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
	GPIO_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // Dont need pull-up because RTC module already pulls line high
	GPIO_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(&GPIO_SDA);



	/* Select I2C clock as HSI */
	I2C_Clock_Choice(HSI);

	/* Enable periperhal clock*/
	I2C_Peri_ClockControl(ENABLE);

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
	// TODO: MODIFY THESE FOR THE EMDM BOARD
	I2C1_Hanlde.I2C_PinConfig.I2C_ADDR_MODE = ADDR_MODE_7BIT;
	I2C1_Hanlde.I2C_PinConfig.I2C_SLAVE_ADDR = 0x34; // Addr of EMDM
	I2C1_Hanlde.I2C_PinConfig.I2C_TRANSFER_DIR = WRITE;
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 1;
	I2C1_Hanlde.I2C_PinConfig.I2C_AUTOEND_EN = ENABLE;

	// Enable I2C peripheral using I2C_Peri_Control(). Must com
	I2C_Peri_Control(I2C1, ENABLE);


	I2C_Config_Master(&I2C1_Hanlde);


	// **** Start battery read code ****

	// Allocate data for RX buffer so I can read battery life
//	uint8_t nbytes = 1;
//	uint8_t battery_lvl_addr = 0x00;
//	uint8_t rx_data[nbytes];
//
//	I2C_Status_t rx_status_1 = I2C_Master_Reg_Read(&I2C1_Hanlde, battery_lvl_addr, rx_data, nbytes);
//	if (rx_status_1 != I2C_OK){
//	    while(1);
//	}
//
	// **** End battery read code ****

	// **** Start of initial motor test code ****

	uint8_t motor_type[2] = {0x14, 3};   // register 20, value 3
	uint8_t polarity[2]   = {0x15, 0};   // register 21, value 0
//	int8_t move_backward[4] = {0x1F, -30, 0, -30, 0};
	uint8_t  move_forward[5] = {0x1F, 50, 0, 50, 0};
	uint8_t  stop[5] = {0x1F, 0, 0, 0, 0};

	I2C_Status_t tx_status;

	// Write motor type
	uint8_t repeated_start_en = 0; // set so function operates correctly
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 2;
	I2C_Config_Master(&I2C1_Hanlde);
	tx_status = I2C_Master_TX(&I2C1_Hanlde, motor_type, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (tx_status != I2C_OK){
		while(1);
	}

	delay();

	// Write polarity
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 2;
	I2C_Config_Master(&I2C1_Hanlde);
	tx_status = I2C_Master_TX(&I2C1_Hanlde, polarity, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (tx_status != I2C_OK){
		while(1);
	}

	delay();


	// Call writes for motor movement
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 5;
	I2C_Config_Master(&I2C1_Hanlde);
	tx_status = I2C_Master_TX(&I2C1_Hanlde, move_forward, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (tx_status != I2C_OK){
		while(1);
	}

	delay();
	delay();
	delay();
	delay();

	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 5;
	I2C_Config_Master(&I2C1_Hanlde);
	tx_status = I2C_Master_TX(&I2C1_Hanlde, stop, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (tx_status != I2C_OK){
		while(1);
	}
//	 **** End of initial motor test code ****

}





