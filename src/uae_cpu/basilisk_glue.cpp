/* 2001 MJ */
/*
 *  basilisk_glue.cpp - Glue UAE CPU to Basilisk II CPU engine interface
 *
 *  Basilisk II (C) 1997-2001 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"
#include "emul_op.h"
#include "m68k.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#include <SDL/SDL_timer.h>
#include "main.h"


// RAM and ROM pointers
uint32 RAMBase = 0;	// RAM base (Atari address space) gb-- init is important
uint8 *RAMBaseHost;	// RAM base (host address space)
uint32 RAMSize = 0x00e00000;		// Size of RAM

uint32 ROMBase = 0x00e00000;		// ROM base (Atari address space)
uint8 *ROMBaseHost;	// ROM base (host address space)
uint32 ROMSize = 0x00200000;		// Size of ROM

uint32 RealROMSize;	// Real size of ROM

uint32 FastRAMBase = 0x01000000;		// Fast-RAM base (Atari address space)
uint8 *FastRAMBaseHost;	// Fast-RAM base (host address space)
uint32 FastRAMSize = 0;	// Size of Fast-RAM

uint32 VideoRAMBase = ARANYMVRAMSTART;	// VideoRAM base (Atari address space)
uint8 *VideoRAMBaseHost;// VideoRAM base (host address space)
//uint32 VideoRAMSize;	// Size of VideoRAM
uint32 InterruptFlags;

#if DIRECT_ADDRESSING
uintptr MEMBaseDiff;	// Global offset between a Atari address and its Host equivalent
#endif
#if REAL_ADDRESSING || DIRECT_ADDRESSING
uintptr VMEMBaseDiff;	// Global offset between a Atari VideoRAM address and /dev/fb0 mmap
#endif

// From newcpu.cpp
extern int quit_program;

/*
 *  Initialize 680x0 emulation, CheckROM() must have been called first
 */

bool Init680x0(void)
{
#if REAL_ADDRESSING
	// Mac address space = host address space
	RAMBase = (uint32)RAMBaseHost;
	ROMBase = (uint32)ROMBaseHost;
#elif DIRECT_ADDRESSING
	InitMEMBaseDiff(RAMBaseHost, RAMBase);
	InitVMEMBaseDiff(VideoRAMBaseHost, VideoRAMBase);
#else
	// Initialize UAE memory banks
	RAMBase = 0;
	ROMBase = 0x00e00000;
	memory_init();
#endif

	init_m68k();
	return true;
}


/*
 *  Deinitialize 680x0 emulation
 */

void Exit680x0(void)
{
	exit_m68k();
}


/*
 *  Reset and start 680x0 emulation (doesn't return)
 */

void Start680x0(void)
{
	m68k_reset();
	m68k_go(true);
}


/*
 *  Trigger interrupt
 */
/*
void TriggerInterrupt(void)
{
	regs.spcflags |= SPCFLAG_INT;
}
*/

const unsigned int VBL = 0x777;	// vymyslena hodnota, kruty hack
void TriggerVBL(void)
{
	InterruptFlags = VBL;
	regs.spcflags |= SPCFLAG_DOINT;
}

void TriggerMFP(int no)
{
	if (no == 5)
		regs.spcflags |= SPCFLAG_MFP_TIMERC;
	else if (no == 6)
		regs.spcflags |= SPCFLAG_MFP_ACIA;
	else
		fprintf(stderr, "Unknown MFP interrupt!\n");
}

void TriggerNMI(void)
{
	//!! not implemented yet
}


/*
 *  Get 68k interrupt level
 */

int intlev(void)
{
	return (InterruptFlags == VBL) ? 4 : 0;
}

/*
 *  Execute MacOS 68k trap
 *  r->a[7] and r->sr are unused!
 */

void Execute68kTrap(uint16 trap, struct M68kRegisters *r)
{
	int i;

	// Save old PC
	uaecptr oldpc = m68k_getpc();

	// Set registers
	for (i=0; i<8; i++)
		m68k_dreg(regs, i) = r->d[i];
	for (i=0; i<7; i++)
		m68k_areg(regs, i) = r->a[i];

	// Push trap and EXEC_RETURN on stack
	m68k_areg(regs, 7) -= 2;
	put_word(m68k_areg(regs, 7), M68K_EXEC_RETURN);
	m68k_areg(regs, 7) -= 2;
	put_word(m68k_areg(regs, 7), trap);

	// Execute trap
	m68k_setpc(m68k_areg(regs, 7));
	fill_prefetch_0();
	quit_program = 0;
	m68k_go(true);

	// Clean up stack
	m68k_areg(regs, 7) += 4;

	// Restore old PC
	m68k_setpc(oldpc);
	fill_prefetch_0();

	// Get registers
	for (i=0; i<8; i++)
		r->d[i] = m68k_dreg(regs, i);
	for (i=0; i<7; i++)
		r->a[i] = m68k_areg(regs, i);
	quit_program = 0;
}


/*
 *  Execute 68k subroutine
 *  The executed routine must reside in UAE memory!
 *  r->a[7] and r->sr are unused!
 */

void Execute68k(uint32 addr, struct M68kRegisters *r)
{
	int i;

	// Save old PC
	uaecptr oldpc = m68k_getpc();

	// Set registers
	for (i=0; i<8; i++)
		m68k_dreg(regs, i) = r->d[i];
	for (i=0; i<7; i++)
		m68k_areg(regs, i) = r->a[i];

	// Push EXEC_RETURN and faked return address (points to EXEC_RETURN) on stack
	m68k_areg(regs, 7) -= 2;
	put_word(m68k_areg(regs, 7), M68K_EXEC_RETURN);
	m68k_areg(regs, 7) -= 4;
	put_long(m68k_areg(regs, 7), m68k_areg(regs, 7) + 4);

	// Execute routine
	m68k_setpc(addr);
	fill_prefetch_0();
	quit_program = 0;
	m68k_go(true);

	// Clean up stack
	m68k_areg(regs, 7) += 2;

	// Restore old PC
	m68k_setpc(oldpc);
	fill_prefetch_0();

	// Get registers
	for (i=0; i<8; i++)
		r->d[i] = m68k_dreg(regs, i);
	for (i=0; i<7; i++)
		r->a[i] = m68k_areg(regs, i);
	quit_program = 0;
}

static long lastMillisecond = 0;

void setVirtualTimer() {
	lastMillisecond = SDL_GetTicks();
}

/*
 * called from the inner loop of CPU emulation
 */ 
void incrementVirtualTimer() {
	long newMillisecond = SDL_GetTicks();
	if (newMillisecond >= lastMillisecond + 20) {// 5 ticks = 5 milliseconds
		lastMillisecond += 20;					// 5 milliseconds = 200 Hz
		invoke200HzInterrupt();					// 200 Hz interrupt
	}
}
