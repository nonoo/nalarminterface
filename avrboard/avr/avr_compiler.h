// This file has been prepared for Doxygen automatic documentation generation.
/*! \file *********************************************************************
 *
 * \brief This file implements some macros that makes the IAR C-compiler and
 *        avr-gcc work with the same code base for the AVR architecture
 *
 * \par Application note:
 *      AVR1010: Minimizing the power consumption of XMEGA devices
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2770 $
 * $Date: 2009-09-11 10:55:22 +0200 (fr, 11 sep 2009) $\n
 *****************************************************************************/

/*! \page License
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef COMPILER_AVR_H
#define COMPILER_AVR_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef F_CPU
#define F_CPU 2000000UL
#endif

/** This macro will protect the following code from interrupts.*/
#define ENTER_CRITICAL_REGION() uint8_t volatile saved_sreg = SREG; \
                                     cli();

/** This macro must always be used in conjunction with ENTER_CRITICAL_REGION
    so that interrupts are enabled again.*/
#define LEAVE_CRITICAL_REGION() SREG = saved_sreg;

#if defined(__ICCAVR__)

#include <inavr.h>
#include <ioavr.h>
#include <intrinsics.h>
#include <pgmspace.h>

#ifndef __HAS_ELPM__
#define _MEMATTR  __flash
#else /* __HAS_ELPM__ */
#define _MEMATTR  __farflash
#endif /* __HAS_ELPM__ */

#define cpu_sleep() __sleep()

#define delay_us( us )   __delay_cycles((F_CPU / 1000000UL) * (us))

/**
 * Some preprocessor magic to allow for a header file abstraction of
 * interrupt service routine declarations for the IAR compiler.  This
 * requires the use of the C99 _Pragma() directive (rather than the
 * old #pragma one that could not be used as a macro replacement), as
 * well as two different levels of preprocessor concetanations in
 * order to do both, assign the correct interrupt vector name, as well
 * as construct a unique function name for the ISR.
 *
 * Do *NOT* try to reorder the macros below, or you'll suddenly find
 * out about all kinds of IAR bugs...
 */
#define PRAGMA(x) _Pragma(#x)
#define ISR(vec) PRAGMA(vector=vec) __interrupt void handler_##vec(void)
#define sei() (__enable_interrupt( ))
#define cli() (__disable_interrupt( ))

#define nop() (__no_operation())

#define INLINE PRAGMA(inline=forced) static

#define PROGMEM_LOCATION(var, loc) const _MEMATTR var @ loc
#define PROGMEM_DECLARE(x) _MEMATTR x
#define PROGMEM_STRING(x) ((_MEMATTR const char *)(x))
#define PROGMEM_STRING_T  char const _MEMATTR *
#define PROGMEM_T const _MEMATTR
#define PROGMEM_PTR_T const _MEMATTR *
#define PROGMEM_BYTE_ARRAY_T uint8_t const _MEMATTR *
#define PROGMEM_WORD_ARRAY_T uint16_t const _MEMATTR *
#define PROGMEM_READ_BYTE(x) *(x)
#define PROGMEM_READ_WORD(x) *(x)

#define EEPROM_DECLARE(var) __eeprom var
#define EEGET(var, adr) __EEGET(var, adr)
#define EEPUT(adr, val) __EEPUT(adr, val)

#define watchdog_reset() __watchdog_reset()

#define SHORTENUM /**/

#elif defined(__GNUC__)

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

#define cpu_sleep() sleep_cpu()

#define delay_us(us)   (_delay_us( us ))

#define INLINE static inline

#define nop()   do {__asm__ __volatile__ ("nop");} while (0)

#define PROGMEM_LOCATION(var, loc) var __attribute__((section (#loc)))
#define PROGMEM_DECLARE(x) x __attribute__((__progmem__))
#define PROGMEM_STRING(x) PSTR(x)
#define PROGMEM_STRING_T  PGM_P
#define PROGMEM_T
#define PROGMEM_PTR_T *
#define PROGMEM_BYTE_ARRAY_T uint8_t*
#define PROGMEM_WORD_ARRAY_T uint16_t*
#define PROGMEM_READ_BYTE(x) pgm_read_byte(x)
#define PROGMEM_READ_WORD(x) pgm_read_word(x)

#define EEPROM_DECLARE(var) var EEMEM
#define EEGET(var, addr) (var) = eeprom_read_byte ((uint8_t *)(addr))
#define EEPUT(addr, var) eeprom_write_byte ((uint8_t *)(addr), var)

#define watchdog_reset() wdt_reset()

#define SHORTENUM __attribute__ ((packed))
#else
#error Compiler not supported.
#endif
#endif

/* EOF */
