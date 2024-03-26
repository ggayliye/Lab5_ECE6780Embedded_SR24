
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
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */



int read_Axis(int address);


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */
      
      RCC->AHBENR |= (3<<18); //Enable GPIOB and GPIOC
      
      

      GPIOB->MODER |= (2<<22); //Set PB11 to alternate function mode
      GPIOB->OTYPER |= (1<<11); //Set PB11 to open-drain.
      
      GPIOB->AFR[1] |= (1 << 12); //Set PB11 to I2C2_SDA
      
      GPIOB->MODER |= (2<<26); //Set PB13 to alternate function mode
      GPIOB->OTYPER |= (1<<13); //Set PB13 to open-drain.
      
      GPIOB->AFR[1] |= (5 << 20); //Set PB13 to I2C2_SCL

      GPIOB->MODER |= (1<<28); //Set PB14 to output mode
      GPIOB->OTYPER &= ~(1<<14); //Set PB14 to push-pull type.

      GPIOC->MODER |= (1);;   //Set PC0 to output mode
      GPIOC->OTYPER &= 0; //Set PC0 to push-pull type.



//Attempt to set 14 and 0 High
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

//5.3
//Enable I2C2 Peripherial in RCC
RCC->APB1ENR |= (1<<22);

//I2C Timing register configuration for 100kHz
I2C2->TIMINGR |= (1<<28);
I2C2->TIMINGR |= 0x13;
I2C2->TIMINGR |= (0xF<<8);
I2C2->TIMINGR |= (0x2<<16);
I2C2->TIMINGR |= (0x4<<20);
//Enable PE bit i2c2 cr1
I2C2->CR1 |= 1;
/////////////////////////////////////////////////

//Set slave address of 0x69 in SADD[7:1]
I2C2->CR2 |= (0x69 << 1);
//Set number of bytes to transmit to 2
I2C2->CR2 |= (1 << 17);
//Set the write register to 0 to indicate a write operation
I2C2->CR2 &= ~(1 << 10);



uint8_t Gyroscope_Address = 0x0F;

uint8_t CTRL_Reg_Address = 0x20;

uint8_t Data = 0x0B;
      
       //Set the start bit
       I2C2->CR2 |= (1 << 13);
      
       //Wait for TXIS or NACKF
       while(!(I2C2->ISR &= (1<<4)) && !(I2C2->ISR &= (1<<1)))
            {
                  
            }
            //If NACKF is set...
            if((I2C2->ISR &= (1<<4)))
            {
                  //Turn Red LED ON
                  GPIOC->BSRR |= (1<<6);
            }
            //If TXIS is set continue...
            else if(I2C2->ISR &= (1<<1))
            {
                  
                  //Turn Green LED ON
                  
                  //Uncomment for Part 1
                  //I2C2->TXDR = Gyroscope_Address; //Set the who am I address register
                  I2C2->TXDR = CTRL_Reg_Address;
                  
                  //Wait until the TC flag is set,

                  while(!(I2C2->ISR &= (1<<4)) && !(I2C2->ISR &= (1<<1)))
                  {
                  
                  }
                  //If NACKF is set...
                  if((I2C2->ISR &= (1<<4)))
                  {
                        //Turn Red LED ON
                        GPIOC->BSRR |= (1<<6);
                  }
                  //If TXIS is set continue...
                  else if(I2C2->ISR &= (1<<1))
                  {
                                          
                        
                                          
                        
                                          I2C2->TXDR = Data;
                        
                                          
                        //Wait until the TC flag is set,
                        while(!(I2C2->ISR &= (1 << 6)))
                        {
                              
                        }
                        
                        I2C2->CR2 |= (1<<14);
                              //set the stop bit
                              //LOOK FOR PROBLEM HERE...
                  }
            }
            


//enable LEDs
GPIOC->MODER |= (85<<12);


  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
       int x_data;
       int y_data;
      
  while (1)
  {
            HAL_Delay(1);
            x_data = read_Axis(0xA8);
            HAL_Delay(1);
            y_data = read_Axis(0xAA);
            
            HAL_Delay(100);
            
            if(x_data < -900)
            {
                  //Turn off Green LED
                  GPIOC->BSRR |= (1<<25);
                  //Turn on Orange LED
                  GPIOC->BSRR |= (1<<8);
            }
            else if(x_data > 900)
            {
                  //Turn off Orange LED
                  GPIOC->BSRR |= (1<<24);
                  //Turn on Green LED
                  GPIOC->BSRR |= (1<<9);
            }
            else{
                  //Turn Both LEDs OFF
                  GPIOC->BSRR |= (1<<25);
                  GPIOC->BSRR |= (1<<24);
            }
            
            if(y_data < -900)
            {
                  //Turn off LED
                        GPIOC->BSRR |= (1<<22);
                  //Turn on Blue LED
                  GPIOC->BSRR |= (1<<7);
            }
            else if(y_data > 900)
            {
                  //Turn off LED
                        GPIOC->BSRR |= (1<<23);
                  //Turn on Red LED
                  GPIOC->BSRR |= (1<<6);
            }
            else{
                  //Turn Both LEDs OFF
                  GPIOC->BSRR |= (1<<22);
                  GPIOC->BSRR |= (1<<23);
            }

  }

}

/**
* Read Axis function
* Takes address of each axis, then assembles and returns data
*/
int read_Axis(int address)
{
            //Temp variables
            int received_data_L;
        int received_data_H;
            int returnval;
            
            //Clear Nbytes and Slave address
            I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
            //Set slave address
            I2C2->CR2 |= (0x69 << 1);
            //Set number of bytes to transmit to 1
            I2C2->CR2 |= (1 << 16);
            //Set the write register to 0 to indicate a write operation
            I2C2->CR2 &= ~(1 << 10);
            //Set the start bit
            I2C2->CR2 |= (1 << 13);
            
    //Wait for TXIS or NACKF
            while(!(I2C2->ISR &= (1<<4)) && !(I2C2->ISR &= (1<<1)))
            {
                  
            }
            //If NACKF is set...
            if((I2C2->ISR &= (1<<4)))
            {
                  
            }
            //If TXIS is set...
            else if(I2C2->ISR &= (1<<1))
            {
                  
            
                  
                  I2C2->TXDR = address; //Set the who am I address register
                  
                  //Wait until the TC flag is set,
                  while(!(I2C2->ISR &= (1 << 6)))
                  {
                        
                  }
                  
                  //Clear Nbytes and Slave address
                  I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));
                  //Set slave address of 0x69 in SADD[7:1]
                  I2C2->CR2 |= (0x69 << 1);
                  //Set number of bytes to transmit to 1
                  I2C2->CR2 |= (1 << 17);
                  //Set the write register to 1 to indicate a read operation
                  I2C2->CR2 |= (1 << 10);
                  //Set the start bit again
                  I2C2->CR2 |= (1 << 13);
                  
                  //Wait for NACK or RXNE is flagged
                  while(!(I2C2->ISR &= (1<<2)) && !(I2C2->ISR &= (1<<4)))
                  {
                  
                  }
                  
                  //If nack is set
                  if((I2C2->ISR &= (1<<4)))
                  {
                        
                  }
                  //If RXNE is set, continue
                  else if(I2C2->ISR &= (1<<2))
                  {
                        //Read Low Byte
                        received_data_L = I2C2->RXDR;
                        
                        //Wait for NACK or RXNE is flagged
                        while(!(I2C2->ISR &= (1<<2)) && !(I2C2->ISR &= (1<<4)))
                        {
                  
                        }
                  
                        if((I2C2->ISR &= (1<<4)))
                        {
                              
                        }
                        //If RXNE is set, continue
                        else if(I2C2->ISR &= (1<<2))
                        {
                        }
                        
                        received_data_H = I2C2->RXDR;
                        
                              //Wait until the TC
                              while(!(I2C2->ISR &= (1 << 6)))
                              {
                              
                              }
                              
                              //Assemble Data
                              returnval = (received_data_H << 8) | received_data_L;
                              
                        
                              //set the stop bit
                              I2C2->CR2 |= (1<<14);
                        
                  }     
            }
            
            return (int16_t)returnval;
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