#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

#include <rtthread.h> 

/* 代码段结构体 */ 
struct rt_backtrace_text
{
    rt_base_t start; 
    rt_base_t end; 
}; 
typedef struct rt_backtrace_text rt_backtrace_text_t; 

/* 配置代码段 */ 
int rt_backtrace_init(struct rt_backtrace_text *texts, rt_size_t size); 

/* 录制调用栈信息 */ 
void rt_backtrace_dump(rt_thread_t thread); 

#endif /* __BACKTRACE_H__ */ 
