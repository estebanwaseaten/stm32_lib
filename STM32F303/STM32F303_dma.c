#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


// ADC1 -> DMA1 ch1
// ADC2 -> DMA2 ch1
// ADC3 -> DMA2 ch5
// ADC4 -> DMA2 ch2

extern uint32_t gDataLength;


//init clocks and reset interrupts
void DMA_init( bool dma1, bool dma2 )
{
    if( dma1 )
    {
        SETBIT( RCC->AHBENR, 0 );       //enable DMA1
        (void)RCC->AHBENR;              // read-back to flush/ensure clock gate opened
        SETWRD( DMA1->IFCR, 0xFFFFFFFF );      //clear all DMA interrupt flags (write to clear register)
    }

    if( dma2 )
    {
        SETBIT( RCC->AHBENR, 1 );       //enable DMA2
        (void)RCC->AHBENR;              // read-back to flush/ensure clock gate opened
        SETWRD( DMA2->IFCR, 0xFFFFFFFF );      //clear all DMA interrupt flags (write to clear register)
    }
}

void DMA_setup( bool dual )      //should be less specific?, DMA 1 or 2, channel?...
{
    CLRWRD( DMA1->CH[0].CCR );      //disable DMA
    (void)DMA1->CH[0].CCR;          //read back

    SETBITS( DMA1->CH[0].CCR, 0x3, 12 );     // set priority to very high
    if( dual )
    {
        SETBITS( DMA1->CH[0].CCR, 0x2, 10 );     // set MSIZE to 32bits
        SETBITS( DMA1->CH[0].CCR, 0x2, 8 );      // set PSIZE to 32bits
    }
    else
    {
        SETBITS( DMA1->CH[0].CCR, 0x1, 10 );     // set MSIZE to 16bits
        SETBITS( DMA1->CH[0].CCR, 0x1, 8 );      // set PSIZE to 16bits
    }

    SETBIT( DMA1->CH[0].CCR, 7 );            // set Memory Increment Mode
    //SETBIT( DMA1->CH[0].CCR, 5)            // DO NOT set circular mode
    SETBIT( DMA1->CH[0].CCR, 1 );            // set transfer complete interrupt enable

    //setup datatransfer scheme

    if( dual )
    {
        SETWRD( DMA1->CH[0].CPAR, (uint32_t)&ADC1_2_COMMON->CDR );           //SOURCE: peripheral address
    }
    else
    {
        SETWRD( DMA1->CH[0].CPAR, (uint32_t)&ADC1->DR );           //SOURCE: peripheral address
    }


    SETWRD( DMA1->CH[0].CMAR, MEMBASE12 );                      //DESTINATION: memory start address
    SETWRD( DMA1->CH[0].CNDTR, gDataLength );                  //number of data transfers
    //second channel for ADCs 3 und 4
    //SETWRD( DMA2->CH[0].CMAR, MEMBASE34 );                      //DESTINATION: memory start address
    //SETWRD( DMA2->CH[0].CNDTR, gDataLength );
}

void DMA_reset( uint32_t DMAnum, uint32_t dma_channel )
{
    uint32_t chan = dma_channel - 1;
    if( DMAnum == 1 )
    {
        CLRBIT( DMA1->CH[chan].CCR, 0 );      //disable DMA
        (void)DMA1->CH[chan].CCR;          //read back
        SETWRD( DMA1->CH[chan].CNDTR, gDataLength );
    }
    else if( DMAnum == 2 )
    {
        CLRBIT( DMA2->CH[chan].CCR, 0 );      //disable DMA
        (void)DMA2->CH[chan].CCR;          //read back
        SETWRD( DMA2->CH[chan].CNDTR, gDataLength );
    }

    DMA_enable( DMAnum, dma_channel );
}

void DMA_enable( uint32_t DMAnum, uint32_t dma_channel )
{
    if( DMAnum == 1 )
    {
        SETBIT( DMA1->CH[dma_channel - 1].CCR, 0 );            // enable DMA channel
    }
    else if ( DMAnum == 2 )
    {
        SETBIT( DMA2->CH[dma_channel - 1].CCR, 0 );
    }
}

void DMA_enable_interrupt( uint32_t DMAnum, uint32_t dma_channel )
{
    uint32_t interrupt = 0;
    if( DMAnum == 1 )
    {
        interrupt = DMA1_CH1_IRQ + dma_channel - 1;
    }
    else if( DMAnum == 2 )
    {
        interrupt = DMA2_CH1_IRQ + dma_channel - 1;
    }

    uint32_t bank = (interrupt >> 5);
    SETBIT( NVIC_ISER->BANK[bank], interrupt - bank*32);
    //SETBIT( NVIC_ISPR->BANK[bank], DMA1_CH1_IRQ - bank*32);     //this sets the interrupt manuelly for testing
}

void DMA_clear_interrupts( uint32_t DMAnum )
{
      SETWRD( DMA1->IFCR, 0xFFFFFFFF );
}
