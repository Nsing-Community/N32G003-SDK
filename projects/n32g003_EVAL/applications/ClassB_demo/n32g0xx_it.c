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

/**
 * @file n32g003_it.c
 * @author Nations
 * @version v1.0.0
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 */
#include "n32g003.h"
#include "n32g0xx_it.h"
#include "n32g0xx_STLparam.h"
#include "n32g0xx_STLclassBvar.h"
#include "n32g0xx_STLlib.h"
#include <stdio.h>



/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSV_Handler exception.
 */
void PendSV_Handler(void)
{
}

/**
* @brief This function handles System tick timer.
 *1.get 2ms SysTick to test main check
 *2.test 
*/
void SysTick_Handler(void)
{
    /* Verify TickCounter integrity */
    if ((TickCounter ^ TickCounterInv) == 0xFFFFFFFFuL)
    {
        TickCounter++;
        TickCounterInv = ~TickCounter;

        if (TickCounter >= SYSTICK_20ms_TB)
        {
            uint32_t RamTestResult;

            /* Reset timebase counter */
            TickCounter = 0u;
            TickCounterInv = 0xFFFFFFFF;

            /* Set Flag read in main loop */
            TimeBaseFlag = 0xAAAAAAAAuL;
            TimeBaseFlagInv = 0x55555555uL;
      
          
          /*----------------------------------------------------------------------*/
          /*------------------     RAM test(in interrupt)  -----------------------*/
          /*----------------------------------------------------------------------*/
            ISRCtrlFlowCnt += RAM_MARCHC_ISR_CALLER;
            __disable_irq();
            RamTestResult = STL_TranspMarch();
            __enable_irq();
            ISRCtrlFlowCntInv -= RAM_MARCHC_ISR_CALLER; 
            switch ( RamTestResult )
            {
                case TEST_RUNNING:
                  break;
                case TEST_OK:
                  #ifdef STL_VERBOSE
                   /* avoid any long string output here in the interrupt, '#' marks ram test completed ok */
                    putchar((int)'#');      /* RAM OK mark */
                  #endif  /* STL_VERBOSE */
                  break;
                case TEST_FAILURE:
                case CLASS_B_DATA_FAIL:
                default:
                  #ifdef STL_VERBOSE
                    printf("\n\r >>>>>>>>>>>>>>>>>>>  RAM Error (March C- Run-time check)\n\r");
                  #endif  /* STL_VERBOSE */
                  FailSafePOR();
                  break;
            } /* End of the switch */

            /* Do we reached the end of RAM test? */
            /* Verify 1st ISRCtrlFlowCnt integrity */
            if ((ISRCtrlFlowCnt ^ ISRCtrlFlowCntInv) == 0xFFFFFFFFuL)
            {
                if (RamTestResult == TEST_OK)
                {
                  if (ISRCtrlFlowCnt != RAM_TEST_COMPLETED)
                  {
                      #ifdef STL_VERBOSE
                        printf("\n\r Control Flow error (RAM test) \n\r");
                      #endif  /* STL_VERBOSE */
                      FailSafePOR();
                  }
                  else  /* Full RAM was scanned */
                  {
                     ISRCtrlFlowCnt = 0u;
                     ISRCtrlFlowCntInv = 0xFFFFFFFFuL;
                  }
                } /* End of RAM completed if*/
            } /* End of control flow monitoring */
            else
            {
              #ifdef STL_VERBOSE
                printf("\n\r Control Flow error in ISR \n\r");
              #endif  /* STL_VERBOSE */
              FailSafePOR();
            }         
       
        } /* End of the 20 ms timebase interrupt */
    }
    else  /* Class B variable error on TickCounter */
    {
        #ifdef STL_VERBOSE
        printf("\n\r Class B Error on TickCounter\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
    }
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C)  *****END OF FILE****/
