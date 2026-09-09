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
#include "n32g0xx_STLparam.h"
#include "n32g0xx_STLlib.h"

/** @addtogroup N32G0xxSelfTestLib_src
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CTRL_TICKINT_Set      ((uint32_t)0x00000002)
#define CTRL_TICKINT_Reset    ((uint32_t)0xFFFFFFFD)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the SysTick for clock frequency measurement and
  *   main time base. The RTC is reset and synchronized with the
  *   SysTick to have a direct comparison with reference period
  *   (RTC counter is read during each SysTick interrupt).
  * @param :  None
  *   None
  * @retval : None
  */
void STL_SysTickConfig(void)
{
  /* Initializes the Systick for run-time clock measurement */
  SysTick->CTRL &= SysTick_Counter_Disable; /* Stop SysTick timer */
  SysTick->VAL = SysTick_Counter_Clear;    /* Reset counter */
  /* Set reload rate. Corresponds to reference period for clock measurement */
  SysTick->LOAD = SYSTICK_TB_RUN;// period = 1/(48M/8)*12000 = 2ms
  /* Enable the SysTick Interrupt */
  SysTick->CTRL |= CTRL_TICKINT_Set;
  /* Start down-counting */ 
  SysTick->CTRL |= SysTick_Counter_Enable;
  
  /*Configure the SysTick IRQ priority */
  NVIC_SetPriority(SysTick_IRQn, TICK_INT_PRIORITY);
}



/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
