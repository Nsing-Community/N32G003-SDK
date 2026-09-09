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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the pointer to the RAM for the run-time
  *   transparent functional test.
  * @param  : None
  * @retval : None
  */
void STL_TranspMarchInit(void)
{
  /* start address of the test has to be aligned to 16 address range */	  
   pRunTimeRamChk = (uint32_t *)((uint32_t)CLASS_B_START & 0xFFFFFFFCuL);
   pRunTimeRamChkInv = (uint32_t *)(uint32_t)(~(uint32_t)pRunTimeRamChk);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies that 6 words of RAM are functional,
  *   overlapping) using the March C- or March X algorithm.
  * @param  : None
  * @retval : ClassBTestStatus (TEST_RUNNING, CLASS_B_DATA_FAIL,
  *   TEST_FAILURE, TEST_OK)
  */
ClassBTestStatus STL_TranspMarch(void)
{
  ClassBTestStatus result = TEST_RUNNING;
  
  /* Check Class B var integrity */
  if ((((uint32_t)pRunTimeRamChk) ^ ((uint32_t)pRunTimeRamChkInv)) == 0xFFFFFFFFuL)
  {
    if (pRunTimeRamChk >= CLASS_B_END)
    {
      /*------------- March test applied on the RAM Buffer itself --------------- */
      if (STL_TranspRamMarchCXStep(&aRunTimeRamBuf[0], &aRunTimeRamBuf[0], BCKGRND) == MARCH_STEP_SUCCESS)         
      {
        /* All the RAM test is completed successfully */
        result = TEST_OK;
      }
      else  /* Buffer is not functional */
      {
        result = TEST_FAILURE;
      }
      /* Init next cycle of the transparent RAM test starting from the begin of the Class B area */
      pRunTimeRamChk = CLASS_B_START;
      pRunTimeRamChkInv = ((uint32_t *)~((uint32_t)CLASS_B_START));
    }
    else
    { /* ------------- March test applied on Class B data area ------------------ */
      /* !!! Application has to ensure that no write or read operation
               is performed within Class B RAM area during this test !!!      */
      if (STL_TranspRamMarchCXStep(pRunTimeRamChk, &aRunTimeRamBuf[1], BCKGRND) == MARCH_STEP_SUCCESS)         
      {
        /* Prepare next Row Transparent RAM test */
        pRunTimeRamChk += RT_RAM_BLOCKSIZE - (2u * RT_RAM_BLOCK_OVERLAP);
        pRunTimeRamChkInv = (uint32_t *)(uint32_t)(~(uint32_t)pRunTimeRamChk);
      }
      else
      {
        result = TEST_FAILURE;  /* Word line under test was not functional */
      }
    }
  }
  else  /* Class B error on pRunTimeRamChk */
  {
    result = CLASS_B_DATA_FAIL;
  }
  return (result);  
}

/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
