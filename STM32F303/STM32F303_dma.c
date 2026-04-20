#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


// ADC1 -> DMA1 ch1
// ADC2 -> DMA2 ch1
// ADC3 -> DMA2 ch5
// ADC4 -> DMA2 ch2

uint32_t    gDMAbufferLength;
bool        gCircular;

//init clocks and reset interrupts
void DMA_start_clock( bool dma1, bool dma2 )
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

void DMA_setup_peri( uint32_t dma, uint32_t chan, volatile uint32_t *sourceAddr, uint32_t *destAddr, uint32_t bitSize, bool circ, uint32_t bufferLength )      //should be less specific?, DMA 1 or 2, channel?...
{
    gDMAbufferLength = bufferLength;
    gCircular = circ;
    chan--;

    DMA_map *currentDMA = DMA1;
    if( dma == 2 )
    currentDMA = DMA2;

    CLRWRD( currentDMA->CH[chan].CCR );      //disable DMA
    (void)currentDMA->CH[chan].CCR;          //read back

    SETBITS( currentDMA->CH[chan].CCR, 0x3, 12 );     // set priority to very high
    if( bitSize == 32 )
    {
        SETBITS( currentDMA->CH[chan].CCR, 0x2, 10 );     // set MSIZE to 32bits
        SETBITS( currentDMA->CH[chan].CCR, 0x2, 8 );      // set PSIZE to 32bits
    }
    else if( bitSize == 16 )
    {
        SETBITS( currentDMA->CH[chan].CCR, 0x1, 10 );     // set MSIZE to 16bits
        SETBITS( currentDMA->CH[chan].CCR, 0x1, 8 );      // set PSIZE to 16bits
    }
    //else if( bitSize == 8 )?

    SETBIT( currentDMA->CH[chan].CCR, 7 );            // set Memory Increment Mode
    if( circ )
    {
        SETBIT( currentDMA->CH[chan].CCR, 5);              // set circular mode
    }
    SETBIT( currentDMA->CH[chan].CCR, 1 );            // set transfer complete interrupt enable
    SETBIT( currentDMA->CH[chan].CCR, 2 );            // set half transfer interrupt enable

    //setup datatransfer scheme
    SETWRD( currentDMA->CH[chan].CPAR, (uint32_t)sourceAddr );
    //(uint32_t)&ADC1_2_COMMON->CDR );          //SOURCE: peripheral address
    //(uint32_t)&ADC1->DR );                    //SOURCE: peripheral address

    SETWRD( currentDMA->CH[chan].CMAR, (uint32_t)destAddr );                      //DESTINATION: memory start address
    SETWRD( currentDMA->CH[chan].CNDTR, bufferLength );                  //number of data transfers
}

void DMA_reset( uint32_t DMAnum, uint32_t dma_channel )
{
    uint32_t chan = dma_channel - 1;
    if( DMAnum == 1 )
    {
        CLRBIT( DMA1->CH[chan].CCR, 0 );      //disable DMA
        (void)DMA1->CH[chan].CCR;          //read back
        SETWRD( DMA1->CH[chan].CNDTR, gDMAbufferLength );
    }
    else if( DMAnum == 2 )
    {
        CLRBIT( DMA2->CH[chan].CCR, 0 );      //disable DMA
        (void)DMA2->CH[chan].CCR;          //read back
        SETWRD( DMA2->CH[chan].CNDTR, gDMAbufferLength );
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
    if( DMAnum == 1 )
        SETWRD( DMA1->IFCR, 0xFFFFFFFF );
    else if( DMAnum == 2)
        SETWRD( DMA2->IFCR, 0xFFFFFFFF );
}

uint16_t DMA_get_pos( uint32_t  DMAnum, uint32_t dma_channel )
{
    if( DMAnum == 1 )
        return gDMAbufferLength - DMA1->CH[dma_channel - 1].CNDTR;

    return gDMAbufferLength - DMA2->CH[dma_channel - 1].CNDTR;
}
