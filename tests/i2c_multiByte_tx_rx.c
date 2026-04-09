/*
 * i2c_multiByte_tx_rx.c
 *
 *  Created on: Mar 11, 2026
 *      Author: amaxw
 *
 *      - I will be conducting a test for phase 3 of API testing. This will verify that I can:
 *       (1) Successfully transfer (TX) single and multiple bytes of data to the RTC module (slave)
 *       (2) Successfully receive (RX) single and multiple bytes of data from the RTC module
 *       (3) Multiple simultaneous calls to TX and RX can be made
 *      - This will ensure:
 *       (1) The RX and TX functions can be called multiple times in one iteration / run which ensures flags are getting set and reset correctly
 *       (2) Ensures that RX and TX can work together and I can successfully TX and RX
 *       (3) The RX function works correctly
 *
 *		- I will do various tests for phase 3 which will be detailed in Driver Notes phase 3 API testing section
 *      - I will most likely do different tests for different phases in this file but this is all for now (and in phase 3)
 *
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
	I2C1_Hanlde.I2C_PinConfig.I2C_ADDR_MODE = ADDR_MODE_7BIT;
	I2C1_Hanlde.I2C_PinConfig.I2C_SLAVE_ADDR = 0x68; // Addr of I2C module
	I2C1_Hanlde.I2C_PinConfig.I2C_TRANSFER_DIR = WRITE;
	I2C1_Hanlde.I2C_PinConfig.I2C_NBYTES = 4;
	I2C1_Hanlde.I2C_PinConfig.I2C_AUTOEND_EN = ENABLE;

	// Enable I2C peripheral using I2C_Peri_Control(). Must com
	I2C_Peri_Control(I2C1, ENABLE);


	I2C_Config_Master(&I2C1_Hanlde);

	/*
	 * Conditions Tested in Phase 3:
	 * (1) TX, STOP, then RX using I2C_Master_Reg_Read()
	 * (2) TX then RX. (START just called again, no STOP bit ever generated; Done in I2C_Master_Reg_Read())
	 *
	 *Conditions Left to Test:
	 * (3)- Multiple writes using one call to I2C_Master_TX() and writing to contiguous slave registers
	 * (3.1)- Multiple writes using multiple calls to I2C_Master_TX() and writing non-contiguous slave registers
	 * (4)- Multiple reads using one call to I2C_Master_Reg_Read() and reading multiple contiguous slave registers
	 * (4.1)- Multiple reads using multiple calls to I2C_Master_TX() and reading multiple non-contiguos slave registers
	 * (5)- Explore how RX functions when I specify various values for num_bytes in I2C_Master_Reg_Read() (Start with 1,2,4,5,8)
	 * (6)- Verify TX and RX can work together throughout testing for the above conditions
	 * (6)- Try to break driver / verify error handling
	 *
	 */


	/*
	 * TEST 1 START:
	 * Test one will involve writing to RAM of the RTC module. Address range for RAM is 0x8 - 0x3F (Verifies conditions 1 and 2)
	 * Test 1 Data:
	 * Byte 1 - 0x0A (Address)
	 * Byte 2 - 0x69 (Random Data)
	 *
	 * */
//	uint8_t repeated_start_en = 0;
//	uint8_t tx_data[2];
//	tx_data[0] = 0x0A; // Same as reg_addr
//	tx_data[1] = 0x69; // 0b 0110 1001
//
//	I2C_Status_t tx_status = I2C_Master_TX(&I2C1_Hanlde, tx_data, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
//	if (tx_status != I2C_OK){
//	    while(1);
//	}
//
//	// Allocate data for RX buffer
//	uint8_t nbytes = 1;
//	uint8_t reg_addr = tx_data[0];
//	uint8_t rx_data[nbytes];
//
//	I2C_Status_t rx_status = I2C_Master_Reg_Read(&I2C1_Hanlde, reg_addr, rx_data, nbytes);
//	if (rx_status != I2C_OK){
//	    while(1);
//	}
	/* TEST 1: END */





	/*
	* TEST 2 START:
	* Description: Will start by writing multiple bytes in same function call. Then will write multiple bytes using different function calls
	* Test one will involve writing to RAM of the RTC module. Address range for RAM is 0x8 - 0x3F (Verifies conditions 3)
	*
	* Current Condition Being Tested; Condition 3 and 4
	*
	* */

//	uint8_t repeated_start_en = 0;
//
//	uint8_t tx_data[6];
//	// Write random data to RAM
//	tx_data[0] = 0x0A;
//	tx_data[1] = 0x69; // 0b 0110 1001; In 0x0A
//
//	// Write random data to RAM
//	tx_data[2] = 0x0B;
//	tx_data[3] = 0x42; // 0b 0100 0010; In 0x0B
//
//	// Config SQW to output 4.096 kHz
//	tx_data[4] = 0x07;
//	tx_data[5] = 0x69; // 0b 0001 0001
//
//	// THIS DOES NOT WORK HOW I EXPECTED; WILL RIGHT TO CONTIGUOUS REGISTERS AND NOT SELECT A REG AFTER EACH BYTE
//	// CALL MULTIPLE TRANSFER TO TX TO NON-CONTIGUOUS REGISTERS.
//
//	I2C_Status_t tx_status = I2C_Master_TX(&I2C1_Hanlde, tx_data, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
//	if (tx_status != I2C_OK){
//	    while(1);
//	}

//	// Read both RAM addresses to further verify TX worked and that RX works
//	// Allocate data for RX buffer
//	uint8_t nbytes = 1;
//	uint8_t reg_addr_1 = tx_data[0];
//	uint8_t rx_data[nbytes];
//
//	I2C_Status_t rx_status_1 = I2C_Master_Reg_Read(&I2C1_Hanlde, reg_addr_1, rx_data, nbytes);
//	if (rx_status_1 != I2C_OK){
//	    while(1);
//	}
//
//	// TODO: AFTER THIS TEST COMPLETES SUCCESSFULLY, SINCE I AM WRITING INTO TWO ADJACENT REGISTERS ON SLAVE (0x0A AND 0x0B) TRY MAKING NUM_BYTES = 8 OR 5 TO FULLY READ BOTH REGS
//	// 		 IN ONE FUNCTION CALL
//
//	nbytes = 1;
//	uint8_t reg_addr_2 = tx_data[2];
//	uint8_t rx_data_2[nbytes];
//	I2C_Status_t rx_status_2 = I2C_Master_Reg_Read(&I2C1_Hanlde, reg_addr_2, rx_data_2, nbytes);
//	if (rx_status_2 != I2C_OK){
//	    while(1);
//	}
	/* TEST 2: END */




	/* TEST 3 and 4: START */
	uint8_t repeated_start_en = 0;

	uint8_t tx_data[6];
	// Write random data to RAM
	tx_data[0] = 0x0A; // Starting address
	tx_data[1] = 0x69; // 0b 0110 1001; In 0x0A

	// Write random data to RAM in next contiguous register (will be 0x0B)
	tx_data[2] = 0x69; // Random data; 0b 0110 1001

	// Write random data to RAM in next contiguous register (will be 0x0C)
	tx_data[3] = 0x42; // Random Data; 0b 0100 0010

	// Config SQW to output 4.096 kHz
//	tx_data[4] = 0x07;
//	tx_data[5] = 0x69; // 0b 0001 0001


	I2C_Status_t tx_status = I2C_Master_TX(&I2C1_Hanlde, tx_data, repeated_start_en); // Believe I can just pass as data since it will decay to a pointer, which is what function expects
	if (tx_status != I2C_OK){
	    while(1);
	}

//	// Read both RAM addresses to further verify TX worked and that RX works
//	// Allocate data for RX buffer
	uint8_t nbytes = 3;
	uint8_t reg_addr_1 = tx_data[0];
	uint8_t rx_data[nbytes];

	I2C_Status_t rx_status_1 = I2C_Master_Reg_Read(&I2C1_Hanlde, reg_addr_1, rx_data, nbytes);
	if (rx_status_1 != I2C_OK){
	    while(1);
	}


	/* TEST 3 and 4: END */


}





