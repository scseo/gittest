///////////////////////////////////////////////////////////////////////////////
/// @file MenuIcon.c
/// @brief Menu Icon functions.
/// @author MStarSemi Inc.
///
/// Functions to deal with Icons.
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////
#define _MENUICON_C

#include "datatype.h"
#include "Board.h"
#include "Global.h"
#include "Keypaddef.h"
#include "menudef.h"
#include "ColorPalette.h"
#include "MenuIconTbl.h"
//#include "DebugMsg.h"
#include "msOSD.h"
#include "ms_reg.h"
#include "Ms_rwreg.h"
#include "Menu.h"
#include "misc.h"

void DrawIcon(BYTE u8XPos, BYTE u8YPos, BYTE *pu8Icon, BYTE u8Flags, BYTE u8ColorSelect)
{
    WORD u16TempValue;
    int i,j;
    BYTE xdata u8LineOffset;
    if (pu8Icon==NULL)
        return;

#define ICON_WIDTH      (*(pu8Icon))
#define ICON_HEIGHT     (*(pu8Icon+1))
#define ICON_COLOR      (*(pu8Icon+2))
#define ICON_HICOLOR    (*(pu8Icon+3))
#define ICON_SELCOLOR   (*(pu8Icon+4))
#define ICON_PTR         (pu8Icon+5)
    switch(u8ColorSelect)
    {
        case CS_NORMAL:
            g_u8OsdFontColor=ICON_COLOR;
            break;
        case CS_HIGHLIGHT:
            g_u8OsdFontColor=ICON_HICOLOR;
            break;
        case CS_SELECTED:
             g_u8OsdFontColor=ICON_SELCOLOR;
            break;
    }
#define IS_DRAW_CODE u8ColorSelect
    if (u8YPos & BIT7)
    {
        u8YPos &= 0x7F;
        IS_DRAW_CODE = FALSE;
    }
    else
        IS_DRAW_CODE = TRUE;

    u16TempValue=g_u8OsdWndNo<<5;
    u8LineOffset=msReadByte(u16TempValue+OSD2_12);
    //u16TempValue=msRead2Byte(u16TempValue+OSD2_10)+(WORD)u8YPos*u8LineOffset+u8XPos; // get real address
    u16TempValue=GET_TXT_CASTART(g_u8OsdWndNo,u8XPos,u8YPos); // get real address
    if (u8Flags&ICO_OVER256)
    {   OSD_TEXT_HI_ADDR_SET_BIT8();
    }
    else if (u8Flags&ICO_OVER512)
        OSD_TEXT_HI_ADDR_SET_BIT9();
    if (u8Flags&ICO_WAIT) //Jison 080521
        WAIT_V_OUTPUT_BLANKING();
    for (i=0 ; i<ICON_HEIGHT; i++)
    {
        #if 0
        if (u8Flags&ICO_BOTTOM && (i!=ICON_HEIGHT-1))
            continue;   ///skip  line
        else if (u8Flags&ICO_TOP && i>0)
            break;
        #endif
        WRITE_CAFSRAM_ADDR();
        msWrite2Byte(PORT_ATTR_ADDR, u16TempValue);
        WRITE_ATTRIBUTE();
        for (j=0 ; j<ICON_WIDTH; j++)
        {   msWriteByte(PORT_ATTR_DATA, g_u8OsdFontColor);
        }
        u16TempValue+=u8LineOffset; // get real address
    }
    if (IS_DRAW_CODE)
    {
        u16TempValue-=(u8LineOffset*ICON_HEIGHT); // recover real address
        for (i=0 ; i<ICON_HEIGHT ; i++)
        {
            #if 0
            if (u8Flags&ICO_BOTTOM && i==0)
                continue;   ///skip 1st line
            else if (u8Flags&ICO_TOP && i==1)
                continue;   ///skip 2nd line
            #endif
            WRITE_CAFSRAM_ADDR();
            msWrite2Byte(PORT_CODE_ADDR, u16TempValue );
            WRITE_CODE();
            for (j=0 ; j<ICON_WIDTH; j++)
            {   msWriteByte(PORT_CODE_DATA, *(ICON_PTR+i*(ICON_WIDTH)+j));
            }
            u16TempValue+=u8LineOffset; // get real address
        }
    }
    if (u8Flags&(ICO_OVER256|ICO_OVER512))
        OSD_TEXT_HI_ADDR_CLR_TO_0();
#undef ICON_WIDTH
#undef ICON_HEIGHT
#undef ICON_COLOR
#undef ICON_HICOLOR
#undef ICON_SELCOLOR
#undef ICON_PTR
#undef IS_DRAW_CODE

}


///=========================================================================
#if 1  //Steven110525

// [lyg-20150302]
#if (MODEL_SELECT == KORTEK_MODEL)//[20150514 BH.Nam] #if BOOT_LOGO == KORTEK_LOGO
BYTE* GetLogoMstarTopIcon(void)
{ return IcoLogoMstarTop;
}
BYTE* GetLogoMstarTop2Icon(void)
{ return IcoLogoMstarTop2;
}
BYTE* GetLogoMstarTop3Icon(void)
{ return IcoLogoMstarTop3;
}

#elif (MODEL_SELECT == CHRISTIE_MODEL)//#elif BOOT_LOGO == CHRISTIE_LOGO
BYTE* GetLogoMstarTopIcon(void)
{ return IcoLogoMstarTop;
}
BYTE* GetLogoMstarTop2Icon(void)
{ return IcoLogoMstarTop2;
}
BYTE* GetLogoMstarTop3Icon(void)
{ return IcoLogoMstarTop3;
}
#elif (MODEL_SELECT == SMART_MODEL)//#elif BOOT_LOGO == SMART_LOGO
BYTE* GetLogoMstarTopIcon(void)
{ return IcoLogoMstarTop;
}
BYTE* GetLogoMstarTop2Icon(void)
{ return IcoLogoMstarTop2;
}
BYTE* GetLogoMstarTop3Icon(void)
{ return IcoLogoMstarTop3;
}

#elif (MODEL_SELECT == BARCO_MODEL)//KORTEK_LOGO
BYTE* GetLogoMstarTopIcon(void)
{ return IcoLogoMstarTop;
}
BYTE* GetLogoMstarTop2Icon(void)
{ return IcoLogoMstarTop2;
}
#else//#elif BOOT_LOGO == MSTAR_LOGO
BYTE* GetLogoMstarTopIcon(void)
{ return IcoLogoMstarTop;
}
#endif

BYTE* GetLogoMstarTop1Icon(void)
{ return IcoLogoMstarTop1;
}
BYTE* GetLogoMstarDownIcon(void)
{ return IcoLogoMstarDown;
}
#endif

BYTE* GetMainMenuLogoIcon(void)  //Steven110526
{ return IcoMainMenuLogo;
}

// [20150716 YG.Lee]
BYTE* GetGuageIcon(void)
{
    // [20151104 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconGuage_RV;
    }
    else
    {
        return IconGuage;
    }
}


// [20150806 YG.Lee]
BYTE* GetPictureIcon(void)
{
    return IconPicture;
}
BYTE* GetSoundIcon(void)
{
    // [20151103 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconSound_RV;
    }
    else
    {
        return IconSound;
    }
}
// [20150901 YG.Lee]
BYTE* GetHotVolumeIcon(void)
{
    if(USER_PREF_AUDIO_MUTE == TRUE)
    {
        if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
        {
            return IconHotMute_RV;
        }
        else
        {
            return IconHotMute;
        }
    }
    else
    {
        if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
        {
            return IconHotVolume_RV;
        }
        else
        {
            return IconHotVolume;
        }
    }
}

BYTE* GetOSDIcon(void)
{
    // [20151103 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconOSD_RV;
    }
    else
    {
        return IconOSD;
    }
}
BYTE* GetSetupIcon(void)
{
    return IconSetup;
}
BYTE* GetMultiIcon(void)
{
    return IconMulti;
}
BYTE* GetAboutIcon(void)
{
    return IconAbout;
}


BYTE* GetUPDNIcon(void)
{
    return IconUPDN;
}
BYTE* GetCircleIcon(void)
{
    return IconCircle;
}
BYTE* GetLTRTIcon(void)
{
    return IconLTRT;
}
// [20150904 YG.Lee]
BYTE* GetLTIcon(void)
{
    return IconLT;
}
BYTE* GetRTIcon(void)
{
    return IconRT;
}


BYTE* GetBarLTIcon(void)
{
    return IconBarLT;
}
BYTE* GetBarCTIcon(void)
{
    return IconBarCT;
}
BYTE* GetBarRTIcon(void)
{
    return IconBarRT;
}
BYTE* GetBarSTIcon(void)
{
    return IconBarST;
}

// [20150902 YG.Lee]
BYTE* GetSleepIcon(void)
{
    // [20151103 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconSleep_RV;
    }
    else
    {
        return IconSleep;
    }
}
BYTE* GetResetIcon(void)
{
    return IconReset;
}
BYTE* GetSleepBorderIcon(void)
{
     // [20151103 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconSleepBorder_RV;
    }
    else
    {
        return IconSleepBorder;
    }
}



// [20150925 YG.Lee]
BYTE* GetRBIcon(void)
{
    // [20151029 YG.Lee]
    if(USER_PREF_LANGUAGE == LANG_ARABIC || USER_PREF_LANGUAGE == LANG_HEBREW)
    {
        return IconRB_RV;
    }
    else
    {
        return IconRB;        
    }    
}




BYTE* GetMenuLockIcon(void)
{
    if (HOT_OSD_LOCK_FLAG || OSD_LOCK_FLAG)
        return IcoLock;
    return IcoUnlock;
}

