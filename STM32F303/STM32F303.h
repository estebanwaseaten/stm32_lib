// header with STM32F303 specific stuff...
//

#include "STM32F303RE.h"        //if??

#ifndef STM32F303_H
#define STM32F303_H

// *** STM32F303xD/STM32F303xE memory map:

//flash memory:
#define FLASH_BASE  0x08000000
//sram memory base:
#define SRAM_BASE   0x20000000
//Peripherals:
#define PERI_BASE   0x40000000

#define APB1_BASE 0x40000000
    #define TIM2_REGS   0x40000000
    #define TIM3_REGS   0x40000400
    #define TIM4_REGS   0x40000800
    #define TIM6_REGS   0x40001000
    #define TIM7_REGS   0x40001400
    #define RTC_REGS    0x40002800
    #define WWDG_REGS   0x40002C00
    #define IWDG_REGS   0x40003000
    #define I2S2EXT_REGS    0x40003400
    #define SPI2I2S2_REGS   0x40003800
    #define SPI3I2S3_REGS   0x40003C00
    #define I2S3EXT_REGS    0x40004000
    #define USART2_REGS 0x40004400
    #define USART3_REGS 0x40004800
    #define UART4_REGS  0x40004C00
    #define UART5_REGS  0x40005000
    #define I2C1_REGS   0x40005400
    #define I2C2_REGS   0x40005800
    #define USBDEV_REGS 0x40005C00
    #define USBRAM_REGS 0x40006000
    #define BXCAN_REGS  0x40006400
    #define PWR_REGS    0x40007000
    #define DAC_REGS    0x40007400
    #define I2C3_REGS   0x40007800

#define APB2_BASE 0x40010000
    #define SYSCFG_REGS 0x40010000
    #define EXTI_REGS   0x40010400
    #define TIM1_REGS   0x40012C00
    #define SPI1_REGS   0x40013000
    #define TIM8_REGS   0x40013400
    #define USART1_REGS 0x40013800
    #define SPI4_REGS   0x40013C00
    #define TIM15_REGS  0x40014000
    #define TIM16_REGS  0x40014400
    #define TIM17_REGS  0x40014800
    #define TIM20_REGS  0x40015000

#define AHB1_BASE 0x40020000
    #define DMA1_REGS     0x40020000
    #define DMA2_REGS     0x40020400
    #define RCC_REGS      0x40021000
    #define FLASHINT_REGS 0x40022000
    #define CRC_REGS      0x40023000
    #define TSC_REGS      0x40024000

#define AHB2_BASE 0x48000000
    #define GPIOA_REGS  0x48000000
    #define GPIOB_REGS  0x48000400
    #define GPIOC_REGS  0x48000800
    #define GPIOD_REGS  0x48000C00
    #define GPIOE_REGS  0x48001000
    #define GPIOF_REGS  0x48001400
    #define GPIOG_REGS  0x48001800
    #define GPIOH_REGS  0x48001C00

#define AHB3_BASE 0x50000000                    //AHB3 0x5000 0000 - 0x5000 07FF
    #define ADC1_REGS           0x50000000
    #define ADC2_REGS           0x50000100
    #define ADC1_2_COMMON_REGS   0x50000300

    #define ADC3_REGS           0x50000400
    #define ADC4_REGS           0x50000500
    #define ADC3_4_COMMON_REGS   0x50000700


// INTERRUPT CONSTANTS
#define NUM_VECTORS     100
#define TIM1_BRK_IRQ    24
#define TIM1_UP_IRQ     25
#define TIM1_TRG_IRQ    26
#define TIM1_CC_IRQ     27
#define TIM2_IRQ 28
#define TIM3_IRQ 29
#define TIM4_IRQ 30
#define SPI1_IRQ 35
#define SPI2_IRQ 36
#define USART1_IRQ 36
#define SPI3_IRQ 51
#define SPI4_IRQ 84
#define DMA1_CH1_IRQ 11
#define DMA1_CH2_IRQ 12
#define DMA1_CH3_IRQ 13
#define DMA1_CH4_IRQ 14
#define DMA1_CH5_IRQ 15
#define DMA1_CH6_IRQ 16
#define DMA1_CH7_IRQ 17
#define DMA2_CH1_IRQ 56
#define DMA2_CH2_IRQ 57
#define DMA2_CH3_IRQ 58
#define DMA2_CH4_IRQ 59
#define DMA2_CH5_IRQ 60
#define ADC1_2_IRQ 18
#define ADC3_IRQ 47
#define ADC4_IRQ 61


//cortex M4 internal peripherals
#define CORTEX_BASE 0xE0000000
    #define CORTEX_SYSCTRL   0xE000E000
    #define CORTEX_SYSTICK   0xE000E010
    #define CORTEX_NVIC_ISER 0xE000E100
    #define CORTEX_NVIC_ICER 0xE000E180
    #define CORTEX_NVIC_ISPR 0xE000E200
    #define CORTEX_NVIC_ICPR 0xE000E280
    #define CORTEX_NVIC_IABR 0xE000E300
    #define CORTEX_NVIC_IPR  0xE000E400
    #define CORTEXT_SCB_BASE 0xE000ED00
    #define CORTEX_NVIC_STIR 0xE000EF00



typedef struct
{
    volatile uint32_t CSR;    //sysTick control reg.
    volatile uint32_t RVR;    //sysTick reload value reg.
    volatile uint32_t CVR;    //sysTick current value reg.
    volatile uint32_t CR;     //sysTick calibration value reg
} SYSTICK_map;
#define SYSTICK ((SYSTICK_map *) CORTEX_SYSTICK)


// NVIC interrupt banks:
typedef struct
{
    volatile uint32_t   BANK[8];  //
} IRQ_map;
#define NVIC_ISER ((IRQ_map *) CORTEX_NVIC_ISER)
#define NVIC_ICER ((IRQ_map *) CORTEX_NVIC_ICER)
#define NVIC_ISPR ((IRQ_map *) CORTEX_NVIC_ISPR)
#define NVIC_ICPR ((IRQ_map *) CORTEX_NVIC_ICPR)
#define NVIC_IABR ((IRQ_map *) CORTEX_NVIC_IABR)

typedef struct
{
    volatile uint32_t   BANK[60];  //
} IRQ_PRIO_map;
#define NVIC_IPR ((IRQ_PRIO_map *) CORTEX_NVIC_IPR)



// system control
typedef struct
{
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t SHPR1;
    volatile uint32_t SHPR2;
    volatile uint32_t SHPR3;
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
    volatile uint32_t res1;
    //continues....
} SCS_map;
#define SCB ((SCS_map *) CORTEXT_SCB_BASE)


// TIMERS
typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
    volatile uint32_t OR;
    volatile uint32_t CCMR3;
    volatile uint32_t CCR5;
    volatile uint32_t CCR6;
} TIMER_adv_map;

typedef struct
{
    volatile uint32_t CR1;          //0x00
    volatile uint32_t CR2;          //0x04
    volatile uint32_t SMCR;         //0x08 not for basic timers
    volatile uint32_t DIER;         //0x0C
    volatile uint32_t SR;           //0x10
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;        //not for basic timers
    volatile uint32_t CCMR2;        //not for basic timers
    volatile uint32_t CCER;         //0x20 not for basic timers
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t res1;         //0x30 not for basic timers
    volatile uint32_t CCR1;         //not for basic timers
    volatile uint32_t CCR2;         //not for basic timers
    volatile uint32_t CCR3;         //not for basic timers
    volatile uint32_t CCR4;         //0x40 not for basic timers
    volatile uint32_t res2;
    volatile uint32_t DCR;          //not for basic timers
    volatile uint32_t DMAR;         //not for basic timers
} TIMER_gp_map;

#define TIM2 ((TIMER_gp_map *) TIM2_REGS)
#define TIM3 ((TIMER_gp_map *) TIM3_REGS)
#define TIM4 ((TIMER_gp_map *) TIM4_REGS)

// TIMERS
typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t res1;         //not for basic timers
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t res2;        //not for basic timers
    volatile uint32_t res3;        //not for basic timers
    volatile uint32_t res4;         //not for basic timers
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;

} TIMER_basic_map;

// DMA
// DMA registers that repeat per DMA channel:
typedef struct
{
    volatile uint32_t CCR;      //0x0
    volatile uint32_t CNDTR;    //0x4
    volatile uint32_t CPAR;     //0x8
    volatile uint32_t CMAR;     //0xC
    uint32_t res1;      //0x10 dont access --> out of bounds for last
} DMA_channel_map;

// DMA1 and 2 including repeated channel structure
typedef struct
{
    volatile uint32_t ISR;      // DMA interrupt status register
    volatile uint32_t IFCR;     // DMA interrupt flag clear register

    DMA_channel_map CH[7];     //starting at 0x008, 0x01C, 0x030...    THIS is an array
} DMA_map;
#define DMA1 ((DMA_map *) DMA1_REGS)
#define DMA2 ((DMA_map *) DMA2_REGS)



// FLASH
typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_map;
#define FLASH ((FLASH_map *)FLASHINT_REGS)


// ******************** ADC
// 4 ADSc with max. 19 channels each
// mapped on AHB bus
// 0.19us for 12bit, 0.16us for 10bit resolution
// up to 5 fast analog input channels from GPIO
typedef struct
{
    volatile uint32_t   ISR;    //0x00
    volatile uint32_t   IER;    //0x04
    volatile uint32_t   CR;    //0x08
    volatile uint32_t   CFGR;    //0x0C
    volatile uint32_t   res0;    //0x10 RESERVEDs
    volatile uint32_t   SMPR1;    //0x14
    volatile uint32_t   SMPR2;    //0x18
    volatile uint32_t   res1;    //0x1C
    volatile uint32_t   TR1;    //0x20
    volatile uint32_t   TR2;    //0x24
    volatile uint32_t   TR3;    //0x28
    volatile uint32_t   res2;    //0x2C
    volatile uint32_t   SQR1;    //0x30
    volatile uint32_t   SQR2;    //0x34
    volatile uint32_t   SQR3;    //0x38
    volatile uint32_t   SQR4;    //0x3C
    volatile uint32_t   DR;    //0x40
    volatile uint32_t   res3;    //0x44
    volatile uint32_t   res4;    //0x48
    volatile uint32_t   JSQR;    //0x4C
    volatile uint32_t   res5;    //0x50
    volatile uint32_t   res6;    //0x54
    volatile uint32_t   res7;    //0x58
    volatile uint32_t   res8;    //0x5C
    volatile uint32_t   OFR1;    //0x60
    volatile uint32_t   OFR2;    //0x64
    volatile uint32_t   OFR3;    //0x68
    volatile uint32_t   OFR4;    //0x6C
    volatile uint32_t   res9;    //0x70
    volatile uint32_t   res10;    //0x74
    volatile uint32_t   res11;    //0x78
    volatile uint32_t   res12;    //0x7C
    volatile uint32_t   JDR1;    //0x80
    volatile uint32_t   JDR2;    //0x84
    volatile uint32_t   JDR3;    //0x88
    volatile uint32_t   JDR4;    //0x8C
    volatile uint32_t   res13;    //0x90
    volatile uint32_t   res14;    //0x94
    volatile uint32_t   res15;    //0x98
    volatile uint32_t   res16;    //0x9C
    volatile uint32_t   AWD2CR;    //0xA0
    volatile uint32_t   AWD3CR;    //0xA4
    volatile uint32_t   res17;    //0xA8
    volatile uint32_t   res18;    //0xAC
    volatile uint32_t   DIFSEL;    //0xB0
    volatile uint32_t   CALFACT;    //0xB4
} ADC_map;
#define ADC1 ((ADC_map *) ADC1_REGS)
#define ADC2 ((ADC_map *) ADC2_REGS)
#define ADC3 ((ADC_map *) ADC3_REGS)
#define ADC4 ((ADC_map *) ADC4_REGS)

extern volatile ADC_map * const ADC[5];    //needs to be initialized

typedef struct
{
    volatile uint32_t   CSR;        // ADC Common status register
    volatile uint32_t   res0;
    volatile uint32_t   CCR;        // ADC common control register
    volatile uint32_t   CDR;        // ADC common regular data register for dual mode
} ADC_common_map;
#define ADC1_2_COMMON ((ADC_common_map *) ADC1_2_COMMON_REGS)
#define ADC3_4_COMMON ((ADC_common_map *) ADC3_4_COMMON_REGS)

extern volatile ADC_common_map * const ADC_common[5];    //needs to be initialized


typedef struct
{
    volatile uint32_t   MODER;      //0x00
    volatile uint32_t   OTYPER;     //0x04
    volatile uint32_t   OSPEEDR;    //0x08
    volatile uint32_t   PUPDR;      //0x0C
    volatile uint32_t   IDR;        //0x10
    volatile uint32_t   ODR;        //0x14
    volatile uint32_t   BSRR;       //0x18
    volatile uint32_t   LCKR;       //0x1C
    volatile uint32_t   AFRL;       //0x20
    volatile uint32_t   AFRH;       //0x24
    volatile uint32_t   BRR;        //0x28
} GPIO_map;
#define GPIOA ((GPIO_map *) GPIOA_REGS)
#define GPIOB ((GPIO_map *) GPIOB_REGS)

//RCC = reset and clock control
typedef struct
{
    volatile uint32_t    CR;         //0x00     Clock control register
    volatile uint32_t    CFGR;       //0x04     Clock configuration register
    volatile uint32_t    CIR;        //0x08     Clock interrupt register
    volatile uint32_t    APB2RSTR;   //0x0C     APB2 peripheral reset registe
    volatile uint32_t    APB1RSTR;   //0x10     APB1 peripheral reset registe
    volatile uint32_t    AHBENR;     //0x14     AHB peripheral clock enable register
    volatile uint32_t    APB2ENR;    //0x18     APB2 peripheral clock enable register (TIMERs, USART, SPI, SYSCFGEN)
    volatile uint32_t    APB1ENR;    //0x1C     APB1 peripheral clock enable register (DAC, PWR, CAN, USB, I2C, UART, USART, SPI, WWD, TIMERs)
    volatile uint32_t    BDCR;       //0x20     RTC domain control register
    volatile uint32_t    CSR;        //0x24     Control/status register
    volatile uint32_t    AHBRSTR;    //0x28     AHB peripheral reset registe
    volatile uint32_t    CFGR2;      //0x2C     Clock configuration register 2  (contains ADC prescalers)
    volatile uint32_t    CFGR3;      //0x30     Clock configuration register 3
} RCC_map;
#define RCC ((volatile RCC_map *) RCC_REGS)     //RCC contains a POINTER!!!


typedef struct
{
    volatile uint32_t   CR1;        //
    volatile uint32_t   CR2;        //
    volatile uint32_t   SR;         //
    volatile uint32_t   DR;         //

    volatile uint32_t   CRCPR;       //
    volatile uint32_t   RXCRCR;      //
    volatile uint32_t   TXCRCR;      //
    volatile uint32_t   I2SCFGR;     //
    volatile uint32_t   I2SPR;       //
} SPI_map;
#define SPI1 ((SPI_map *) SPI1_REGS)
#define SPI2 ((SPI_map *) SPI2I2S2_REGS)
#define SPI3 ((SPI_map *) SPI3I2S3_REGS)
#define SPI4 ((SPI_map *) SPI4_REGS)

//generate array for easier access!
extern volatile SPI_map * const SPI[5];    //needs to be initialized



#endif
