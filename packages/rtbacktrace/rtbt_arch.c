/*
 * RT-Thread Secure
 * 
 * Copyright (c) 2021, Shanghai Real-Thread Electronic Technology Co., Ltd.
 *
 * All rights reserved.
 */
#include <rtthread.h> 
#include <rtbt_cfg.h> 

#if defined(ARCH_ARMV8) 

// Form: Arm Architecture Reference Manual Armv8, for Armv8-A architecture profile.pdf C4: A64 Instruction Set Encoding 

// Conditional branch (immediate)
static int is_assembly_b_cond(rt_uint32_t instruction)
{
    if((instruction & 0xFF000010) == 0x54000000)
        return 1; 
    else
        return 0; 
}

// Unconditional branch (register)
#if 0
static int is_assembly_br_blr_ret_eret_drps(rt_uint32_t instruction) 
{
    if((instruction & 0xFE000000) == 0xD6000000)
        return 1; 
    else
        return 0; 
}
#endif  

static int is_assembly_br_blr(rt_uint32_t instruction) 
{
    if((instruction & 0xFFC00000) == 0xD6000000)
        return 1; 
    else
        return 0; 
}

// Unconditional branch (immediate)
static int is_assembly_b_bl(rt_uint32_t instruction) 
{
    if((instruction & 0x7C000000) == 0x14000000)
        return 1; 
    else
        return 0; 
}

// Compare and branch (immediate)
static int is_assembly_cbz_cbnz(rt_uint32_t instruction) 
{
    if((instruction & 0x7E000000) == 0x34000000)
        return 1; 
    else
        return 0; 
}

// Test and branch (immediate)
static int is_assembly_tbz_tbnz(rt_uint32_t instruction) 
{
    if((instruction & 0x7E000000) == 0x36000000)
        return 1; 
    else
        return 0; 
}

// branch instruction
rt_bool_t is_assembly_branch(rt_uint32_t instruction)
{
    int is_branch = 0; 

    is_branch += is_assembly_b_cond(instruction); 
    // is_branch += is_assembly_br_blr_ret_eret_drps(instruction); 
    is_branch += is_assembly_br_blr(instruction); 
    is_branch += is_assembly_b_bl(instruction); 
    is_branch += is_assembly_cbz_cbnz(instruction); 
    is_branch += is_assembly_tbz_tbnz(instruction); 

    return (!!is_branch); 
} 

#elif defined(ARCH_MIPS64) 

static int is_assembly_j(rt_uint32_t instruction)
{
    if((instruction & 0xFC000000) == 0x0C000000)
        return 1; 
    else
        return 0; 
}

static int is_assembly_jal(rt_uint32_t instruction)
{
    if((instruction & 0xFC000000) == 0x08000000)
        return 1; 
    else
        return 0; 
}

static int is_assembly_jr(rt_uint32_t instruction)
{
    if((instruction & 0xFC000000) == 0x00000000)
        return 1; 
    else
        return 0; 
}

static int is_assembly_beq(rt_uint32_t instruction)
{
    if((instruction & 0xFC000000) == 0x10000000)
        return 1; 
    else
        return 0;  
}

static int is_assembly_bne(rt_uint32_t instruction)
{
    if((instruction & 0xFC000000) == 0x14000000)
        return 1; 
    else
        return 0;      
}

rt_bool_t is_assembly_branch(rt_uint32_t instruction)
{
    int is_branch = 0; 

    is_branch += is_assembly_j(instruction); 
    is_branch += is_assembly_jal(instruction); 
    is_branch += is_assembly_jr(instruction); 
    is_branch += is_assembly_beq(instruction); 
    is_branch += is_assembly_bne(instruction); 

    return (!!is_branch); 
} 

#endif /* ARCH */ 
