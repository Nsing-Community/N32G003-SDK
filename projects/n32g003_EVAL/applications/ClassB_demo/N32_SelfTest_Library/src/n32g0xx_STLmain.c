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
ErrorStatus STL_CheckStack(void);

/**
  * @brief  Initializes the Class B variables and their inverted
  *   redundant counterparts. Init also the Systick and RTC timer
  *   for clock frequency monitoring.
  * @param :  None
  * @retval : None
  */
void STL_InitRunTimeChecks(void)
{
  /* Init Class B variables required in main routine and SysTick interrupt
  service routine for timing purposes */    
    
  TickCounter = 0uL;
  TickCounterInv = 0xFFFFFFFFuL;

  TimeBaseFlag = 0uL;
  TimeBaseFlagInv = 0xFFFFFFFFuL;

  LastCtrlFlowCnt = 0uL;
  LastCtrlFlowCntInv = 0xFFFFFFFFuL;

  /* Initialize variables for SysTick interrupt routine control flow monitoring */
  ISRCtrlFlowCnt = 0uL;
  ISRCtrlFlowCntInv = 0xFFFFFFFFuL;


  /* Initialize SysTick for clock frequency measurement and main time base */
  STL_SysTickConfig();

  /* Initialize variables for run time invariable memory check */  
  STL_FlashCrc32Init();
  
  STL_TranspMarchInit();

  #if defined(USE_INDEPENDENT_WDOG) 
    initialize_system_wdogs();
  #endif  /* USE_INDEPENDENT_WDOG */ 
       
  /* Initialize variables for main routine control flow monitoring */
  CtrlFlowCnt = 0uL;
  CtrlFlowCntInv = 0xFFFFFFFFuL;
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  Provide a short description of the function
  * @param :  None
  * @retval : None
  */
void STL_DoRunTimeChecks(void)
{
  /* Is the time base duration elapsed? */
  if (TimeBaseFlag == 0xAAAAAAAAuL)
  {
     uint32_t TmpFlag = TimeBaseFlagInv;

    /* Verify its integrity (class B variable) */
    if ((TimeBaseFlag ^ TmpFlag) == 0xFFFFFFFFuL)
    {
      uint32_t RomTest;

      /* Reset Flag (no need to reset the redundant: it is not tested if
      TimeBaseFlag != 0xAAAAAAAA, it means that 100ms elapsed */
      TimeBaseFlag = 0uL;

      /*----------------------------------------------------------------------*/
      /*---------------------------- CPU registers ----------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += CPU_TEST_CALLER;
      if (STL_RunTimeCPUTest() != CPUTEST_SUCCESS)
      {
        #ifdef STL_VERBOSE
          printf("Run-time CPU Test Failure\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CtrlFlowCntInv -= CPU_TEST_CALLER;
      }

      /*----------------------------------------------------------------------*/
      /*------------------------- Stack overflow -----------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += STACK_OVERFLOW_TEST;
      if (STL_CheckStack() != SUCCESS)
      {
        #ifdef STL_VERBOSE
          printf("Stack overflow\n\r");
        #endif /* STL_VERBOSE */
        FailSafePOR();
      }
      else
      {
        CtrlFlowCntInv -= STACK_OVERFLOW_TEST;
      }

      /*----------------------------------------------------------------------*/
      /*------------------------- Clock monitoring ---------------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += CLOCK_TEST_CALLER;
      switch ( STL_MainClockTest() )
      {
        case FREQ_OK:
          CtrlFlowCntInv -= CLOCK_TEST_CALLER;
          break;

        case EXT_SOURCE_FAIL:
          #ifdef STL_VERBOSE
          /* Loop until the end of current transmission */
            while(UART_Flag_Status_Get(UART1, UART_FLAG_TXC) == RESET)
            {
            }
            /* Re-config UART baudrate FOR 115200 bds with HSI clock (8MHz) */
            UART_ReConfigurationClk();
            printf("\n\r Clock Source failure (Run-time)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;

        case CLASS_B_VAR_FAIL:
          #ifdef STL_VERBOSE
            printf("\n\r Class B variable error (clock test)\n\r");
          #endif /* STL_VERBOSE */
          FailSafePOR();
          break;

        case LSI_START_FAIL:
        case HSE_START_FAIL:
        case HSI_HSE_SWITCH_FAIL:
        case TEST_ONGOING:
        case CTRL_FLOW_ERROR:
        default:
          #ifdef STL_VERBOSE
            printf("Abnormal Clock Test routine termination \n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }

      /*----------------------------------------------------------------------*/
      /*------------------ Invariable memory CRC check -----------------------*/
      /*----------------------------------------------------------------------*/
      CtrlFlowCnt += FLASH_TEST_CALLER;

      RomTest = STL_crc32Run();
      
      switch ( RomTest )
      {
        case TEST_RUNNING:
            CtrlFlowCntInv -= FLASH_TEST_CALLER;
          break;

        case TEST_OK:
          #ifdef STL_VERBOSE
            putchar((int)'*');      /* FLASH test OK mark */
          #endif  /* STL_VERBOSE */
          CtrlFlowCntInv -= FLASH_TEST_CALLER;
          break;

        case TEST_FAILURE:
        case CLASS_B_DATA_FAIL:
        default:
          #ifdef STL_VERBOSE
            printf("\n\r Run-time FLASH CRC Error\n\r");
          #endif  /* STL_VERBOSE */
          FailSafePOR();
          break;
      }

      /*----------------------------------------------------------------------*/
      /*---------------- Check Safety routines Control flow  -----------------*/
      /*-------------      Refresh independent watchdogs       ---------------*/
      /*----------------------------------------------------------------------*/
           
      /* Reload IWDG counter */
      #ifdef USE_INDEPENDENT_WDOG
        IWDG_Key_Reload();
      #endif  /* USE_INDEPENDENT_WDOG */
           
      if (((CtrlFlowCnt ^ CtrlFlowCntInv) == 0xFFFFFFFFuL)
        &&((LastCtrlFlowCnt ^ LastCtrlFlowCntInv) == 0xFFFFFFFFuL))
      {
        if (RomTest == TEST_OK)
        {
	  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
  /* ==============================================================================*/
  /* MISRA violation of rule 17.4 - pointer arithmetic is used for Control flow calculation */
	    #pragma diag_suppress=Pm088
	  #endif   /* IAR Compiler */
          if ((CtrlFlowCnt == FULL_FLASH_CHECKED)\
          && ((CtrlFlowCnt - LastCtrlFlowCnt) == (LAST_DELTA_MAIN)))
	  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
	    #pragma diag_default=Pm088
  /* ==============================================================================*/
	  #endif   /* IAR Compiler */
          {
            CtrlFlowCnt = 0uL;
            CtrlFlowCntInv = 0xFFFFFFFFuL;
          }
          else  /* Return value form crc check was corrupted */
          {
            #ifdef STL_VERBOSE
              printf("Control Flow Error (main loop, Flash CRC)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }
        else  /* Flash test not completed yet */
        {
          if ((CtrlFlowCnt - LastCtrlFlowCnt) != DELTA_MAIN)
          {
            #ifdef STL_VERBOSE
              printf("Control Flow Error (main loop, Flash CRC on-going)\n\r");
            #endif  /* STL_VERBOSE */
            FailSafePOR();
          }
        }

        LastCtrlFlowCnt = CtrlFlowCnt;
        LastCtrlFlowCntInv = CtrlFlowCntInv;
      }
      else
      {
        #ifdef STL_VERBOSE
          printf("Control Flow Error (main loop)\n\r");
        #endif  /* STL_VERBOSE */
        FailSafePOR();
      }
          
    } /* End of periodic Self-test routine */
    else  /* Class B variable error (can be Systick interrupt lost) */
    {
      #ifdef STL_VERBOSE
        printf("\n\r Class B variable error (clock test)\n\r");
      #endif  /* STL_VERBOSE */
      FailSafePOR();
    }
  } /* End of periodic Self-test routine */
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies the frequency using the measurement
  *   done in Systick interrupt.
  * @param :  None
  * @retval : ClockStatus = (LSI_START_FAIL, HSE_START_FAIL,
  *   HSI_HSE_SWITCH_FAIL, TEST_ONGOING, EXT_SOURCE_FAIL,
  *   CLASS_B_VAR_FAIL, CTRL_FLOW_ERROR, FREQ_OK)
  */
ClockStatus STL_MainClockTest(void)
{
  ClockStatus Result = TEST_ONGOING; /* In case of unexpected exit */

  CtrlFlowCnt += CLOCK_TEST_CALLEE;

  if ((PeriodValue ^ PeriodValueInv) == 0xFFFFFFFFuL)
  {
      
       /*-------------------- HSI measurement check -------------------------*/
      if (PeriodValue < LSI_LimitLow)
      {
        /* HSI -20% below expected */
        Result = EXT_SOURCE_FAIL;
      }
      else if (PeriodValue > LSI_LimitHigh)
      {
        /* HSI +20% above expected */
        Result = EXT_SOURCE_FAIL;
      }
      else
      {
        Result = FREQ_OK;         /* Crystal or Resonator started correctly */   
      }
      
  }
  else  /* Inverse redundant variables failure */
  {
    Result = CLASS_B_VAR_FAIL;
  }

  CtrlFlowCntInv -= CLOCK_TEST_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function verifies that Stack didn't overflow
  * @param :  None
  * @retval : ErrorStatus = (ERROR, SUCCESS)
  */
ErrorStatus STL_CheckStack(void)
{
  ErrorStatus Result = ERROR;

  CtrlFlowCnt += STACK_OVERFLOW_CALLEE;

  if (aStackOverFlowPtrn[0] != 0xAAAAAAAAuL)
  {
    Result = ERROR;
  }
  else /* aStackOverFlowPtrn[0] == 0xAAAAAAAA */
  {
    if (aStackOverFlowPtrn[1] != 0xBBBBBBBBuL)
    {
      Result = ERROR;
    }
    else /* aStackOverFlowPtrn[1] == 0xBBBBBBBB */
    {
      if (aStackOverFlowPtrn[2] != 0xCCCCCCCCuL)
      {
        Result = ERROR;
      }
      else /* aStackOverFlowPtrn[2] == 0xCCCCCCCC */
      {
        if (aStackOverFlowPtrn[3] != 0xDDDDDDDDuL)
        {
          Result = ERROR;
        }
        else
        {
          Result = SUCCESS;
        }
      }
    }
  }

  CtrlFlowCntInv -= STACK_OVERFLOW_CALLEE;

  return (Result);
}

/* ---------------------------------------------------------------------------*/
/**
  * @brief  This function initialize both independent & window system watch dogs
  * @param :  None
  * @retval None
  */
void initialize_system_wdogs(void)
{
  #ifdef USE_INDEPENDENT_WDOG
    while (RCC_Flag_Status_Get(RCC_FLAG_LSIRDF)!= SET)
    { }
       
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_Write_Protection_Disable();
    /* IWDG clock: 32KHz(LSI) / 32 = 1KHz */
    IWDG_Prescaler_Division_Set(IWDG_CONFIG_PRESCALER_DIV32);;
    /* Set counter reload value to 1/1k*30=30ms */
    IWDG_Counter_Reload(500u);
    /* Reload IWDG counter */
    IWDG_Key_Reload();
    /* Enable IWDG */
    IWDG_Enable();
  #endif  /* USE_INDEPENDENT_WDOG */
}

/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
