/*****************************************************************************
  FileName:        main.c
  Processor:       PIC32MM0256GPM048
  Compiler:        XC32 ver 2.10
  IDE :            MPLABX-IDE 5.10
  Created by:      http://strefapic.blogspot.com
 ******************************************************************************/
/*------------------------------------------------------------------*/
/* Reading of the SD card image data and display on the ILI9163 1.8"                          */
/*------------------------------------------------------------------*/
// ** Sample application for PIC32MM **
//
//    
//	  Required connections for LCD ILI9163:
//     - RESET -  RB11 
//     - A0    -  RC3 
//     - SDA   -  RD0 
//     - SCK   -  RB8 
//    
//   
//    Reqired connections for SD card:
//     - SCK   - RA0 
//     - MISO  - RA1 
//     - MOSI  - RA3 
//     - CS    - RB3 
//
//******************************************************************************

#include "mcc_generated_files/system.h"
#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "diskio.h"
#include "integer.h"
#include "mmc_pic32mm.h"
#include <stdlib.h>
#include "delay.h"
#include "ili9163.h"

DWORD AccSize;			/* Work register for fs command */
WORD AccFiles, AccDirs;
FILINFO Finfo;
FATFS FatFs;		/* File system object */
FIL File[2];	    /* File objects */
TCHAR Buff[128];	/* Working buffer */
UINT s1,s2,cnt;
const BYTE ft[] = {0,12,16,32};
FRESULT res;
FATFS *fs;			/* Pointer to file system object */
//DIR dir;			/* Directory object */


volatile UINT Timer;	/* 1kHz increment timer */
volatile WORD rtcYear = 2018;
volatile BYTE rtcMon = 11, rtcMday = 11, rtcHour, rtcMin, rtcSec;

void DrawBitmapRGB565_withSDcard(const TCHAR* path, uint8_t width, uint8_t height, uint8_t x, uint8_t y);
void __attribute__ ((vector(_TIMER_1_VECTOR), interrupt(IPL1SOFT))) TMR1_ISR();
DWORD get_fattime (void);
static FRESULT scan_files (char* path)	;	/* Pointer to the path name working buffer */
          

int main (void)
{		
	// Enable the Global Interrupts
    //INTERRUPT_GlobalEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalDisable();
	
    SYSTEM_Initialize();
    TMR1_Start();       
	RESET_OFF; /*Reset LCD high*/
    lcdInitialize();
    setRotation(3); /*ROTATION DISPLAY value 0...3*/
    lcdFillScreen(BLACK);  
   
    /*Mount device*/	
	if(f_mount(&FatFs, "", 0)!=FR_OK) {
        /*error handling*/
        lcdPutS("f_mount error!", 5, 20, RED, BLACK, 1);  
        while(1);
	}
        
    /*Initialize drive*/
	DSTATUS driveStatus = disk_initialize(0);
    if(driveStatus) {
       /*error handling*/
       lcdPutS("disk init error", 5, 35, RED, BLACK, 1);
       while(1);
    }
    
    /*slide show*/
    DrawBitmapRGB565_withSDcard("pickup.txt",160, 128, 0, 0);  
    delayMs(1000);
    DrawBitmapRGB565_withSDcard("mikolaj.txt",160, 128, 0, 0);
    
           
    while(1);
    
}

void DrawBitmapRGB565_withSDcard(const TCHAR* path, uint8_t width, uint8_t height, uint8_t x, uint8_t y)
{
    uint16_t count, rgb, num , r, g, b;
    uint32_t licz = 0 ;
   
        lcdWriteCommand(SET_COLUMN_ADDRESS); // Horizontal Address Start Position
    	lcdWriteParameter(0x00);
    	lcdWriteParameter(x);
    	lcdWriteParameter(0x00);
    	lcdWriteParameter(x+(width-1));

    	lcdWriteCommand(SET_PAGE_ADDRESS); // Vertical Address end Position
    	lcdWriteParameter(0x00);
    	lcdWriteParameter(y);
    	lcdWriteParameter(0x00);
    	lcdWriteParameter(0x9f); //9f = 160px

    	lcdWriteCommand(WRITE_MEMORY_START);
        
        /*read data from file*/
        res = f_open(&File[0], path, FA_READ);
        if(res)
        {
         /*error handling*/
         lcdPutS("open file err", 5, 30, RED, BLACK, 2);
        }
    	     
        /*Plot the bitmap data RGB656 16bit*/
    		for(count = width * height; count != 0; count--) //Write horizontal line
    		{
            /*read data from SDcard*/
             res =  f_read(&File[0], Buff, 6, &s2);

            if(res)
            {
            lcdPutS("read file err", 5, 50, RED, BLACK, 2);
            while(1);
            }
            
            /*convert string to int, like this 0x0821 --> 2081*/
            num = (uint16_t)strtol(Buff, NULL, 16);

                rgb = num;
    			
    			/*zamiana pozycji r z b*/
    			r = (rgb & 0x1F) << 11;
    			g = (rgb & 0x7E0);
    			b = (rgb & 0xF800) >> 11;
                rgb = g + r + b ;

                  if(rgb == 0x0000)
    			   {
    			      rgb = BLACK;
    			   }
                /*Plot the bitmap data RGB656 16bit*/
    			lcdWriteData(rgb >> 8 , rgb);
                
                /* Move to offset of 8 and multiplication from top of the file */
                f_lseek(&File[0], licz); //"0x0000, 0x0023, " --> offset 8
                
                licz = licz + 8;
                
    		}
        /*close file*/        
        f_close(&File[0]);
}

/*---------------------------------------------------------*/
/* 1000Hz timer interrupt generated by Timer1              */
/*---------------------------------------------------------*/

void __attribute__ ((vector(_TIMER_1_VECTOR), interrupt(IPL1SOFT))) TMR1_ISR()
{
	static const BYTE samurai[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	static UINT div1k;
	BYTE n;


	IFS0CLR= 1 << _IFS0_T1IF_POSITION;			/* Clear irq flag */
	Timer++;			/* Performance counter for this module */
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */

	/* Real Time Clock */
	if (++div1k >= 1000) {
		div1k = 0;
		if (++rtcSec >= 60) {
			rtcSec = 0;
			if (++rtcMin >= 60) {
				rtcMin = 0;
				if (++rtcHour >= 24) {
					rtcHour = 0;
					n = samurai[rtcMon - 1];
					if ((n == 28) && !(rtcYear & 3)) n++;
					if (++rtcMday > n) {
						rtcMday = 1;
						if (++rtcMon > 12) {
							rtcMon = 1;
							rtcYear++;
						}
					}
				}
			}
		}
	}
}

///*---------------------------------------------------------*/
///* User Provided RTC Function for FatFs module             */
///*---------------------------------------------------------*/
///* This is a real time clock service to be called from     */
///* FatFs module. Any valid time must be returned even if   */
///* the system does not support an RTC.                     */
///* This function is not required in read-only cfg.         */
//
DWORD get_fattime (void)
{
	DWORD tmr;


	//INTERRUPT_GlobalDisable();//_DI();
	/* Pack date and time into a DWORD variable */
	tmr =	  (((DWORD)rtcYear - 1980) << 25)
			| ((DWORD)rtcMon << 21)
			| ((DWORD)rtcMday << 16)
			| (WORD)(rtcHour << 11)
			| (WORD)(rtcMin << 5)
			| (WORD)(rtcSec >> 1);
	//INTERRUPT_GlobalEnable();//_EI();

	return tmr;
}

static
FRESULT scan_files (
	char* path		/* Pointer to the path name working buffer */
)
{
	DIR dirs;
	FRESULT res;
	int i;

	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (Finfo.fattrib & AM_DIR) {
				i = strlen(path);
				AccDirs++;
				path[i] = '/'; strcpy(&path[i+1], Finfo.fname);
				res = scan_files(path);
				path[i] = 0;
				if (res != FR_OK) break;
			} else {
				AccFiles++;
				AccSize += Finfo.fsize;
			}
		}
	}

	return res;
}