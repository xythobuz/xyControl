/*
 * error.h
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
#ifndef _error_h
#define _error_h

/** \addtogroup error Error Reporting
 *  \ingroup System
 *  Error reporting with human readable strings.
 *  @{
 */

/** \file error.h
 *  Global listing of different error conditions.
 *  Can be returned to signalise error or success.
 *  Also allows to print human-readable error descriptions.
 */

/** Error Conditions */
typedef enum {
    SUCCESS = 0, /**< No Error */
    TWI_NO_ANSWER, /**< No answer from TWI Slave */
    TWI_WRITE_ERROR, /**< Error while writing to TWI Slave */
    MALLOC_FAIL, /**< Malloc failed */
    ERROR, /**< General Error */
    ARGUMENT_ERROR, /**< Invalid arguments */
} Error;

/** Check an Error Code. Return it if an error occured. */
#define CHECKERROR(x) if(x!=SUCCESS){return x;}

/** Report an error, if it occured. Using printf() */
#define REPORTERROR(x) { \
    if (x != SUCCESS) { \
        char *s = getErrorString(x); \
        printf("Error: %s\n", s); \
        free(s); \
    } \
}

/** Returns a human-readable error description.
 *  Free the string after use!
 */
char *getErrorString(Error e);

#endif
/** @} */