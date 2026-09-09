/**
*     Copyright (c) 2022, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
*
*     NATIONS does not grant any license under its patents, copyrights, trademarks, or other 
* intellectual property rights. Names and brands of third party may be mentioned or referred 
* thereto (if any) for identification purposes only.
*
*     NATIONS reserves the right to make changes, corrections, enhancements, modifications, and 
* improvements to this software at any time without notice. Please contact NATIONS and obtain 
* the latest version of this software before placing orders.

*     Although NATIONS has attempted to provide accurate and reliable information, NATIONS assumes 
* no responsibility for the accuracy and reliability of this software.
* 
*     It is the responsibility of the user of this software to properly design, program, and test 
* the functionality and safety of any application made of this information and any resulting product. 
* In no event shall NATIONS be liable for any direct, indirect, incidental, special,exemplary, or 
* consequential damages arising in any way out of the use of this software or the Product.
*
*     NATIONS Products are neither intended nor warranted for usage in systems or equipment, any
* malfunction or failure of which may cause loss of human life, bodily injury or severe property 
* damage. Such applications are deemed, "Insecure Usage".
*
*     All Insecure Usage shall be made at user's risk. User shall indemnify NATIONS and hold NATIONS 
* harmless from and against all claims, costs, damages, and other liabilities, arising from or related 
* to any customer's Insecure Usage.

*     Any express or implied warranty with regard to this software or the Product, including,but not 
* limited to, the warranties of merchantability, fitness for a particular purpose and non-infringement
* are disclaimed to the fullest extent permitted by law.

*     Unless otherwise explicitly permitted by NATIONS, anyone may not duplicate, modify, transcribe
* or otherwise distribute this software for any purposes, in whole or in part.
*
*     NATIONS products and technologies shall not be used for or incorporated into any products or systems
* whose manufacture, use, or sale is prohibited under any applicable domestic or foreign laws or regulations. 
* User shall comply with any applicable export control laws and regulations promulgated and administered by 
* the governments of any countries asserting jurisdiction over the parties or transactions.
**/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "n32g003.h"

/* USER CODE BEGIN Includes */
#include "n32g0xx_STLparam.h"
#include "n32g0xx_STLlib.h"
/* USER CODE END Includes */



int main(void)
{
  
    System_Initializes();
      
    #ifdef STL_VERBOSE
    STL_VerbosePORInit();
    printf(" \n\r ");
    printf(" %s\n\r", " ... N32G0xx Cortex M0    ... ");
    printf(" %s\n\r", " ... IEC60730 Classb Test ... ");
    printf(" %s\n\r", " ... Main Routine Start   ...\r\n");
    #endif /* STL_VERBOSE */
    
    STL_InitRunTimeChecks();
    while (1)
    {

        /* Add your application tasks here  */
        if(TimeBaseFlag == 0xAAAAAAAA)
        {
          /* run time tests */
          STL_DoRunTimeChecks();         
          TimeBaseFlag= 0;
          TimeBaseFlagInv= ~TimeBaseFlag;
        }

    }
 

}


/* -------------------------------------------------------------------------*/
/**
  * @brief  Retargets the C library printf function to the UART.
  * @param :  None
  * @retval None
  */
static int is_lr_sent = 0;
int fputc(int ch, FILE* f)
{
    if (ch == '\r')
    {
        is_lr_sent = 1;
    }
    else if (ch == '\n')
    {
        if (!is_lr_sent)
        {
            UART_Data_Send(UART1, (uint8_t)'\r');
            /* Loop until the end of transmission */
            while (UART_Flag_Status_Get(UART1, UART_FLAG_TXC) == RESET)
            {
            }
        }
        is_lr_sent = 0;
    }
    else
    {
        is_lr_sent = 0;
    }
    UART_Data_Send(UART1, (uint8_t)ch);
    /* Loop until the end of transmission */
    while (UART_Flag_Status_Get(UART1, UART_FLAG_TXC) == RESET)
    {
    }
    return ch;
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) *****END OF FILE****/
