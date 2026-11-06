///*
// * main.c
// *
// *  Created on: Dec 5, 2025
// *      Author: amaxw
// *
// *      This will be the main file that controls the entire system.
// *      All code should be called here.
// *
// *      As of now, all that will be here is GPIO_init() and I2C_init() which will handle set up of GPIO and I2C to allow for communication with the EMDM.
// *      I will be leaving them here until I decide on a better place for them to live and I will be copying and pasting them onto test files as well until I decide on how to
// *      structure these and make files for these functions to live
// */
//
//
//#include <string.h> //  For memset()
//#include "stm32f334r8.h"
//#include "stm32f334xx_gpio_driver.h"
//#include "stm32f334xx_i2c_driver.h"
//
//void delay(void){
//
//	for (int i = 0; i < 2500000; i++);
//}
//
///**************************************************************************************
// * @fn					- GPIO_init_I2C1
// *
// * @brief				- All this function does as of now is choose GPIO pins PB6 as SCL and PB7 as SDA and configures them for I2C communication.
// * 						  May do more in the future but as of now this is all it does
// *
// * @param[in]			-
// *
// * @return				- All GPIO handles; VERIFY IF THIS IS BEST WAY TO DO THIS
// *
// * @Note				- May have more functionallity later if more pins are set. Will change name to GPIO_init() if so.
// *
// **************************************************************************************/
//void GPIO_init_I2C1(){
//	GPIO_Handle_t GPIO_SCL;
//	GPIO_Handle_t GPIO_SDA;
//
//	memset(&GPIO_SCL, 0, sizeof(GPIO_SCL));
//	memset(&GPIO_SDA, 0, sizeof(GPIO_SDA));
//
//	// SCL (PB6)
//	GPIO_SCL.pGPIOx = GPIOB;
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // // Dont need pull-up because RTC module already pulls line high
//	GPIO_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope
//
//	GPIO_PeriClockControl(GPIOB, ENABLE);
//	GPIO_Init(&GPIO_SCL);
//
//
//	// SDA (PB7)
//	GPIO_SDA.pGPIOx = GPIOB;
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD; // I2C Requirement
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU; // Dont need pull-up because RTC module already pulls line high
//	GPIO_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPD_HIGH; // Possibly lower if ringing/noise on scope
//
//	GPIO_PeriClockControl(GPIOB, ENABLE);
//	GPIO_Init(&GPIO_SDA);
//}
//
///**************************************************************************************
// * @fn					- GPIO_init_I2C1
// *
// * @brief				- Calls all functions from the I2C API that are required to activate and configure the I2C1 periperhal for master communication
// *
// * @param[in]			- ReWrDir: Chooses if the master will be reading or writing to/from slave
// *
// * @return				- ALL I2C HANDLES
// *
// * @Note				- REALLY NEED TO CONSIDER WHAT THESE FUNCTIONS (OTHER INIT ONE) SHOULD RETURN AND PARAMETERS.
// *
// **************************************************************************************/
//void I2C1_master_setup(){
//	I2C_Handle_t I2C1_Hanlde;
//
//
//	/* Select I2C clock as HSI */
//	I2C_Clock_Choice(HSI);
//
//	/* Enable periperhal clock*/
//	I2C_Peri_ClockControl(ENABLE);
//
//	I2C1_Hanlde.pI2C1 = I2C1;
//	// Set freq and mode then pass to I2C_clock config
//	I2C1_Hanlde.I2C_PinConfig.I2C_FREQ = I2C_FREQ_8MHZ;
//	I2C1_Hanlde.I2C_PinConfig.I2C_MODE = STANDARD_100K;
//	I2C_Clock_Config(&I2C1_Hanlde);
//
//	// Set analog/digital filtering and clock stretching in init() function
//	I2C1_Hanlde.I2C_PinConfig.I2C_ANLG_FILTER_CTRL = ANLG_FILTER_EN;
//	I2C1_Hanlde.I2C_PinConfig.I2C_DIGI_FILTER_CTRL = DIGI_FILTER_DI;
//	I2C1_Hanlde.I2C_PinConfig.I2C_CLK_STRETCH = ENABLE;
//	I2C_Init(&I2C1_Hanlde);
//
//	// Set all configs required by config_master()
//	// TODO: MODIFY THESE FOR THE EMDM BOARD
//	I2C1_Hanlde.I2C_PinConfig.I2C_ADDR_MODE = ADDR_MODE_7BIT;
//	I2C1_Hanlde.I2C_PinConfig.I2C_SLAVE_ADDR = 0x34; // Addr of EMDM
//	I2C1_Hanlde.I2C_PinConfig.I2C_TRANSFER_DIR = WRITE;
//	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 1;
//	I2C1_Hanlde.I2C_PinConfig.I2C_AUTOEND_EN = ENABLE;
//
//	// Enable I2C peripheral using I2C_Peri_Control(). Must com
//	I2C_Peri_Control(I2C1, ENABLE);
//
//
//	I2C_Config_Master(&I2C1_Hanlde);
//}
//
//
//
//
//int main(){
//
//}
