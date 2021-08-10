/*
 * RT-Thread Secure
 * 
 * Copyright (c) 2021, Shanghai Real-Thread Electronic Technology Co., Ltd.
 *
 * All rights reserved.
 */
#include <rt_backtrace.h> 
#include <stdio.h>
#include <rthw.h>
#include <rtbt_arch.h>
#include <rtbt_cfg.h>

struct backtrace_info
{
    rt_bool_t is_init; 
    struct rt_backtrace_text *texts; 
    rt_size_t text_size; 
    rt_base_t call_stack[BACKTRACE_DEPTH]; 
    rt_size_t call_size; 
    rt_base_t *stack_bak; 
};
typedef struct backtrace_info backtrace_info_t; 

static struct backtrace_info info = {0}; 

extern rt_bool_t is_assembly_branch(rt_uint32_t instruction); 

/* 地址是否处于代码段 */ 
static rt_bool_t is_in_text_section(rt_base_t addr)
{
    int index; 
    struct rt_backtrace_text *text = RT_NULL;

    for(index = 0; index < info.text_size; index++)
    {
        text = &info.texts[index]; 
        if((addr >= text->start) && (addr <= text->end))
        {
            return RT_TRUE; 
        }
    }

    return RT_FALSE; 
}

static void backtrace_assert_hook(const char* ex, const char* func, rt_size_t line)
{
    rt_hw_interrupt_disable();

#ifdef RT_USING_FINSH
    extern long list_thread(void); 
    list_thread();
#endif

    rt_backtrace_dump(RT_NULL); 
    while(1); 
}

void rt_backtrace_dump(rt_thread_t thread) 
{
    /* 线程栈空间范围 */ 
    int index = 0; 
    rt_base_t stack_start, stack_end; /* 线程栈顶栈底 */ 
    rt_base_t sp, sp_content, addr; /* 当前线程sp, sp指向的内容, 跳转地址 */ 

    rt_base_t level = rt_hw_interrupt_disable();

    if (thread == RT_NULL) 
    {
        thread = rt_thread_self(); 
    }

    info.call_size = 0; 
    
    /* 获取线程栈空间信息 */ 
    sp = get_SP(); 
    stack_start = (rt_base_t)thread->stack_addr; 
    stack_end = stack_start + (rt_base_t)thread->stack_size; 

    printf("thread: %.8s, stack: %p-%p, sp = %p, stackoverflow = %d\n", 
        thread->name, (void *)stack_start, (void *)stack_end, (void *)sp, 
        ((sp < stack_start) || (sp > stack_end)) ); 

    /* 遍历栈空间 */ 
    for (index = 0; sp < stack_end; index++, sp += sizeof(rt_base_t)) 
    {
        sp_content = *((volatile rt_base_t *)sp); 
        printf("sp[%.4d]: %lx", index, sp_content); 

        /* 判断线程栈空间中的数据是否是处于代码段 */ 
        if(is_in_text_section(sp_content))
        {
            /* 取代码对应指令, 这里-4字节的原因是分支指令再跳转之前会把跳转处的下一条指令压入栈空间, 
               所以-4就是真实的跳转指令处, 这个-4对应 aarch32 和 aarch64 均是一样，因为指令是4字节 */ 
            addr = sp_content-4; 
            rt_uint32_t instruct = *((volatile rt_base_t *)addr);

            printf(" %lx 0x%x text", addr, instruct); 

            /* 指令是否属于分支指令 */ 
            if(is_assembly_branch(instruct))
            {
                printf(" branch");  

                info.call_stack[info.call_size++] = addr; 
                if(info.call_size + 1 >= BACKTRACE_DEPTH)
                {
                    break;
                }
            }
        }

        printf("\n"); 
    }

    if (info.call_size > 0)
    {
        index = 0; 
        printf("%s -e rtthread.elf -a -f", BACKTRACE_ADDR2LINE_NAME);
        for (index = 0; index < info.call_size; index++) 
        {
            printf(" %lx", info.call_stack[index]);
        }
        printf("\n");
    }

    rt_hw_interrupt_enable(level);
}

int rt_backtrace_init(struct rt_backtrace_text *texts, rt_size_t size)
{
    RT_ASSERT(texts != RT_NULL); 
    RT_ASSERT(size > 0); 

    info.texts = texts; 
    info.text_size = size; 
    info.call_size = 0; 

    rt_assert_set_hook(backtrace_assert_hook); 

    return RT_EOK; 
}
