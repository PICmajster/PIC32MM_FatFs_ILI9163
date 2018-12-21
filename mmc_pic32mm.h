/*****************************************************************************
  FileName:        mmc_pic32mm.h
  Processor:       PIC32MM0256GPM048
  Compiler:        XC32 ver 2.10
  IDE :            MPLABX-IDE 5.10
  Created by:      http://strefapic.blogspot.com
 ******************************************************************************/

#ifndef MMC_PIC32MM_H
#define	MMC_PIC32MM_H


static void power_on (void);
static void power_off (void);
static BYTE xchg_spi (BYTE dat);
static void xmit_spi_multi (
	const BYTE* buff,	/* Data to be sent */
	UINT cnt			/* Number of bytes to send */
);
static void rcvr_spi_multi (
	BYTE* buff,		/* Buffer to store received data */
	UINT cnt		/* Number of bytes to receive */
);
static int wait_ready (void);
static void deselect (void);
static int select (void);	/* 1:Successful, 0:Timeout */
static
int rcvr_datablock (	/* 1:OK, 0:Failed */
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
);
static
int xmit_datablock (	/* 1:OK, 0:Failed */
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data token */
);
static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
);
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0) */
);
DSTATUS disk_initialize (
	BYTE pdrv		/* Physical drive nmuber (0) */
);
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Sector count (1..128) */
);
DRESULT disk_write (
	BYTE pdrv,				/* Physical drive nmuber (0) */
	const BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	UINT count				/* Sector count (1..128) */
);
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
);
void disk_timerproc (void);





#endif	/* MMC_PIC32MM_H */

