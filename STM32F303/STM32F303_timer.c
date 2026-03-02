#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


// not sure which clocks need to be running
// source can be APB1 (6,7)
// source can either be APB1 clock or sysClk (2,3,4)
// source can either be APB2 clock or sysClk (1,8,15,16,17,20)
void TIMER_init( void )
{

}

//interrupt is
void TIMER_enable( uint32_t tim, uint32_t divider ) //clock cycle divider
{
    //divider
    switch( tim )
    {
        case 1:
            break;
        case 2:
            
            break;
    }
}

void TIMER_disable( uint32_t tim )
{

}
