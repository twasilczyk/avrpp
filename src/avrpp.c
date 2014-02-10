/*---------------------------------------------------------------------------*/
/* AVRPP - AVR Parallel Programming Controller                               */
/* R0.43d (C)ChaN, 2012                                                      */
/*---------------------------------------------------------------------------*/
/* R0.31  Nov 11, '04  Migration from MS-DOS based AVRXP R0.25               */
/* R0.32  Feb 02, '05  mega406                                               */
/* R0.33  Feb 11, '05  90PWM2/3                                              */
/* R0.34  Feb 15, '05  tiny25/45/85                                          */
/* R0.35  Mar 12, '05  mega640/1280/1281/2560/2561                           */
/* R0.36  Aug 10, '05  tiny25/45/85                                          */
/* R0.37  Jan 30, '06  90CAN32/64/128, -q switch                             */
/* R0.38  Mar 15, '06  ATmega644, Fixed number of cals for tiny2313          */
/* R0.39  Mar 18, '07  ATmega164P/324P/644P, ATtiny261/461/861               */
/* R0.40  Aug 08, '07  ATmega48P/88P/168P/328P                               */
/* R0.41  Dec  7, '08  ATmega325P/3250P/324PA, AT90PWM216/316                */
/* R0.42  Feb  8, '10  ATtiny43U/48/88/87/167                                */
/* R0.43  Jul 21, '10  Supported Flash/EEPROM/Fuse combined hex files        */
/* R0.43b Sep 02, '10  Added -ff switch, ATtiny4313                          */
/* R0.43d Dec 23, '12  ATinty1634                                            */
/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "avrpp.h"



/*-----------------------------------------------------------------------
  Device properties
-----------------------------------------------------------------------*/

const DEVPROP DevLst[] =	/* Device property list */
{
	/* Name,          ID,    Signature,              FS,  FP,   ES, EP, EW, FW,   LB,FT,Cal, FuseMask[]       , FuseDefault[] */
	{ "90S1200",      S1200, {0x1E, 0x90, 0x01},   1024,   0,   64,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S2313",      S2313, {0x1E, 0x91, 0x01},   2048,   0,  128,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S4414",      S4414, {0x1E, 0x92, 0x01},   4096,   0,  256,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S8515",      S8515, {0x1E, 0x93, 0x01},   8192,   0,  512,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S2333",      S2333, {0x1E, 0x91, 0x05},   2048,   0,  128,  0, 10,  2, 0xF9, 1, 0, {0x1F, 0x00, 0x00}, {0xDA, 0xFF, 0xFF} },
	{ "90S4433",      S4433, {0x1E, 0x92, 0x03},   4096,   0,  256,  0, 10,  2, 0xF9, 1, 0, {0x1F, 0x00, 0x00}, {0xDA, 0xFF, 0xFF} },
	{ "90S4434",      S4434, {0x1E, 0x92, 0x02},   4096,   0,  256,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S8535",      S8535, {0x1E, 0x93, 0x03},   8192,   0,  512,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S2323",      S2323, {0x1E, 0x91, 0x02},   2048,   0,  128,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "90S2343",      S2343, {0x1E, 0x91, 0x03},   2048,   0,  128,  0, 10,  2, 0xF9, 0, 0, {0x21, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "tiny11",       T11,   {0x1E, 0x90, 0x04},   1024,   0,    0,  0,  0,  2, 0x00, 1, 0, {0x1F, 0x00, 0x00}, {0xFC, 0xFF, 0xFF} },
	{ "tiny12",       T12,   {0x1E, 0x90, 0x05},   1024,   0,   64,  0,  0,  0, 0x00, 1, 1, {0xFF, 0x00, 0x00}, {0xD2, 0xFF, 0xFF} },
	{ "tiny15",       T15,   {0x1E, 0x90, 0x06},   1024,   0,   64,  0,  0,  0, 0xF9, 1, 1, {0xF3, 0x00, 0x00}, {0x5C, 0xFF, 0xFF} },
	{ "tiny22",       T22,   {0x1E, 0x91, 0x06},   2048,   0,  128,  0, 10,  2, 0xF9, 1, 0, {0x21, 0x00, 0x00}, {0xDE, 0xFF, 0xFF} },
	{ "tiny26",       T26,   {0x1E, 0x91, 0x09},   2048,  32,  128,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0x1F, 0x00}, {0xE1, 0xF7, 0xFF} },
	{ "tiny261",      T261,  {0x1E, 0x91, 0x0C},   2048,  32,  128,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny461",      T461,  {0x1E, 0x92, 0x08},   4096,  64,  256,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny861",      T861,  {0x1E, 0x93, 0x0D},   8192,  64,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny28",       T28,   {0x1E, 0x91, 0x07},   2048,   0,    0,  0,  0,  0, 0xF9, 1, 1, {0x1F, 0x00, 0x00}, {0xF2, 0xFF, 0xFF} },
	{ "tiny13",       T13,   {0x1E, 0x90, 0x07},   1024,  32,   64,  4,  0,  0, 0x00, 2, 1, {0xFF, 0x1F, 0x00}, {0x6A, 0xFF, 0xFF} },
	{ "tiny25",       T25,   {0x1E, 0x91, 0x08},   2048,  32,  128,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny45",       T45,   {0x1E, 0x92, 0x06},   4096,  64,  256,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny85",       T85,   {0x1E, 0x93, 0x0B},   8192,  64,  512,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny24",       T24,   {0x1E, 0x91, 0x0B},   2048,  32,  128,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny44",       T44,   {0x1E, 0x92, 0x07},   4096,  64,  256,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny84",       T84,   {0x1E, 0x93, 0x0C},   8192,  64,  512,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny87",       T87,   {0x1E, 0x93, 0x87},   8192, 128,  512,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny167",      T167,  {0x1E, 0x94, 0x87},  16384, 128,  512,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny43U",      T43U,  {0x1E, 0x92, 0x0C},   4096,  64,   64,  4,  0,  0, 0x00, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "tiny48",       T48,   {0x1E, 0x92, 0x09},   4096,  64,   64,  4,  0,  0, 0x00, 3, 1, {0xF3, 0xFF, 0x01}, {0x6E, 0xDF, 0xFF} },
	{ "tiny88",       T88,   {0x1E, 0x93, 0x11},   8192,  64,   64,  4,  0,  0, 0x00, 3, 1, {0xF3, 0xFF, 0x01}, {0x6E, 0xDF, 0xFF} },
	{ "tiny2313",     T2313, {0x1E, 0x91, 0x0A},   2048,  32,  128,  4,  0,  0, 0xFC, 3, 2, {0xFF, 0xFF, 0x01}, {0x64, 0xDF, 0xFF} },
	{ "tiny4313",     T4313, {0x1E, 0x92, 0x0D},   4096,  64,  256,  4,  0,  0, 0xFC, 3, 2, {0xFF, 0xFF, 0x01}, {0x64, 0xDF, 0xFF} },
	{ "tiny1634",     T1634, {0x1E, 0x94, 0x12},  16384,  32,  256,  4,  0,  0, 0xFC, 3, 1, {0xDF, 0xDF, 0x1F}, {0x62, 0xDF, 0xFF} },
	{ "mega161",      M161,  {0x1E, 0x94, 0x01},  16384, 128,  512,  0,  0,  0, 0xFC, 1, 0, {0x77, 0x00, 0x00}, {0xDA, 0xFF, 0xFF} },
	{ "mega162",      M162,  {0x1E, 0x94, 0x04},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x1E}, {0x62, 0x99, 0xFF} },
	{ "mega8515",     M8515, {0x1E, 0x93, 0x06},   8192,  64,  512,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0xFF, 0x00}, {0xE1, 0xD9, 0xFF} },
	{ "mega8535",     M8535, {0x1E, 0x93, 0x08},   8192,  64,  512,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0xFF, 0x00}, {0xE1, 0xD9, 0xFF} },
	{ "mega163",      M163,  {0x1E, 0x94, 0x02},  16384, 128,  512,  0,  0,  0, 0xFC, 2, 1, {0xEF, 0x07, 0x00}, {0xD2, 0xFF, 0xFF} },
	{ "mega323",      M323,  {0x1E, 0x95, 0x01},  32768, 128, 1024,  0,  0,  0, 0xFC, 2, 1, {0xCF, 0xEF, 0x00}, {0xF2, 0x9F, 0xFF} },
	{ "mega48",       M48,   {0x1E, 0x92, 0x05},   4096,  64,  256,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "mega48P",      M48P,  {0x1E, 0x92, 0x0A},   4096,  64,  256,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x01}, {0x62, 0xDF, 0xFF} },
	{ "mega88",       M88,   {0x1E, 0x93, 0x0A},   8192,  64,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0xDF, 0xF9} },
	{ "mega88P",      M88P,  {0x1E, 0x93, 0x0F},   8192,  64,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0xDF, 0xF9} },
	{ "mega168",      M168,  {0x1E, 0x94, 0x06},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0xDF, 0xF9} },
	{ "mega168P",     M168P, {0x1E, 0x94, 0x0B},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0xDF, 0xF9} },
	{ "mega328P",     M328P, {0x1E, 0x95, 0x0F},  32768, 128, 1024,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0xDF, 0xF9} },
	{ "mega8",        M8,    {0x1E, 0x93, 0x07},   8192,  64,  512,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0xFF, 0x00}, {0xE1, 0xD9, 0xFF} },
	{ "mega16",       M16,   {0x1E, 0x94, 0x03},  16384, 128,  512,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0xFF, 0x00}, {0xE1, 0x99, 0xFF} },
	{ "mega32",       M32,   {0x1E, 0x95, 0x02},  32768, 128, 1024,  4,  0,  0, 0xFC, 2, 4, {0xFF, 0xFF, 0x00}, {0xE1, 0x99, 0xFF} },
	{ "mega164P",     M164P, {0x1E, 0x94, 0x0A},  16384, 128,  512,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega324P",     M324P, {0x1E, 0x95, 0x08},  32768, 128, 1024,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega324PA",    M324PA,{0x1E, 0x95, 0x11},  32768, 128, 1024,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega644P",     M644P, {0x1E, 0x96, 0x0A},  65536, 256, 2048,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega1284P",    M1284P,{0x1E, 0x97, 0x05}, 131072, 256, 4096,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega644",      M644,  {0x1E, 0x96, 0x09},  65536, 256, 2048,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega165",      M165,  {0x1E, 0x94, 0x07},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x0E}, {0x62, 0x99, 0xFF} },
	{ "mega169",      M169,  {0x1E, 0x94, 0x05},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x0F}, {0x62, 0x99, 0xFF} },
	{ "mega325P",     M325P, {0x1E, 0x95, 0x0D},  32768, 128, 1024,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega3250P",    M3250P,{0x1E, 0x95, 0x0E},  32768, 128, 1024,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega325/9",    M325,  {0x1E, 0x95, 0x03},  32768, 128, 1024,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega3250/90",  M3250, {0x1E, 0x95, 0x04},  32768, 128, 1024,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega645/9",    M645,  {0x1E, 0x96, 0x03},  65536, 256, 2048,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega6450/90",  M6450, {0x1E, 0x96, 0x04},  65536, 256, 2048,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x07}, {0x62, 0x99, 0xFF} },
	{ "mega603",      M603,  {0x1E, 0x96, 0x01},  65536, 256, 2048,  0,  0,  0, 0xF9, 1, 0, {0x2B, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "mega103",      M103,  {0x1E, 0x97, 0x01}, 131072, 256, 4096,  0,  0,  0, 0xF9, 1, 0, {0x2B, 0x00, 0x00}, {0xDF, 0xFF, 0xFF} },
	{ "mega64",       M64,   {0x1E, 0x96, 0x02},  65536, 256, 2048,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0xE1, 0x99, 0xFD} },
	{ "mega128",      M128,  {0x1E, 0x97, 0x02}, 131072, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0xE1, 0x99, 0xFD} },
	{ "mega640",      M640,  {0x1E, 0x96, 0x07},  65536, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0x62, 0x99, 0xFF} },
	{ "mega1280",     M1280, {0x1E, 0x97, 0x03}, 131072, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0x62, 0x99, 0xFF} },
	{ "mega1281",     M1281, {0x1E, 0x97, 0x04}, 131072, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0x62, 0x99, 0xFF} },
	{ "mega2560",     M2560, {0x1E, 0x98, 0x01}, 262144, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0x62, 0x99, 0xFF} },
	{ "mega2561",     M2561, {0x1E, 0x98, 0x02}, 262144, 256, 4096,  8,  0,  0, 0xFC, 3, 4, {0xFF, 0xFF, 0x03}, {0x62, 0x99, 0xFF} },
	{ "mega406",      M406,  {0x1E, 0x95, 0x07},  40960, 128,  512,  4,  0,  0, 0xFC, 2,11, {0xFF, 0x03, 0x00}, {0xCD, 0xFE, 0xFF} },
	{ "90PWM2/3",     PWM2,  {0x1E, 0x93, 0x81},   8192,  64,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0xF7}, {0x62, 0xDF, 0xF9} },
	{ "90PWM216/316", PWM216,{0x1E, 0x94, 0x83},  16384, 128,  512,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0xF7}, {0x62, 0xDF, 0xF9} },
	{ "CAN32",        CAN32, {0x1E, 0x95, 0x81},  32768, 256, 1024,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x0F}, {0x62, 0x99, 0xFF} },
	{ "CAN64",        CAN64, {0x1E, 0x96, 0x81},  65536, 256, 2048,  4,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x0F}, {0x62, 0x99, 0xFF} },
	{ "CAN128",       CAN128,{0x1E, 0x97, 0x81}, 131072, 256, 4096,  8,  0,  0, 0xFC, 3, 1, {0xFF, 0xFF, 0x0F}, {0x62, 0x99, 0xFF} },
	{ NULL,           N0000 }	/* Unknown */
};

const DEVPROP *Device = NULL;		/* Pointer to the current device property */



/*-----------------------------------------------------------------------
  Global variables (initialized by load_commands())
-----------------------------------------------------------------------*/

BYTE CodeBuff[MAX_FLASH];		/* Program code R/W buffer */
BYTE DataBuff[MAX_EEPROM];		/* EEPROM data R/W buffer */

BYTE CalBuff[4];				/* Calibration bytes read buffer */
BYTE FuseBuff[3];				/* Fuse bytes read buffer */
BYTE SignBuff[3];				/* Device signature read buffer */


/*---------- Command Parameters ------------*/

char CmdRead[2];		/* -r Read command (1st,2nd char) */

char CmdErase;			/* -e Erase device */

struct {				/* Code/Data write command */
	DWORD CodeSize;		/* Loaded program code size (.hex) */
	DWORD DataSize;		/* Loaded EEPROM data size (.eep) */
	char Verify;		/* -v Verify only */
	char CopyCal;		/* -c Copy calibration bytes into end of flash */
} CmdWrite;

struct {				/* Fuse write command */
	int Low;			/* -fl[<bin>] : Write fuse low byte (1:Write specified byte, 2:Write default byte) */
	int High;			/* -fh[<bin>] : Write fuse high byte */
	int Extend;			/* -fx[<bin>] : Write fuse extended byte */
	int Lock;			/* -l[<bin>] : Write lock bits */
    int LoadFile;		/* -ff : Enable to load from file */
	BYTE Data[4];		/* fuse bytes to be written {Low,High,Extend,Lock} */
} CmdFuse;

int Pause;				/* -w Pause before exiting program */


/*---------- Hardware Control ------------*/

static PORTPROP CtrlPort = {  1,	/* -p<n> .PortNum (port number or port address (>=0x100)) */
							  0,	/*       .PortAdr */
							  0,	/* -8 -5 .Mode */
							  0,	/* -q .Quick */
							  0 };	/*       .Stat */



/*-----------------------------------------------------------------------
  Messages
-----------------------------------------------------------------------*/

const char MesUsage[] = {
	"AVRPP - AVR Parallel Programming tool R0.43c (C)ChaN,2012  http://elm-chan.org/\n\n"
	"Write code and/or data  : <hex file> [<hex file>] ...\n"
	"Verify code and/or data : -V <hex file> [<hex file>] ...\n"
	"Read code, data or fuse : -R{P|E|F}\n"
	"Write fuse byte         : -F{L|H|X}[<bin>]\n"
	"Load fuse from hex file : -FF\n"
	"Lock device             : -L[<bin>]\n"
	"Erase device            : -E\n"
	"Copy calibration bytes  : -C\n"
	"Control port [-p1]      : -P<n>\n"
	"For more options, refer avrx32.txt.\n\n"
	"Supported Device:\n"
	"AT90S 1200,2313,2323,2333,2343,4414,4433,4434,8515,8535\n"
	"ATtiny 11,12,13,15,22,24,25,26,28,43U,44,45,48,84,85,87,88,167,261,461,861,2313,1634\n"
	"ATmega 8,16,32,48,48P,64,88,88P,103,128,161,162,163,164P,165,168,168P,169,323,324P,324PA,325/329,325P,3250P,328P,406,603,640,644,644P,1284P,645/649,1280,1281,2560,2561,3250/3290,6450/6490,8515,8535\n"
	"AT90CAN32,64,128, AT90PWM 2,3,216,316\n"
};



/* Output the device information */

void output_deviceinfo (void)
{
	printf("\nDevice Signature  = %02X-%02x-%02X\n",
			Device->Sign[0], Device->Sign[1], Device->Sign[2]);
	printf("Flash Memory Size = %d bytes\n", Device->FlashSize);
	if (Device->FlashPage)
		printf("Flash Memory Page = %d bytes x %d pages\n",
				Device->FlashPage, Device->FlashSize / Device->FlashPage);
	if (Device->EepromSize) {
		printf("EEPROM Size       = %d bytes\n", Device->EepromSize);
		if (Device->EepromPage)
			printf("EEPROM Page       = %d bytes x %d pages\n",
					Device->EepromPage, Device->EepromSize / Device->EepromPage);
	}
}



/* Output a fuse byte and description if present */

void put_fuseval (BYTE val, BYTE mask, const char *head, FILE *fp)
{
	int	n;
	char Line[100];


	fputs(head, stdout);
	for (n = 1; n <= 8; n++) {
		putchar((mask & 0x80) ? ((val & 0x80) ? '1' : '0') : '-');
		val <<= 1; mask <<= 1;
	}
	putchar('\n');

	if (fp == NULL) return;
	while (1) {	/* seek to the fuse header */
		if (fgets(Line, sizeof(Line), fp) == NULL) return;
		if (strstr(Line, head) == Line) break;
	}
	do {		/* output fuse bit descriptions */
		if (fgets(Line, sizeof(Line), fp) == NULL) return;
		fputs(Line, stdout);
	} while (Line[0] != '\n');

}



/* Output fuse bytes and calibration byte */

void output_fuse (void)
{
	int n;
	FILE *fp;
	char Line[100], *cp;

	/* Open FUSE.TXT and seek to the device */
	fp = open_cfgfile(FUSEFILE);
	if (fp == NULL) {
		MESS("WARNING: Fuse description file was not found.\n");
	} else {
		while (1) {
			if (fgets(Line, sizeof(Line), fp) == NULL) break;
			if ((Line[0] != 'D') || ((cp = strstr(Line, Device->Name)) == NULL)) continue;
			if (strlen(cp) == strlen(Device->Name) + 1) break;
		}
	}

	MESS("\n");
	put_fuseval(FuseBuff[0], Device->FuseMask[0], "Low: ", fp);

	if (Device->Fuses >= 2)
		put_fuseval(FuseBuff[1], Device->FuseMask[1], "High:", fp);

	if (Device->Fuses >= 3)
		put_fuseval(FuseBuff[2], Device->FuseMask[2], "Ext: ", fp);

	/* Output calibration values */
	if (Device->Cals) {
		fputs("Cal:", stdout);
		for (n = 0; n < Device->Cals; n++)
			printf(" %d", CalBuff[n]);
		putchar('\n');
	}

	if (fp != NULL) fclose(fp);	/* Close FUSE.TXT */
}



/*-----------------------------------------------------------------------
  Hex format manupilations
-----------------------------------------------------------------------*/


/* Pick a hexdecimal value from hex record */

DWORD get_valh (
	char **lp,	/* pointer to line read pointer */
	int count, 	/* number of digits to get (2,4,6,8) */
	BYTE *sum	/* byte check sum */
) {
	DWORD val = 0;
	BYTE n;


	while (count--) {
		n = *(*lp)++;
		if ((n -= '0') >= 10) {
			if ((n -= 7) < 10) return 0xFFFFFFFF;
			if (n > 0xF) return 0xFFFFFFFF;
		}
		val = (val << 4) + n;
		if ((count & 1) == 0) *sum += (BYTE)val;
	}
	return val;
}



/* Store a byte into data buffer */

void store_buffer (
	BYTE dat,		/* Byte to be put */
	DWORD addr		/* Offset address */
)
{
	if (addr >= BASE_FLASH && addr < BASE_FLASH + MAX_FLASH) {
		addr -= BASE_FLASH;
		CodeBuff[addr++] = dat;
		if (addr > CmdWrite.CodeSize) CmdWrite.CodeSize = addr;
		return;
	}

	if (addr >= BASE_EEPROM && addr < BASE_EEPROM + MAX_EEPROM) {
		addr -= BASE_EEPROM;
		DataBuff[addr++] = dat;
		if (addr > CmdWrite.DataSize) CmdWrite.DataSize = addr;		
		return;
	}

	if (CmdFuse.LoadFile == 0) return;

	if (addr >= BASE_FUSE && addr < BASE_FUSE + MAX_FUSE) {
		addr -= BASE_FUSE;
		CmdFuse.Data[addr] = dat;
		switch (addr) {
		case 0:
			CmdFuse.Low = 1;
			break;
		case 1:
			CmdFuse.High = 1;
			break;
		case 2:
			CmdFuse.Extend = 1;
			break;
		}
		return;
	}

	if (addr >= BASE_LOCK && addr < BASE_LOCK + MAX_LOCK) {
		CmdFuse.Data[3] = dat;
		CmdFuse.Lock = 1;
		return;
	}
}




/* Input Intel/Motorola hex file into data buffer */ 

long input_hexfile (
	FILE *fp,			/* Input stream */
	DWORD base			/* Base address to be added to the hex file */
) {
	char line[600];			/* line input buffer */
	char *lp;				/* line read pointer */
	long lnum = 0;			/* input line number */
	WORD seg = 0, hadr = 0;	/* address expantion values for intel hex */
	DWORD addr, count, n;
	BYTE sum;


	while (fgets(line, sizeof(line), fp) != NULL) {
		lnum++;
		lp = &line[1]; sum = 0;

		if (line[0] == ':') {	/* Intel Hex format */
			if ((count = get_valh(&lp, 2, &sum)) > 0xFF) return lnum;	/* byte count */
			if ((addr = get_valh(&lp, 4, &sum)) > 0xFFFF) return lnum;	/* offset */

			switch (get_valh(&lp, 2, &sum)) {	/* block type? */
			case 0x00 :	/* data block */
				addr += (seg << 4) + (hadr << 16);
				while (count--) {
					n = get_valh(&lp, 2, &sum);			/* Pick a byte */
					if (n > 0xFF) return lnum;
					store_buffer((BYTE)n, base + addr);	/* Store it into buffer */
					addr++;
				}
				break;

			case 0x01 :	/* end */
				if (count != 0) return lnum;
				break;

			case 0x02 :	/* segment base [19:4] */
				if (count != 2) return lnum;
				if ((seg = (WORD)get_valh(&lp, 4, &sum)) == 0xFFFF) return lnum;
				break;

			case 0x03 :	/* program start address (segment:offset) */
				if (count != 4) return lnum;
				get_valh(&lp, 8, &sum);
				break;

			case 0x04 :	/* high address base [31:16] */
				if (count != 2) return lnum;
				if ((hadr = (WORD)get_valh(&lp, 4, &sum)) == 0xFFFF) return lnum;
				break;

			case 0x05 :	/* program start address (linear) */
				if (count != 4) return lnum;
				get_valh(&lp, 8, &sum);
				break;

			default:	/* invalid block */
				return lnum;
			} /* switch */
			if (get_valh(&lp, 2, &sum) > 0xFF) return lnum;	/* get check sum */
			if (sum) return lnum;								/* test check sum */
		} /* if */

		if (line[0] == 'S') {	/* Motorola S format */
			if ((*lp >= '1')&&(*lp <= '3')) {

				switch (*lp++) {	/* record type? (S1/S2/S3) */
				case '1' :
					if ((count = get_valh(&lp, 2, &sum) - 3) > 0xFF) return lnum;
					if ((addr = get_valh(&lp, 4, &sum)) == 0xFFFFFFFF) return lnum;
					break;
				case '2' :
					if ((count = get_valh(&lp, 2, &sum) - 4) > 0xFF) return lnum;
					if ((addr = get_valh(&lp, 6, &sum)) == 0xFFFFFFFF) return lnum;
					break;
				default :
					if ((count = get_valh(&lp, 2, &sum) - 5) > 0xFF) return lnum;
					if ((addr = get_valh(&lp, 8, &sum)) == 0xFFFFFFFF) return lnum;
				}
				while (count--) {
					n = get_valh(&lp, 2, &sum);					/* Pick a byte */
					if (n > 0xFF) return lnum;
					store_buffer((BYTE)n, base + addr);			/* Store it into buffer */
					addr++;
				}
				if (get_valh(&lp, 2, &sum) > 0xFF) return lnum;	/* get check sum */
				if (sum != 0xFF) return lnum;					/* test check sum */
			} /* switch */
		} /* if */

	} /* while */

	return feof(fp) ? 0 : -1;
}



/* Put an Intel Hex data block */

void put_hexline (
	FILE *fp,			/* output stream */
	const BYTE *buffer,	/* pointer to data buffer */
	WORD ofs,			/* block offset address */
	BYTE count,			/* data byte count */
	BYTE type			/* block type */
) {
	BYTE sum;

	/* Byte count, Offset address and Record type */
	fprintf(fp, ":%02X%04X%02X", count, ofs, type);
	sum = count + (ofs >> 8) + ofs + type;

	/* Data bytes */
	while (count--) {
		fprintf(fp, "%02X", *buffer);
		sum += *buffer++;
	}

	/* Check sum */
	fprintf(fp, "%02X\n", (BYTE)-sum);
}



/* Output data buffer in Intel Hex format */

void output_hexfile (
	FILE *fp,			/* output stream */
	const BYTE *buffer,	/* pointer to data buffer */
	DWORD datasize,		/* number of bytes to be output */
	BYTE blocksize		/* HEX block size (1,2,4,..,128) */
) {
	WORD seg = 0, ofs = 0;
	BYTE segbuff[2], d, n;
	DWORD bc = datasize;


	while (bc) {
		if ((ofs == 0) && (datasize > 0x10000)) {
			segbuff[0] = (BYTE)(seg >> 8); segbuff[1] = (BYTE)seg;
			put_hexline(fp, segbuff, 0, 2, 2);
			seg += 0x1000;
		}
		if (bc >= blocksize) {	/* full data block */
			for (d = 0xFF, n = 0; n < blocksize; n++) d &= *(buffer+n);
			if (d != 0xFF) put_hexline(fp, buffer, ofs, blocksize, 0);
			buffer += blocksize;
			bc -= blocksize;
			ofs += blocksize;
		} else {				/* fractional data block */
			for (d = 0xFF, n = 0; n < bc; n++) d &= *(buffer+n);
			if (d != 0xFF) put_hexline(fp, buffer, ofs, (BYTE)bc, 0);
			bc = 0;
		}
	}

	put_hexline(fp, NULL, 0, 0, 1);	/* End block */
}



/*-----------------------------------------------------------------------
  Command line analysis
-----------------------------------------------------------------------*/

int load_commands (int argc, char **argv)
{
	char *cp, c, *cmdlst[20], cmdbuff[256];
	int cmd, def;
	FILE *fp;
	DWORD ln;


	/* Clear data buffers */
	memset(CodeBuff, 0xFF, sizeof(CodeBuff));
	memset(DataBuff, 0xFF, sizeof(DataBuff));

	cmd = 0; cp = cmdbuff;
	/* Import ini file as command line parameters */
	fp = open_cfgfile(INIFILE);
	if (fp != NULL) {
		while (fgets(cp, cmdbuff + sizeof(cmdbuff) - cp, fp) != NULL) {
			if (cmd >= (sizeof(cmdlst) / sizeof(cmdlst[0]) - 1)) break;
			if (*cp <= ' ') break;
			cmdlst[cmd++] = cp; cp += strlen(cp) + 1;
		}
		fclose(fp);
	}

	/* Get command line parameters */
	while (--argc && (cmd < (sizeof(cmdlst) / sizeof(cmdlst[0]) - 1)))
		cmdlst[cmd++] = *++argv;
	cmdlst[cmd] = NULL;

	/* Analyze command line parameters... */
	for (cmd = 0; cmdlst[cmd] != NULL; cmd++) {
		cp = cmdlst[cmd];

		if (*cp == '-') {	/* Command switches... */
			cp++;
			switch (tolower(*cp++)) {
			case 'v' :	/* -v */
				CmdWrite.Verify = 1; break;

			case 'c' :	/* -c */
				CmdWrite.CopyCal = 1; break;

			case 'e' :	/* -e */
				CmdErase = 1; break;

			case 'r' :	/* -r{p|e|f} */
				CmdRead[0] = 1;
				if (*cp) CmdRead[1] = tolower(*cp++);
				break;

			case 'f' :	/* -f{l|h|x}[<bin>] */
				c = tolower(*cp++);
				if (c == 'f') {	/* -ff */
					CmdFuse.LoadFile = 1;
					break;
				}
				def = (*cp >= '0' && *cp <= '1') ? 1 : 2;
				ln = strtoul(cp, &cp, 2);
				switch (c) {
				case 'l' :	/* -fl[<bin>] */
					CmdFuse.Low = def;
					CmdFuse.Data[0] = (BYTE)ln;
					break;
				case 'h' :	/* -fh[<bin>] */
					CmdFuse.High = def;
					CmdFuse.Data[1] = (BYTE)ln;
					break;
				case 'x' :	/* -fx[<bin>] */
					CmdFuse.Extend = def;
					CmdFuse.Data[2] = (BYTE)ln;
					break;
				default :
					return RC_SYNTAX;
				}
				break;

			case 'l' :	/* -l[<bin>] */
				CmdFuse.Lock = 1;
				CmdFuse.Data[3] = (BYTE)strtoul(cp, &cp, 2);
				break;

			case 'p' :	/* -p<num> */
				ln = strtoul(cp, &cp, 16);
				if ((ln >= 1 && ln <= 3) || ln >= 0x100)
					CtrlPort.PortNum = (WORD)ln;
				else
					return RC_SYNTAX;
				break;

			case 'w' :	/* -w<num> (pause before exit) */
				Pause = strtoul(cp, &cp, 2) + 1;
				break;

			case '8' :	/* -8 (detect device as 8 pin) */
				CtrlPort.Mode = 1;
				break;

			case '5' :	/* -5 (detect device as tn15 ) */
				CtrlPort.Mode = 2;
				break;

			case 'q' :	/* -q (quick power-up) */
				CtrlPort.Quick = 1;
				break;

			default :	/* invalid command */
				return RC_SYNTAX;
			} /* switch */
			if (*cp >= ' ') return RC_SYNTAX;	/* option trails garbage */
		} /* if */

		else {	/* HEX Files (Write command) */
			if ((fp = fopen(cp, "rt")) == NULL) {
				fprintf(stderr, "%s : Unable to open.\n", cp);
				return RC_FILE;
			}
			/* .eep files are read as EEPROM data, others are read as program code */
			if ((strstr(cp, ".EEP") == NULL) && (strstr(cp, ".eep") == NULL)) {
				ln = input_hexfile(fp, BASE_FLASH);
			} else {
				ln = input_hexfile(fp, BASE_EEPROM);
			}
			fclose(fp);
			if (ln) {
				if (ln < 0) {
					fprintf(stderr, "%s : File access failure.\n", cp);
				} else {
					fprintf(stderr, "%s (%ld) : Hex format error.\n", cp, ln);
				}
				return RC_FILE;
			}
		} /* else */

	} /* for */

	return 0;
}



/*-----------------------------------------------------------------------
  Device control functions
-----------------------------------------------------------------------*/


/* Read a byte from device */

BYTE read_byte (char src,	/* read from.. FLASH/EEPROM/SIGNATURE/CALIBS/FUSE */
				DWORD adr)	/* byte address */
{
	BYTE s;


	if (!CtrlPort.Mode) {	/* Parallel Mode */
		switch (src) {
		case FLASH :
			if (adr & 1)
				return rcv_byte(BS_1);
			adr >>= 1;
			if (adr == 0)
				set_byte(XA_1, C_RD_PRG);
			if (((adr & 0xFFFF) == 0) && (Device->FlashSize > (128*1024)))
				set_byte(BS_2, (BYTE)(adr >> 16));
			if ((adr & 0xFF) == 0)
				set_byte(BS_1, (BYTE)(adr >> 8));
			set_byte(0, (BYTE)adr);
			return rcv_byte(0);

		case EEPROM :
			if (adr == 0)
				set_byte(XA_1, C_RD_EEP);
			if ((adr & 0xFF) == 0)
				set_byte(BS_1, (BYTE)(adr >> 8));
			set_byte(0, (BYTE)adr);
			return rcv_byte(0);

		case SIGNATURE :
			set_byte(XA_1, C_RD_SIG);
			set_byte(0, (BYTE)adr);
			return rcv_byte(0);

		case CALIBS :
			set_byte(XA_1, C_RD_SIG);
			set_byte(0, (BYTE)adr);
			return rcv_byte(BS_1);

		case FUSE :
			set_byte(XA_1, C_RD_FB);
			switch (adr) {
			case 2 :
				s = XA_1 | BS_2; break;
			case 1 :
				s = XA_1 | BS_2 | BS_1; break;
			default :
				s = Device->Fuses ? 0 : BS_1;
			}
			return rcv_byte(s);
		}

	} else {	/* HVS Mode */
		switch (src) {
		case FLASH :
			if (adr == 0)
				xfer8(I_LDCMD, C_RD_PRG);
			if (adr & 1) {
				xfer8(I_RDLH1, 0);
				return xfer8(I_RDLH2, 0);
			}
			if ((adr & 0x1FF) == 0)
				xfer8(I_LDAH, (BYTE)(adr >> 9));
			xfer8(I_LDAL, (BYTE)(adr >> 1));
			xfer8(I_RDLL1, 0);
			return xfer8(I_RDLL2, 0);

		case EEPROM :
			if (adr == 0)
				xfer8(I_LDCMD, C_RD_EEP);
			if ((adr & 0xFF) == 0)
				xfer8(I_LDAH, (BYTE)(adr >> 8));
			xfer8(I_LDAL, (BYTE)adr);
			xfer8(I_RDLL1, 0);
			return xfer8(I_RDLL2, 0);

		case SIGNATURE :
			xfer8(I_LDCMD, C_RD_SIG);
			xfer8(I_LDAL, (BYTE)adr);
			xfer8(I_RDLL1, 0);
			return xfer8(I_RDLL2, 0);

		case CALIBS :
			xfer8(I_LDCMD, C_RD_SIG);
			xfer8(I_LDAL, (BYTE)adr);
			xfer8(I_RDLH1, 0);
			return xfer8(I_RDLH2, 0);

		case FUSE :
			xfer8(I_LDCMD, C_RD_FB);
			switch (adr) {
			case 2 :	/* Extended */
				xfer8(I_RDHL1, 0);
				return xfer8(I_RDHL2, 0);
			case 1 :	/* High */
				xfer8(I_RDHH1, 0);
				return xfer8(I_RDHH2, 0);
			default :	/* Low */
				if (Device->Fuses) {
					xfer8(I_RDLL1, 0);
					return xfer8(I_RDLL2, 0);
				}
				xfer8(I_RDLH1, 0);
				return xfer8(I_RDLH2, 0);
			}
		}
	}

	return 0xFF;
}



/* Write a byte into memory */

int write_byte (
	char dst,	/* write to.. FLASH/EEPROM */
	DWORD adr,	/* byte address */
	BYTE wd		/* data to be written */
) {
	
	if (!CtrlPort.Mode) {	/* Parallel Mode */
		switch (dst) {
		case FLASH :
			if ((adr & 1) == 0) {
				if (adr == 0)
					set_byte(XA_1, C_WR_PRG);
				if ((adr & 0x1FF) == 0)
					set_byte(BS_1, (BYTE)(adr >> 9));
				set_byte(0, (BYTE)(adr >> 1));
			}
			if (wd == 0xFF) return 1;	/* Skip if the value is 0xFF */
			set_byte(XA_0, wd);
			stb_wr((BYTE)(adr & 1 ? BS_1 : 0), 0);
			break;

		case EEPROM :
			if (adr == 0)
				set_byte(XA_1, C_WR_EEP);
			if ((adr & 0xFF) == 0)
				set_byte(BS_1, (BYTE)(adr >> 8));
			set_byte(0, (BYTE)adr);
			set_byte(XA_0, wd);
			stb_wr(0, 0);
			break;
		}

	} else {	/* HVS mode */
		switch (dst) {
		case FLASH :
			if (adr == 0)
				xfer8(I_LDCMD, C_WR_PRG);
			if ((adr & 1) == 0) {
				if ((adr & 0x1FF) == 0)
					xfer8(I_LDAH, (BYTE)(adr >> 9));
				xfer8(I_LDAL, (BYTE)(adr >> 1));
				if (wd == 0xFF) return 1;	/* Skip if the value is 0xFF */
				xfer8(I_LDDL, wd);
				xfer8(I_WRLL1, 0);
				xfer8(I_WRLL2, 0);
			} else {
				if (wd == 0xFF) return 1;	/* Skip if the value is 0xFF */
				xfer8(I_LDDH, wd);
				xfer8(I_WRLH1, 0);
				xfer8(I_WRLH2, 0);
			}
			break;

		case EEPROM :
			if (adr == 0)
				xfer8(I_LDCMD, C_WR_EEP);
			if ((adr & 0xFF) == 0)
				xfer8(I_LDAH, (BYTE)(adr >> 8));
			xfer8(I_LDAL, (BYTE)adr);
			xfer8(I_LDDL, wd);
			xfer8(I_WRLL1, 0);
			xfer8(I_WRLL2, 0);
			break;
		}
	}

	return wait_ready();	/* Wait for end of internal process */
}



/* Write a page into memory */

int write_page (
	char dst,		/* write to.. FLASH/EEPROM */
	DWORD adr,		/* byte address (must be page boundary) */
	const BYTE *wd	/* pointer to the page data */
) {
	BYTE d = 0xFF;
	int n;


	if (!CtrlPort.Mode) {	/* Parallel Mode */
		switch (dst) {
		case FLASH :
			/* Skip page if all data in the page are 0xFF */
			for (n = 0; n < Device->FlashPage; n++) d &= wd[n];
			if (d == 0xFF) return 1;
			set_byte(XA_1, C_WR_PRG);
			for (n = 0; n < Device->FlashPage; n += 2) {
				set_byte(0, (BYTE)((adr + n) >> 1));
				set_byte(XA_0, wd[n]);
				set_byte(XA_0 | BS_1, wd[n+1]);
				stb_pagel();
			}
			if (Device->FlashSize > (128*1024))
				set_byte(BS_2, (BYTE)(adr >> 17));
			set_byte(BS_1, (BYTE)(adr >> 9));
			stb_wr(0, 0);
			break;

		case EEPROM :
			set_byte(XA_1, C_WR_EEP);
			for (n = 0; n < Device->EepromPage; n++) {
				set_byte(0, (BYTE)(adr + n));
				set_byte(XA_0, wd[n]);
				stb_pagel();
			}
			set_byte(BS_1, (BYTE)(adr >> 8));
			stb_wr(0, 0);
			break;
		}

	} else {			/* HVS mode */
		switch (dst) {
		case FLASH :
			/* Skip page if all data in the page are 0xFF */
			for (n = 0; n < Device->FlashPage; n++) d &= wd[n];
			if (d == 0xFF) return 1;
			xfer8(I_LDCMD, C_WR_PRG);
			for (n = 0; n < Device->FlashPage; n += 2) {
				xfer8(I_LDAL, (BYTE)((adr + n) >> 1));
				xfer8(I_LDDL, wd[n]);
				xfer8(I_LDDH, wd[n + 1]);
				xfer8(I_PSTH1, 0);
				xfer8(I_PSTH2, 0);
			}
			xfer8(I_LDAH, (BYTE)(adr >> 9));
			xfer8(I_WRLL1, 0);
			xfer8(I_WRLL2, 0);
			break;

		case EEPROM :
			xfer8(I_LDCMD, C_WR_EEP);
			for (n = 0; n < Device->EepromPage; n++) {
				xfer8(I_LDAL, (BYTE)(adr + n));
				xfer8(I_LDDL, wd[n]);
				xfer8(I_PSTL1, 0);
				xfer8(I_PSTL2, 0);
			}
			xfer8(I_LDAH, (BYTE)(adr >> 8));
			xfer8(I_WRLL1, 0);
			xfer8(I_WRLL2, 0);
			break;
		}
	}

	return wait_ready();	/* Wait for end of internal process */
}



/* Write Fuse or Lock byte */

int write_fuselock (
	int dst,	/* write to... F_LOCK/F_LOW/F_HIGH/F_EXTEND */
	BYTE val	/* byte value to be written */
) {
	if (!CtrlPort.Mode) {	/* Parallel Mode */
		if (dst == F_LOCK) {	/* Device Lock byte */
			set_byte(XA_1, C_WR_LB);
			set_byte(XA_0, val);
			stb_wr(0, 0);
		} /* if */
		else {
			set_byte(XA_1, C_WR_FB);
			set_byte(XA_0, val);
			switch (dst) {
			case F_LOW :	/* Fuse Low byte */
				stb_wr(0, Device->FuseWait);
				break;
			case F_HIGH :	/* Fuse High byte */
				stb_wr(BS_1, Device->FuseWait);
				break;
			case F_EXTEND :	/* Fuse Extend byte */
				stb_wr(XA_1 | BS_2, Device->FuseWait);
			}
		}

	} else {				/* HVS mode */
		if (dst == F_LOCK) {	/* Device Lock byte */
			xfer8(I_LDCMD, C_WR_LB);
			xfer8(I_LDDL, val);
			xfer8(I_WRLL1, 0);
			xfer8(I_WRLL2, 0);
		}
		else {
			xfer8(I_LDCMD, C_WR_FB);
			xfer8(I_LDDL, val);
			switch (dst) {
			case F_LOW :	/* Fuse Low byte */
				xfer8(I_WRLL1, 0);
				delay_ms(Device->FuseWait);
				xfer8(I_WRLL2, 0);
				break;
			case F_HIGH :	/* Fuse High byte */
				xfer8(I_WRLH1, 0);
				xfer8(I_WRLH2, 0);
				break;
			case F_EXTEND :	/* Fuse Extended byte */
				xfer8(I_WRHL1, 0);
				xfer8(I_WRHL2, 0);
				break;
			}
		}
	}

	return wait_ready();	/* Wait for end of internal process */
}



/* Chip erase */

int erase_memory (void)
{
	if (!CtrlPort.Mode) {	/* Parallel Mode */
		set_byte(XA_1, C_ERASE);
		stb_wr(0, Device->EraseWait);

	}
	else {					/* HVS mode */
		xfer8(I_LDCMD, C_ERASE);
		xfer8(I_WRLL1, 0);
		xfer8(I_WRLL2, 0);
		delay_ms(Device->EraseWait);
		if (Device->EepromPage == 0)
			xfer8(I_LDCMD, C_NOP);
	}

	return wait_ready();
}



/* Initialize control port */

int initialize_port (void)
{
	open_ifport(&CtrlPort);		/* Open interface port and check port status */

	switch (CtrlPort.Stat) {	/* Main result code (error status) */
	case RES_DRVFAIL :
		MESS("GIVEIO initialization failed.\n");
		break;
	case RES_BADENV :
		MESS("Incorrect envilonment.\n");
		break;
	case RES_NOPORT :
		fprintf(stderr, "No LPT port at 0x%X.\n", CtrlPort.PortAddr);
		break;
	case RES_NOADAPTER :
		fprintf(stderr, "Programmer is not attached on the LPT port 0x%X.\n", CtrlPort.PortAddr);
		break;
	case RES_OPENED :
		return 0;
	}

	return 1;
}



/* Initialize control port and detect device type */

int init_devices (void)
{
	DWORD adr;
	const char *const DetMode[] = {"PAR", "HVS", "HVS15" };

	/* Execute initialization if not initialized yet */
	if (Device != NULL) return 0;
	if (initialize_port()) return RC_INIT;

	MESS("Put a device on the socket and type Enter...");
	getchar();

	for ( ; CtrlPort.Mode < 3; CtrlPort.Mode++) {
		power_on(1);
		/* read device signature */
		for (adr = 0; adr < 3; adr++)
			SignBuff[adr] = read_byte(SIGNATURE, adr);
		/* search device table */
		for (Device = DevLst; Device->ID != N0000; Device++) {
			if (memcmp(SignBuff, Device->Sign, 3) == 0) break;
		}
		if (Device->ID != N0000) break; /* Break if a device is detected */
		power_on(0);
		fprintf(stderr, "%s->Unknown device (%02X-%02X-%02X).\n",
				DetMode[CtrlPort.Mode], SignBuff[0], SignBuff[1], SignBuff[2]);
		delay_ms(50);
	} /* for */

	if (Device->ID == N0000) return RC_DEV;	/* Failed to detect device type */

	/* Show the device name */
	fprintf(stderr, "%s->Detected device is AT%s.\n",
			DetMode[CtrlPort.Mode], Device->Name);
	return 0;
}



/* Read fuse bytes and calibration bytes into buffer */

void read_fusecal (void)
{
	DWORD adr = 0;


	do
		FuseBuff[adr] = read_byte(FUSE, adr);
	while ((int)(++adr) < Device->Fuses);

	for (adr = 0; (int)adr < Device->Cals; adr++)
		CalBuff[adr] = read_byte(CALIBS, adr);
}



/*-----------------------------------------------------------------------
  Programming functions
-----------------------------------------------------------------------*/


/* -e command */

int erase_device (void)
{
	int rc;


	if (rc = init_devices()) return rc;

	if (!erase_memory()) {
		MESS("Failed.\n");
		return RC_FAIL;
	}
	MESS("Erased.\n");

	return 0;
}



/* -r command */

int read_device (char cmd)
{
	DWORD adr;
	int rc;


	if (rc = init_devices()) return rc;

	switch (cmd) {
	case 'p' :	/* -rp : read program memory */
		MESS("Reading Flash...");
		for (adr = 0; adr < Device->FlashSize; adr++)
			CodeBuff[adr] = read_byte(FLASH, adr);
		MESS("Passed.\n");
		output_hexfile(stdout, CodeBuff, Device->FlashSize, 32);
		break;

	case 'e' :	/* -re : read eeprom */
		if (Device->EepromSize == 0) {
			MESS("No EEPROM.\n");
		} else {
			MESS("Reading EEPROM...");
			for (adr = 0; adr < Device->EepromSize; adr++)
				DataBuff[adr] = read_byte(EEPROM, adr);
			MESS("Passed.\n");
			output_hexfile(stdout, DataBuff, Device->EepromSize, 32);
		}
		break;

	case 'f' :	/* -rf : read fuses and cals */
		read_fusecal();
		output_fuse();
		break;

	default :
		output_deviceinfo();
	}

	return 0;
}



/* .hex files write command */

int write_flash (void)
{
	DWORD adr;
	BYTE rd;
	int rc, n;


	if (rc = init_devices()) return rc;

	MESS("Flash: ");

	if (CmdWrite.CodeSize > Device->FlashSize) {
		MESS("error: program size > memory size.\n");
		return RC_FAIL;
	}
		CmdWrite.CodeSize = Device->FlashSize;

	if (!CmdWrite.Verify) {	/* -v : Skip programming process when verify mode */

		MESS("Erasing...");						/* Erase device before programming */
		if (!erase_memory()) {
			MESS("Failed.\n");
			return RC_FAIL;
		}

		if (CmdWrite.CopyCal && Device->Cals) {	/* -c : Copy calibration bytes */
			read_fusecal();
			for (n = 0; n < Device->Cals; n++)
				CodeBuff[Device->FlashSize - 1 - n] = CalBuff[n];
			CmdWrite.CodeSize = Device->FlashSize;
		}

		MESS("Writing...");
		if (Device->FlashPage) {		/* Write flash in page mode */
			for (adr = 0; adr < CmdWrite.CodeSize; adr += Device->FlashPage) {
				if (!write_page(FLASH, adr, &CodeBuff[adr])) {
					MESS("Failed.\n");	return RC_FAIL;
				}
			}
		}
		else {						/* Write flash in byte-by-byte mode */
			for (adr = 0; adr < CmdWrite.CodeSize; adr++) {
				if (!write_byte(FLASH, adr, CodeBuff[adr])) {
					MESS("Failed.\n");	return RC_FAIL;
				}
			}
		}
	}

	MESS("Verifying...");
	for (adr = 0; adr < CmdWrite.CodeSize; adr++) {
		rd = read_byte(FLASH, adr);
		if (rd != CodeBuff[adr]) {
			fprintf(stderr, "Failed at %04X:%02X-%02X\n", adr, CodeBuff[adr], rd);
			return RC_FAIL;
		}
	}

	MESS("Passed.\n");
	return 0;
}



/* .eep files write command */

int write_eeprom (void)
{
	DWORD adr;
	BYTE rd;
	int rc;


	if (rc = init_devices()) return rc;
	if (Device->EepromSize == 0) return 0;

	MESS("EEPROM: ");

	if (CmdWrite.DataSize > Device->EepromSize) {
		MESS("error: data size > memory size.\n");
		return RC_FAIL;
	}

	if (!CmdWrite.Verify) {	/* -v : Skip programming process when verify mode */
		MESS("Writing...");
		if (Device->EepromPage) {	/* Write flash in page mode */
			for (adr = 0; adr < CmdWrite.DataSize; adr += Device->EepromPage) {
				if (!write_page(EEPROM, adr, &DataBuff[adr])) {
					MESS("Failed.\n");	return RC_FAIL;
				}
			}
		}
		else {						/* Write flash in byte-by-byte mode */
			for (adr = 0; adr < CmdWrite.DataSize; adr++) {
				if (!write_byte(EEPROM, adr, DataBuff[adr])) {
					MESS("Failed.\n");	return RC_FAIL;
				}
			}
		}
	}

	MESS("Verifying...");
	for (adr = 0; adr < CmdWrite.DataSize; adr++) {
		rd = read_byte(EEPROM, adr);
		if (rd != DataBuff[adr]) {
			fprintf(stderr, "Failed at %04X:%02X-%02X\n", adr, DataBuff[adr], rd);
			return RC_FAIL;
		}
	}

	MESS("Passed.\n");
	return 0;
}



/* -f{l|h|x}, -l command */

int write_fuse (void)
{
	int	rc;
	BYTE d;


	if (rc = init_devices()) return rc;

	if (CmdFuse.Low) {
		MESS("Fuse(l): ");
		if (CmdFuse.Low == 2) CmdFuse.Data[0] = Device->FuseDefault[0];
		if (!CmdWrite.Verify) {
			MESS("Writing...");
			write_fuselock(F_LOW, (BYTE)(CmdFuse.Data[0] | ~Device->FuseMask[0]));
		}
		MESS("Verifying...");
		d = read_byte(FUSE, 0);
		if ((d ^ CmdFuse.Data[0]) & Device->FuseMask[0]) {
			MESS("Failed.\n");
			return RC_FAIL;
		}
		MESS("Passed.\n");
	}

	if (CmdFuse.High && (Device->Fuses >= 2)) {
		MESS("Fuse(h): ");
		if (CmdFuse.High == 2) CmdFuse.Data[1] = Device->FuseDefault[1];
		if (!CmdWrite.Verify) {
			MESS("Writing...");
			write_fuselock(F_HIGH, (BYTE)(CmdFuse.Data[1] | ~Device->FuseMask[1]));
		}
		MESS("Verifying...");
		d = read_byte(FUSE, 1);
		if ((d ^ CmdFuse.Data[1]) & Device->FuseMask[1]) {
			MESS("Failed.\n");
			return RC_FAIL;
		}
		MESS("Passed.\n");
	}

	if (CmdFuse.Extend && (Device->Fuses >= 3)) {
		MESS("Fuse(x): ");
		if (CmdFuse.Extend == 2) CmdFuse.Data[2] = Device->FuseDefault[2];
		if (!CmdWrite.Verify) {
			MESS("Writing...");
			write_fuselock(F_EXTEND, (BYTE)(CmdFuse.Data[2] | ~Device->FuseMask[2]));
		}
		MESS("Verifying...");
		d = read_byte(FUSE, 2);
		if ((d ^ CmdFuse.Data[2]) & Device->FuseMask[2]) {
			MESS("Failed.\n");
			return RC_FAIL;
		}
		MESS("Passed.\n");
	}

	if (CmdFuse.Lock && !CmdWrite.Verify) {
		MESS("Lock byte: Writing...");
		write_fuselock(F_LOCK, (BYTE)(CmdFuse.Data[3] ? CmdFuse.Data[3] : Device->LockData));
		MESS("Passed.\n");
	}

	return 0;
}



/* Terminate process */

void terminate (void)
{
	close_ifport();
	Device = NULL;

	if (Pause) {
		MESS("\nType Enter to exit...");
		getchar();
	}
}



/*-----------------------------------------------------------------------
  Main
-----------------------------------------------------------------------*/


int main (int argc, char *argv[])
{
	int rc;

	if (rc = load_commands(argc, argv)) { 
		if (rc == RC_SYNTAX) MESS(MesUsage);
		terminate();
		return rc;
	}

	/* Read device and terminate if -r{p|e|f} command is specified */
	if (CmdRead[0]) {
		rc = read_device(CmdRead[1]);
		terminate();
		return rc;
	}

	/* Erase device and terminate if -e command is specified */
	if (CmdErase) {
		rc = erase_device();
		terminate();
		return rc;
	}

	/* Write to device if any file is loaded */
	if (CmdWrite.CodeSize) {
		if (rc = write_flash()) {
			terminate();
			return rc;
		}
	}
	if (CmdWrite.DataSize) {
		if (rc = write_eeprom()) {
			terminate();
			return rc;
		}
	}

	/* Write fuse,lock if -f{l|h|x}, -l are specified */
	if (CmdFuse.Low || CmdFuse.High || CmdFuse.Extend || CmdFuse.Lock) {		
		if (rc = write_fuse()) {
			terminate();
			return rc;
		}
	}

	if (Device == NULL) MESS(MesUsage);
	terminate();
	return 0;
}

