
#ifndef __BIT_H__
#define __BIT_H__

#include "stm32f0xx.h"

#define BITBAND(addr,bitnum) ((addr & 0xf0000000) + 0x2000000 + ((addr & 0xfffff) << 5) + (bitnum << 2))
#define MEMADDR(addr) *((volatile unsigned long*)(addr))      //将地址强制转换为指针并获取数据
#define BIT_ADDR(addr,bitnum) MEMADDR(BITBAND(addr, bitnum))  //把位带别名区的地址转换为指针并获取地址内的数据

#define GPIOA_ODR_Addr (GPIOA_BASE + 20)
#define GPIOB_ODR_Addr (GPIOB_BASE + 20)
#define GPIOC_ODR_Addr (GPIOC_BASE + 20)
#define GPIOD_ODR_Addr (GPIOD_BASE + 20)
#define GPIOE_ODR_Addr (GPIOE_BASE + 20)
#define GPIOF_ODR_Addr (GPIOF_BASE + 20)

#define GPIOA_IDR_Addr (GPIOA_BASE + 16)
#define GPIOB_IDR_Addr (GPIOB_BASE + 16)
#define GPIOC_IDR_Addr (GPIOC_BASE + 16)
#define GPIOD_IDR_Addr (GPIOD_BASE + 16)
#define GPIOE_IDR_Addr (GPIOE_BASE + 16)
#define GPIOF_IDR_Addr (GPIOF_BASE + 16)


//IO口操作，只对单一的IO口 确保n的值小于16
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n)

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n)

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n)

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n)

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n)
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n)


#endif

