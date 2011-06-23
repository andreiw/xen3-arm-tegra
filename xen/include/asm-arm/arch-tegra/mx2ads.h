/*
 * linux/include/asm-arm/arch-imx/mx2ads.h
 *
 * Copyright (C) 2004 Robert Schwebel, Pengutronix
 *
 * Modified By: Ron Melvin (ron.melvin@timesys.com)
 * Copyright (C) 2005 TimeSys Corporation 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __ASM_ARCH_MX2ADS_H
#define __ASM_ARCH_MX2ADS_H

/* ------------------------------------------------------------------------ */
/* Memory Map for the M9328IMX21ADS (IMX21ADS) Board                            */
/* ------------------------------------------------------------------------ */

#define MX2ADS_FLASH_PHYS		0xc8000000
#define MX2ADS_FLASH_SIZE		(32*1024*1024)

#define CLK32 32768

#define MX2ADS_ETH_VIRT IMX_CS1_VIRT
#define MX2ADS_ETH_PHYS IMX_CS1_PHYS
#define MX2ADS_ETH_SIZE IMX_CS1_SIZE
#define MX2ADS_ETH_IRQ  IRQ_GPIOE(11) //INT_UART3


#endif /* __ASM_ARCH_MX2ADS_H */
