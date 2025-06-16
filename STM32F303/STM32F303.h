// header with STM32F303 specific stuff...
//



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

#define AHB3_BASE 0x50000000
    #define ADC1_REGS           0x50000000
    #define ADC2_REGS           0x50000100
    #define ADC1_2_COMMON_REGS   0x50000300
    #define ADC3_REGS           0x50000400
    #define ADC4_REGS           0x50000500
    #define ADC3_4_COMMON_REGS   0x50000700

//cortex M4 internal peripherals
#define CORTEX_BASE 0xE0000000

// ******************** ADC
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

typedef struct
{
    volatile uint32_t   CSR;
    volatile uint32_t   res0;
    volatile uint32_t   CCR;
    volatile uint32_t   CDR;
} ADC_common_map;
#define ADC1_2_COMMON ((ADC_common_map *) ADC1_2_COMMON_REGS)
#define ADC3_4_COMMON ((ADC_common_map *) ADC3_4_COMMON_REGS)

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

typedef struct
{
    volatile uint32_t    CR;         //0x00
    volatile uint32_t    CFGR;       //0x04
    volatile uint32_t    CIR;        //0x08
    volatile uint32_t    APB2RSTR;   //0x0C
    volatile uint32_t    APB1RSTR;   //0x10
    volatile uint32_t    AHBENR;     //0x14
    volatile uint32_t    APB2ENR;    //0x18
    volatile uint32_t    APB1ENR;    //0x1C
    volatile uint32_t    BDCR;       //0x20
    volatile uint32_t    CSR;        //0x24
    volatile uint32_t    AHBRSTR;    //0x28
    volatile uint32_t    CFGR2;      //0x2C
    volatile uint32_t    CFGR3;      //0x30
} RCC_map;
#define RCC ((volatile RCC_map *) RCC_REGS)



#endif
