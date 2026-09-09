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
#include "crc32_software.h"

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
  * @brief  Inializes the pointers to the Flash memory required during
  *   run-time
  * @param :  None
  * @retval : None
  */
void STL_FlashCrc32Init(void)
{
  /* MISRA violation of rules 11.3, 11.4: pointer casting is used to check the 
     memory area and keep class B variable integrity */
#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  #pragma diag_suppress=Pm140, Pm141
#endif /* IAR Compiler */
  p_RunCrc32Chk = (uint32_t*)ROM_START;
  p_RunCrc32ChkInv = ((uint32_t *)(uint32_t)(~(uint32_t)(ROM_START)));
#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  #pragma diag_default=Pm140, Pm141
#endif /* IAR Compiler */

}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  Computes the crc in multiple steps and compare it with the
  *   ref value when the whole memory has been tested
  * @param :  None
  * @retval : ClassBTestStatus = (TEST_RUNNING, CLASS_B_DATA_FAIL,
  *   TEST_FAILURE, TEST_OK)
  */
ClassBTestStatus STL_crc32Run(void)
{
  ClassBTestStatus Result = CTRL_FLW_ERROR; /* In case of abnormal func exit*/

  CtrlFlowCnt += CRC32_RUN_TEST_CALLEE;

  /* MISRA violation of rules 10.1, 10.3, 11.3, 11.4 and 17.4: integral casting and pointer arithmetic 
    is used here to manage the crc compuation and Check Class B var integrity */
#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  #pragma diag_suppress=Pm088,Pm129,Pm136,Pm140,Pm141
#endif /* IAR Compiler */
  
  if ((((uint32_t)p_RunCrc32Chk) ^ ((uint32_t)p_RunCrc32ChkInv)) == 0xFFFFFFFFuL)
  {
    if (p_RunCrc32Chk < (uint32_t *)ROM_END)
    {
      CtrlFlowCnt += CRC32_TEST_CALLEE;
      if(p_RunCrc32Chk==(uint32_t*)ROM_START)
      {
        crc32_software_continuous(0, (void *) p_RunCrc32Chk, FLASH_BLOCK_WORDS*4, 0);
      }
      else
      {
        crc32_software_continuous(0, (void *) p_RunCrc32Chk, FLASH_BLOCK_WORDS*4, 1);
      }
      CtrlFlowCntInv -= CRC32_TEST_CALLEE;
      
      p_RunCrc32Chk += FLASH_BLOCK_WORDS;     /* Increment pointer to next block */
      p_RunCrc32ChkInv = ((uint32_t *)~(uint32_t)((uint32_t)p_RunCrc32Chk));

      #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  #pragma diag_default=Pm088,Pm129,Pm136,Pm140,Pm141
#endif /* IAR Compiler */
      
      Result = TEST_RUNNING;
    }
    else
    {
      if ((RefCrc32 ^ RefCrc32Inv) == 0xFFFFFFFFuL)
      {
        /* The reference CRC was stored in a Class B variable */
        if(crc32_software_continuous(0, (void *) p_RunCrc32Chk, FLASH_BLOCK_WORDS*4, 2) == REF_CRC32)
        {
          Result = TEST_OK;
        }
        else
        {
          Result = TEST_FAILURE;
        }
        STL_FlashCrc32Init(); /* Prepare next test (or redo it if this one failed) */
      }
      else /* Class B error on RefCrc32 */
      {
        Result = CLASS_B_DATA_FAIL;
      }
    }
  }
  else  /* Class B error p_RunCrc32Chk */
  {
    Result = CLASS_B_DATA_FAIL;
  }

  CtrlFlowCntInv -= CRC32_RUN_TEST_CALLEE;

  return (Result);
}
/**
  * @}
  */
/******************* (C)  *****END OF FILE****/
