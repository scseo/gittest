///////////////////////////////////////////////////////////////////////////////
/// @file ColorPalette.c
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to load color palette.
///
/// Features
///  -
///////////////////////////////////////////////////////////////////////////////
#include "datatype.h"
#include "Board.h"
#include "ms_reg.h"
#include "Global.h"
#include "menudef.h"
#include "ColorPalette.h"
#include "ColorPaletteTbl.h"
//#include "debugMsg.h"
#include "Ms_rwreg.h"
#include "mStar.h"
#include "MenuFont.h"
#include "Menu.h"
#include "drvOSD.h"
#include "msOSD.h"
#test111

void Load256ColorPalette(BYTE u8Group, BYTE u8Addr, ColorPaletteType *colorPtr, WORD u16Num)
{
	 WORD i;
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    //write R/G/B
    msWriteByte(OSD1_66, u8Addr);
    if (u8Group==0)
        WRITE_PSRAM0_COLOR();
    else
        WRITE_PSRAM1_COLOR();

    for (i=0;i<u16Num;i++)
    {
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Blue));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Green));
        msWriteByte(OSD1_66, ((colorPtr+i)->u8Green));
    }

    //write colorkey
    if (u8Group==0)
        WRITE_PSRAM0_ADDR();
    else
        WRITE_PSRAM1_ADDR();
    msWriteByte(OSD1_66, u8Addr);
    if (u8Group==0)
        WRITE_PSRAM0_COLOR_KEY();
    else
        WRITE_PSRAM0_COLOR_KEY();
    while (u16Num--)
    {
        if(colorPtr->u8ColorKey)
            msWriteByteMask(OSD1_66, BIT0,BIT0);
        else
            msWriteByteMask(OSD1_66, 0,BIT0);
        colorPtr++;
    }


}

void LoadLogoCP(void)
{

    Load256ColorPalette(0, 0, (ColorPaletteType *)(&tLogoColorPalette), sizeof(tLogoColorPalette)/sizeof(ColorPaletteType) );
}

void LoadNormalCP(void)
{

    Load256ColorPalette(0, 0, (ColorPaletteType *)(&tOSDColorPalette0), sizeof(tOSDColorPalette0)/sizeof(ColorPaletteType) );
}


