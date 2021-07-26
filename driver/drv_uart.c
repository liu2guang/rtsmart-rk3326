/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_uart.h" 
#include "drv_clk.h" 
#include <rthw.h>
#include "board.h" 

/* 四舍五入计算 */ 
#define DIV_ROUND_CLOSEST(x, divisor)({          \
    typeof(x) __x = x;                           \
    typeof(divisor) __d = divisor;               \
    (((typeof(x))-1) > 0 ||                      \
     ((typeof(divisor))-1) > 0 || (__x) > 0) ?   \
        (((__x) + ((__d) / 2)) / (__d)) :        \
        (((__x) - ((__d) / 2)) / (__d));         \
})

#define BSP_UART_DEVICE_DEFAULT(i, addr, irq) {{ \
        .ops = &ops,                             \
        .config = RT_SERIAL_CONFIG_DEFAULT       \
    },                                           \
    .index = i,                                  \
    .base = addr,                                \
    .irqno = irq                                 \
}

struct bsp_uart_device
{
    struct rt_serial_device device; 
    rt_uint32_t base;
    rt_uint32_t irqno;
    rt_uint32_t index; /* 串口编号 */ 
};
typedef struct bsp_uart_device bsp_uart_device_t; 

/* 配置串口 */ 
static void ns16550_config(rt_uint32_t base, rt_uint32_t clksrc, struct serial_configure *cfg)
{
    rt_uint32_t divisor; 

    RT_ASSERT(cfg != RT_NULL); 

    /* 计算波特率对应的 divisor */ 
    divisor = DIV_ROUND_CLOSEST(clksrc, 16 * cfg->baud_rate);

    /* 复位串口外设, uboot配置过串口, 不复位会无法修改波特率 */ 
    rk_writel(base + UART_SRR, 0x01); 
    rk_writel(base + UART_SRR, 0x00); 

    /* 关闭串口中断 */
    rk_writel(base + UART_IER, 0x00); 

    /* 使能fifo 并清空发送和接收 fifo */
    rk_writel(base + UART_FCR, UART_FCR_FIFO_EN | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT); 

    /* 配置modem */
    rk_writel(base + UART_MCR, 0x00); 

    /* 配置波特率 */
    rt_uint32_t lcr = rk_readl(base + UART_LCR);
    rk_writel(base + UART_LCR, lcr | UART_LCR_DLAB);
    rk_writel(base + UART_DLL, divisor & 0xff);
    rk_writel(base + UART_DLH, (divisor >> 8) & 0xff);
    lcr = rk_readl(base + UART_LCR);
    lcr &= ~UART_LCR_DLAB;
    rk_writel(base + UART_LCR, lcr);

    /* 配置数据位、停止位、奇偶校验 */ 
    lcr = 0; 
    lcr |= ((cfg->data_bits - DATA_BITS_5) << 0);

    switch(cfg->stop_bits)
    {
        case STOP_BITS_1: lcr |= (0x0 << 2); break; 
        case STOP_BITS_2: lcr |= (0x1 << 2); break; 
        default: RT_ASSERT(0); 
    }

    switch(cfg->parity)
    {
        case PARITY_NONE: lcr |= (0x0 << 3); break; 
        case PARITY_ODD:  lcr |= (0x1 << 3); break; 
        case PARITY_EVEN: lcr |= (0x3 << 3); break; 
    }

    rk_writel(base + UART_LCR, lcr);

    /* 使能接收中断 */ 
    rk_writel(base + UART_IER, UART_IER_RDI);
}

static void rt_hw_uart_isr(int irqno, void *param)
{ 
    struct bsp_uart_device *uart = (struct bsp_uart_device *)param;
    struct rt_serial_device *serial = &(uart->device);
    
    /* 读取中断种类 */ 
    rt_uint32_t int_id = rk_readl(uart->base + UART_IIR);

    /* received data available */ 
    if((int_id & 0x0F) == 0x04) 
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND); 
    }
} 

static rt_err_t uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct bsp_uart_device *uart = (struct bsp_uart_device *)serial;
    rt_uint32_t clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, cfg); 
    return RT_EOK; 
}

static rt_err_t uart_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct bsp_uart_device *uart = (struct bsp_uart_device *)serial;

    switch(cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT: 
        /* Disable the UART Interrupt */
        rt_hw_interrupt_mask(uart->irqno);
        rk_writel(uart->base + UART_IER, 0);
        break;
    case RT_DEVICE_CTRL_SET_INT: 
        /* Enable the UART Interrupt */
        rt_hw_interrupt_umask(uart->irqno);
        rk_writel(uart->base + UART_IER, UART_IER_RDI);
        break;
    }
    return RT_EOK;
}

static int uart_putc(struct rt_serial_device *serial, char c)
{
    struct bsp_uart_device *uart = (struct bsp_uart_device *)serial;

    RT_ASSERT(uart != RT_NULL); 

    /* Wait for Tx FIFO not full */
    rt_uint8_t status = 0;
    do {
        status = rk_readb(uart->base + UART_USR);
    } while ((status & (0x01 << 1)) == 0); 

    rk_writeb(uart->base + UART_THR, c);

    return 1;
}

static int uart_getc(struct rt_serial_device *serial)
{
    rt_uint8_t ch = -1;
    struct bsp_uart_device *uart = (struct bsp_uart_device *)serial;

    RT_ASSERT(uart != RT_NULL); 

    rt_uint8_t status;
    status = rk_readb(uart->base + UART_USR);
    if ((status & (0x01 << 3)) == 0) 
    {
        return -1; 
    }
    ch = rk_readb(uart->base + UART_RBR);

    return ch; 
}

static const struct rt_uart_ops ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
}; 

#ifdef BSP_USING_UART0
static struct bsp_uart_device uart0 = 
    BSP_UART_DEVICE_DEFAULT(0, UART0_BASE, UART0_IRQ); 
#endif /* BSP_USING_UART0 */ 

#ifdef BSP_USING_UART1
static struct bsp_uart_device uart1 = 
    BSP_UART_DEVICE_DEFAULT(1, UART1_BASE, UART1_IRQ); 
#endif /* BSP_USING_UART1 */ 

#ifdef BSP_USING_UART2
static struct bsp_uart_device uart2 = 
    BSP_UART_DEVICE_DEFAULT(2, UART2_BASE, UART2_IRQ); 
#endif /* BSP_USING_UART2 */ 

#ifdef BSP_USING_UART3
static struct bsp_uart_device uart3 = 
    BSP_UART_DEVICE_DEFAULT(3, UART3_BASE, UART3_IRQ); 
#endif /* BSP_USING_UART3 */ 

#ifdef BSP_USING_UART4
static struct bsp_uart_device uart4 = 
    BSP_UART_DEVICE_DEFAULT(4, UART4_BASE, UART4_IRQ); 
#endif /* BSP_USING_UART4 */ 

#ifdef BSP_USING_UART5
static struct bsp_uart_device uart5 = 
    BSP_UART_DEVICE_DEFAULT(5, UART5_BASE, UART5_IRQ); 
#endif /* BSP_USING_UART5 */ 

int rt_hw_uart_init(void) 
{
    rt_uint32_t clksrc = 0; 
    struct bsp_uart_device *uart = RT_NULL; 

    /* TODO 时钟源目前都是24M, 这里没有配置 */ 
    /* TODO 管脚复用没有配置 */ 

    /* register UART0 device */
#ifdef BSP_USING_UART0
    uart = &uart0; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart0");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART0 */ 

    /* register UART1 device */
#ifdef BSP_USING_UART1
    uart = &uart1; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart1");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART1 */ 

    /* register UART2 device */
#ifdef BSP_USING_UART2
    uart = &uart2; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart2", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart2");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART2 */ 

    /* register UART3 device */
#ifdef BSP_USING_UART3
    uart = &uart3; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart3", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart3");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART3 */ 

    /* register UART4 device */
#ifdef BSP_USING_UART4
    uart = &uart4; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart4", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart4");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART4 */ 

    /* register UART5 device */
#ifdef BSP_USING_UART5
    uart = &uart5; 
    clksrc = rk3326_clk_get_rate_clk_uart(uart->index); 
    ns16550_config(uart->base, clksrc, &uart->device.config); 
    rt_hw_serial_register(&uart->device, "uart5", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX, uart);
    rt_hw_interrupt_install(uart->irqno, rt_hw_uart_isr, uart, "uart5");
    rt_hw_interrupt_umask(uart->irqno); 
#endif /* BSP_USING_UART5 */ 

    return RT_EOK; 
} 
