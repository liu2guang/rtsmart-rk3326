#ifndef __RT_BACKTRACE_ARCH_H__
#define __RT_BACKTRACE_ARCH_H__

#include <rtthread.h>

#if defined(ARCH_ARMV8) 
__attribute__((always_inline)) static inline rt_base_t get_SP(void) 
{
    register uint32_t result;
    __asm volatile ("MOV %0, sp\n" : "=r" (result)); 
    return(result);
}
#elif defined(ARCH_MIPS64) 
__attribute__((always_inline)) static inline rt_base_t get_SP(void) 
{
    register uint32_t result;
    // __asm volatile ("MOV %0, sp\n" : "=r" (result)); 
    return(result);
}
#endif /* ARCH */ 

rt_bool_t is_assembly_branch(rt_uint32_t instruction); 

#endif /* __RT_BACKTRACE_ARCH_H__ */ 
