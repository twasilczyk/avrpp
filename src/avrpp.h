
#ifndef _WINDEF_
typedef unsigned char   BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
#endif /* _WINDEF_ */



#define MESS(str)	fputs(str, stderr)
#define QUERY(str)	{ fputs(str, stderr); getchar(); }
#define FUSEFILE "fuse.txt"
#define INIFILE "avrpp.ini"



/* Device property structure */

enum _devid {	/* device identifier */
	N0000,		/* Unknown */
	S1200, S2313, S4414, S8515, S2333, S4433, S4434, S8535, S2323, S2343,
	T11, T12, T13, T22, T26, T28, T261, T461, T861, T2313, T15, T25, T45, T85, T24, T43U, T44, T48, T84, T87, T88, T167,
	M161, M162, M8515, M8535, M163, M323, M48, M48P, M88, M88P, M168, M168P, M328P, M8, M16, M32, M164P, M324P, M644P, M1284P, M644, M325, M3250, M165, M169, M603, M645, M6450, M103, M64, M128, M640, M1280, M1281, M2560, M2561,	M325P, M3250P, M324PA,
	CAN32, CAN64, CAN128,
	PWM2, PWM216,
	M406
};


typedef struct _DEVPROP {
	char	*Name;			/* Device name */
	char	ID;				/* Device ID */
	BYTE	Sign[3];		/* Device signature bytes */
	DWORD	FlashSize;		/* Flash memory size in unit of byte */
	WORD	FlashPage;		/* Flash page size (0 is byte-by-byte) */
	DWORD	EepromSize;		/* EEPROM size in unit of byte */
	WORD	EepromPage;		/* EEPROM page size (0 is byte-by-byte) */
	WORD	EraseWait;		/* Wait time for chip erase (0 is polling) */
	WORD	FuseWait;		/* Wait time for fuse write (0 is poiling) */
	BYTE	LockData;		/* Default lock byte (program LB1 and LB2) */
	char	Fuses;			/* Number of fuses */
	char	Cals;			/* Number of calibration bytes */
	BYTE	FuseMask[3];	/* Valid fuse bit mask [low, high, ext] */
	BYTE	FuseDefault[3];	/* Fuse default value [low, high, ext] */
} DEVPROP;




/* Device programming commands */

#define C_NOP		0x00
#define C_ERASE		0x80
#define C_WR_PRG	0x10
#define C_RD_PRG	0x02
#define C_WR_EEP	0x11
#define C_RD_EEP	0x03
#define C_WR_FB		0x40
#define C_RD_FB		0x04
#define C_WR_LB		0x20
#define C_RD_SIG	0x08

#define	I_LDCMD		0x4C
#define	I_LDAL		0x0C
#define	I_LDAH		0x1C
#define	I_LDDL		0x2C
#define	I_LDDH		0x3C
#define	I_PSTL1		0x6D
#define	I_PSTL2		0x6C
#define	I_PSTH1		0x7D
#define	I_PSTH2		0x7C
#define	I_RDLL1		0x68
#define	I_RDLL2		0x6C
#define	I_RDLH1		0x78
#define	I_RDLH2		0x7C
#define	I_RDHL1		0x6A
#define	I_RDHL2		0x6E
#define	I_RDHH1		0x7A
#define	I_RDHH2		0x7E
#define	I_WRLL1		0x64
#define	I_WRLL2		0x6C
#define	I_WRLH1		0x74
#define	I_WRLH2		0x7C
#define	I_WRHL1		0x66
#define	I_WRHL2		0x6E



/* Program return codes */

#define	RC_FAIL		1
#define	RC_FILE		2
#define	RC_INIT		3
#define RC_DEV		4
#define	RC_SYNTAX	5



/* Byte read/write identifier */

#define FLASH		0
#define EEPROM		1
#define SIGNATURE	2
#define	CALIBS		3
#define FUSE		4


/* Fuse read/write identifier */

#define F_LOW		0
#define	F_HIGH		1
#define	F_EXTEND	2
#define	F_LOCKF		3


/* XA/BS identifier for read/write commands */

#define XA_0		0x01
#define	XA_1		0x02
#define	BS_1		0x04
#define BS_2		0x08


/* Buffer size for flash/eeprom */

#define	MAX_FLASH	(256*1024)
#define	MAX_EEPROM	(  4*1024)



/* Physical port properties */

typedef struct _PORTPROP {
	WORD	PortNum;		/* Port number (1..3) */
	WORD	PortAddr;		/* Port base address */
	char	Mode;			/* Device mode 0:Parallel, 1:8pin, 2:tn15 */
	char	Quick;			/* Quick power-on */
	char	Stat;			/* Control status codes */
} PORTPROP;


enum _ctrlstat {	/* port control status */
	RES_OPENED, RES_NOADAPTER, RES_NOPORT, RES_DRVFAIL, RES_BADENV
};


/* Prototypes for hardware controls */

void open_ifport (PORTPROP *);
void close_ifport ();
void power_on (int);
void set_byte (BYTE, BYTE);
BYTE rcv_byte (BYTE);
BYTE xfer8 (BYTE, BYTE);
void stb_pagel ();
void stb_wr (BYTE, WORD);
int wait_ready ();
void delay_ms (WORD);
FILE *open_cfgfile(char *);

void set_sbmode(int v);
int get_sbmode();
