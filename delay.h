/*****************************************************************************
  FileName:        delay.h
  Processor:       PIC32MM0256GPM048
  Compiler:        XC32 ver 2.10
  IDE :            MPLABX-IDE 5.10
  Created by:      http://strefapic.blogspot.com 
 ******************************************************************************/

#ifndef DELAY_H
#define	DELAY_H

#define SYSTEMCLOCK 24000000 /*SYSCLK zegar systemowy*/

   extern void __attribute__((used)) delayUs(unsigned int usec);
   extern void __attribute__((used)) delayMs(unsigned int usec);

#endif	/* DELAY_H */

