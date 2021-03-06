/*
 * debug.h
 *
 * Copyright (c) 2013, Thomas Buck <xythobuz@me.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _debug_h
#define _debug_h

#include <avr/wdt.h>
#include <serial.h>
#include <stdio.h>

/** \addtogroup debug Debug Output
 *  \ingroup System
 *  Allows debug ouput and assert usage.
 *
 *  Usage:
 *  Before including this file, define DEBUG as the debuglevel, eg:
 *
 *  \code{.c}
 *  #define DEBUG 1
 *  \endcode
 *
 *  for debuglevel 1.
 *  Then use debugPrint("Foo") in your code.
 *  If you need to calculate stuff for your debug output, enclose it:
 *
 *  \code{.c}
 *  #if DEBUG >= 1
 *      debugPrint("Bar");
 *  #endif
 *  \endcode
 *  @{
 */

/** \file debug.h
 *  Debug and Assert Header and Implementation
 */

#define DEBUGOUT(x) printf("!%s\n", x) /**< Debug Output Function */

/** Simple Assert Implementation */
#define ASSERTFUNC(x) ({ \
    if (!(x)) { \
        if (DEBUG != 0) { \
            printf("\nError: %s:%i in %s(): Assert '%s' failed!\n", __FILE__, __LINE__, __func__, #x); \
            wdt_enable(WDTO_1S); \
            while (!serialTxBufferEmpty()) \
                wdt_reset(); \
            while(1); \
        } else { \
            wdt_enable(WDTO_15MS); \
            while(1); \
        } \
    } \
})

// Macro Magic
// Controls Debug Output with DEBUG definition.
// Define DEBUG before including this file!
// Disables all debug output if NODEBUG is defined

#ifndef NODEBUG
#define assert(x) ASSERTFUNC(x) /**< Enable assert() */
#else
#define assert(ignore) /**< Disable assert() */
#endif

#if (!(defined(NODEBUG))) && (DEBUG >= 1)
#define debugPrint(x) DEBUGOUT(x) /**< Enable debugPrint() */
#else
#define debugPrint(ignore) /**< Disable debugPrint() */
#endif

#endif // _DEBUG_H
/** @} */
