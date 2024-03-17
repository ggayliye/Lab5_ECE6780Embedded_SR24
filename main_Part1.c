/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

//5.7.1 Reading the L3GD20 “WHO_AM_I” Register
	//5.1 Connecting The Sensor Pins
	//The Discovery board is designed to use the SPI interface to connect to the sensor
	
	//Enable GPIOB and GPIOC in the RCC.
	//Bit 19 IOPCEN: I/O port C clock enable
	//Bit 18 IOPBEN: I/O port B clock enable
	
	RCC->AHBENR |=0xC0000;
	
//2. Set PB11 to alternate function mode, open-drain output type, 
	//and select I2C2_SDA as its alternate function.
	//3. Set PB13 to alternate function mode, open-drain output type, 
	//and select I2C2_SCL as its alternate function.
	//Setting  PB11 & PB13 at the same time:

  GPIOB->MODER =((GPIOB->MODER & 0xF33FFFFF) | 0x8800000);	//10: Alternate function mode. Set zero then alternative function mode
	
	//Setting  PB11 & PB13  open-drain output type
	//1: Output open-drain
	//Binary: 00000000000000000010100000000000
	GPIOB->OTYPER |= 0x2800; // GPIO port output type register
	
	GPIOB->OSPEEDR &= 0x0; //GPIO port output speed register to Low speed

	GPIOB->PUPDR &= 0x0; //GPIO port pull-up/pull-down register //double check this
	//Setting  PB11 I2C2_SDA as its alternate function.
	// Look at the PDF "STM32F072x8 STM32F072xB" page 45 to find alt fxn for PB11. 
	//It's AF1.
	
		 // AF1 should be coded as :0001 (as binary). PDF "RM0091" page 163.
	 // PB11 is in AFRH (will be coded as AFR[1]).
	 
	 
	 // PB13 and PB11 are in the AFRH becuase they are over "7" or over AFSEL7[3:0].
	 // They will be in the AFSEL11[3:0] and AFSEL13[3:0].
	 
	 	//Setting  PB13 I2C2_SCL as its alternate function. 
		//It's AF5.Look at the PDF "STM32F072x8 STM32F072xB" page 45 to find alt fxn for PB13. 
		//Binary:For AND: 11111111000011110000111111111111, for OR:00000000010100000001000000000000
  	GPIOB->AFR[1] = ((GPIOB->AFR[1] & 0xFF0F0FFF) | 0x501000); //set them to "0" first. Then set PB11 is AF1 (0001) and PB13 is AF5 (0101) in binary.
		
//4. Set PB14 to output mode, push-pull output type, and initialize/set the pin high.
//01: General purpose output mode
//AND Binary: 11001111111111111111111111111111; OR: 00010000000000000000000000000000
GPIOB->MODER =( (GPIOB->MODER &0xCFFFFFFF) | 0x10000000);//set to 0, then output mode.

//push-pull output type
//AND binary: 11111111111111111011111111111111
GPIOB->OTYPER &= 0xFFFFBFFF; // GPIO port push-pull output type register.0: Output push-pull (reset state)

//initialize/set the pin high
//01: Pull-up
//Binar AND:11001111111111111111111111111111; OR:00010000000000000000000000000000
GPIOB->PUPDR = ((GPIOB->PUPDR & 0xCFFFFFFF) | 0x10000000); //01: Pull-up. GPIO port pull-up/pull-down register.




//5. Set PC0 to output mode, push-pull output type, and initialize/set the pin high.
//Set PC0 to output mode
//AND:11111111111111111111111111111100; OR:00000000000000000000000000000001
GPIOC->MODER =( (GPIOC->MODER &0xFFFFFFFC) | 0x1);//set to 0, then output mode.
//push-pull output type
//AND:11111111111111111111111111111110
GPIOC->OTYPER &= 0xFFFFFFFE; // GPIO port push-pull output type register:0: Output push-pull (reset state)
//initialize/set the pin high.
GPIOC->PUPDR = ((GPIOC->PUPDR & 0xFFFFFFFC) | 0x1); //01: Pull-up. GPIO port pull-up/pull-down register.


		
//5.3 Initializing the I2C Peripheral

//1. Enable the I2C2 peripheral in the RCC.
//The I2C2 peripheral is simpler and requires less configuration than I2C1.
		//APB peripheral clock enable register 1 (RCC_APB1ENR)
//		Bit 22 I2C2EN: I2C2 clock enable
//		Set and cleared by software.
//		0: I2C2 clock disabled
//		1: I2C2 clock enabled
//Page 124
//Binary:00000000010000000000000000000000
		RCC->APB1ENR |=0x400000;
		
//2. Set the parameters in the TIMINGR register to use 100 kHz standard-mode I2C.
// See section 5.5.2 and figure 5.4 in this lab manual.
// You will need to shift the parameter values into the proper bit-location and use bitwise
//operators to apply them to the register.
		//I2Cx timing register (I2Cx_TIMINGR)
		// get the values from lab manual Figure 5.4: Timing table for the default 8MHz processor speed
		//I converted all to finary and converted back hex and assigning all at once
		//AND:00001111000000001111000000000000; OR: 00010000010000100000111100010011
	I2C2->TIMINGR =( (I2C2->TIMINGR & 0xF00F000) | 0x10420F13 ); //set the necessary ports to 0, then proper values from figure 5.4 in this lab manual.
	
	//3. Enable the I2Cx peripheral using the PE bit in the CR1 register.
	//page 684, 26.7 I2C registers on the "RM0091"
	
//	Bit 0 PE: Peripheral enable
//0: Peripheral disable
//1: Peripheral enable
//Note: When PE = 0, the I2C SCL and SDA lines are released. Internal state machines and
//status bits are put back to their
	
	I2C2->CR1 |=0x1;
	
	//5.4 Reading the Register
	//Set the transaction parameters in the CR2 register
	//I2C control register 2 (I2C_CR2)
	
//	1.• Set the L3GD20 slave address = 0x69
//– The slave address is documented in the I2C section of the sensor datasheet. It is
//modified by the state of the SDO pin.

//1. Set the slave address in the SADD[7:1] bit field.

//• Set the number of bytes to transmit = 1.
//Bit 10 RD_WRN: Transfer direction (master mode)
//0: Master requests a write transfer.
//1: Master requests a read transfer.
//Bits 23:16 NBYTES[7:0]: Number of bytes
//The number of bytes to be transmitted/received is programmed there. This field is don’t care
//in slave mode with SBC = 0.
//Note: Changing these bits when the START bit is set is not allowed.

		

//• Set the RD_WRN bit to indicate a "write" operation.
//Bit 10 RD_WRN: Transfer direction (master mode)
//0: Master requests a write transfer.
//1: Master requests a read transfer
//Note: Changing this bit when the START bit is set is not allowed.
//AND:11111111111111111111101111111111
///////////////////////////////////
//Will do all at one except the START
//AND:11111111000000001111101100000000 ; OR: 00000000000000010000000001101001
I2C2->CR2 =((I2C2->CR2 & 0xFF00FB00) | 0x10069); //

//• Set the START bit.
//Bit 13 START: Start generation
//binary:00000000000000000010000000000000
I2C2->CR2 =(I2C2->CR2 | 0x2000); //

//I2C2->CR2 =((I2C2->CR2 & 0xFFFEDB00) | 0x12069); //set them 0 first, then adjust "1"s with OR.
//Binary: 11111111111111101101101100000000 for AND, 00000000000000010010000001101001 for OR.

//I2C2->CR2 |=0x12000; //
//Binary For both reg set up at the same time:00000000000000010010000000000000

//0: No Start generation.
//1: Restart/Start generation:
//00000000000000000010000000000000
//I2C2->CR2 |=0x2000; //


//Repeat the polling and writing process with the NACKF and TXIS
//bits until the number of bytes in the NBYTES bit field has been written:

//I2C interrupt and status register (I2C_ISR)
//this maybe wrong:[I2C2->ISR ==  (I2C2->ISR & 0xFFFFFF7F)
//repeat until TCR is 1.(continue as long as TCR is zero)
//Binary: for AND 11111111111111111111111101111111]

//Binary for NBYTES check 11111111000000001111111111111111
//I2C2->CR2 != (I2C2->CR2 & 0xFF00FFFF)
//Binary OR 00000000000000000000000010000000, AND: 11111111111111111111111101111111
////////////////
//binary:11111110111111111111111111111111
while(I2C2->CR2 != (I2C2->CR2 & 0xFEFFFFFF)){


//while(I2C2->ISR != ( (I2C2->ISR & 0xFFFFFF7F) | 0x80 ) ){
//while(I2C2->ISR !=(I2C2->ISR | 0x2) ){
//(I2C2->ISR !=((I2C2->ISR & 0xFFFFFFFD) | 0x2))
//while(I2C2->ISR ==(I2C2->ISR & 0xFFFFFFFD) )


//2. Wait until either of the TXIS (Transmit Register Empty/Ready) or NACKF (Slave Not-
//Acknowledge) flags are set.
//• If the NACKF flag is set, the slave did not respond to the address frame. You may have
//a wiring or configuration error.
//• Continue if the TXIS flag is set.

//This section is realted to "I2C interrupt and status register (I2C_ISR)"

//Bit 4 NACKF: Not Acknowledge received flag
//This flag is set by hardware when a NACK is received after a byte transmission. It is cleared
//by software by setting the NACKCF bit.
//Note: This bit is cleared by hardware when PE = 0.

//wait if NACKF is set (1) AND TXIS is not set (0)
//(I2C2->ISR & 0xFFFFFFFF) ==(I2C2->ISR | 0x0 )
	// NACKF: OR: 00000000000000000000000000010000; AND: 11111111111111111111111111101111
	// TXIS : AND: 11111111111111111111111111111101
	//&& (I2C2->ISR !=((I2C2->ISR & 0xFFFFFFFD) | 0x2))
//while( (I2C2->ISR ==((I2C2->ISR & 0xFFFFFFEF) | 0x10) ) )
//while(I2C2->ISR !=(I2C2->ISR | 0x2) ){}
	
while( (I2C2->ISR ==(I2C2->ISR | 0x10) ) | (I2C2->ISR ==(I2C2->ISR | 0x2) ) ){
		//I2C interrupt clear register (I2C_ICR)
		//Bit 4 NACKCF: Not Acknowledge flag clear
		//Writing 1 to this bit clears the NACKF flag in I2C_ISR register.
		//	if((I2C2->ISR ==((I2C2->ISR & 0xFFFFFFEF) | 0x10) )){}
	if((I2C2->ISR ==(I2C2->ISR | 0x10) )){
					I2C2->ICR |=	0x10;
	}
	else if((I2C2->ISR ==(I2C2->ISR | 0x2) )){
		continue;
	}
	
	
	
	}


	
//if( I2C2->ISR ==(I2C2->ISR | 0x2)  ){
//continue;
//}





	
//	else if((I2C2->ISR ==((I2C2->ISR & 0xFFFFFFFD) | 0x2))){
//		
//		continue;
//		
//	}
	//wait if TXIS is not set (0)





//3. Write the address of the “WHO_AM_I” 
//register into the I2C transmit register. (TXDR)

//I2C transmit data register (I2C_TXDR)
//Bits 7:0 TXDATA[7:0]: 8-bit transmit data
//Data byte to be transmitted to the I2C bus
//Note: These bits can be written only when TXE = 1.

I2C2->TXDR = 0xD3;
}

//tested:working until here//










//4. Wait until the TC (Transfer Complete) flag is set.
//(I2C2->ISR) !=((I2C2->ISR & 0xFFFFFFBF) | 0x40) 
//while( I2C2->ISR == (I2C2->ISR & 0xFFFFFFBF) ){
//	
//}
//Bit 6 TC: Transfer Complete (master mode)
//This flag is set by hardware when RELOAD=0, AUTOEND=0 and NBYTES data have been
//transferred. It is cleared by software when START bit or STOP bit is set.
//Note: This bit is cleared by hardware when PE = 0.
//binary:00000000000000000000000001000000
//while( I2C2->ISR != (I2C2->ISR | 0x40) )
while( I2C2->ISR == (I2C2->ISR & 0xFFFFFFBF) ){

	
}
//HAL_Delay(2);





//5. Reload the CR2 register with the same parameters as before, 
//but set the RD_WRN bit to indicate a "read" operation.
//Don’t forget to set the START bit again to perform a I2C restart condition.
//Binary: AND: 11111111000000001101101100000000; OR :00000000000000010000010001101001
//I2C2->CR2 =((I2C2->CR2 & 0xFF00DB00) | 0x10469); //
//////////////////////////////////
//Binary:reload and "read" are set:00000001000000000000010000000000
I2C2->CR2 |= 0x1000000;//reload
I2C2->CR2 |= 0x400; //read/write bit set to 1
//I2C2->CR2 =((I2C2->CR2 & 0xFFFEDB00) | 0x12469); 
//I2C2->CR2 |= 0x2400; //set them 0 first, then adjust "1"s with OR.
//I2C2->CR2 |= 0x400; //RE/WR bit

I2C2->CR2 |= 0x2000; //Start bit
//Binary: 11111111111111101101101100000000 for AND, 00000000000000010010010001101001 for OR.



//6. Wait until either of the RXNE (Receive Register Not Empty) or NACKF (Slave Not-
//Acknowledge) flags are set.
//• Continue if the RXNE flag is set.

//wait as long as they are "0"
//(I2C2->ISR) ==(I2C2->ISR & 0xFFFFFFEF)
//while((I2C2->ISR ==(I2C2->ISR & 0xFFFFFFEF) ) && (I2C2->ISR ==(I2C2->ISR & 0xFFFFFFFB) ) )
//while(I2C2->CR2 != (I2C2->CR2 & 0xFEFFFFFF)){}
	
//while(I2C2->TXDR != 0x0){}
	while((I2C2->ISR ==(I2C2->ISR | 0x10) ) | (I2C2->ISR ==(I2C2->ISR | 0x4) ) ){
		
		//I2C interrupt clear register (I2C_ICR)
		//Bit 4 NACKCF: Not Acknowledge flag clear
		//Writing 1 to this bit clears the NACKF flag in I2C_ISR register.
		if((I2C2->ISR ==(I2C2->ISR | 0x10) )){
					I2C2->ICR |=	0x10;
	}
	else if((I2C2->ISR ==(I2C2->ISR | 0x4) )){
		continue;
	}
}
	
//Binary: 11111111111111111111111111101111 && 11111111111111111111111111111011
//I2C receive data register (I2C_RXDR)
//Bits 7:0 RXDATA[7:0]: 8-bit receive data
//Data byte received from the I2C bus

//the following received data will just disappear.
//it can be saved to a variable and recorded somewhare if needed
I2C2->RXDR;

//7. Wait until the TC (Transfer Complete) flag is set.

//while(  (I2C2->ISR) !=((I2C2->ISR & 0xFFFFFFBF) | 0x40) ){
//	
//}

//while( I2C2->ISR == (I2C2->ISR & 0xFFFFFFBF) ){
//	
//}
//while( I2C2->ISR != (I2C2->ISR | 0x40) ){
//	
//}
//while( I2C2->ISR == (I2C2->ISR & 0xFFFFFFBF) ){

//	
//}

//while( I2C2->ISR == (I2C2->ISR & 0xFFFFFFBF) ){

//	
//}

//Binary:11111111111111111111111101111111

while( I2C2->ISR == (I2C2->ISR & 0xFFFFFF7F) ){

	
}



//8. Check the contents of the RXDR register to see if 
//	it matches 0xD4. (expected value of the “WHO_AM_I” register)
//I2C receive data register (I2C_RXDR)
//value should be 0xD3
//I check it using a while loop
//I2C2->RXDR  !=0xD3
while(  I2C2->RXDR  == ((I2C2->RXDR & 0xFFFFFF00) | 0xD3 ) ){
	
}



//9. Set the STOP bit in the CR2 register to release the I2C bus.
//Bit 14 STOP: Stop generation (master mode)
//The bit is set by software, cleared by hardware when a STOP condition is detected, or when
//PE = 0.
//In Master mode:
//0: No Stop generation.
//1: Stop generation after current byte transfer.

I2C2->CR2 |=0x4000 ; //.
//Binary: 00000000000000000100000000000000 for OR.


// PB11  PB13 PB14 PC0

///////This part is used to test is the code is working://////////
//check turning on the GREEN
// PC9 is connected to GREEN LED.
		GPIOC->MODER &= 0xFFF3FFFF; //reset 9(GREEN). Binbary 11111111111100111111111111111111
		GPIOC->MODER |= 0x40000; //PC9 to "01:General purpose output mode"
		GPIOC->OTYPER &= 0x0; //no OR (|) . GPIO port output type register

		GPIOC->OSPEEDR &= 0x0; //GPIO port output speed register to Low speed

		GPIOC->PUPDR &= 0x0; //GPIO port pull-up/pull-down register
	
		GPIOC->ODR |= 0x200; //Green On
///////END: // This part is used to test is the code is working://///////







  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
