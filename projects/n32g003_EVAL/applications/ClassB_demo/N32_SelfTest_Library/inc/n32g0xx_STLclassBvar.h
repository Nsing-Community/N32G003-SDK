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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STL_CLASS_B_VAR_H
#define __STL_CLASS_B_VAR_H

/* This avoids having multiply defined global variables */
#ifdef ALLOC_GLOBALS
#define EXTERN
#else
#define EXTERN extern
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __IAR_SYSTEMS_ICC__  /* IAR Compiler */
 /* Temporary RAM buffer used during transparent run-time tests */
  /* WARNING: Real reserved RAM location from 0x20000000 to 0x20000024*/
  __no_init EXTERN uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE] @ "RUN_TIME_RAM_BUF";

  /* RAM pointer for run-time tests */
  __no_init EXTERN uint32_t *pRunTimeRamChk     @ "RUN_TIME_RAM_PNT";
  __no_init EXTERN uint32_t *pRunTimeRamChkInv  @ "RUN_TIME_RAM_PNT";



  __no_init EXTERN uint32_t SelfTestFlag        @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t SelfTestFlagInv     @ "CLASS_B_RAM_REV";
   
  /* Counter for verifying correct program execution at start */
  __no_init EXTERN uint32_t CtrlFlowCnt          @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t CtrlFlowCntInv       @ "CLASS_B_RAM_REV";

  /* Counter for verifying correct program execution in interrupt */
  __no_init EXTERN uint32_t ISRCtrlFlowCnt       @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t ISRCtrlFlowCntInv    @ "CLASS_B_RAM_REV";
  
  /* LSI period measurement  */
  __no_init EXTERN uint32_t PeriodValue           @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t PeriodValueInv        @ "CLASS_B_RAM_REV";

  /* First measure stored as reference for run-time checks */
  __no_init EXTERN uint32_t StartUpClockFreq     @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t StartUpClockFreqInv  @ "CLASS_B_RAM_REV";

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  __no_init EXTERN uint32_t TickCounter          @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t TickCounterInv       @ "CLASS_B_RAM_REV";

  /* Indicates to the main routine a 100ms tick */
  __no_init EXTERN __IO uint32_t TimeBaseFlag        @ "CLASS_B_RAM";
  __no_init EXTERN __IO uint32_t TimeBaseFlagInv     @ "CLASS_B_RAM_REV";

  /* Stores the Control flow counter from one main loop to the other */
  __no_init EXTERN uint32_t LastCtrlFlowCnt      @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t LastCtrlFlowCntInv   @ "CLASS_B_RAM_REV";

  /* Pointer to FLASH for crc32 run-time tests */
  __no_init EXTERN uint32_t *p_RunCrc32Chk       @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t *p_RunCrc32ChkInv    @ "CLASS_B_RAM_REV";

  /* Reference 32-bit CRC for run-time tests */
  __no_init EXTERN uint32_t RefCrc32             @ "CLASS_B_RAM";
  __no_init EXTERN uint32_t RefCrc32Inv          @ "CLASS_B_RAM_REV";

  /* Magic pattern for Stack overflow in this array */
  __no_init EXTERN __IO uint32_t aStackOverFlowPtrn[4] @ "STACK_BOTTOM";
#endif  /* __IAR_SYSTEMS_ICC__ */


#ifdef __CC_ARM   /* KEIL Compiler */
  /* Temporary RAM buffer used during transparent run-time tests */
  /* WARNING: Uses RAM location from 0x20000000 to 0x20000020 included */
  EXTERN uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE] __attribute__((section("RUN_TIME_RAM_BUF")));

  /* RAM pointer for run-time tests */
  EXTERN uint32_t *pRunTimeRamChk        __attribute__((section("RUN_TIME_RAM_PNT")));
  EXTERN uint32_t *pRunTimeRamChkInv     __attribute__((section("RUN_TIME_RAM_PNT")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables would be cleared by the C startup */

  /* Counter for verifying correct program execution at start */
  EXTERN uint32_t SelfTestFlag             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t SelfTestFlagInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));
  
  EXTERN uint32_t CtrlFlowCnt             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t CtrlFlowCntInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Counter for verifying correct program execution in interrupt */
  EXTERN uint32_t ISRCtrlFlowCnt          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t ISRCtrlFlowCntInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

   /* LSI period measurement  */
  EXTERN uint32_t PeriodValue           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t PeriodValueInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));
  
  /* First frequency measure stored as reference for run-time checks */
  EXTERN uint32_t StartUpClockFreq        __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t StartUpClockFreqInv     __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  EXTERN uint32_t TickCounter             __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t TickCounterInv          __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Indicates to the main routine a 100ms tick */
  EXTERN __IO uint32_t TimeBaseFlag           __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN __IO uint32_t TimeBaseFlagInv        __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Stores the Control flow counter from one main loop to the other */
  EXTERN uint32_t LastCtrlFlowCnt         __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t LastCtrlFlowCntInv      __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Pointer to FLASH for crc32 run-time tests */
  EXTERN uint32_t *p_RunCrc32Chk          __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t *p_RunCrc32ChkInv       __attribute__((section("CLASS_B_RAM_REV"), zero_init));

/* Reference 32-bit CRC for run-time tests */
  EXTERN uint32_t RefCrc32                __attribute__((section("CLASS_B_RAM"), zero_init));
  EXTERN uint32_t RefCrc32Inv             __attribute__((section("CLASS_B_RAM_REV"), zero_init));

  /* Magic pattern for Stack overflow in this array */
  EXTERN __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section("STACK_BOTTOM"), zero_init));
#endif  /* __CC_ARM */

#ifdef __GNUC__   /* GCC Compiler */
  #ifndef ALLOC_GLOBALS
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  extern uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section(".run_time_ram_buf")));

  /* RAM pointer for run-time tests */
  extern uint32_t *pRunTimeRamChk        __attribute__((section(".run_time_ram_pnt")));
  extern uint32_t *pRunTimeRamChkInv     __attribute__((section(".run_time_ram_pnt")));
  extern uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section(".run_time_ram_pnt")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  extern uint32_t CtrlFlowCnt             __attribute__((section (".class_b_ram")));
  extern uint32_t CtrlFlowCntInv          __attribute__((section (".class_b_ram_rev")));

  /* Counter for verifying correct program execution in interrupt */
  extern uint32_t ISRCtrlFlowCnt          __attribute__((section(".class_b_ram")));
  extern uint32_t ISRCtrlFlowCntInv       __attribute__((section(".class_b_ram_rev")));

  /* LSI period measurement at TIM5 IRQHandler */
  extern uint32_t PeriodValue           __attribute__((section(".class_b_ram")));
  extern uint32_t PeriodValueInv        __attribute__((section(".class_b_ram_rev")));

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  extern uint32_t TickCounter             __attribute__((section(".class_b_ram")));
  extern uint32_t TickCounterInv          __attribute__((section(".class_b_ram_rev")));

  /* Indicates to the main routine a 100ms tick */
  extern __IO uint32_t TimeBaseFlag       __attribute__((section(".class_b_ram")));
  extern __IO uint32_t TimeBaseFlagInv    __attribute__((section(".class_b_ram_rev")));

  /* Indicates to the main routine a 100ms tick */
  extern __IO uint32_t LSIPeriodFlag      __attribute__((section(".class_b_ram")));
  extern __IO uint32_t LSIPeriodFlagInv   __attribute__((section(".class_b_ram_rev")));

  /* Stores the Control flow counter from one main loop to the other */
  extern uint32_t LastCtrlFlowCnt         __attribute__((section(".class_b_ram")));
  extern uint32_t LastCtrlFlowCntInv      __attribute__((section(".class_b_ram_rev")));

  /* Pointer to FLASH for crc32 run-time tests */
  extern uint32_t *pRunCrc32Chk           __attribute__((section(".class_b_ram")));
  extern uint32_t *pRunCrc32ChkInv        __attribute__((section(".class_b_ram_rev")));

/* Reference 32-bit CRC for run-time tests */
  extern uint32_t RefCrc32                __attribute__((section(".class_b_ram")));
  extern uint32_t RefCrc32Inv             __attribute__((section(".class_b_ram_rev")));

  /* Magic pattern for Stack overflow in this array */
  extern __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section(".stack_bottom")));
  #else
  /* RAM location for temporary storage of original values at run time RAM transparent test */
  uint32_t aRunTimeRamBuf[RT_RAM_BLOCKSIZE + 2] __attribute__((section(".run_time_ram_buf")));

  /* RAM pointer for run-time tests */
  uint32_t *pRunTimeRamChk        __attribute__((section(".run_time_ram_pnt")));
  uint32_t *pRunTimeRamChkInv     __attribute__((section(".run_time_ram_pnt")));
  uint32_t aGAP_FOR_RAM_TEST_OVERLAY[2] __attribute__((section(".run_time_ram_pnt")));

  /* Note: the zero_init forces the linker to place variables in the bss section */
  /* This allows the UNINIT directive (in scatter file) to work. On the contrary */
  /* all Class B variables pairs should be initialized properly by user before using them */

  /* Counter for verifying correct program execution at start */
  uint32_t CtrlFlowCnt             __attribute__((section (".class_b_ram"))) = { 0 };
  uint32_t CtrlFlowCntInv          __attribute__((section (".class_b_ram_rev"))) = { 0 };

  /* Counter for verifying correct program execution in interrupt */
  uint32_t ISRCtrlFlowCnt          __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t ISRCtrlFlowCntInv       __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* LSI period measurement at TIM5 IRQHandler */
  uint32_t PeriodValue           __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t PeriodValueInv        __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Sofware time base used in main program (incremented in SysTick timer ISR */
  uint32_t TickCounter             __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t TickCounterInv          __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Indicates to the main routine a 100ms tick */
  __IO uint32_t TimeBaseFlag       __attribute__((section(".class_b_ram"))) = { 0 };
  __IO uint32_t TimeBaseFlagInv    __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Indicates to the main routine a 100ms tick */
  __IO uint32_t LSIPeriodFlag      __attribute__((section(".class_b_ram"))) = { 0 };
  __IO uint32_t LSIPeriodFlagInv   __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Stores the Control flow counter from one main loop to the other */
  uint32_t LastCtrlFlowCnt         __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t LastCtrlFlowCntInv      __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Pointer to FLASH for crc32 run-time tests */
  uint32_t *pRunCrc32Chk           __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t *pRunCrc32ChkInv        __attribute__((section(".class_b_ram_rev"))) = { 0 };

/* Reference 32-bit CRC for run-time tests */
  uint32_t RefCrc32                __attribute__((section(".class_b_ram"))) = { 0 };
  uint32_t RefCrc32Inv             __attribute__((section(".class_b_ram_rev"))) = { 0 };

  /* Magic pattern for Stack overflow in this array */
  __IO uint32_t aStackOverFlowPtrn[4]   __attribute__((section(".stack_bottom"))) = { 0 };
  #endif /* ALLOC_GLOBALS */
#endif  /* __GNUC__ */

#endif /* __STL_CLASS_B_VAR_H */

/******************* (C)  *****END OF FILE****/
