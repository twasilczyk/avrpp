/* Port number and port address */

#define	LPT1ADR	0x0378
#define	LPT2ADR	0x0278
#define	LPT3ADR	0x03BC


/* LPT data port offset and control bit assgnment */

#define L_DAT	0
#define B_XT1	0x01
#define B_OE	0x02
#define B_WR	0x04
#define B_BS1	0x08
#define B_XA0	0x10
#define B_XA1	0x20
#define B_SDAT	0x10
#define B_SCLK	0x40
#define B_SCMD	0x80
#define B_CHK	0x80

#define B_CLK	0x01
#define B_DAT	0x02
#define B_CMD	0x04
#define B_GND	0x08


/* LPT status port offset and bit assignment */

#define	L_STA	1
#define	S_PE	0x20
#define	S_ACK	0x40
#define S_BUSY	0x80


/* LPT control port offset and bit assignment */

#define	L_CTL	2
#define	B_VPP	0x01
#define	B_VCC	0x02
#define	B_BS2	0x04
#define	B_PAGEL	0x08


