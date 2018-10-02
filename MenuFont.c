///////////////////////////////////////////////////////////////////////////////
/// @file MenuFont.c
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load Non-Prop fonts and color fonts.
///
/// Features
///  - Using CompFont.Lib
///////////////////////////////////////////////////////////////////////////////
#include "datatype.h"
#include "Board.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Global.h"
#include "menudef.h"
#include "msOSD.h"

#include "MenuFont.h"
#include "Menu.h"
#include "ColorPalette.h"

#include "Common.h"
#include "LoadPropFont1218.h"

#include "MenuFontTbl.h"
123
ABCDEFG
extern void mStar_LoadCompressedFont(BYTE u8Addr, const WORD *pu16FontPtr, WORD u16Count);
extern void LoadCompressColorFont(const BYTE *pu8FontTbl, WORD *pu16MapPtr, BYTE countu8Count);

#define MENUFONT_DEBUG    0 // [lyg-20150324] : 1
#if ENABLE_MSTV_UART_DEBUG&&MENUFONT_DEBUG
    #define MENUFONT_printData(str, value)   printData(str, value)
    #define MENUFONT_printMsg(str)           printMsg(str)
#else
    #define MENUFONT_printData(str, value)
    #define MENUFONT_printMsg(str)
#endif

#if 0  //Steven110523, for 8 color font
//8 color font raw data
void Osd_Load8ColorFont(BYTE u8Addr, const BYTE *pu8Font, WORD u16Num)
{
    DWORD xdata u32Data;
    BYTE xdata u8FontData0,u8FontData1,u8FontData2;
    BYTE xdata u8PixelBit;
    BYTE xdata i;

    OSD_SET_WRITE_FONT_MODE(WR_FONT_MODE_3BIT);
    WRITE_CAFSRAM_ADDR();
    msWrite2Byte(PORT_FONT_ADDR, GET_FONT_RAM_ADDR(u8Addr));
    WRITE_FONT();
    // handle 2 lines, 8 pixels each time, 2 lines x 12 pixels = 8 pixels x 3 times
    while(u16Num)
    {
        for (i=0;i<3;i++)
        {
        //    0        1        2        3        4        5        6        7        8
        //11110000 00000000 11111111 00002222 22222222 00000000 22221111 11111111 22222222 //line 0/1
            if (i==0)
            {
                u8FontData0=((*(pu8Font))<<4)+((*(pu8Font+1))>>4);
                u8FontData1=((*(pu8Font))&0xF0)+((*(pu8Font+2))>>4);
                u8FontData2=((*(pu8Font+3))<<4)+((*(pu8Font+4))>>4);
            }
            else if (i==1)
            {
                u8FontData0=((*(pu8Font+1))<<4)+((*(pu8Font+3))>>4);
                u8FontData1=((*(pu8Font+2))<<4)+((*(pu8Font+6))&0x0F);
                u8FontData2=((*(pu8Font+4))<<4)+((*(pu8Font+6))>>4);
            }
            else
            {
                u8FontData0=(*(pu8Font+5));
                u8FontData1=(*(pu8Font+7));
                u8FontData2=(*(pu8Font+8));
            }
            u8PixelBit=BIT7;
            u32Data=0;
            while(u8PixelBit)
            {
                u32Data<<=3;
                if (u8FontData0&u8PixelBit) //color bit0
                    u32Data|=BIT0;
                if (u8FontData1&u8PixelBit) //color bit1
                    u32Data|=BIT1;
                if (u8FontData2&u8PixelBit) //color bit2
                    u32Data|=BIT2;
                u8PixelBit>>=1;
            }
            msWriteByte(PORT_FONT_DATA, u32Data>>16);
            msWriteByte(PORT_FONT_DATA, u32Data>>8);
            msWriteByte(PORT_FONT_DATA, u32Data);
        }
        if (u16Num>=9)
        {
            u16Num-=9;
            pu8Font+=9;
        }
        else
            u16Num=0;
    }
    OSD_SET_WRITE_FONT_MODE(WR_FONT_MODE_MONO); //recover
}
#endif

///============================================================
void InitGradualColor(void)
{
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL0,
                     (OWC0_FP_MODE|
                      OWC0_GROUP_SEL0|
                      OWC0_LV1_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_LV2_ALPHA_SRC(OSD_A_FIX)|
                      OWC0_OUT_ALPHA_SRC(OSD_A_FIX)),
                      OWC0MASK_ALL);
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL1,
                     (OWC1_1BP_44MODE_EN|
                      OWC1_44MODE_TRS_EN|
                      OWC1_FONT_HEIGHT(18)),
                      OWC1MASK_ALL);
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL2,
                     (OWC2_GD_COLOR_R(OSD_GD_EG3)|
                      OWC2_GD_COLOR_G(OSD_GD_EG4)|
                      OWC2_GD_COLOR_B(OSD_GD_EG5)),
                      OWC2MASK_ALL);
    drvOSD_SetWndCtrl(MAIN_GD_WIN, OSD_WND_CTRL3,
                      (OWC3_1BPP_ATTR_BIT1|OWC3MASK_CKEY_INV),
                      OWC3MASK_ALL);

    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_LV2,0x00); //PG   //(1-a)*low_lvl+a*top_lvl
    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_LV1,0x00); //PG
    drvOSD_SetWndFixAlpha(MAIN_GD_WIN,OSD_ALPHA_OUT,0x3F); //OSD

}
extern void LoadMainMenuPropFont(void);

void InitOsdAfterLogo(void)		// 20150127
{
//    MENUFONT_printMsg("load osd data");

    InitGradualColor();
    LoadNormalCP();
    //Static Mono Font -0x00~0x100
    mStar_LoadCompressedFont(COMMON_FONT_START, tCommonArea, 0);/// Load Common font
    mStar_LoadCompressedFont(SM_CHECK_2C_FONT_START, tCheckMark2ColorIconFont, 0);

    LoadMainMenuPropFont();

    OSD_FONT_HI_ADDR_SET_BIT9(); //enable bit 9 0x200~
    //Load 4 color Cursor and 8 color gauge font
    OSD_WRITE_FONT_ADDRESS(MM_4COLOR_CURSOR_START);
    LoadCompressColorFont(tCursor4ColorIconFont, NULL, MM_4COLOR_CURSORFONT_CNT);

    //Load 8 Color Font
    //Osd_LoadFont(MM_8COLOR_ICON_START, tMain8ColorIconFont, sizeof(tMain8ColorIconFont)); //101125 move to LoadMainMenuIconFont()



// [20150625 YG.Lee]	// [lyg-20150319]
    OSD_WRITE_FONT_ADDRESS(MENU_LOGO_4C_START+2);
    LoadCompressColorFont(tLogo4Color_IconFont, NULL, MENU_LOGO_4C_ICON_CNT);
//    Osd_LoadFont(MENU_LOGO_4C_START+1, tMainMenuLogo8Color_IconFont, sizeof(tMainMenuLogo8Color_IconFont));


// [20150716 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(GUAGE_4C_START+2);
    LoadCompressColorFont(tGuage4Color_IconFont, NULL, GUAGE_4C_ICON_CNT);

// [20150806 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(PICTURE_4C_START+2);
    LoadCompressColorFont(tPicture4Color_IconFont, NULL, PICTURE_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SOUND_4C_START+2);
    LoadCompressColorFont(tSound4Color_IconFont, NULL, SOUND_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(OSD_4C_START+2);
    LoadCompressColorFont(tOSD4Color_IconFont, NULL, OSD_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SETUP_4C_START+2);
    LoadCompressColorFont(tSetup4Color_IconFont, NULL, SETUP_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(MULTI_4C_START+2);
    LoadCompressColorFont(tMulti4Color_IconFont, NULL, MULTI_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(ABOUT_4C_START+2);
    LoadCompressColorFont(tAbout4Color_IconFont, NULL, ABOUT_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(UPDN_4C_START+2);
    LoadCompressColorFont(tUPDN4Color_IconFont, NULL, UPDN_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(CIRCLE_4C_START+2);
    LoadCompressColorFont(tCircle4Color_IconFont, NULL, CIRCLE_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(LTRT_4C_START+2);
    LoadCompressColorFont(tLTRT4Color_IconFont, NULL, LTRT_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(BARLT_4C_START+2);
    LoadCompressColorFont(tBarLT4Color_IconFont, NULL, BARLT_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(BARCT_4C_START+2);
    LoadCompressColorFont(tBarCT4Color_IconFont, NULL, BARCT_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(BARRT_4C_START+2);
    LoadCompressColorFont(tBarRT4Color_IconFont, NULL, BARRT_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(BARST_4C_START+2);
    LoadCompressColorFont(tBarST4Color_IconFont, NULL, BARST_4C_ICON_CNT);
    // [20150902 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(SLEEP_4C_START+2);
    LoadCompressColorFont(tSleep4Color_IconFont, NULL, SLEEP_4C_ICON_CNT);    
    OSD_WRITE_FONT_ADDRESS(RESET_4C_START+2);
    LoadCompressColorFont(tReset4Color_IconFont, NULL, RESET_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SLEEP_BORDER_4C_START+2);
    LoadCompressColorFont(tSleepBorder4Color_IconFont, NULL, SLEEP_BORDER_4C_ICON_CNT);

    // [20150925 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(RB_4C_START+2);
    LoadCompressColorFont(tRB4Color_IconFont, NULL, RB_4C_ICON_CNT);

    // [20151029 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(RB_RV_4C_START+2);
    LoadCompressColorFont(tRB_RV4Color_IconFont, NULL, RB_RV_4C_ICON_CNT);

    // [20151103 YG.Lee]
    OSD_WRITE_FONT_ADDRESS(OSD_RV_4C_START+2);
    LoadCompressColorFont(tOSD_RV4Color_IconFont, NULL, OSD_RV_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SOUND_RV_4C_START+2);
    LoadCompressColorFont(tSOUND_RV4Color_IconFont, NULL, SOUND_RV_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SLEEP_RV_4C_START+2);
    LoadCompressColorFont(tSLEEP_RV4Color_IconFont, NULL, SLEEP_RV_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(SLEEP_BORDER_RV_4C_START+2);
    LoadCompressColorFont(tSLEEP_BORDER_RV4Color_IconFont, NULL, SLEEP_BORDER_RV_4C_ICON_CNT);
    OSD_WRITE_FONT_ADDRESS(GUAGE_RV_4C_START+2);
    LoadCompressColorFont(tGuage4_RV_Color_IconFont, NULL, GUAGE_RV_4C_ICON_CNT);



    OSD_FONT_HI_ADDR_CLR_TO_0();

    msWrite2ByteMask(OSD1_0A,0x0200+_4COLOR_FONT_START,0x03FF); //4 color start
    msWrite2ByteMask(OSD1_0C,0x03FF,0x03FF); //8 color start
    msWrite2ByteMask(OSD1_0E,0x03FF,0x03FF);                     //16 color start
}
void LoadLogoIconFont(void)
{
    //Steven110525
    MENUFONT_printMsg("LoadLogoIconFont");
	// [lyg-20150302]
  #if (MODEL_SELECT == KORTEK_MODEL)//[20150514 BH.Nam] #if BOOT_LOGO == KORTEK_LOGO
	OSD_FONT_HI_ADDR_CLR_TO_0();
	OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTopFont, NULL, 54);
	OSD_FONT_HI_ADDR_SET_BIT8();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTop2Font, NULL, 50);
	OSD_FONT_HI_ADDR_SET_BIT9();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTop3Font, NULL, 9);
    OSD_FONT_HI_ADDR_CLR_TO_0();	
	msWrite2ByteMask(OSD1_0A,0x0000,0x03FF); //4 color start
  #elif (MODEL_SELECT == CHRISTIE_MODEL)//#elif BOOT_LOGO == CHRISTIE_LOGO
	OSD_FONT_HI_ADDR_CLR_TO_0();
	OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTopFont, NULL, 76);
	OSD_FONT_HI_ADDR_SET_BIT8();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTop2Font, NULL, 61);
	OSD_FONT_HI_ADDR_SET_BIT9();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	LoadCompressColorFont(tMstarLogoTop3Font, NULL, 34);
    OSD_FONT_HI_ADDR_CLR_TO_0();	
	msWrite2ByteMask(OSD1_0A,0x0000,0x03FF); //4 color start

  #elif (MODEL_SELECT == SMART_MODEL)
	  OSD_FONT_HI_ADDR_CLR_TO_0();
	  OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	  LoadCompressColorFont(tMstarLogoTopFont, NULL, 40);
	  OSD_FONT_HI_ADDR_SET_BIT8();
	  OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	  LoadCompressColorFont(tMstarLogoTop2Font, NULL, 46);
	  OSD_FONT_HI_ADDR_SET_BIT9();
	  OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
	  LoadCompressColorFont(tMstarLogoTop3Font, NULL, 19);
	  OSD_FONT_HI_ADDR_CLR_TO_0();	  
	  msWrite2ByteMask(OSD1_0A,0x0000,0x03FF); //4 color start
  #elif (MODEL_SELECT == BARCO_MODEL)//KORTEK_LOGO
    OSD_FONT_HI_ADDR_CLR_TO_0();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
    LoadCompressColorFont(tMstarLogoTopFont, NULL, 86);//[20150601 BH.Nam]logo_display font size barco_logo
    OSD_FONT_HI_ADDR_SET_BIT8();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
    LoadCompressColorFont(tMstarLogoTop2Font, NULL, 50);//[20150601 BH.Nam]logo_display font size barco_logo
    OSD_FONT_HI_ADDR_CLR_TO_0();
    msWrite2ByteMask(OSD1_0A,0x0000,0x03FF); //4 color start	
  #else//#elif BOOT_LOGO == MSTAR_LOGO
	mStar_LoadCompressedFont(1, tMstarLogoTop1Font, 0);
    mStar_LoadCompressedFont(LOGO_2C_START+0x01, tMstarLogoDownFont, LOGO_2C_ICON_DOWN_CNT);
    OSD_FONT_HI_ADDR_SET_BIT8();
    OSD_WRITE_FONT_ADDRESS(LOGO_4C_START+2);
    LoadCompressColorFont(tMstarLogoTopFont, NULL, LOGO_4C_ICON_TOP_CNT);
    OSD_FONT_HI_ADDR_CLR_TO_0();
	msWrite2ByteMask(OSD1_0A,0x0100,0x03FF); //4 color start
	#endif
    msWrite2ByteMask(OSD1_0C,0x03FF,0x03FF);//0x0200+_8COLOR_FONT_START); //8 color start
    msWrite2ByteMask(OSD1_0E,0x03FF,0x03FF);//0x2C0);                     //16 color start
}

////Icon Fonts
void LoadMenuLockIconFont(void)
{
    mStar_LoadCompressedFont(0x2A,tHotLockIcon,0);
}

#if 1
void LoadFMNumberFont(void)
{
    mStar_LoadCompressedFont(FM_NUMBER_START, tNumber, 16);/// Load Number font
}
#endif


