#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


void DMA_init( void )
{
    SETBIT( RCC->AHBENR, 0 ); //enable DMA1
    (void)RCC->AHBENR;             // read-back to flush/ensure clock gate opened
    SETBIT( RCC->AHBENR, 1 );
    (void)RCC->AHBENR;             // read-back to flush/ensure clock gate opened

}
