#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>

//generate array for easier access!
volatile SPI_map * const SPI[5] = {
    NULL,
    [1] = (volatile SPI_map *)SPI1_REGS,
    [2] = (volatile SPI_map *)SPI2I2S2_REGS,
    [3] = (volatile SPI_map *)SPI3I2S3_REGS,
    [4] = (volatile SPI_map *)SPI4_REGS
};

uint8_t kSPIbitsPerWord[5] = {0, };


/************** SPI functions
 *  //interrupts
 *
 */

int SPI_init( uint32_t SPInum )         //enables BUS clock
{
    switch( SPInum )
    {
        case 1:
            if( !CHKBIT( RCC->APB2ENR, 12 ) )   //on APB2
            { SETBIT( RCC->APB2ENR, 12 ); }     //enable SPI1 clock
            break;
        case 2:
            if( !CHKBIT( RCC->APB1ENR, 14 ) )   //on APB1
            { SETBIT( RCC->APB1ENR, 14 ); }     //enable SPI2 clock
            break;
        case 3:
            if( !CHKBIT( RCC->APB1ENR, 15 ) )   //on APB1
            { SETBIT( RCC->APB1ENR, 15 ); }     //enable SPI3 clock
            break;
        case 4:
            if( !CHKBIT( RCC->APB2ENR, 15 ) )   //on APB2
            { SETBIT( RCC->APB2ENR, 15 ); }     //enable SPI4 clock
            break;
        default:
            return -1;
    }

    waitCycles(4);
    return 0;
}


//SPI_8BITSPERWORD or SPI_16BITSPERWORD
int SPI_enable( uint32_t SPInum, uint8_t bitsPerWord )
{   // possible mappings found in datasheet
    //setup pins:



    CLRBITS( GPIOA->MODER, 0x3, 5*2 );      //clear function
    CLRBITS( GPIOA->MODER, 0x3, 6*2 );      //clear function
    CLRBITS( GPIOA->MODER, 0x3, 7*2 );      //clear function

    SETBITS( GPIOA->MODER, 0x2, 5*2 );      //pin 5 alternate function
    SETBITS( GPIOA->MODER, 0x2, 6*2 );      //pin 6 alternate function
    SETBITS( GPIOA->MODER, 0x2, 7*2 );      //pin 7 alternate function

    // speed GPIOA->OSPEEDR: 11 (high speed) OUTPUT!!!
    CLRBIT( GPIOA->OTYPER, 5 );
    CLRBIT( GPIOA->OTYPER, 6 );
    CLRBIT( GPIOA->OTYPER, 7 );
    SETBITS( GPIOA->OSPEEDR, 0x3, 6*2 );    // MISO -> output
    CLRBITS( GPIOA->PUPDR, 0x3, 6*2 );


    //alternate function select: PA4, 5, 6 and 7: AF5 = 0101
    CLRBITS( GPIOA->AFRL, 0xF, 5*4 );
    CLRBITS( GPIOA->AFRL, 0xF, 6*4 );
    CLRBITS( GPIOA->AFRL, 0xF, 7*4 );

    SETBITS( GPIOA->AFRL, 0x5, 5*4 );
    SETBITS( GPIOA->AFRL, 0x5, 6*4 );
    SETBITS( GPIOA->AFRL, 0x5, 7*4 );

    //could enable cyclic redundancy check (CRC):
    //SETBIT( SPI1->CR1, 13 );
    //SETBIT( SPI1->CR1, 12 );
    //SETBIT( SPI1->CR1, 11 );

    //disable for config
    CLRWRD( SPI[1]->CR1 );
    CLRWRD( SPI[1]->CR2 );

    SETBIT( SPI[1]->CR1, 9 );         // SSM software slave management SSM enabled
    CLRBIT( SPI[1]->CR1, 8 );         // SSI the value of this bit is forced onto NSS pin

    //SETBIT( SPI[1]->CR1, 7 );      // LSB first
    CLRBIT( SPI[1]->CR1, 7 );        // MSB first

    CLRBIT( SPI[1]->CR1, 2 );         //slave configuration
    //SETBIT( SPI[1]->CR1, 2 );       //master configuration

    //clock polarity
    CLRBIT( SPI[1]->CR1, 1 );         //CK 0 when idle
    //SETBIT( SPI[1]->CR1, 1 );       //CK 1 when idle

    //clock phase
    CLRBIT( SPI[1]->CR1, 0 );         //The first clock transition is the first data capture edg
    //SETBIT( SPI[1]->CR1, 0 );       //The second clock transition is the first data capture edge

    CLRBITS( SPI[1]->CR2, 0xF, 8 );   //reset data size
    // also set FIFO reception threshold for 8 vs 16 bit transfer
    if( bitsPerWord == SPI_16BITSPERWORD )
    {
        SETBITS( SPI[1]->CR2, 0xF, 8 );   //set data size to 1111 = 16-bit
        CLRBIT( SPI[1]->CR2, 12 );        // FRXTH set to 1/2 FIFO (=16 bits)
        kSPIbitsPerWord[SPInum] = SPI_16BITSPERWORD;
    }
    else
    {
        SETBITS( SPI[1]->CR2, 0x7, 8 );   //set data size to 0111 = 8-bit
        SETBIT( SPI[1]->CR2, 12 );      // FRXTH set to 1/4 FIFO (=8 bits)
        kSPIbitsPerWord[SPInum] = SPI_8BITSPERWORD;
    }

    SETBIT( SPI[1]->CR1, 6 );        // SPI enable

    return 0;
}

//#define SPI_TXEI 4
//#define SPI_RXNEI 2
//#define SPI_ERRI 1

// available interrupts: Transmit TXFIFO ready to be loaded, Data received in RXFIFO, Master Mode fault event, Overrun error, TI frame format error, CRC protocol error
int SPI_enable_interrupt( uint32_t SPInum, int interruptType )
{
    if( interruptType > 7 )
        return -1;

    uint32_t bank;
    switch( SPInum )
    {
        case 1:
            (void)SPI1->SR; (void)SPI1->DR;
            SETBITS( SPI1->CR2, interruptType, 5 );
            bank = (SPI1_IRQ >> 5);
            SETBIT( NVIC_ISER->BANK[bank], SPI1_IRQ - bank*32);   //interrupt #35 is the SPI interrupt
            break;
        case 2:
            (void)SPI2->SR; (void)SPI2->DR;
            SETBITS( SPI2->CR2, interruptType, 5  );
            bank = (SPI2_IRQ >> 5);
            SETBIT( NVIC_ISER->BANK[bank], SPI2_IRQ - bank*32);   //interrupt #35 is the SPI interrupt
            break;
        case 3:
            (void)SPI3->SR; (void)SPI3->DR;
            SETBITS( SPI3->CR2, interruptType, 5  );
            bank = (SPI3_IRQ >> 5);
            SETBIT( NVIC_ISER->BANK[bank], SPI3_IRQ - bank*32);   //interrupt #35 is the SPI interrupt
            break;
        case 4:
            (void)SPI4->SR; (void)SPI4->DR;
            SETBITS( SPI4->CR2, interruptType, 5  );
            bank = (SPI4_IRQ >> 5);
            SETBIT( NVIC_ISER->BANK[bank], SPI4_IRQ - bank*32);   //interrupt #35 is the SPI interrupt
            break;
        default:
            return -1;
            break;
    }
    return 0;
}

int32_t SPI_receive( void )
{
    //setWord( 0x20009010, 0xAB );                    //testing
    uint32_t counter = getWord( 0x20009008 );       //testing
    setWord( 0x20009008, counter + 1 );             //testing

    //if( kSPIbitsPerWord[1] == SPI_16BITSPERWORD )

    uint16_t received = 0;
    //clear interrupt by reading:
    if( CHKBIT( SPI1->SR , 0 ) )
    {
        received = *(uint16_t *)&SPI1->DR;
//        setWord( 0x20009000, (uint32_t)received );
        *(uint16_t *)&SPI1->DR = received + 1;
        return received;
    }
    return -1;
}



//test
void SPI_test( void )
{
    setWord( 0x20009008, 0);    //counter

    while(1)
    {
        setWord( 0x2000900C, SPI1->SR );


        while( !CHKBIT( SPI1->SR, 1 )){}    //wait until transmit buffer is empty
        //*(uint8_t *)&SPI1->DR = 0xAA;
        *(uint8_t *)&SPI1->DR = 0xF0;
    //    *(uint8_t *)&SPI1->DR = 0x0F;
    //    *(uint8_t *)&SPI1->DR = 0xAA;
    //    *(uint8_t *)&SPI1->DR = 0xAA;
    //    *(uint8_t *)&SPI1->DR = 0xAA;
        // here FIFO is  full

        waitCycles(10);
        setWord( 0x20009004, SPI1->SR);


        while( !CHKBIT( SPI1->SR, 0 )){}     //wait until receive buffer is not empty
        //uint8_t received = *(uint8_t *)&SPI1->DR;


        //setWord( 0x20009008, counter + 1);
        //setWord( 0x20009000, (uint32_t)received );

        for( int i = 0; i < 100; i++ )
        {
            __asm("nop");
        }
    }
}


int SPI_disable( uint32_t SPInum )
{
    return 0;
}
