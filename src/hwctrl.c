/*-----------------------------------------------------------------------*/
/*  Hardware control functions for AVRPP  R0.42T1                        */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#ifdef _WIN32
	#include <conio.h>
	#include <windows.h>
#else
	#include <stdlib.h>
	#include <string.h>
	#include <sys/io.h>
	#include <sys/perm.h> 
	#include <unistd.h>
#endif /* _WIN32 */
#include "avrpp.h"
#include "hwctrl.h"
#include "lptaddr.h"


#ifdef _WIN32
	void _outp(short port, short databyte);
	short _inp(short port);
#else
	#define _outp(PORT, VAL) outb(VAL, PORT)
	#define _inp(PORT) inb(PORT)
#endif /* _WIN32 */


/*----------------------------------------------------------------------
  Control Variables
----------------------------------------------------------------------*/

static PORTPROP *CtrlPort;
static BYTE RegDat, RegCtl;
static WORD PortBase;


#define IODLY(dly)	for(d = dly; d > 0; d--) _inp(PortBase)
#define LPT_DAT	(WORD)(PortBase + L_DAT)
#define LPT_STA	(WORD)(PortBase + L_STA)
#define LPT_CTL	(WORD)(PortBase + L_CTL)


/*----------------------------------------------------------------------
  Control Functions
----------------------------------------------------------------------*/


/* Initialize and use of inpout32 */

#ifdef _WIN32
typedef short _stdcall (*inpout32_inpfuncPtr)(short portaddr);
typedef void _stdcall (*inpout32_oupfuncPtr)(short portaddr, short datum);
inpout32_inpfuncPtr inpout32_inp32;
inpout32_oupfuncPtr inpout32_out32;

static int init_driver ()
{
	HINSTANCE hLib = LoadLibrary("inpout32.dll");
	
	if (hLib == NULL)
		return -1;

	inpout32_inp32 = (inpout32_inpfuncPtr)GetProcAddress(hLib, "Inp32");
	inpout32_out32 = (inpout32_oupfuncPtr)GetProcAddress(hLib, "Out32");
	
	if (inpout32_inp32 == NULL || inpout32_out32 == NULL)
		return -1;
	
	return 0;
}

void _outp(short port, short databyte)
{
	(inpout32_out32)(port, databyte);
}

short _inp(short port)
{
	return (inpout32_inp32)(port);
}
#endif /* _WIN32 */



/* Search and Open configuration file */

FILE *open_cfgfile(char *filename)
{
	FILE *fp;
#ifdef _WIN32
	char filepath[256], *dmy;


	if((fp = fopen(filename, "rt")) != NULL) 
		return fp;
	if(SearchPath(NULL, filename, NULL, sizeof(filepath), filepath, &dmy)) {
		if((fp = fopen(filepath, "rt")) != NULL) 
			return fp;
	}
	return NULL;
#else
	static char searchPaths[5][256];
	static char searchPathsReady = 0;
	int i;
	
	if (!searchPathsReady)
	{
		strcpy(searchPaths[0], "./");
		
		strncpy(searchPaths[1], getenv("HOME"), 256);
		searchPaths[1][255] = '\0';
		if (searchPaths[1][0] == '\0' || strlen(searchPaths[1]) > 150)
			strcpy(searchPaths[1], "~/");
		if (searchPaths[1][strlen(searchPaths[1])] != '/')
			strncat(searchPaths[1], "/", 256);
		strncat(searchPaths[1], ".avrxtool32/", 256);
		
		strcpy(searchPaths[2], "/usr/share/avrxtool32/");
		strcpy(searchPaths[3], "/etc/avrxtool32/");
		searchPaths[4][0] = '\0';
		
		searchPathsReady = 1;
	}
	
	i = 0;
	while (searchPaths[i][0] != '\0')
	{
		char path[256];
		
		strncpy(path, searchPaths[i++], 256);
		strncat(path, filename, 256);
		
		fp = fopen(path, "rt");
		if (fp != NULL)
			return fp;
	}
	
	return NULL;
#endif /* _WIN32 */
}


/* Wait for dly usec */

void delay_us(WORD dly)
{
#ifdef _WIN32
	LARGE_INTEGER val1, val2;


	QueryPerformanceCounter(&val1);
	QueryPerformanceFrequency(&val2);
	val1.QuadPart += val2.QuadPart * dly / 1000000;

	do
		QueryPerformanceCounter(&val2);
	while(val2.QuadPart < val1.QuadPart);
	
#else
	usleep(dly);
#endif /* _WIN32 */
}


void delay_ms(WORD dly)
{
	delay_us(dly * 1000);
}


/* Initialize I/F port and return port status */

void open_ifport (PORTPROP *pc)
{
#ifdef _WIN32
	OSVERSIONINFO vinfo;
	LARGE_INTEGER val1;

	/* Check if high resolution timer is supported */
	QueryPerformanceFrequency(&val1);
	if (val1.QuadPart == 0) {
		pc->Stat = RES_BADENV;
		return;
	}

	/* Open driver if needed */
	vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&vinfo) == FALSE) {
		pc->Stat = RES_BADENV;
		return;
	}
	if(vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if(init_driver()) {
			pc->Stat = RES_DRVFAIL;
			return;
		}
	}
#endif /* _WIN32 */

	pc->Stat = RES_NOPORT;

	/* Check parameter validity */
	if (pc->PortNum > 99)
	{
		pc->PortNum = 0;
		return;
	}

	/* Get port address and check if the port is present */
	if (pc->PortNum == 0)
		PortBase = pc->PortAddr;
	else
	{
		PortBase = getLPTPortBaseAddress(pc->PortNum);
		if (PortBase == 0)
		{
			pc->PortAddr = 0;
			return;
		}
		pc->PortAddr = PortBase;
	}

#ifndef _WIN32
	if(ioperm(PortBase, 3, 1))
	{
		MESS("WARNING: Access denied, try sudo.\n");
		pc->Stat = RES_NOADAPTER;
		return;
	}
#endif /* not _WIN32 */
	
	_outp(LPT_CTL, RegCtl = B_VPP | B_VCC | B_PAGEL);	/* power off */
	_outp(LPT_DAT, 0x08);
	if((BYTE)_inp(LPT_DAT) != 0x08) return;
	_outp(LPT_DAT, 0x40);
	if((BYTE)_inp(LPT_DAT) != 0x40) return;

	/* Check if the adapter is AVRSP (D7-PE) */
	_outp(LPT_DAT, 0x80);
	if(_inp(LPT_STA) & S_PE) {
		_outp(LPT_DAT, RegDat = 0);
		if((_inp(LPT_STA) & S_PE) == 0) {
			pc->Stat = RES_OPENED;
			CtrlPort = pc;
			return;
		}
	}

	pc->Stat = RES_NOADAPTER;
	return;
}



/* Power control */

void power_on (int sw)
{
	WORD d;
	int n;


	if(!sw) {	/* Power OFF */
		_outp(LPT_CTL, RegCtl |= B_VPP);	/* Vpp off */
		delay_ms(1);						/* 1ms */
		_outp(LPT_DAT, RegDat = 0);			/* Signals = L */
		_outp(LPT_CTL, RegCtl |= B_VCC);	/* Vcc off */
		delay_ms(10);						/* 10ms */
		return;
	}

	/* Power ON */

	if (CtrlPort->Mode) {			/* For 8 pin device */
		_outp(LPT_DAT, B_GND);				/* Set Prog_enable state to "000" */
		delay_ms(1);						/* 1ms */
		if (CtrlPort->Quick) {				/* Is Quick power-up? */
			_outp(LPT_CTL, RegCtl &= (BYTE)~(B_VCC|B_VPP));	/* Vcc+Vpp on */
		} else {							/* Normal power-up */
			_outp(LPT_CTL, RegCtl &= (BYTE)~B_VCC);	/* Vcc on */
			IODLY(3);
			for (n = 0; n < 3; n++) {		/* Toggle CLK six times */
				_outp(LPT_DAT, B_GND | B_CLK);
				_outp(LPT_DAT, B_GND);
			}
			_outp(LPT_CTL, RegCtl &= (BYTE)~B_VPP);	/* Vpp on */
		}
		_outp(LPT_DAT, RegDat = 0);			/* Signals = L */
		delay_ms(1);						/* 1ms */
		return;
	}

									/* For parallel device */
	_outp(LPT_CTL, RegCtl &= (BYTE)~B_VCC);	/* Vcc on */
	if (!CtrlPort->Quick) {
		IODLY(3);
		for (n = 0; n < 3; n++) {			/* Toggle XTAL1 six times */
			_outp(LPT_DAT, B_XT1);
			
			_outp(LPT_DAT, 0);
		}
	}
	_outp(LPT_DAT, RegDat = B_OE);			/* OE = H */
	_outp(LPT_CTL, RegCtl &= (BYTE)~B_VPP);	/* Vpp on */
	_outp(LPT_DAT, RegDat |= B_WR);			/* WR = H */
	delay_ms(1);							/* 1ms */
	return;
}




/* Release port */

void close_ifport()
{
	if (CtrlPort)
	{
		power_on (0);
		CtrlPort = NULL;
#ifndef _WIN32
		ioperm(PortBase, 3, 0);
#endif /* not _WIN32 */
	}
}


static int sbmode = 0;

void set_sbmode(int v)
{
	sbmode = v;
}

int get_sbmode()
{
	return sbmode;
}

const char *byte_to_binary(int x)
{
	static char b[9];
	int pos;
	
	for (pos = 0; pos < 8; pos++)
		b[7 - pos] = (x & (1 << pos)) ? '1' : '0';
	b[8] = '\0';
	return b; /* returns buffer! */
}


/* Set a byte for parallel device */

void set_byte (BYTE mode, BYTE dat)
{
	int n;

	if (sbmode)
		printf("d=%x : %s [XA0: %u, XA1: %u, BS1: %u, BS2: %u]\n", dat, byte_to_binary(dat),
			((mode & XA_0) > 0), ((mode & XA_1) > 0), ((mode & BS_1) > 0), ((mode & BS_2) > 0));

	for (n = 0; n < 8; n++) {			/* Send data into shift register */
		RegDat = B_OE | B_WR;
		if (dat & 0x80) RegDat |= B_SDAT;
		_outp(LPT_DAT, RegDat);
		delay_us(1);
		_outp(LPT_DAT, RegDat | B_SCLK);
		dat <<= 1;
	}

	RegDat = B_OE | B_WR;				/* Setup XA[1:0], BS[2:1] */
	if (mode & BS_2) _outp(LPT_CTL, RegCtl | B_BS2);
	if (mode & XA_0) RegDat |= B_XA0;
	if (mode & XA_1) RegDat |= B_XA1;
	if (mode & BS_1) RegDat |= B_BS1;
	_outp(LPT_DAT, RegDat);

	if (sbmode)
		QUERY("SBM\n");

	_outp(LPT_DAT, RegDat | B_XT1);		/* Give XTAL1 pulse */
	if (sbmode)
		QUERY("XALT\n");
	_outp(LPT_DAT, RegDat);
	if (sbmode)
		QUERY("noXALT\n");

	_outp(LPT_CTL, RegCtl &= (BYTE)~B_BS2);	/* BS2 = L */
	
}



/* Receive a byte from parallel device */

BYTE rcv_byte (BYTE mode)
{
	int n;
	BYTE dat = 0;


	RegDat = B_OE | B_WR;				/* Setup XA[1:0], BS[2:1], S1=H, OE=L */
	if (mode & BS_2) _outp(LPT_CTL, RegCtl | B_BS2);
	if (mode & XA_0) RegDat |= B_XA0;
	if (mode & XA_1) RegDat |= B_XA1;
	if (mode & BS_1) RegDat |= B_BS1;
	RegDat |= B_SCMD;
	RegDat &= (BYTE)~B_OE;
	_outp(LPT_DAT, RegDat);

	_outp(LPT_DAT, RegDat | B_SCLK);	/* Latch data into shift register */

	RegDat &= (BYTE)~B_SCMD;			/* Set S1=L, OE=H */
	RegDat |= B_OE;
	_outp(LPT_DAT, RegDat);
	_outp(LPT_CTL, RegCtl);				/* BS2 = L */

	for (n = 0; n < 8; n++) {			/* Read data from shift register */
		dat <<= 1;
		if (_inp(LPT_STA) & S_ACK) dat++;
		_outp(LPT_DAT, RegDat | B_SCLK);
		_outp(LPT_DAT, RegDat);
	}

	return (dat);
}



/* Apply a PAGEL pulse for parallel devices */

void stb_pagel ()
{
	_outp(LPT_CTL, RegCtl & (BYTE)~B_PAGEL);
	delay_us(1);
	_outp(LPT_CTL, RegCtl);
}



/* Apply a WR pulse and delay for parallel devices */

void stb_wr (BYTE mode, WORD dly)
{
	RegDat = B_OE | B_WR;					/* Setup XA[1:0], BS[2:1] */
	if (mode & BS_2) _outp(LPT_CTL, RegCtl | B_BS2);
	if (mode & XA_0) RegDat |= B_XA0;
	if (mode & XA_1) RegDat |= B_XA1;
	if (mode & BS_1) RegDat |= B_BS1;
	_outp(LPT_DAT, RegDat);

	_outp(LPT_DAT, RegDat & (BYTE)~B_WR);	/* WR = L */
	delay_ms(dly);							/* delay */
	_outp(LPT_DAT, RegDat);					/* WR = H */
	_outp(LPT_CTL, RegCtl);
}



/* Send a serial command and Receive a byte for HVS devices */

BYTE xfer8 (BYTE cmd, BYTE dat)
{
	int n, m;


	RegDat = 0;
	for (n = 0; n < 8; n++) {
		for (m = 0; m < 16; m++) {			/* Apply 16 SCLKs for tn15 mode, */
			_outp(LPT_DAT, RegDat | B_CLK);
			if (CtrlPort->Mode != 2) break;	/* But only a rise edge for others */
			_outp(LPT_DAT, RegDat);
		}
		RegDat = 0;
		if (dat & 0x80) RegDat |= B_DAT;	/* SCLK=L and Set next data and instruction */
		if (cmd & 0x80) RegDat |= B_CMD;
		_outp(LPT_DAT, RegDat);
		cmd <<= 1; dat <<= 1;
		if ((_inp(LPT_STA) & S_BUSY) ==  0) dat++;	/* Read SDO */
	}
	for (n = 0; n < 3; n++) {				/* Push-in trailing 3 bits... */
		for (m = 0; m < 16; m++) {
			_outp(LPT_DAT, RegDat | B_CLK);
			if (CtrlPort->Mode != 2) break;
			_outp(LPT_DAT, RegDat);
		}
		RegDat = 0;
		_outp(LPT_DAT, RegDat);
	}

	return (dat);
}



/* Wait for end of internal process */
/* (return with 0 means time out)   */

int wait_ready ()
{
	int n;


	_inp(LPT_STA);
	for (n = 50000; n && (_inp(LPT_STA) & S_BUSY); n--);

	return (n);
}

