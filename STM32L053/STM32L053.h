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
    #define RCC_REGS  0x40021000

#define APB2_BASE 0x40010000

#define APB1_BASE 0x40000000

#define IOPORT_BASE 0x50000000
    #define GPIOA_REGS 0x50000000
    #define GPIOB_REGS 0x50000400
    #define GPIOC_REGS 0x50000800
    #define GPIOD_REGS 0x50000C00
    #define GPIOE_REGS 0x50001000

    #define GPIOH_REGS 0x50001C00


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
