#ifndef STM32F303RE_H
#define STM32F303RE_H



// 1 datapoint (12bit) = 2 bytes --> 4 datapoints = 8 bytes.
// 64kB = 64*1024 = 65536 bytes available
// for simplicity and to have some space leftover: 64000 / 8 = 8000 datapoints can be stored for 4 channels.
// 4 * 8000 = 32000 (+1) --> end of first batch of data (ch1+ch2)
#define MAXDATAPTS 8000 		// --> 8192 * 32bit = 32.768 bytes (half of the 64k in the F303RE)... other F303s:
#define MAXVALIDDATAPTS 4000    //

#define MEMBASE12 0x20000000    // 0x20000000 to 08x20007D00 (+1 because of length byte)
#define MEMBASE34 0x20008000    // 0x20000000 + 32.768 = 0x20008000 to 0x2000FD00 (+1 because of length byte)

// notes:
// STM32F303x6/x8 only has 12kB of SRAM  and 4kB CCM SRAM
// STM32F303xB STM32F303xC 32/40kB SRAM and 8kB CCM SRAM

#define CLKSPEED_HSI 8000000UL  //8MHz RC oscillator clock
#define CLKSPEED_HSE 8000000UL  //8MHz ext osc clock


#endif
