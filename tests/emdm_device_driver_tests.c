/*
 * emdm_device_driver_tests.c
 *
 *  Created on: Jun 2, 2026
 *      Author: amaxw
 *
 *  TODO: I NEED TO ADD THE FUNCTIONS GPIO_init_I2C1() AND I2C1_master_setup() to main for now and eventually a driver / API dedicated to setup of pins
 */


#include <string.h> //  For memset()

#include "stm32f334r8.h"
#include "stm32f334xx_gpio_driver.h"
#include "stm32f334xx_i2c_driver.h"
#include "emdm_device_driver.h"



void delay(void){

	for (int i = 0; i < 250000; i++);
}

/**************************************************************************************
 * @fn					- GPIO_init_I2C1
 *
 * @brief				- All this function does as of now is choose GPIO pins PB6 as SCL and PB7 as SDA and configures them for I2C communication.
 * 						  May do more in the future but as of now this is all it does
 *
 * @param[in]			-
 *
 * @return				- Pass by reference in a param so nothing needs to be returned
 *
 * @Note				- May have more functionallity later if more pins are set. Will change name to GPIO_init() if so.
 *
 **************************************************************************************/
void GPIO_init_I2C1(GPIO_Handle_t* GPIO_SCL, GPIO_Handle_t* GPIO_SDA){
//	GPIO_Handle_t GPIO_SCL;
//	GPIO_Handle_t GPIO_SDA;
//
//	memset(&GPIO_SCL, 0, sizeof(GPIO_SCL));
//	memset(&GPIO_SDA, 0, sizeof(GPIO_SDA));
	// DECLARING THESE IN MAIN

	// SCL (PB6)
	GPIO_SCL->pGPIOx = GPIOB;
	GPIO_SCL->GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_SCL->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIO_SCL->GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	GPIO_SCL->GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
	GPIO_SCL->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // // Dont need pull-up because RTC module already pulls line high
	GPIO_SCL->GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(GPIO_SCL);


	// SDA (PB7)
	GPIO_SDA->pGPIOx = GPIOB;
	GPIO_SDA->GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_SDA->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	GPIO_SDA->GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	GPIO_SDA->GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
	GPIO_SDA->GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // Dont need pull-up because RTC module already pulls line high
	GPIO_SDA->GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope

	GPIO_PeriClockControl(GPIOB, ENABLE);
	GPIO_Init(GPIO_SDA);
}

/**************************************************************************************
 * @fn					- GPIO_init_I2C1
 *
 * @brief				- Calls all functions from the I2C API that are required to activate and configure the I2C1 periperhal for master communication
 *
 * @param[in]			- ReWrDir: Chooses if the master will be reading or writing to/from slave
 *
 * @return				- ALL I2C HANDLES
 *
 * @Note				- REALLY NEED TO CONSIDER WHAT THESE FUNCTIONS (OTHER INIT ONE) SHOULD RETURN AND PARAMETERS.
 *
 **************************************************************************************/
void I2C1_master_setup(I2C_Handle_t* I2C1_Handle){
//	I2C_Handle_t I2C1_Handle;


	/* Select I2C clock as HSI */
	I2C_Clock_Choice(HSI);

	/* Enable periperhal clock*/
	I2C_Peri_ClockControl(ENABLE);

	I2C1_Handle->pI2C1 = I2C1;
	// Set freq and mode then pass to I2C_clock config
	I2C1_Handle->I2C_PinConfig.I2C_FREQ = I2C_FREQ_8MHZ;
	I2C1_Handle->I2C_PinConfig.I2C_MODE = STANDARD_100K;
	I2C_Clock_Config(I2C1_Handle);

	// Set analog/digital filtering and clock stretching in init() function
	I2C1_Handle->I2C_PinConfig.I2C_ANLG_FILTER_CTRL = ANLG_FILTER_EN;
	I2C1_Handle->I2C_PinConfig.I2C_DIGI_FILTER_CTRL = DIGI_FILTER_DI;
	I2C1_Handle->I2C_PinConfig.I2C_CLK_STRETCH = ENABLE;
	I2C_Init(I2C1_Handle);

	// Set all configs required by config_master()
	// TODO: MODIFY THESE FOR THE EMDM BOARD; MAY JUST NOT SET THESE AND SET THEM IN EMDM FUNCTIONS. DECIDE LATER !!!!!
	I2C1_Handle->I2C_PinConfig.I2C_ADDR_MODE = ADDR_MODE_7BIT;
//	I2C1_Handle->I2C_PinConfig.I2C_SLAVE_ADDR = 0x34; // Addr of EMDM (THIS IS BEING SET BY EMDM_Set_I2C_Slave() IN THE EMDM DEVICE DRIVER FILE
	I2C1_Handle->I2C_PinConfig.I2C_TRANSFER_DIR = WRITE;
	I2C1_Handle->I2C_PinConfig.I2C_NBYTES = 1;
	I2C1_Handle->I2C_PinConfig.I2C_AUTOEND_EN = ENABLE;

	// Enable I2C peripheral
	I2C_Peri_Control(I2C1, ENABLE);


	I2C_Config_Master(I2C1_Handle);
}


int main(){
	GPIO_Handle_t GPIO_SCL;
	GPIO_Handle_t GPIO_SDA;
	I2C_Handle_t I2C1_Handle;
	EMDM_RegConfig_t EMDM_Handle;


	memset(&GPIO_SCL, 0, sizeof(GPIO_SCL));
	memset(&GPIO_SDA, 0, sizeof(GPIO_SDA));
	memset(&I2C1_Handle, 0, sizeof(I2C1_Handle));
	memset(&EMDM_Handle, 0, sizeof(EMDM_Handle));

	GPIO_init_I2C1(&GPIO_SCL, &GPIO_SDA);
	I2C1_master_setup(&I2C1_Handle);

	EMDM_Set_I2C_Slave(&I2C1_Handle);

	/* START TEST 1: EMDM GET BATTERY TEST	*/
//	EMDM_Status_t emdm_status = EMDM_Get_Battery_Raw(&I2C1_Handle);
	/* END TEST 1: EMDM GET BATTERY TEST	*/

	/* START TEST 2: EMDM SET MOTOR TYPE TEST	*/
//	EMDM_Handle.EMDM_MOTOR_TYPE = MOTOR_TYPE_JGB;
//
//	// Goal: Verify EMDM_Set_Motor_Type() functions correctly and actually writes to the correct register in EMDM. Do so by reading the motor type register in EMDM
//	EMDM_Set_Motor_Type(&EMDM_Handle, &I2C1_Handle);
//
//	uint8_t reg = 0x14;
//	uint8_t nbytes = 1;
//
//	EMDM_Status_t emdm_status = EMDM_Get_Reg_Value(reg, nbytes, &I2C1_Handle);
	/* END TEST 2: EMDM SET MOTOR TYPE TEST	*/


	/* START TEST 3: EMDM SET POLARITY TEST	*/
	// Goal ensure EMDM_Set_Polarity() functions correctly and sets the polarity of the EMDM. Should be very similar to Set_Motor_Type()
//	EMDM_Handle.EMDM_POLARITY = DEFAULT;
//	EMDM_Set_Polarity(&EMDM_Handle, &I2C1_Handle);
//
//	uint8_t reg = 0x15;
//	uint8_t nbytes = 1;
//
//	EMDM_Status_t emdm_status = EMDM_Get_Reg_Value(reg, nbytes, &I2C1_Handle);
	/* END TEST 3: EMDM SET POLARITY TEST	*/


	/* START TEST 4: EMDM SET SPEED TEST	*/
	// STEP 1: use functions to set motor type and polarity
	EMDM_Handle.EMDM_MOTOR_TYPE = MOTOR_TYPE_JGB;
	EMDM_Set_Motor_Type(&EMDM_Handle, &I2C1_Handle);

	EMDM_Handle.EMDM_POLARITY = DEFAULT;
	EMDM_Set_Polarity(&EMDM_Handle, &I2C1_Handle);


	// STEP 2: Use new EMDM_Set_Fixed_Speed_Reg() function to set speed of motors
	// Works pretty well, becareful with this code! Figure out more tmw!
//	for (uint8_t i = 40; i >= 0; i-=10){
//		EMDM_Handle.EMDM_SPEED[0] = i;
//		EMDM_Handle.EMDM_SPEED[1] = i;
//		EMDM_Handle.EMDM_SPEED[2] = i;
//		EMDM_Handle.EMDM_SPEED[3] = i;
//		EMDM_Set_Fixed_Speed_Reg(&EMDM_Handle, &I2C1_Handle);
//		delay();
//	}

	// I WILL USE THIS CODE TO MAKE THE
	EMDM_Handle.EMDM_SPEED[0] = -30;
	EMDM_Handle.EMDM_SPEED[1] = 0;
	EMDM_Handle.EMDM_SPEED[2] = 30;
	EMDM_Handle.EMDM_SPEED[3] = 0;
	EMDM_Set_Fixed_Speed_Reg(&EMDM_Handle, &I2C1_Handle);

	delay();
	delay();
	delay();


	EMDM_Handle.EMDM_SPEED[0] = -20;
	EMDM_Handle.EMDM_SPEED[1] = 0;
	EMDM_Handle.EMDM_SPEED[2] = 20;
	EMDM_Handle.EMDM_SPEED[3] = 0;
	EMDM_Set_Fixed_Speed_Reg(&EMDM_Handle, &I2C1_Handle);

	delay();
	delay();
	delay();


	EMDM_Handle.EMDM_SPEED[0] = -10;
	EMDM_Handle.EMDM_SPEED[1] = 0; // Not Connected; Dont Care About !!!
	EMDM_Handle.EMDM_SPEED[2] = 10;
	EMDM_Handle.EMDM_SPEED[3] = 0; // Not Connected; Dont Care About !!!
	EMDM_Set_Fixed_Speed_Reg(&EMDM_Handle, &I2C1_Handle);

	delay();
	delay();
	delay();


//	EMDM_Handle.EMDM_SPEED[0] = 0;
//	EMDM_Handle.EMDM_SPEED[1] = 0;
//	EMDM_Handle.EMDM_SPEED[2] = 0;
//	EMDM_Handle.EMDM_SPEED[3] = 0;
//	EMDM_Set_Fixed_Speed_Reg(&EMDM_Handle, &I2C1_Handle);

	EMDM_Stop(&EMDM_Handle, &I2C1_Handle);

	/* END TEST 4: EMDM SET SPEED TEST	*/


	return 1;

}
