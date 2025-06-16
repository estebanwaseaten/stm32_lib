// header with STM32F303 specific stuff...
//

#include <stdint.h>

#ifndef STM32L053_H
#define STM32L053_H

//should we share this??
//flash memory:
#define FLASH_BASE  0x08000000
//sram memory base:
#define SRAM_BASE   0x20000000
//Peripherals:
#define PERI_BASE   0x40000000

#define AHB1_BASE 0x40020000
    #define DMA1_REGS   0x40020000
    #define RCC_REGS    0x40021000
    #define FLASH_REGS  0x40022000

#define APB2_BASE 0x40010000
    #define ADC1_REGS   0x40012400
    #define ADC1_COMMON 0x40012708
    #define SPI1REGS    0x40013000
    #define USART1_REGS 0x40013800
    #define DBG_REGS    0x40015800

#define APB1_BASE 0x40000000

#define IOPORT_BASE 0x50000000
    #define GPIOA_REGS 0x50000000
    #define GPIOB_REGS 0x50000400
    #define GPIOC_REGS 0x50000800
    #define GPIOD_REGS 0x50000C00
    #define GPIOE_REGS 0x50001000

    #define GPIOH_REGS 0x50001C00



// ******************** ADC
typedef struct
{
    volatile uint32_t ISR;      //0x00 - status & interrupt (bit0 -> adc ready)
    volatile uint32_t IER;      //0x04 - interrupt enable
    volatile uint32_t CR;       //0x08 - control (!)
    volatile uint32_t CFGR1;    //0x0C

    volatile uint32_t CFGR2;    //0x10
    volatile uint32_t SMPR;     //0x14 - sampling time (1,5 - 160,5 clock cycles)
    volatile uint32_t res1;     //0x18
    volatile uint32_t res2;     //0x1C

    volatile uint32_t TR;       //0x20- watchdog threshold - trigger???
    volatile uint32_t res3;     //0x24
    volatile uint32_t CHSELR;   //0x28 - channel select
    volatile uint32_t res4;     //0x2C

    volatile uint32_t res5[4];  //0x30

    volatile uint32_t DR;       //0x40 - data register
    volatile uint32_t res6;
    volatile uint32_t res7;
    volatile uint32_t res8;     //0x4C

    volatile uint32_t res9[4];  //0x50
    volatile uint32_t res10[4]; //0x60
    volatile uint32_t res11[4]; //0x70
    volatile uint32_t res12[4]; //0x80
    volatile uint32_t res13[4]; //0x90
    volatile uint32_t res14[4]; //0xA0

    volatile uint32_t res15;    //0xB0
    volatile uint32_t CALFACT;  //0xB4 - calibration factor
} ADC_map;
#define ADC ((ADC_map *) ADC1_REGS)

typedef struct
{
    volatile uint32_t CCR;
} ADC_com_map;
#define ADC_COM ((ADC_com_map *) ADC1_COMMON)

// ******************** SPI
typedef struct
{
    volatile uint32_t   CR1;      //0x00
    volatile uint32_t   CR2;      //0x04
    volatile uint32_t   SR;       //0x08
    volatile uint32_t   DR;       //0x0C

    volatile uint32_t   CRCPR;      //0x10
    volatile uint32_t   RXCRCR;     //0x14
    volatile uint32_t   TXCRCR;     //0x18
    volatile uint32_t   I2SCFGR;    //0x1C

    volatile uint32_t   I2SPR;      //0x20
} SPI_map;
#define SPI ((SPI_map *) SPI1REGS)

// ******************** GPIO
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
//...


// ******************** DMA
typedef struct
{
    volatile uint32_t    ISR;         //0x00
    volatile uint32_t    IFCR;        //0x04
    volatile uint32_t    CCR1;        //0x08
    volatile uint32_t    CNDTR1;      //0x0C

    volatile uint32_t    CPAR1;      //0x10
    volatile uint32_t    CMAR1;      //0x14
    volatile uint32_t    res1;       //0x18
    volatile uint32_t    CCR2;       //0x1C

    volatile uint32_t    CNDTR2;      //0x20
    volatile uint32_t    CPAR2;       //0x24
    volatile uint32_t    CMAR2;       //0x28
    volatile uint32_t    res2;        //0x2C

    volatile uint32_t    CCR3;        //0x30
    volatile uint32_t    CNDTR3;      //0x34
    volatile uint32_t    CPAR3 ;      //0x38
    volatile uint32_t    CMAR3 ;      //0x3C

    volatile uint32_t    res3;        //0x40
    volatile uint32_t    CCR4;      //0x44
    volatile uint32_t    CNDTR4;      //0x48
    volatile uint32_t    CPAR4;      //0x4C

    volatile uint32_t    CMAR4;      //0x50
    volatile uint32_t    res4;       //0x54
    volatile uint32_t    CCR5;      //0x58
    volatile uint32_t    CNDTR5;      //0x5C

    volatile uint32_t    CPAR5;      //0x60
    volatile uint32_t    CMAR5;      //0x64
    volatile uint32_t    res5;       //0x68
    volatile uint32_t    CCR6;       //0x6C

    volatile uint32_t    CNDTR6;     //0x70
    volatile uint32_t    CPAR6;      //0x74
    volatile uint32_t    CMAR6;      //0x78
    volatile uint32_t    res6;       //0x7C

    volatile uint32_t    CCR7;       //0x80
    volatile uint32_t    CNDTR7;     //0x84
    volatile uint32_t    CPAR7;      //0x88
    volatile uint32_t    CMAR7;      //0x8C

    volatile uint32_t    res7[4];    //0x90

    volatile uint32_t    res8;       //0xA0
    volatile uint32_t    res9;       //0xA4
    volatile uint32_t    CSELR;      //0xA8
} DMA_map;
#define DMA ((DMA_map *) DMA1_REGS)


// ******************** RCC
typedef struct
{
    volatile uint32_t    CR;         //0x00
    volatile uint32_t    ICSCR;
    volatile uint32_t    CRRCR;
    volatile uint32_t    CFGR;
    volatile uint32_t    CIER;       //0x10
    volatile uint32_t    CIFR;
    volatile uint32_t    CICR;
    volatile uint32_t    IOPRSTR;
    volatile uint32_t    AHBRSTR;    //0x20
    volatile uint32_t    APB2RSTR;
    volatile uint32_t    APB1RSTR;
    volatile uint32_t    IOPENR;
    volatile uint32_t    AHBENR;    //0x30
    volatile uint32_t    APB2ENR;
    volatile uint32_t    APB1ENR;
    volatile uint32_t    IOPSMEN;
    volatile uint32_t    AHBSMENR;  //0x40
    volatile uint32_t    APB2SMENR;
    volatile uint32_t    APB1SMENR;
    volatile uint32_t    CCIPR;
    volatile uint32_t    CSR;       //0x50
} RCC_map;
#define RCC ((RCC_map *) RCC_REGS)

#endif
