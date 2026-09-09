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
#include "n32g0xx_STLlib.h"


/** @addtogroup N32G0xxSelfTestLib_src
  * @{
  */ 


#define STL_SyncSysTick() { while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0uL) {} }
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Start up the internal and external oscillators and verifies
  *   that clock source is within the expected range
  * @param :  None
  * @retval : ClockStatus = (LSI_START_FAIL; HSE_START_FAIL;
  *   HSI_HSE_SWITCH_FAIL; TEST_ONGOING; EXT_SOURCE_FAIL;
  *   CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK)
  */
ClockStatus STL_ClockStartUpTest(void)
{
  ClockStatus clck_sts = TEST_ONGOING;
  uint32_t TimeOut = LSI_START_TIMEOUT;
  uint32_t LSIcnt[5] = {0};
  uint32_t Delaycount = 0;
  uint16_t i = 0;
  
  CtrlFlowCnt += CLOCK_TEST_CALLEE;

  /* confirm LSI is ready */
  do
  {
    TimeOut--;
  }
  while((RCC_Flag_Status_Get(RCC_FLAG_LSIRDF) == RESET) && (TimeOut != 0uL));

  if (TimeOut == 0uL)
  {
    clck_sts = LSI_START_FAIL;     /* Internal low speed oscillator failure */
  }
  
  if(clck_sts == TEST_ONGOING)
  {
    
      /* Wait for five subsequent LSI periods measurements */
    for(i = 0; i<5; i++) // Calculate 5 times
	{
        RCC_LSI_Detect_Enable();
		
		while(!(RCC->LSICTRL&RCC_LSICTRL_LSIDETFF));
		
		LSIcnt[i] = RCC_LSI_Clocks_Frequencies_Get();// LSICLK(Hz) = 8*48000000/LSIcnt = 384000000/LSIcnt
        RCC->LSICTRL |= RCC_FLAG_LSIDETFF; // clear done flag
        
		RCC_LSI_Detect_Disable(); // close LSI detect
		
		Delaycount = 0xFFFF;
        while(Delaycount--)
        {
        }
	}
    PeriodValue = (uint32_t)384000000*5/(LSIcnt[0]+LSIcnt[1]+LSIcnt[2]+LSIcnt[3]+LSIcnt[4]); //Take the average of 5 times
    PeriodValueInv = ~PeriodValue;

      /*-------------------- HSI measurement check -------------------------*/
      if (PeriodValue < LSI_LimitLow)
      {
        /* HSI -20% below expected */
        clck_sts = EXT_SOURCE_FAIL;
      }
      else if (PeriodValue > LSI_LimitHigh)
      {
        /* HSI +20% above expected */
        clck_sts = EXT_SOURCE_FAIL;
      }
    
  }
  
     
  if(clck_sts == TEST_ONGOING)
  {
    /* the test was finished correctly */
    clck_sts = FREQ_OK;
  }
  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;
  
  return(clck_sts);
}




/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
