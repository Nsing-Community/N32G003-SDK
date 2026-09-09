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
#define ALLOC_GLOBALS
#include "n32g0xx_STLlib.h"
#include "crc32_software.h"
/** @addtogroup N32G0xxSelfTestLib_src
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  
 


/******************************************************************************/
/**
  * @brief  This routine is executed in case of failure is detected by one of
  *    self-test routines. The routine is empty and it has to be filled by end
  *    user to keep application safe while define proper recovery operation
  * @param  : None
  * @retval : None
  */
void FailSafePOR(void)
{
   /* SysTick could be disabled here */
  SysTick->CTRL &= 0xFFFFFFFD;

  #ifdef STL_VERBOSE_POR
    printf(" >>>>>>>>>> POR FailSafe Mode <<<<<<<<<<\n\r");
  #endif  /* STL_VERBOSE_POR */
  
  while(1)
  {
    /* Generate system reset */
	#ifdef  GENERATE_RESET_AT_FAIL_SAFE
        NVIC_SystemReset();
	#else
		#ifdef USE_INDEPENDENT_WDOG
			IWDG_Key_Reload();
		#endif /* USE_INDEPENDENT_WDOG */           
	#endif  /* GENERATE_RESET_AT_FAIL_SAFE */
  }
}


#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
#pragma optimize = none
#endif

#ifdef __CC_ARM             /* KEIL Compiler */
/******************************************************************************/
/**
  * @brief  Switch between startup and main code
  * @param  : None
  * @retval : None
  */
  void $Sub$$main(void)
  {
    if(SelfTestFlag != 0xAA)
    {
     STL_StartUp();		/* trick to call StartUp before main entry */
    }
    SelfTestFlag = 0;
    
    $Super$$main(); 
  }
#endif /* __CC_ARM */
	
/******************************************************************************/
/**
  * @brief  Contains the very first test routines executed right after
  *   the reset
  * @param  : None
  *   Flash interface initialized, Systick timer ON (2ms timebase)
  * @retval : None
  */
void STL_StartUp(void)
{
  uint32_t crc_result;
  

  /* block run time tests performed at SysTick interrupt */
   
  /* Reset of all peripherals, Initializes the Flash interface and the Systick */
  
  #ifdef STL_VERBOSE_POR
  /* Update the SystemCoreClock global variable as UART Baud rate setting depends on it */
  STL_VerbosePORInit();
  printf("\r\n*******  Self Test Library Init  *******\r\n");
  #endif
  
  /* Initialization of counters for control flow monitoring */
  init_control_flow();
 
  /*--------------------------------------------------------------------------*/
  /*------------------- CPU registers and flags self test --------------------*/
  /*--------------------------------------------------------------------------*/ 
  /* WARNING: all registers destroyed when exiting this function (including
  preserved registers R4 to R11) while excluding stack pointer R13) */
  
  control_flow_call(CPU_TEST_CALLER);
  
  if (STL_StartUpCPUTest() != CPUTEST_SUCCESS)
  {    
    #ifdef STL_VERBOSE_POR
      printf("Start-up CPU Test Failure\n\r");
    #endif /* STL_VERBOSE_POR */
    FailSafePOR();
  }
  else  /* Test OK */
  {
    /* If else statement is not executed, it will be detected by control flow monitoring */
    control_flow_resume(CPU_TEST_CALLER);
    #ifdef STL_VERBOSE_POR
      printf("Start-up CPU Test OK\n\r");
    #endif /* STL_VERBOSE_POR */
  }
   /*--------------------------------------------------------------------------*/
   /*------------------------- Watch dogs Self Test ---------------------------*/
   /*--------------------------------------------------------------------------*/
  CtrlFlowCnt += WDG_TEST_CALLER;
  STL_WDGSelfTest();
  CtrlFlowCntInv -= WDG_TEST_CALLER;

  /*--------------------------------------------------------------------------*/
  /*--------------------- Invariable memory CRC check ------------------------*/
  /*--------------------------------------------------------------------------*/
  
  control_flow_call(CRC32_TEST_CALLER);
  /* Compute the 32-bit crc of the whole Flash by CRC unit except the checksum
     pattern stored at top of FLASH */
 
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    /* ==============================================================================*/
    /* MISRA violation of rule 11.4, 17.4 - pointer arithmetic is used for
       CRC calculation */
    #pragma diag_suppress=Pm088,Pm141
  #endif  /* __IAR_SYSTEMS_ICC__ */

/* the next lines replaces the standard HAL function call
   crc_result = HAL_CRC_Calculate(&CrcHandle, (uint32_t *)ROM_START, (uint32_t)ROM_SIZEinWORDS); 
  due to bug at IAR linker - check sum computation can't support both big & little endian  */
  crc_result = crc32_software(0, (void*)ROM_START, ROM_SIZE);
  

  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_default=Pm088,Pm141
    /* ==============================================================================*/
  #endif  /* __IAR_SYSTEMS_ICC__ */
          
  control_flow_resume(CRC32_TEST_CALLER);

  
  
  /* Store pattern for regular 32-bit crc computation */
  control_flow_call(CRC_TEST_CALLER);
  /* ==============================================================================*/
  /* MISRA violation of rule 10.1, 11.3 and 11.4: integral casting and pointer arithmetic 
     is used here to manage the crc computation and Check Class B var integrity */
  
  #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
    #pragma diag_suppress=Pm129,Pm140,Pm141

    if(crc_result != *(uint32_t *)(&REF_CRC32))

    #pragma diag_default=Pm129,Pm140,Pm141
  #endif  /* __IAR_SYSTEMS_ICC__ */
  /* ==============================================================================*/
      
  #if defined(__CC_ARM) || defined(__GNUC__)             /* KEIL or GCC Compiler */
    /* Computed 32-bit crc check is temporary stored at crc_result. This value must be copied
       into __Check_Sum address placed at the end of Flash area 
       Condition here can be reversed for debugging */

    if(crc_result != *(uint32_t *)(&REF_CRC32))
  #endif  /* __CC_ARM */   
    {
      #ifdef STL_VERBOSE_POR
        printf("FLASH 32-bit CRC Error at Start-up\n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
    }
    else
    { /* Test OK */
      /* add just check sum pattern to be included at overall FLASH calculation at crc unit */
     #ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
			//CRC32_CalcCrc(*(uint32_t *)(&REF_CRC32));
      #endif  /* __IAR_SYSTEMS_ICC__ */
      //CRC32_SetIDat(~((uint8_t)(CRC32_GetCrc())));
      #ifdef STL_VERBOSE_POR
        printf("Start-up FLASH 32-bit CRC OK\n\r");
      #endif  /* STL_VERBOSE_POR */

      /* If else statement is not executed, it will be detected by control flow monitoring */
      control_flow_resume(CRC_TEST_CALLER);  
    }
    

   /*--------------------------------------------------------------------------*/
  /*   Verify Control flow before RAM init (which clears Ctrl flow counters)  */
  /*--------------------------------------------------------------------------*/
  if (control_flow_check_point(CHECKPOINT1) == ERROR)
  {
     #ifdef STL_VERBOSE_POR
     printf("Control Flow Error Checkpoint 1\r\n");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
  }
  else
  {
   #ifdef STL_VERBOSE_POR
   printf("Control Flow Checkpoint 1 OK\r\n");
   #endif  /* STL_VERBOSE_POR */
  }
 
  /*--------------------------------------------------------------------------*/
  /* --------------------- Variable memory functional test -------------------*/
  /*--------------------------------------------------------------------------*/
 
  /* no stack operation can be performed during the test */  
   __disable_irq();
  
  /* WARNING: Stack is zero-initialized when exiting from this routine */
  if (STL_FullRamMarchC(RAM_START, RAM_END, BCKGRND) != SUCCESS)
  {
    #ifdef STL_VERBOSE_POR
      printf("RAM Test Failure\n\r");
    #endif  /* STL_VERBOSE_POR */
    FailSafePOR();
  }
   else
  {
   #ifdef STL_VERBOSE_POR
   printf("Full RAM Test OK\r\n");
   #endif  /* STL_VERBOSE_POR */
  }


  /* restore interrupt capability */
  __enable_irq();
  
  /* Initialization of counters for control flow monitoring (destroyed during RAM test) */
  init_control_flow();
  
 
  /*------------- Store reference 32-bit CRC in RAM after RAM test -----------*/   
   RefCrc32 = 0;  
   RefCrc32Inv = ~RefCrc32;

  
  //--------------------------------------------------------------------------*/
  //----------------------- Clock Frequency Self Test ------------------------*/
  //--------------------------------------------------------------------------*/
  control_flow_call(CLOCK_TEST_CALLER);
  /* test LSI & HSI clock systems */
  switch ( STL_ClockStartUpTest() )
  {
    case FREQ_OK:
      #ifdef STL_VERBOSE_POR
        printf("Clock frequency OK \n\r");
      #endif  /* STL_VERBOSE_POR */
      break;

    case LSI_START_FAIL:
       #ifdef STL_VERBOSE_POR
        printf("LSI start-up failure \n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
      break;

    case HSE_START_FAIL:
      #ifdef STL_VERBOSE_POR
        printf("HSE start-up failure \n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
      break;

    case HSI_HSE_SWITCH_FAIL:
      #ifdef STL_VERBOSE_POR
        printf("Clock switch failure \n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
      break;

    case EXT_SOURCE_FAIL:
      #ifdef STL_VERBOSE_POR
        printf("Clock Source failure \n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
      break;

    case TEST_ONGOING:
    default:
      #ifdef STL_VERBOSE_POR
        printf("Abnormal Clock Test routine termination \n\r");
      #endif  /* STL_VERBOSE_POR */
      FailSafePOR();
      break;
  }
  /* Either switch back to HSI or start PLL on HSE asap */
  control_flow_resume(CLOCK_TEST_CALLER); 
 
  
 
  /* -----  Store verify pattern to stack bottom for its later testing  ----- */
   CtrlFlowCnt += STACK_OVERFLOW_TEST;

   aStackOverFlowPtrn[0] = 0xAAAAAAAAuL;
   aStackOverFlowPtrn[1] = 0xBBBBBBBBuL;
   aStackOverFlowPtrn[2] = 0xCCCCCCCCuL;
   aStackOverFlowPtrn[3] = 0xDDDDDDDDuL;

   CtrlFlowCntInv -= STACK_OVERFLOW_TEST;
   /*--------------------------------------------------------------------------*/
  /* -----  Verify Control flow before Starting main program execution ------ */
  /*--------------------------------------------------------------------------*/ 
    if (control_flow_check_point(CHECKPOINT2) == ERROR)
   {
     #ifdef STL_VERBOSE_POR
       printf("Control Flow Error Checkpoint 2 \n\r");
     #endif  /* STL_VERBOSE_POR */
     FailSafePOR();
   }
   #ifdef STL_VERBOSE_POR
    printf("Control Flow Checkpoint 2 OK \n\r");
   #endif  /* STL_VERBOSE_POR */

   GotoCompilerStartUp();
}


    
    
/******************************************************************************/
/**
  * @brief  Verifies the consistency and value of control flow counters
  * @param  : check value of the positive counter
  * @retval : ErrorStatus (SUCCESS, ERROR)
  */
ErrorStatus control_flow_check_point(uint32_t chck)
{
  ErrorStatus Result= SUCCESS;
  
  if ((CtrlFlowCnt != (chck)) || ((CtrlFlowCnt ^ CtrlFlowCntInv) != 0xFFFFFFFFuL))
  {
    Result= ERROR;
  }
  return(Result);
}

/* ------------------------------------------------------------ */
/**
  * @brief  Initializes the UART1 and few I/Os for test purposes
  * @param :  None
  * @retval : None
  */
void STL_VerbosePORInit(void)
{
    GPIO_InitType GPIO_InitStructure;
    UART_InitType UART_InitStructure;

    GPIO_Structure_Initialize(&GPIO_InitStructure);

    RCC_APB_Peripheral_Clock_Enable(RCC_APB_PERIPH_UART1 | RCC_APB_PERIPH_AFIO | RCC_APB_PERIPH_IOPA );
    
    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = GPIO_PIN_2;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF5_UART1;
    GPIO_Peripheral_Initialize(GPIOA, &GPIO_InitStructure);

    UART_InitStructure.BaudRate            = 115200;
    UART_InitStructure.WordLength          = UART_WL_8B;
    UART_InitStructure.StopBits            = UART_STPB_1;
    UART_InitStructure.Parity              = UART_PE_NO;
    UART_InitStructure.Mode                = UART_MODE_RX | UART_MODE_TX;
    // init uart
    UART_Initializes(UART1, &UART_InitStructure);

    // enable uart
    UART_Enable(UART1);
}

/* ------------------------------------------------------------ */
/**
  * @brief  Reinitializes the UART1 with new clock frequency
  * @param  : None
  * @retval : None
  */
void UART_ReConfigurationClk()
{
    UART_InitType UART_InitStructure;

    UART_Disable(UART1);
    
    UART_InitStructure.BaudRate            = 115200;
    UART_InitStructure.WordLength          = UART_WL_8B;
    UART_InitStructure.StopBits            = UART_STPB_1;
    UART_InitStructure.Parity              = UART_PE_NO;
    UART_InitStructure.Mode                = UART_MODE_RX | UART_MODE_TX;
    // init uart
    UART_Initializes(UART1, &UART_InitStructure);

    // enable uart
    UART_Enable(UART1);
}


/* ============================================================================ */
/**
  * @brief  Verifies the watchdog by forcing watchdog resets
  * @param :  None
  * @retval : None
  */
void STL_WDGSelfTest(void)
{  
 
   #ifdef STL_VERBOSE_POR  
    if (RCC_Flag_Status_Get(RCC_FLAG_PINRST)  != RESET) printf("Pin reset \r\n");
    if (RCC_Flag_Status_Get(RCC_FLAG_PORRST)  != RESET) printf("POR reset \r\n");
    if (RCC_Flag_Status_Get(RCC_FLAG_SFTRST)  != RESET) printf("SW reset \r\n");
    if (RCC_Flag_Status_Get(RCC_FLAG_IWDGRST) != RESET) printf("IWDG reset \r\n");
    if (RCC_Flag_Status_Get(RCC_FLAG_LPWRRST) != RESET) printf("LP reset \r\n");
  #endif /* STL_VERBOSE_POR */

  /* start watchdogs test if one of the 4 conditions below is valid */
  if ( (RCC_Flag_Status_Get(RCC_FLAG_PORRST) != RESET)\
   ||  (RCC_Flag_Status_Get(RCC_FLAG_SFTRST) != RESET)\
   ||  (RCC_Flag_Status_Get(RCC_FLAG_LPWRRST) != RESET)\
   || ((RCC_Flag_Status_Get(RCC_FLAG_PINRST) != RESET) && (RCC_Flag_Status_Get(RCC_FLAG_IWDGRST) == RESET)))
  {
    #ifdef STL_VERBOSE_POR
    printf("... Power-on or software reset, testing IWDG ... \r\n");
    #endif  /* STL_VERBOSE_POR */

    /* IWDG at debug mode */
    //RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_PWR, ENABLE);
   // DBG_ConfigPeriph(DBG_IWDG_STOP, ENABLE);

    /* Clear all flags before resuming test */
    RCC_Reset_Flag_Clear();
   
    /* Setup IWDG to minimum period */
    IWDG_Write_Protection_Disable();
    IWDG_Prescaler_Division_Set(IWDG_CONFIG_PRESCALER_DIV4);
    IWDG_Counter_Reload(1);
    IWDG_Key_Reload();
    IWDG_Enable();
    
    /* Wait for an independent watchdog reset */
    while(1)
    { }
  }
  else  /* Watchdog test or software reset triggered by application failure */
  {
    /* If WWDG only was set, re-start the complete test (indicates a reset triggered by safety routines */
    if ((RCC_Flag_Status_Get(RCC_FLAG_PINRST)  != RESET) 
     && (RCC_Flag_Status_Get(RCC_FLAG_IWDGRST) != RESET))
    {
      RCC_Reset_Flag_Clear();
      #ifdef STL_VERBOSE_POR
      printf("... IWDG reset, WDG test completed ... \r\n");
      #endif  /* STL_VERBOSE_POR */
    }
    else  /* If IWDG only was set, continue the test with WWDG test*/
    {
      RCC_Reset_Flag_Clear();
      #ifdef STL_VERBOSE_POR
      printf("...Unexpected Flag configuration, re-start WDG test... \r\n");
      #endif  /* STL_VERBOSE_POR */
      NVIC_SystemReset();
    } 
  }
}
/**
  * @}
  */

/******************* (C)  *****END OF FILE****/
