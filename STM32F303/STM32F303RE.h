#ifndef STM32F303RE_H
#define STM32F303RE_H

#define MEMBASE12 0x20000004
#define MEMBASE34 0x20008004
#define MAXDATAPTS 8000 		// --> 8192 * 32bit = 32.768 bytes (half of the 64k in the F303RE)... other F303s:
#define MAXVALIDDATAPTS 4000
// STM32F303x6/x8 only has 12kB of SRAM  and 4kB CCM SRAM
// STM32F303xB STM32F303xC 32/40kB SRAM and 8kB CCM SRAM


#endif
