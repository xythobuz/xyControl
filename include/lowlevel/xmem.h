/*
 * xmem.h
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
#ifndef _xmem_h
#define _xmem_h

/** \addtogroup xmem External Memory Interface
 *  \ingroup System
 *  Allows access to external RAM with bank-switching.
 *  @{
 */

/** \file xmem.h
 *  XMEM API Header.
 */

/** Switch the bank, if needed.
 *  Stores the old bank in a variable oldMemBank.
 *  \param x New Bank
 */
#define MEMSWITCH(x) uint8_t oldMemBank=xmemGetBank();if(oldMemBank!=x)xmemSetBank(x);

/** Switch back to the last bank, if needed.
 *  \param x New (current) Bank
 */
#define MEMSWITCHBACK(x) if(oldMemBank!=x)xmemSetBank(oldMemBank);

#define MEMBANKS 8 /**< Available Memory Banks */
#define BANK_GENERIC 0 /**< Generic Memory Bank */

#define BANK0PORT PORTG /**< First Bank Selection Port */
#define BANK0DDR DDRG   /**< First Bank Selection Data Direction Register */
#define BANK0PIN PG3    /**< First Bank Selection Pin */
#define BANK1PORT PORTG /**< Second Bank Selection Port */
#define BANK1DDR DDRG   /**< Second Bank Selection Data Direction Register */
#define BANK1PIN PG4    /**< Second Bank Selection Pin */
#define BANK2PORT PORTL /**< Third Bank Selection Port */
#define BANK2DDR DDRL   /**< Third Bank Selection Data Direction Register */
#define BANK2PIN PL5    /**< Third Bank Selection Pin */

/** All Malloc related State.
 *  The Heap is bank-switched, so this state
 *  has to be switched with the banks to allow
 *  different memory allocations on different banks.
 */
typedef struct {
    char *start; /**< Start of Heap */
    char *end; /**< End of Heap */
    void *val; /**< Highest Heap Point */
    void *fl; /**< Free List */
} MallocState;

extern MallocState states[MEMBANKS]; /**< MallocState for all Memory Banks */
extern uint8_t currentBank; /**< Current active Memory Bank */

/** Initialize the External Memory Interface */
void xmemInit(void);

/** Switch the active memory bank.
 *  \param bank New Memory Bank
 */
void xmemSetBank(uint8_t bank);

/** Get the current memory bank.
 *  \returns Current Memory Bank.
 */
uint8_t xmemGetBank(void);

#endif
/** @} */
