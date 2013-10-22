/*-----------------------------------------------------------------------*/
/*  Hardware control functions for AVRPP  R0.34                          */
/*-----------------------------------------------------------------------*/

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include "avrpp.h"
#include "hwctrl.h"



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


/* Initialize GIVEIO */

static int init_driver () {
	int ls = 0;
	HANDLE hdev;
	SC_HANDLE hsc, hsv;
	char filepath[_MAX_PATH], *cp;
	BOOL res;


	while (1) {
		ls++;

		if(ls >= 4) return (-1);

		if(ls >= 3) {	/* Register GIVEIO.SYS to the SCM database */
			if(SearchPath(NULL, "giveio.sys", NULL, sizeof(filepath), filepath, &cp) == 0) continue;
			if((hsc = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) != NULL) {
				if((hsv = CreateService(hsc,
										"giveio", "giveio", 
										SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE,
										filepath,
										NULL, NULL, NULL, NULL, NULL)) != NULL) {
					CloseServiceHandle(hsv);
				} else {
					if((hsv = OpenService(hsc, "giveio", SERVICE_ALL_ACCESS)) != NULL) {
						DeleteService(hsv);
						CloseServiceHandle(hsv);
						hsv = NULL;
					}
				}
				CloseServiceHandle(hsc);
			}
			if((hsc == NULL) || (hsv == NULL)) continue;
		}

		if(ls >= 2) {	/* Start GIVEIO */
			if((hsc = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) != NULL) {
				if((hsv = OpenService(hsc, "giveio", SERVICE_ALL_ACCESS)) != NULL) {
					res = StartService(hsv, 0, NULL);
					CloseServiceHandle(hsv);
				}
				CloseServiceHandle(hsc);
			}
			if((hsc == NULL) || (hsv == NULL) || (res == FALSE)) continue;
		}

		/* Open GIVEIO to clear IOPM of this process */
		if((hdev = CreateFile("\\\\.\\giveio", GENERIC_READ, 0, NULL,
							  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)
			) == INVALID_HANDLE_VALUE) continue;
		CloseHandle(hdev);
		break;
	} /* while */

	return (0);

}



/* Search and Open configuration file */

FILE *open_cfgfile(char *filename)
{
	FILE *fp;
	char filepath[256], *dmy;


	if((fp = fopen(filename, "rt")) != NULL) 
		return fp;
	if(SearchPath(NULL, filename, NULL, sizeof(filepath), filepath, &dmy)) {
		if((fp = fopen(filepath, "rt")) != NULL) 
			return fp;
	}
	return NULL;
}



/* Wait for dly msec */

void delay_ms (WORD dly)
{
	LARGE_INTEGER val1, val2;


	QueryPerformanceCounter(&val1);
	QueryPerformanceFrequency(&val2);
	val1.QuadPart += val2.QuadPart * dly / 1000;

	do
		QueryPerformanceCounter(&val2);
	while(val2.QuadPart < val1.QuadPart);
}



/* Initialize I/F port and return port status */

void open_ifport (PORTPROP *pc)
{
	OSVERSIONINFO vinfo = { sizeof(OSVERSIONINFO) };
	LARGE_INTEGER val1;
	const WORD PortLst[] = { LPT1ADR, LPT2ADR, LPT3ADR };


	/* Check if high resolution timer is supported */
	QueryPerformanceFrequency(&val1);
	if (val1.QuadPart == 0) {
		pc->Stat = RES_BADENV;
		return;
	}

	/* Open driver if needed */
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

	pc->Stat = RES_NOPORT;

	/* Check parameter validity */
	if((pc->PortNum < 1) || (pc->PortNum > 3)) {
		pc->PortNum = 0;
		return;
	}

	/* Get port address and check if the port is present */
	PortBase = PortLst[pc->PortNum - 1];
	pc->PortAddr = PortBase;
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
	if (CtrlPort) {
		power_on (0);
		CtrlPort = NULL;
	}
}



/* Set a byte for parallel device */

void set_byte (BYTE mode, BYTE dat)
{
	int n;


	for (n = 0; n < 8; n++) {			/* Send data into shift register */
		RegDat = B_OE | B_WR;
		if (dat & 0x80) RegDat |= B_SDAT;
		_outp(LPT_DAT, RegDat);
		_outp(LPT_DAT, RegDat | B_SCLK);
		dat <<= 1;
	}

	RegDat = B_OE | B_WR;				/* Setup XA[1:0], BS[2:1] */
	if (mode & BS_2) _outp(LPT_CTL, RegCtl | B_BS2);
	if (mode & XA_0) RegDat |= B_XA0;
	if (mode & XA_1) RegDat |= B_XA1;
	if (mode & BS_1) RegDat |= B_BS1;
	_outp(LPT_DAT, RegDat);

	_outp(LPT_DAT, RegDat | B_XT1);		/* Give XTAL1 pulse */
	_outp(LPT_DAT, RegDat);

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

