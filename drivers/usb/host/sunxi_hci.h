/*
 * drivers/usb/host/sunxi_hci.h
 * (C) Copyright 2010-2015
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * yangnaitian, 2011-5-24, create this file
 *
 * Include file for AW1623 HCI Host Controller Driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 */

#ifndef __SUNXI_HCI_SUNXI_H__
#define __SUNXI_HCI_SUNXI_H__

#include <linux/delay.h>
#include <linux/types.h>

#include <linux/io.h>
#include <linux/irq.h>

#define  DMSG_ERR(format,args...)               printk("[sunxi_hci_sunxi]: "format,##args)
#define  DMSG_PRINT(format,args...)             printk("[sunxi_hci_sunxi]: "format,##args)


#if 0
#define DMSG_DEBUG                              DMSG_PRINT
#else
#define DMSG_DEBUG(...)
#endif

#if 1
#define DMSG_INFO                               DMSG_PRINT
#else
#define DMSG_INFO(...)
#endif

#if	1
#define DMSG_PANIC                              DMSG_ERR
#else
#define DMSG_PANIC(...)
#endif

#define  USBC_Readb(reg)                        (*(volatile unsigned char * __force)(reg))
#define  USBC_Readw(reg)                        (*(volatile unsigned short * __force)(reg))
#define  USBC_Readl(reg)                        (*(volatile unsigned long * __force)(reg))

#define  USBC_Writeb(value, reg)                (*(volatile unsigned char * __force)(reg) = (value))
#define  USBC_Writew(value, reg)                (*(volatile unsigned short * __force)(reg) = (value))
#define  USBC_Writel(value, reg)                (*(volatile unsigned long * __force)(reg) = (value))

#define  USBC_REG_test_bit_b(bp, reg)           (USBC_Readb(reg) & (1 << (bp)))
#define  USBC_REG_test_bit_w(bp, reg)           (USBC_Readw(reg) & (1 << (bp)))
#define  USBC_REG_test_bit_l(bp, reg)           (USBC_Readl(reg) & (1 << (bp)))

#define  USBC_REG_set_bit_b(bp, reg)            (USBC_Writeb((USBC_Readb(reg) | (1 << (bp))) , (reg)))
#define  USBC_REG_set_bit_w(bp, reg)            (USBC_Writew((USBC_Readw(reg) | (1 << (bp))) , (reg)))
#define  USBC_REG_set_bit_l(bp, reg)            (USBC_Writel((USBC_Readl(reg) | (1 << (bp))) , (reg)))

#define  USBC_REG_clear_bit_b(bp, reg)          (USBC_Writeb((USBC_Readb(reg) & (~ (1 << (bp)))) , (reg)))
#define  USBC_REG_clear_bit_w(bp, reg)          (USBC_Writew((USBC_Readw(reg) & (~ (1 << (bp)))) , (reg)))
#define  USBC_REG_clear_bit_l(bp, reg)          (USBC_Writel((USBC_Readl(reg) & (~ (1 << (bp)))) , (reg)))

#define SUNXI_USB_EHCI_BASE_OFFSET              0x00
#define SUNXI_USB_OHCI_BASE_OFFSET              0x400
#define SUNXI_USB_EHCI_LEN                      0x58
#define SUNXI_USB_OHCI_LEN                      0x58

#define SUNXI_USB_PMU_IRQ_ENABLE                0x800

#define EHCI_CAP_OFFSET		(0x00)
#define EHCI_CAP_LEN		(0x10)

#define EHCI_CAP_CAPLEN		(EHCI_CAP_OFFSET + 0x00)
#define EHCI_CAP_HCIVER		(EHCI_CAP_OFFSET + 0x00)
#define EHCI_CAP_HCSPAR		(EHCI_CAP_OFFSET + 0x04)
#define EHCI_CAP_HCCPAR		(EHCI_CAP_OFFSET + 0x08)
#define EHCI_CAP_COMPRD		(EHCI_CAP_OFFSET + 0x0c)


#define EHCI_OPR_OFFSET		(EHCI_CAP_OFFSET + EHCI_CAP_LEN)

#define EHCI_OPR_USBCMD		(EHCI_OPR_OFFSET + 0x00)
#define EHCI_OPR_USBSTS		(EHCI_OPR_OFFSET + 0x04)
#define EHCI_OPR_USBINTR	(EHCI_OPR_OFFSET + 0x08)
#define EHCI_OPR_FRINDEX	(EHCI_OPR_OFFSET + 0x0c)
#define EHCI_OPR_CRTLDSS	(EHCI_OPR_OFFSET + 0x10)
#define EHCI_OPR_PDLIST		(EHCI_OPR_OFFSET + 0x14)
#define EHCI_OPR_ASLIST		(EHCI_OPR_OFFSET + 0x18)
#define EHCI_OPR_CFGFLAG	(EHCI_OPR_OFFSET + 0x40)
#define EHCI_OPR_PORTSC		(EHCI_OPR_OFFSET + 0x44)

/*-------------------------------------------------------------------------------------------------------------*/
/*PORT Control and Status Register*/
/*port_no is 0 based, 0, 1, 2, .....*/
/*********************************************************************
 *   Reg EHCI_OPR_PORTSC
 * *********************************************************************/

//Port Test Control bits
#define EHCI_PORTSC_PTC_MASK		(0xf<<16)
#define EHCI_PORTSC_PTC_DIS			(0x0<<16)
#define EHCI_PORTSC_PTC_J			(0x1<<16)
#define EHCI_PORTSC_PTC_K			(0x2<<16)
#define EHCI_PORTSC_PTC_SE0NAK		(0x3<<16)
#define EHCI_PORTSC_PTC_PACKET		(0x4<<16)
#define EHCI_PORTSC_PTC_FORCE		(0x5<<16)

#define EHCI_PORTSC_OWNER			(0x1<<13)
#define EHCI_PORTSC_POWER			(0x1<<12)

#define EHCI_PORTSC_LS_MASK			(0x3<<10)
#define EHCI_PORTSC_LS_SE0			(0x0<<10)
#define EHCI_PORTSC_LS_J			(0x2<<10)
#define EHCI_PORTSC_LS_K			(0x1<<10)
#define EHCI_PORTSC_LS_UDF			(0x3<<10)

#define EHCI_PORTSC_RESET			(0x1<<8)
#define EHCI_PORTSC_SUSPEND			(0x1<<7)
#define EHCI_PORTSC_RESUME			(0x1<<6)
#define EHCI_PORTSC_OCC				(0x1<<5)
#define EHCI_PORTSC_OC				(0x1<<4)
#define EHCI_PORTSC_PEC				(0x1<<3)
#define EHCI_PORTSC_PE				(0x1<<2)
#define EHCI_PORTSC_CSC				(0x1<<1)
#define EHCI_PORTSC_CCS				(0x1<<0)

#define	EHCI_PORTSC_CHANGE			(EHCI_PORTSC_OCC | EHCI_PORTSC_PEC | EHCI_PORTSC_CSC)

#define  SUNXI_USB_HCI_DEBUG

#if defined (CONFIG_FPGA_V4_PLATFORM) || defined (CONFIG_FPGA_V7_PLATFORM)
#define SUNXI_USB_FPGA
#endif

enum sunxi_usbc_type{
	SUNXI_USB_UNKOWN = 0,
	SUNXI_USB_EHCI,
	SUNXI_USB_OHCI,
};

struct sunxi_hci_hcd{
	__u32 usbc_no;                          /* usb controller number */
	__u32 irq_no;                           /* interrupt number */
	char hci_name[32];                      /* hci name */

	struct resource	*usb_base_res;          /* USB  resources */
	struct resource	*usb_base_req;          /* USB  resources */
	void __iomem	*usb_vbase;             /* USB  base address */

	void __iomem	* ehci_base;
	__u32 ehci_reg_length;
	void __iomem	* ohci_base;
	__u32 ohci_reg_length;

	struct resource	*sram_base_res;   	/* SRAM resources */
	struct resource	*sram_base_req;         /* SRAM resources */
	void __iomem	*sram_vbase;            /* SRAM base address */
	__u32 sram_reg_start;
	__u32 sram_reg_length;

	struct resource	*clock_base_res;        /* clock resources */
	struct resource	*clock_base_req;        /* clock resources */
	void __iomem	*clock_vbase;           /* clock base address */
	__u32 clock_reg_start;
	__u32 clock_reg_length;

	struct resource	*gpio_base_res;         /* gpio resources */
	struct resource	*gpio_base_req;         /* gpio resources */
	void __iomem	*gpio_vbase;            /* gpio base address */
	__u32 gpio_reg_start;
	__u32 gpio_reg_length;

	struct resource	*sdram_base_res;        /* sdram resources */
	struct resource	*sdram_base_req;        /* sdram resources */
	void __iomem	*sdram_vbase;           /* sdram base address */
	__u32 sdram_reg_start;
	__u32 sdram_reg_length;

	struct platform_device *pdev;
	struct usb_hcd *hcd;

	struct clk	*ahb;                   /* ahb clock handle */
	struct clk	*mod_usb;               /* mod_usb otg clock handle */
	struct clk	*mod_usbphy;            /* PHY0 clock handle */
	__u32 clk_is_open;                      /* is usb clock open */

	script_item_u drv_vbus_gpio_set;
	script_item_u restrict_gpio_set;

	char* regulator_io;
	int   regulator_value;
	struct regulator* regulator_io_hdle;
	u32 drv_vbus_gpio_valid;
	u32 usb_restrict_valid;
	__u8 power_flag;                        /* flag. power on or not */

	__u8 used;                              /* flag. in use or not */
	__u8 probe;                             /* hc initialize */
	__u8 host_init_state;                   /* usb hc initialize state, 0: not work, 1: work */
	__u8 usb_restrict_flag;
	__u8 usbc_type;                         /* usb controller type  */
	__u8 not_suspend;                       /* flag. not suspend */

	int (* open_clock)(struct sunxi_hci_hcd *sunxi_hci, u32 ohci);
	int (* close_clock)(struct sunxi_hci_hcd *sunxi_hci, u32 ohci);
	void (* set_power)(struct sunxi_hci_hcd *sunxi_hci, int is_on);
	void (* port_configure)(struct sunxi_hci_hcd *sunxi_hci, u32 enable);
	void (* usb_passby)(struct sunxi_hci_hcd *sunxi_hci, u32 enable);
};

#ifdef  SUNXI_USB_FPGA
/* otg and hci share the same phy in fpga, so need switch phy to hci here */
static inline void fpga_config_use_hci(__u32 sram_vbase)
{
	u32 reg_value = 0;
	reg_value = USBC_Readl(sram_vbase + 0x04);
	/* bit1: 1 - select phy for otg, 0 - select phy for hci */
#ifdef CONFIG_ARCH_SUN8IW8
	reg_value |= 0x01;
#else
	reg_value &= ~(0x01);
#endif
	USBC_Writel(reg_value, (sram_vbase + 0x04));
}
#endif

#endif   //__SUNXI_HCI_SUNXI_H__

