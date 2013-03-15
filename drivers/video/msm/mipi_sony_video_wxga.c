/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_sony.h"

static struct msm_panel_info pinfo;

/* 20110420, kkcho, for EF39S
* 4.5'' WXGA (800*1280), 500Mbps
*/
static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
#if defined (MIPI_CLOCK_400MBPS)
/* DSI_BIT_CLK at 400Mbps, 4 lane, RGB888 */
		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0xad, 0x8b, 0x19, 0x00, 0x1c, 0x92, 0x1c,
		0x8d, 0x1c, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x03, 0x86, 0x03},	/* strength */
		/* pll control */
		{0x40, 0x8f, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x31, 0x0f, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};
#elif defined (MIPI_CLOCK_450MBPS)
/* DSI_BIT_CLK at 450Mbps, 4 lane, RGB888 */
		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0xb3, 0x8c, 0x1d, 0x00, 0x20, 0x94, 0x20,
		0x8e, 0x20, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x03, 0x86, 0x03},	/* strength */
		/* pll control */
		{0x40, 0xC1, 0x31, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x31, 0x0f, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};
#elif defined (MIPI_CLOCK_500MBPS)
#if 1
/* DSI_BIT_CLK at 500Mbps, 4 lane, RGB888 */
		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0xb9, 0x8e, 0x20, 0x00, 0x24, 0x96, 0x23,
		0x90, 0x24, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x03, 0x86, 0x03},	/* strength */
		/* pll control */
		{0x40, 0xF9, 0x30, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x30, 0x07, 0x01,
		//0x31, 0x0f, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};
#else
/* DSI_BIT_CLK at 490Mbps, 4 lane, RGB888 */

		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0xb8, 0x8e, 0x1f, 0x00, 0x23, 0x96, 0x22,
		0x90, 0x23, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x03, 0x86, 0x03},	/* strength */
		/* pll control */
		{0x40, 0xF9, 0x30, 0xda, 0x00, 0x50, 0x48, 0x63,
		//0x30, 0x07, 0x01,
		0x31, 0x0f, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};
#endif
#else
/* DSI_BIT_CLK at 500MHz, 4 lane, RGB888 */
		{0x03, 0x01, 0x01, 0x00},	/* regulator */
		/* timing   */
		{0x96, 0x26, 0x23, 0x0, 0x50, 0x4B, 0x1e,
		0x28, 0x28, 0x03, 0x04},
		{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
		{0xee, 0x03, 0x86, 0x03},	/* strength */
		/* pll control */
		{0x40, 0xf9, 0xb0, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x31, 0x0f, 0x03,
		0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};
#endif
static int __init mipi_video_sony_wxga_init(void)
{
    int ret;

#ifdef CONFIG_FB_MSM_MIPI_PANEL_DETECT
    if (msm_fb_detect_client("mipi_video_sony_wxga"))
        return 0;
#endif

/* Try-case
ref-2. 20110420, kkcho
3030->3035 qualcomm-patch이후, 
mipi_dsi.c의 mipi_dsi_phy_pll_config(panel_info->clk_rate);  코드를 막아야 함. 
그렇지 않으면.. clk value변경으로 화면 안나옴. 
*/
        pinfo.xres = 800;
        pinfo.yres = 1280;
#ifdef F_SKYDISP_DSI_PADDING
		/*
		 *
		 * Panel's Horizontal input timing requirement is to
		 * include dummy(pad) data of 200 clk in addition to
		 * width and porch/sync width values
		 */
#if defined (MIPI_CLOCK_500MBPS)
		pinfo.lcdc.xres_pad = 252;
		pinfo.lcdc.yres_pad = 0;
#else
		pinfo.lcdc.xres_pad = 0;
		pinfo.lcdc.yres_pad = 0;
#endif
#endif
        pinfo.type = MIPI_VIDEO_PANEL;
        pinfo.pdest = DISPLAY_1;
        pinfo.wait_cycle = 0;
        pinfo.bpp = 24;
#if defined (MIPI_CLOCK_400MBPS) || defined (MIPI_CLOCK_450MBPS) || defined (MIPI_CLOCK_500MBPS) 
        pinfo.lcdc.h_back_porch = 10;
        pinfo.lcdc.h_front_porch = 16;
        pinfo.lcdc.h_pulse_width = 2;
        pinfo.lcdc.v_back_porch = 2;
        pinfo.lcdc.v_front_porch = 2;
        pinfo.lcdc.v_pulse_width = 2;
#else
    	pinfo.lcdc.h_back_porch = 96;
    	pinfo.lcdc.h_front_porch = 32;
    	pinfo.lcdc.h_pulse_width = 8;
        
    	pinfo.lcdc.v_back_porch = 10;
    	pinfo.lcdc.v_front_porch = 2;
    	pinfo.lcdc.v_pulse_width = 2;
#endif
        pinfo.lcdc.border_clr = 0;	/* blk */
        pinfo.lcdc.underflow_clr = 0xff;	/* blue */
        pinfo.lcdc.hsync_skew = 0;
        pinfo.bl_max = 32;
        pinfo.bl_min = 1;
        pinfo.fb_num = 2;
#if defined (MIPI_CLOCK_400MBPS)
        pinfo.clk_rate = 400000000;
#elif defined (MIPI_CLOCK_450MBPS)
        pinfo.clk_rate = 450000000;
#elif defined (MIPI_CLOCK_500MBPS)
        pinfo.clk_rate = 500000000;
#else
        pinfo.clk_rate = 500000000;
#endif
		pinfo.lcd.vsync_enable = FALSE;
		pinfo.lcd.hw_vsync_mode = FALSE;

        pinfo.mipi.mode = DSI_VIDEO_MODE;
#if defined (MIPI_CLOCK_400MBPS) || defined (MIPI_CLOCK_450MBPS) || defined (MIPI_CLOCK_500MBPS)
        pinfo.mipi.pulse_mode_hsa_he = TRUE;
        pinfo.mipi.hfp_power_stop = FALSE;
        pinfo.mipi.hbp_power_stop = FALSE;
        pinfo.mipi.hsa_power_stop = TRUE;
        pinfo.mipi.eof_bllp_power_stop = TRUE;
        pinfo.mipi.bllp_power_stop = TRUE;
        pinfo.mipi.traffic_mode = DSI_BURST_MODE;
#else
        pinfo.mipi.pulse_mode_hsa_he = TRUE;
        pinfo.mipi.hfp_power_stop = TRUE;
        pinfo.mipi.hbp_power_stop = TRUE;
        pinfo.mipi.hsa_power_stop = TRUE;
        pinfo.mipi.eof_bllp_power_stop = TRUE;
        pinfo.mipi.bllp_power_stop = FALSE;
        pinfo.mipi.traffic_mode = DSI_NON_BURST_SYNCH_EVENT;
#endif
        pinfo.mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
        pinfo.mipi.vc = 0;
        pinfo.mipi.rgb_swap = DSI_RGB_SWAP_BGR;
        pinfo.mipi.data_lane0 = TRUE;
        pinfo.mipi.data_lane1 = TRUE;
        pinfo.mipi.data_lane2 = TRUE;
        pinfo.mipi.data_lane3 = TRUE;
#if defined (MIPI_CLOCK_400MBPS)
	    pinfo.mipi.t_clk_post = 0x22;
	    pinfo.mipi.t_clk_pre = 0x3d;
#elif defined (MIPI_CLOCK_450MBPS)
	    pinfo.mipi.t_clk_post = 0x22;
	    pinfo.mipi.t_clk_pre = 0x3F;
#elif defined (MIPI_CLOCK_500MBPS)
        //500Mbps
	    pinfo.mipi.t_clk_post = 0x22;
		pinfo.mipi.t_clk_pre = 0x30;
	    //pinfo.mipi.t_clk_pre = 0x41;
        //490Mbps
	    //pinfo.mipi.t_clk_post = 0x22; 
	    //pinfo.mipi.t_clk_pre = 0x40;	    
#else
        //250Mbps
	    pinfo.mipi.t_clk_post = 10;
	    pinfo.mipi.t_clk_pre = 30;
#endif
        pinfo.mipi.stream = 0; /* dma_p */
        pinfo.mipi.mdp_trigger = DSI_CMD_TRIGGER_SW;
        pinfo.mipi.dma_trigger = DSI_CMD_TRIGGER_SW;
        pinfo.mipi.dsi_phy_db = &dsi_video_mode_phy_db;

	    pinfo.mipi.frame_rate = 60;
		pinfo.lcd.refx100 = 6000; /* 2011.07.29 lived */

        ret = mipi_sony_device_register(&pinfo, MIPI_DSI_PRIM,
				MIPI_DSI_PANEL_WVGA);
        if (ret)
			printk(KERN_ERR "%s: failed to register device!\n", __func__);

        return ret;
}

module_init(mipi_video_sony_wxga_init);
