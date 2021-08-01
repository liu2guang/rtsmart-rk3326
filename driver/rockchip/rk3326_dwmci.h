/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __RK3326_DWMCI_H__
#define __RK3326_DWMCI_H__

/* DWMCI寄存器基地址 */ 
#define SDMMC_BASE              0xFF370000
#define SDIO_BASE               0xFF380000
#define EMMC_BASE               0xFF390000

#define DWMCI_CTRL              0x0000 // W 0x00000000 Control register
#define DWMCI_PWREN             0x0004 // W 0x00000000 Power-enable register
#define DWMCI_CLKDIV            0x0008 // W 0x00000000 Clock-divider register
#define DWMCI_CLKSRC            0x000c // W 0x00000000 SD Clock Source Register
#define DWMCI_CLKENA            0x0010 // W 0x00000000 Clock-enable register
#define DWMCI_TMOUT             0x0014 // W 0xffffff40 Time-out register
#define DWMCI_CTYPE             0x0018 // W 0x00000000 Card-type register
#define DWMCI_BLKSIZ            0x001c // W 0x00000200 Block-size register
#define DWMCI_BYTCNT            0x0020 // W 0x00000200 Byte-count register
#define DWMCI_INTMASK           0x0024 // W 0x00000000 Interrupt-mask register
#define DWMCI_CMDARG            0x0028 // W 0x00000000 Command-argument register
#define DWMCI_CMD               0x002c // W 0x00000000 Command register
#define DWMCI_RESP0             0x0030 // W 0x00000000 Response-0 register
#define DWMCI_RESP1             0x0034 // W 0x00000000 Response-1 register
#define DWMCI_RESP2             0x0038 // W 0x00000000 Response-2 register
#define DWMCI_RESP3             0x003c // W 0x00000000 Response-3 register
#define DWMCI_MINTSTS           0x0040 // W 0x00000000 Masked interrupt-status register
#define DWMCI_RINTSTS           0x0044 // W 0x00000000 Ra// W interrupt-status register
#define DWMCI_STATUS            0x0048 // W 0x00000406 Status register
#define DWMCI_FIFOTH            0x004c // W 0x00000000 FIFO threshold register
#define DWMCI_CDETECT           0x0050 // W 0x00000000 Card-detect register
#define DWMCI_WRTPRT            0x0054 // W 0x00000000 Write-protect register
#define DWMCI_TCBCNT            0x005c // W 0x00000000 Transferred CIU card byte count
#define DWMCI_TBBCNT            0x0060 // W 0x00000000 Transferred host/DMA to/from BIU-FIFO byte count
#define DWMCI_DEBNCE            0x0064 // W 0x00ffffff Card detect debounce register
#define DWMCI_USRID             0x0068 // W 0x07967797 User ID register
#define DWMCI_VERID             0x006c // W 0x5342270a Synopsys version ID register
#define DWMCI_HCON              0x0070 // W 0x00000000 Hardware configuration register
#define DWMCI_UHS_REG           0x0074 // W 0x00000000 UHS-1 register
#define DWMCI_RSTN              0x0078 // W 0x00000001 Hardware reset register
#define DWMCI_BMOD              0x0080 // W 0x00000000 Bus mode register
#define DWMCI_PLDMND            0x0084 // W 0x00000000 Poll demand register
#define DWMCI_DBADDR            0x0088 // W 0x00000000 Descriptor list base address register
#define DWMCI_IDSTS             0x008c // W 0x00000000 Internal DMAC status register
#define DWMCI_IDINTEN           0x0090 // W 0x00000000 Internal DMAC interrupt enable register
#define DWMCI_DSCADDR           0x0094 // W 0x00000000 Current host descriptor address register
#define DWMCI_BUFADDR           0x0098 // W 0x00000000 Current buffer descriptor address register
#define DWMCI_CARDTHRCTL        0x0100 // W 0x00000000 Card read threshold enable register
#define DWMCI_BACK_END_POWER    0x0104 // W 0x00000000 Back-end power register
#define DWMCI_EMMC_DDR_REG      0x010c // W 0x00000000 eMMC 4.5 ddr start bit detection control register
#define DWMCI_FIFO_BASE         0x0200 // W 0x00000000 FIFO base address register

/* CTRL register */
#define DWMCI_CTRL_RESET	    (1 << 0)
#define DWMCI_CTRL_FIFO_RESET	(1 << 1)
#define DWMCI_CTRL_DMA_RESET	(1 << 2)
#define DWMCI_DMA_EN		    (1 << 5)
#define DWMCI_CTRL_SEND_AS_CCSD	(1 << 10)
#define DWMCI_IDMAC_EN		    (1 << 25)
#define DWMCI_RESET_ALL		    (DWMCI_CTRL_RESET | DWMCI_CTRL_FIFO_RESET | DWMCI_CTRL_DMA_RESET)

/* CMD register */
#define DWMCI_CMD_RESP_EXP	    (1 << 6)
#define DWMCI_CMD_RESP_LENGTH	(1 << 7)
#define DWMCI_CMD_CHECK_CRC	    (1 << 8)
#define DWMCI_CMD_DATA_EXP	    (1 << 9)
#define DWMCI_CMD_RW		    (1 << 10)
#define DWMCI_CMD_SEND_STOP	    (1 << 12)
#define DWMCI_CMD_ABORT_STOP	(1 << 14)
#define DWMCI_CMD_PRV_DAT_WAIT	(1 << 13)
#define DWMCI_CMD_UPD_CLK	    (1 << 21)
#define DWMCI_CMD_USE_HOLD_REG	(1 << 29)
#define DWMCI_CMD_START		    (1 << 31)

/* CLKENA register */
#define DWMCI_CLKEN_ENABLE	    (1 << 0)
#define DWMCI_CLKEN_LOW_PWR	    (1 << 16)

/* Card-type registe */
#define DWMCI_CTYPE_1BIT	    0
#define DWMCI_CTYPE_4BIT	    (1 << 0)
#define DWMCI_CTYPE_8BIT	    (1 << 16)

/* Status Register */
#define DWMCI_BUSY		        (1 << 9)
#define DWMCI_FIFO_MASK		    0x1fff
#define DWMCI_FIFO_SHIFT	    17
 
/* UHS register */ 
#define DWMCI_DDR_MODE	        (1 << 16)

/* HCON Register */
#define DMA_INTERFACE_IDMA		(0x0)
#define DWMCI_GET_TRANS_MODE(x) (((x)>>16) & 0x3)

#define DWMCI_IDMAC_OWN		    (1 << 31)
#define DWMCI_IDMAC_CH		    (1 << 4)
#define DWMCI_IDMAC_FS		    (1 << 3)
#define DWMCI_IDMAC_LD		    (1 << 2)

/* FIFOTH Register */
#define MSIZE(x)		        ((x) << 28)
#define RX_WMARK(x)		        ((x) << 16)
#define TX_WMARK(x)		        (x)
#define RX_WMARK_SHIFT		    16
#define RX_WMARK_MASK		    (0xfff << RX_WMARK_SHIFT)

/* Interrupt Mask register */
#define DWMCI_INTMSK_ALL	    0xffffffff
#define DWMCI_INTMSK_RE		    (1 << 1)
#define DWMCI_INTMSK_CDONE	    (1 << 2)
#define DWMCI_INTMSK_DTO	    (1 << 3)
#define DWMCI_INTMSK_TXDR	    (1 << 4)
#define DWMCI_INTMSK_RXDR	    (1 << 5)
#define DWMCI_INTMSK_DCRC	    (1 << 7)
#define DWMCI_INTMSK_RTO	    (1 << 8)
#define DWMCI_INTMSK_DRTO	    (1 << 9)
#define DWMCI_INTMSK_HTO	    (1 << 10)
#define DWMCI_INTMSK_FRUN	    (1 << 11)
#define DWMCI_INTMSK_HLE	    (1 << 12)
#define DWMCI_INTMSK_SBE	    (1 << 13)
#define DWMCI_INTMSK_ACD	    (1 << 14)
#define DWMCI_INTMSK_EBE	    (1 << 15)

/* Bus Mode Register */
#define DWMCI_BMOD_IDMAC_RESET	(1 << 0)
#define DWMCI_BMOD_IDMAC_FB	    (1 << 1)
#define DWMCI_BMOD_IDMAC_EN	    (1 << 7)

/* Raw interrupt Regsiter */
#define DWMCI_DATA_ERR          (DWMCI_INTMSK_EBE | DWMCI_INTMSK_SBE | DWMCI_INTMSK_HLE | DWMCI_INTMSK_FRUN | DWMCI_INTMSK_EBE | DWMCI_INTMSK_DCRC)
#define DWMCI_DATA_TOUT	        (DWMCI_INTMSK_HTO | DWMCI_INTMSK_DRTO)

#endif /* __RK3326_DWMCI_H__ */ 
