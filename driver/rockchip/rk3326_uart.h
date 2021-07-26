/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __RK3326_UART_H__
#define __RK3326_UART_H__

/* 串口寄存器基地址 */ 
#define UART0_BASE              0xFF030000
#define UART1_BASE              0xFF158000
#define UART2_BASE              0xFF160000
#define UART3_BASE              0xFF168000
#define UART4_BASE              0xFF170000
#define UART5_BASE              0xFF178000

/* 串口寄存器偏移地址 */ 
#define UART_RBR                0x0000 // W 0x00000000 Receive Buffer Register
#define UART_THR                0x0000 // W 0x00000000 Transmit Holding Register
#define UART_DLL                0x0000 // W 0x00000000 Divisor Latch (Low)
#define UART_DLH                0x0004 // W 0x00000000 Divisor Latch (High)
#define UART_IER                0x0004 // W 0x00000000 Interrupt Enable Register
#define UART_IIR                0x0008 // W 0x00000001 Interrupt Identification Register
#define UART_FCR                0x0008 // W 0x00000000 FIFO Control Register
#define UART_LCR                0x000c // W 0x00000000 Line Control Register
#define UART_MCR                0x0010 // W 0x00000000 Modem Control Register
#define UART_LSR                0x0014 // W 0x00000060 Line Status Register
#define UART_MSR                0x0018 // W 0x00000000 Modem Status Register
#define UART_SCR                0x001c // W 0x00000000 Scratchpad Register
#define UART_SRBR               0x0030 // W 0x00000000 Shadow Receive Buffer Register
#define UART_STHR               0x006c // W 0x00000000 Shadow Transmit Holding Register
#define UART_FAR                0x0070 // W 0x00000000 FIFO Access Register
#define UART_TFR                0x0074 // W 0x00000000 Transmit FIFO Read
#define UART_RFW                0x0078 // W 0x00000000 Receive FIFO // Write
#define UART_USR                0x007c // W 0x00000006 UART Status Register
#define UART_TFL                0x0080 // W 0x00000000 Transmit FIFO Level
#define UART_RFL                0x0084 // W 0x00000000 Receive FIFO Level
#define UART_SRR                0x0088 // W 0x00000000 Software Reset Register
#define UART_SRTS               0x008c // W 0x00000000 Shadow Request to Send
#define UART_SBCR               0x0090 // W 0x00000000 Shadow Break Control Register
#define UART_SDMAM              0x0094 // W 0x00000000 Shadow DMA Mode
#define UART_SFE                0x0098 // W 0x00000000 Shadow FIFO Enable
#define UART_SRT                0x009c // W 0x00000000 Shadow RCVR Trigger
#define UART_STET               0x00a0 // W 0x00000000 Shadow TX Empty Trigger
#define UART_HTX                0x00a4 // W 0x00000000 Halt TX
#define UART_DMASA              0x00a8 // W 0x00000000 DMA Software Acknowledge
#define UART_CPR                0x00f4 // W 0x00000000 Component Parameter Register
#define UART_UCV                0x00f8 // W 0x3330382a UART Component Version
#define UART_CTR                0x00fc // W 0x44570110 Component Type Register

/* UART_LSR */ 
#define UART_LSR_DR             0x01 /* Data ready */
#define UART_LSR_OE             0x02 /* Overrun */
#define UART_LSR_PE             0x04 /* Parity error */
#define UART_LSR_FE             0x08 /* Framing error */
#define UART_LSR_BI             0x10 /* Break */
#define UART_LSR_THRE           0x20 /* Xmit holding register empty */
#define UART_LSR_TEMT           0x40 /* Xmitter empty */
#define UART_LSR_ERR            0x80 /* Error */
        
/* UART_MSR */         
#define UART_MSR_DCD            0x80 /* Data Carrier Detect */
#define UART_MSR_RI             0x40 /* Ring Indicator */
#define UART_MSR_DSR            0x20 /* Data Set Ready */
#define UART_MSR_CTS            0x10 /* Clear to Send */
#define UART_MSR_DDCD           0x08 /* Delta DCD */
#define UART_MSR_TERI           0x04 /* Trailing edge ring indicator */
#define UART_MSR_DDSR           0x02 /* Delta DSR */
#define UART_MSR_DCTS           0x01 /* Delta CTS */
        
/* UART_MCR */         
#define UART_MCR_DTR	        0x01 /* DTR   */
#define UART_MCR_RTS	        0x02 /* RTS   */
#define UART_MCR_OUT1	        0x04 /* Out 1 */
#define UART_MCR_OUT2	        0x08 /* Out 2 */
#define UART_MCR_LOOP	        0x10 /* Enable loopback test mode */
#define UART_MCR_AFE	        0x20 /* Enable auto-RTS/CTS */
#define UART_MCR_DMA_EN	        0x04 
#define UART_MCR_TX_DFR	        0x08 

/* UART_LCR */ 
#define UART_LCR_WLS_MSK        0x00 /* character length select mask */
#define UART_LCR_WLS_5	        0x00 /* 5 bit character length */
#define UART_LCR_WLS_6	        0x01 /* 6 bit character length */
#define UART_LCR_WLS_7	        0x02 /* 7 bit character length */
#define UART_LCR_WLS_8	        0x03 /* 8 bit character length */
#define UART_LCR_STB	        0x04 /* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN	        0x08 /* Parity eneble */
#define UART_LCR_EPS	        0x10 /* Even Parity Select */
#define UART_LCR_STKP	        0x20 /* Stick Parity */
#define UART_LCR_SBRK	        0x40 /* Set Break */
#define UART_LCR_BKSE	        0x80 /* Bank select enable */
#define UART_LCR_DLAB	        0x80 /* Divisor latch access bit */

/* UART_FCR */ 
#define UART_FCR_FIFO_EN	    0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR	    0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	    0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	    0x08 /* For DMA applications */
#define UART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1	    0x00 /* Mask for trigger set at 1 */
#define UART_FCR_TRIGGER_4	    0x40 /* Mask for trigger set at 4 */
#define UART_FCR_TRIGGER_8	    0x80 /* Mask for trigger set at 8 */
#define UART_FCR_TRIGGER_14	    0xC0 /* Mask for trigger set at 14 */

/* UART_IER */ 
#define UART_IER_MSI            0x08 /* Enable Modem status interrupt */
#define UART_IER_RLSI           0x04 /* Enable receiver line status interrupt */
#define UART_IER_THRI           0x02 /* Enable Transmitter holding register int. */
#define UART_IER_RDI            0x01 /* Enable receiver data interrupt */

#endif /* __RK3326_UART_H__ */ 
