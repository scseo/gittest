///////////////////////////////////////////////////////////////////////////////
/// @file msOSD.c
/// @brief
/// @author MStarSemi Inc.
///
/// Functions to draw OSD.
///
/// Features
///  -OSD registers read/write.
///  -Draw number/Hex number/text/gauge etc.
///  -
///////////////////////////////////////////////////////////////////////////////
#define _MSOSD_C osdsdosdosodsodo

#include <stdlib.h>
#include "datatype.h"
#include "ms_reg.h"
#include "Board.h"
#include "MDebug.h"
#include "Common.h"
#include "Ms_rwreg.h"
#include "mStar.h"
#include "misc.h"
#include "Global.h"
#include "menudef.h"
#include "drvOSD.h"
#include "msOSD.h"
#if ENABLE_OSDC_OSDD
#include "drvOSDblend.h"
#endif
#include "MenuFont.h"
#include "ColorPalette.h"
#include "LoadPropFont1218.h"

//==================================================================================
// variable declaration
//==================================================================================
WORD xdata g_u8OsdWindowWidth = 10;	// maximum value may over 1 byte. 4096/12 = 381.3333
BYTE xdata g_u8OsdWindowHeight = 10;
#if 0//defined(_OSD_ROTATION_270_)
BYTE xdata g_u8OsdWindow2Height=10;
#endif
BYTE xdata g_u8OsdFontColor = 0x01;
BYTE xdata g_u8OsdWndNo=OSD_MAIN_WND;
BYTE xdata g_u8FontAddrHiBits=0;
BYTE xdata g_u8OsdFontDataHighByte=0;
//WrapLayerFlagType xdata WrapLayerFlag;

extern BYTE xdata g_u8MenuPageIndex;
//==================================================================================
// Code start

// setup osd window size
void Osd_SetWindowSize(BYTE u8Width, BYTE u8Height/*, Bool bReset*/)
{
    g_u8OsdWindowWidth = u8Width;
    g_u8OsdWindowHeight = u8Height;

    //bReset=bReset;
    drvOSD_SetWndLineOffset(OSD_MAIN_WND,g_u8OsdWindowWidth); //for OSD_MAIN_WND only
  #if ENABLE_OSD_ROTATION //reset CABase addr after line offset was set
    if (MENU_ROTATION_FLAG)
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, GET_CABASE_0_90(OSD_MAIN_WND_CA_BASE, g_u8OsdWindowWidth));
    else
        drvOSD_SetWndCABaseAddr(OSD_MAIN_WND, OSD_MAIN_WND_CA_BASE);
  #endif
}

// setup
void SetOsdPositionReg(WORD u16XPos, WORD u16YPos)
{
    //only for main wnd
#if ENABLE_OSD_ROTATION
    if (MENU_ROTATION_FLAG)
    {
        drvOSD_SetWndHPosition(OSD_MAIN_WND, u16XPos, u16XPos+((WORD)g_u8OsdWindowHeight*18));
        drvOSD_SetWndVPosition(OSD_MAIN_WND, u16YPos, u16YPos+((WORD)g_u8OsdWindowWidth*12));
    }
    else
#endif
    {
        drvOSD_SetWndHPosition(OSD_MAIN_WND, u16XPos, u16XPos+((WORD)g_u8OsdWindowWidth*12));
        drvOSD_SetWndVPosition(OSD_MAIN_WND, u16YPos, u16YPos+((WORD)g_u8OsdWindowHeight*18));
    }
}

//Return position with pixel unit, input value is %
WORD Osd_MenuHPosition(BYTE u8XPos, Bool Set)
{
    WORD u16PosValue;
    WORD u16width = PANEL_WIDTH;
#if ENABLE_OSDC_OSDD
#if _LIMIT_OSDSIZE_
    if(u16width > LIMIT_OSDC_HSIZE)
        u16width = LIMIT_OSDC_HSIZE;
#endif
#else
	#if ((CHIP_ID == MST9U3) && (!ENABLE_OSD_LR))
    u16width /=2;
	#endif
#endif
    //only for main wnd
#if ENABLE_OSD_ROTATION
    if (MENU_ROTATION_FLAG)
    {
        u16PosValue=((DWORD)u8XPos*(u16width-(WORD)g_u8OsdWindowHeight*18)/100);
        if (Set)
            drvOSD_SetWndHPosition(OSD_MAIN_WND, u16PosValue, u16PosValue+((WORD)g_u8OsdWindowHeight*18));
    }
    else
#endif
    {
        u16PosValue=((DWORD)u8XPos*(u16width-(WORD)g_u8OsdWindowWidth*12)/100);
        if (Set)
            drvOSD_SetWndHPosition(OSD_MAIN_WND, u16PosValue, u16PosValue+((WORD)g_u8OsdWindowWidth*12));
    }
    return u16PosValue;
}
//Return position with pixel unit, input value is %
WORD Osd_MenuVPosition(BYTE u8YPos, Bool Set)
{
    WORD u16PosValue;
    WORD u16hiegh = PANEL_HEIGHT;
#if ENABLE_OSDC_OSDD
#if _LIMIT_OSDSIZE_
    if(u16hiegh > LIMIT_OSDC_VSIZE)
        u16hiegh = LIMIT_OSDC_VSIZE;
#endif
#endif
    //only for main wnd
#if ENABLE_OSD_ROTATION
    if (MENU_ROTATION_FLAG)
    {
        u16PosValue=((DWORD)u8YPos*(u16hiegh-(WORD)g_u8OsdWindowWidth*12)/100);
        if (Set)
            drvOSD_SetWndVPosition(OSD_MAIN_WND, u16PosValue, u16PosValue+((WORD)g_u8OsdWindowWidth*12));
    }
    else
#endif
    {
        u16PosValue=((DWORD)u8YPos*(u16hiegh-(WORD)g_u8OsdWindowHeight*18)/100);
        if (Set)
            drvOSD_SetWndVPosition(OSD_MAIN_WND, u16PosValue, u16PosValue+((WORD)g_u8OsdWindowHeight*18));
    }
    return u16PosValue;
}
void Osd_SetPosition(BYTE u8XPos, BYTE u8YPos)
{
    Osd_MenuHPosition(u8XPos,TRUE);
    Osd_MenuVPosition(u8YPos,TRUE);
}

void Osd_DrawRealChar(BYTE u8XPos, BYTE u8YPos, BYTE u8Char)
{
    drvOSD_DrawRealChar(g_u8OsdWndNo, u8XPos, u8YPos, u8Char);
}
void Osd_DrawContinuousChar(BYTE u8XPos, BYTE u8YPos, BYTE u8Char, BYTE u8Num)
{
    drvOSD_DrawContinuousChar(g_u8OsdWndNo,u8XPos,u8YPos,u8Char,u8Num);
}

#ifndef FM_NUMBER_START
#define FM_NUMBER_START 0x00
#endif
#define NUM2ASCII(Num) ((Num)+FM_NUMBER_START)//(Num+0x30)
void DrawNum(BYTE u8XPos, BYTE u8YPos, char len, int s16Value)
{
    char _minus=0;

    if (s16Value<0)
    {
        s16Value=0-s16Value;
        _minus=1;
    }
    if (s16Value==0)
        Osd_DrawRealChar(u8XPos+(len--), u8YPos, (BYTE)NUM2ASCII(0));
    else
    {
        while (s16Value && len) // translate integer to string
        {
            Osd_DrawRealChar(u8XPos+(len--), u8YPos, (BYTE)NUM2ASCII(s16Value%10));
            s16Value/=10;
        }
    }
    if (_minus && len>=0)// add sign
        Osd_DrawRealChar(u8XPos+(len--), u8YPos, 0x0A);//(BYTE)'-');

    _minus=(_minus)?(0):(1);

    for (; _minus<=len; _minus++)
        Osd_DrawRealChar(u8XPos+_minus, u8YPos, 0x00);// space
}

void DrawNumUnsigned(BYTE u8XPos, BYTE u8YPos, char len, WORD u16Value)
{
    BYTE i;
    if (u16Value==0)
        Osd_DrawRealChar(u8XPos+(len--), u8YPos, (BYTE)NUM2ASCII(0));
    else
    {
        while (u16Value && len) // translate integer to string
        {
            Osd_DrawRealChar(u8XPos+(len--), u8YPos, (BYTE)NUM2ASCII(u16Value%10));
            u16Value/=10;
        }
    }
    for (i=1; i<=len; i++)
        Osd_DrawRealChar(u8XPos+i, u8YPos, 0x00);// space
}

void Osd_DrawNum(BYTE u8XPos, BYTE u8YPos, int s16Value)
{
    DrawNum(u8XPos, u8YPos, 3, s16Value);
}
#if 1
void Osd_Draw5Num(BYTE u8XPos, BYTE u8YPos, WORD u16Value)
{
    DrawNumUnsigned(u8XPos, u8YPos, 5, u16Value);
}
#endif

//===================================================================================
void Osd_SetWindowAtr(BYTE u8Win_Num,BYTE u8HStart,BYTE u8HEnd ,BYTE u8VStart, BYTE u8VEnd,BYTE u8Color)
{
    //drvOSD_SetWndOn(u8Win_Num, FALSE); //Jison 110922
#if ENABLE_OSD_ROTATION
    if (MENU_ROTATION_FLAG)
    {
        WORD xdata u16MainHStart=drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_H_START);
        WORD xdata u16MainVEnd=drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_V_END);
        drvOSD_SetWndCABaseAddr(u8Win_Num, (u8HEnd-u8HStart+OSD_MAIN_WND_CA_BASE)+ //(u8HEnd-u8HStart+1-1+OSD_MAIN_WND_CA_BASE)
            (WORD)u8VStart*drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_LINE_OFFSET)+u8HStart);
        drvOSD_SetWndHPosition(u8Win_Num, u16MainHStart+(WORD)u8VStart*18, u16MainHStart+((WORD)u8VEnd+1)*18);
        drvOSD_SetWndVPosition(u8Win_Num, u16MainVEnd-((WORD)u8HEnd+1)*12, u16MainVEnd-(WORD)u8HStart*12);
      #ifndef _MSTAR_OSD_
        drvOSD_SetTGenHPosition(OSD_TG0, u16MainHStart+(WORD)u8VStart*18, u16MainHStart+((WORD)u8VEnd+1)*18);
        drvOSD_SetTGenVPosition(OSD_TG0, u16MainVEnd-((WORD)u8HEnd+1)*12, u16MainVEnd-(WORD)u8HStart*12);
        drvOSD_SetTGenHPosition(OSD_TG1, u16MainHStart+(WORD)u8VStart*18+9, u16MainHStart+((WORD)u8VEnd+1)*18);
        drvOSD_SetTGenVPosition(OSD_TG1, u16MainVEnd-((WORD)u8HEnd+1)*12, u16MainVEnd-(WORD)u8HStart*12);
      #endif
    }
    else
#endif
    {
        WORD xdata u16MainHStart=drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_H_START);
        WORD xdata u16MainVStart=drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_V_START);
        drvOSD_SetWndCABaseAddr(u8Win_Num, drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_CABASE)+
            (WORD)u8VStart*drvOSD_GetWndInfo(OSD_MAIN_WND,OSD_WND_INFO_LINE_OFFSET)+u8HStart);
        drvOSD_SetWndHPosition(u8Win_Num, u16MainHStart+(WORD)u8HStart*12, u16MainHStart+((WORD)u8HEnd+1)*12);
        drvOSD_SetWndVPosition(u8Win_Num, u16MainVStart+(WORD)u8VStart*18, u16MainVStart+((WORD)u8VEnd+1)*18);
      #ifndef _MSTAR_OSD_
        drvOSD_SetTGenHPosition(OSD_TG0, u16MainHStart+(WORD)u8HStart*12, u16MainHStart+((WORD)u8HEnd+1)*12);
        drvOSD_SetTGenVPosition(OSD_TG0, u16MainVStart+(WORD)u8VStart*18, u16MainVStart+((WORD)u8VEnd+1)*18);
        drvOSD_SetTGenHPosition(OSD_TG1, u16MainHStart+(WORD)u8HStart*12, u16MainHStart+((WORD)u8HEnd+1)*12);
        drvOSD_SetTGenVPosition(OSD_TG1, u16MainVStart+(WORD)u8VStart*18+9, u16MainVStart+((WORD)u8VEnd+1)*18);
      #endif
    }
  #if 1
    #ifdef _MSTAR_OSD_
    u8Color=MONO_COLOR(CP_MASK_COLOR, CP_BLACK); // Jison CP_MASK_COLOR to see PG
    #else
    u8Color=MONO_COLOR(CP_CURSOR_SELECT_COLOR, CP_MASK_COLOR); // Jison CP_MASK_COLOR to see PG
    #endif
    drvOSD_SetWndFixAttr(u8Win_Num,_ENABLE,u8Color);
  #else
    u8Color=u8Color;
  #endif

  #ifndef _MSTAR_OSD_
    if (msReadByte((OSD_MAIN_WND<<5)+OSD2_00)&BIT0) // check if main wnd on
    {
        WAIT_V_OUTPUT_BLANKING_START(); //Jison wait 1 frame to avoid the last text garbage
        drvOSD_SetWndOn(u8Win_Num, TRUE);
        drvOSD_SetTGenOn(OSD_TG0,TRUE);
        drvOSD_SetTGenOn(OSD_TG1,TRUE);
    }
    else
        WrapLayerFlag.bSync2Main=1;
  #endif
}
void Osd_SetWindowEnable(BYTE u8Win_Num,Bool u8Enable)
{
    drvOSD_SetWndOn(u8Win_Num, u8Enable);
}

void Osd_SetTransparency(BYTE u8Value)
{
#if 0 //use OSD alpha blending
    msWriteByte(SCL0_10_46, u8Value);
#else //use fix Alpha
    u8Value=0x3F-(u8Value*8);
    drvOSD_SetWndFixAlpha(OSD_MAIN_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_BUTTON_WND, OSD_ALPHA_OUT, u8Value);
    drvOSD_SetWndFixAlpha(OSD_CURSOR_WND, OSD_ALPHA_OUT, u8Value);
#endif
}
///==============================

void Osd_DrawBlankPlane(BYTE u8XPos,BYTE u8YPos,BYTE u8Width,BYTE u8Height)
{
    drvOSD_DrawBlankPlane(g_u8OsdWndNo,u8XPos,u8YPos,u8Width,u8Height);
}
void Osd_DrawHex(BYTE u8XPos, BYTE u8YPos, BYTE u8Value)
{
#define HEX2ASCII(u8Num) ((u8Num)+FM_NUMBER_START)
    Osd_DrawRealChar(u8XPos+1, u8YPos, HEX2ASCII(u8Value&0x0F));
    Osd_DrawRealChar(u8XPos, u8YPos, HEX2ASCII(u8Value>>4));
#undef HEX2ASCII
}

BYTE Osd_DoubleBuffer(Bool u8Enable)
{
    u8Enable=0;
    return u8Enable;
}




