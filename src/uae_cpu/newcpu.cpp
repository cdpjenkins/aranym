/* 2002 MJ */
 /*
  * UAE - The Un*x Amiga Emulator
  *
  * MC68000 emulation
  *
  * (c) 1995 Bernd Schmidt
  */

#include "sysdeps.h"

#include "cpu_emulation.h"
#include "main.h"
#include "emul_op.h"
#include "natfeats.h"
#include "m68k.h"
#include "memory.h"
#include "readcpu.h"
#include "newcpu.h"
#ifdef USE_JIT
# include "compiler/compemu.h"
#endif
#include "fpu/fpu.h"
#ifdef DSP_EMULATION
#include "../include/dsp_cpu.h"
#endif

#include <cstdlib>

#define DEBUG 1
#include "debug.h"

#if defined(ENABLE_EXCLUSIVE_SPCFLAGS) && !defined(HAVE_HARDWARE_LOCKS)
SDL_mutex *spcflags_lock;
#endif

#define SANITY_CHECK_ATC 1

int quit_program = 0;

#ifndef USE_TIMERS
long maxInnerCounter = 10000;	// default value good for 1GHz Athlon machines
static long innerCounter = 1;
extern void ivoke200HzInterrupt(void);	// in main.cpp
#endif /* !USE_TIMERS */

struct flag_struct regflags;

/* LongJump buffers */
jmp_buf excep_env;
#ifdef DISDIP
jmp_buf loop_env;
#endif
/* Opcode of faulting instruction */
uae_u16 last_op_for_exception_3;
/* PC at fault time */
uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
uaecptr last_fault_for_exception_3;

#ifdef DISDIP
/* Is opcode label tabel initialized?*/
bool initial;

/* Jump table */
void *op_smalltbl_0_lab[65536];
#endif

int areg_byteinc[] = { 1,1,1,1,1,1,1,2 };
int imm8_table[] = { 8,1,2,3,4,5,6,7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

cpuop_func *cpufunctbl[65536];

int broken_in;

static __inline__ unsigned int cft_map (unsigned int f)
{
#ifndef HAVE_GET_WORD_UNSWAPPED
    return f;
#else
    return do_byteswap_16(f);
#endif
}

cpuop_rettype REGPARAM2 op_illg_1 (uae_u32 opcode) REGPARAM;

cpuop_rettype REGPARAM2 op_illg_1 (uae_u32 opcode)
{
    cpuop_return( op_illg (cft_map (opcode)) );
}

static void build_cpufunctbl (void)
{
    int i;
    unsigned long opcode;
    int cpu_level = 4;
    struct cputbl *tbl = op_smalltbl_0_ff;

    for (opcode = 0; opcode < 65536; opcode++)
	cpufunctbl[cft_map (opcode)] = op_illg_1;
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (! tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
    for (opcode = 0; opcode < 65536; opcode++) {
	cpuop_func *f;

	if (table68k[opcode].mnemo == i_ILLG || (unsigned)table68k[opcode].clev > (unsigned)cpu_level)
	    continue;

	if (table68k[opcode].handler != -1) {
	    f = cpufunctbl[cft_map (table68k[opcode].handler)];
	    if (f == op_illg_1)
		abort();
	    cpufunctbl[cft_map (opcode)] = f;
	}
    }
    for (i = 0; tbl[i].handler != NULL; i++) {
	if (tbl[i].specific)
	    cpufunctbl[cft_map (tbl[i].opcode)] = tbl[i].handler;
    }
#ifdef DISDIP
    for (i = 0; i < 65536; i++)
	(*cpufunctbl[i])(i);
    initial = true;
#endif
}

void init_m68k (void)
{
    int i;

    for (i = 0 ; i < 256 ; i++) {
	int j;
	for (j = 0 ; j < 8 ; j++) {
		if (i & (1 << j)) break;
	}
	movem_index1[i] = j;
	movem_index2[i] = 7-j;
	movem_next[i] = i & (~(1 << j));
    }
    read_table68k ();
    do_merges ();

    build_cpufunctbl ();
    fpu_init (CPUType == 4);

#if (ENABLE_EXCLUSIVE_SPCFLAGS && !(HAVE_HARDWARE_LOCKS))
    if ((spcflags_lock = SDL_CreateMutex()) ==  NULL) {
	panicbug("Error by SDL_CreateMutex()");
	exit(EXIT_FAILURE);
    }
#endif
}

void exit_m68k (void)
{
	fpu_exit ();
}

struct regstruct regs, lastint_regs;
// MJ static struct regstruct regs_backup[16];
// MJ static int backup_pointer = 0;
static long int m68kpc_offset;
int lastint_no;


#ifdef FULLMMU
static __inline__ uae_u8 get_ibyte_1(uae_u32 o)
{
    uaecptr addr = m68k_getpc() + o + 1;
    return phys_get_byte(mmu_translate(addr, FC_INST, 0, addr, sz_byte, 0));
}
static __inline__ uae_u16 get_iword_1(uae_u32 o)
{
    uaecptr addr = m68k_getpc() + o;
    return phys_get_word(mmu_translate(addr, FC_INST, 0, addr, sz_word, 0));
}
static __inline__ uae_u32 get_ilong_1(uae_u32 o)
{
    uaecptr addr = m68k_getpc() + o;
    return phys_get_long(mmu_translate(addr, FC_INST, 0, addr, sz_long, 0));
}
#else
# define get_ibyte_1(o) get_byte(m68k_getpc() + (o) + 1)
# define get_iword_1(o) get_word(m68k_getpc() + (o))
# define get_ilong_1(o) get_long(m68k_getpc() + (o))
#endif

uae_s32 ShowEA (int reg, amodes mode, wordsizes size, char *buf)
{
    uae_u16 dp;
    uae_s8 disp8;
    uae_s16 disp16;
    int r;
    uae_u32 dispreg;
    uaecptr addr;
    uae_s32 offset = 0;
    char buffer[80];

    strcpy(buffer, "");

    switch (mode){
     case Dreg:
	sprintf (buffer,"D%d", reg);
	break;
     case Areg:
	sprintf (buffer,"A%d", reg);
	break;
     case Aind:
	sprintf (buffer,"(A%d)", reg);
	break;
     case Aipi:
	sprintf (buffer,"(A%d)+", reg);
	break;
     case Apdi:
	sprintf (buffer,"-(A%d)", reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (uae_s16)disp16;
	sprintf (buffer,"(A%d,$%04x) == $%08lx", reg, disp16 & 0xffff,
					(unsigned long)addr);
	break;
     case Ad8r:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0, disp = 0;
	    uae_s32 base = m68k_areg(regs,reg);
	    char name[10];
	    sprintf (name,"A%d, ",reg);
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%ld)+%ld == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    (unsigned long)disp, (unsigned long)outer,
		    (unsigned long)addr);
	} else {
	  addr = m68k_areg(regs,reg) + (uae_s32)((uae_s8)disp8) + dispreg;
	  sprintf (buffer,"(A%d, %c%d.%c*%d, $%02x) == $%08lx", reg,
	       dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
	       1 << ((dp >> 9) & 3), disp8,
	       (unsigned long)addr);
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (uae_s16)disp16;
	sprintf (buffer,"(PC,$%04x) == $%08lx", disp16 & 0xffff,(unsigned long)addr);
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0,disp = 0;
	    uae_s32 base = addr;
	    char name[10];
	    sprintf (name,"PC, ");
	    if (dp & 0x80) { base = 0; name[0] = 0; }
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	    sprintf (buffer,"(%s%c%d.%c*%d+%ld)+%ld == $%08lx", name,
		    dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
		    1 << ((dp >> 9) & 3),
		    (unsigned long)disp, (unsigned long)outer,
		    (unsigned long)addr);
	} else {
	  addr += (uae_s32)((uae_s8)disp8) + dispreg;
	  sprintf (buffer,"(PC, %c%d.%c*%d, $%02x) == $%08lx", dp & 0x8000 ? 'A' : 'D',
		(int)r, dp & 0x800 ? 'L' : 'W',  1 << ((dp >> 9) & 3),
		disp8, (unsigned long)addr);
	}
	break;
     case absw:
	sprintf (buffer,"$%08lx", (unsigned long)(uae_s32)(uae_s16)get_iword_1 (m68kpc_offset));
	m68kpc_offset += 2;
	break;
     case absl:
	sprintf (buffer,"$%08lx", (unsigned long)get_ilong_1 (m68kpc_offset));
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    sprintf (buffer,"#$%02x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xff));
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    sprintf (buffer,"#$%04x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xffff));
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    sprintf (buffer,"#$%08lx", (unsigned long)(get_ilong_1 (m68kpc_offset)));
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	break;
     case imm0:
	offset = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%02x", (unsigned int)(offset & 0xff));
	break;
     case imm1:
	offset = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	sprintf (buffer,"#$%04x", (unsigned int)(offset & 0xffff));
	break;
     case imm2:
	offset = (uae_s32)get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	sprintf (buffer,"#$%08lx", (unsigned long)offset);
	break;
     case immi:
	offset = (uae_s32)(uae_s8)(reg & 0xff);
	sprintf (buffer,"#$%08lx", (unsigned long)offset);
	break;
     default:
	break;
    }
    if (buf == 0)
	printf ("%s", buffer);
    else
	strcat (buf, buffer);
    return offset;
}

#if 0
/* The plan is that this will take over the job of exception 3 handling -
 * the CPU emulation functions will just do a longjmp to m68k_go whenever
 * they hit an odd address. */
static int verify_ea (int reg, amodes mode, wordsizes size, uae_u32 *val)
{
    uae_u16 dp;
    uae_s8 disp8;
    uae_s16 disp16;
    int r;
    uae_u32 dispreg;
    uaecptr addr;
// MJ    uae_s32 offset = 0;

    switch (mode){
     case Dreg:
	*val = m68k_dreg (regs, reg);
	return 1;
     case Areg:
	*val = m68k_areg (regs, reg);
	return 1;

     case Aind:
     case Aipi:
	addr = m68k_areg (regs, reg);
	break;
     case Apdi:
	addr = m68k_areg (regs, reg);
	break;
     case Ad16:
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr = m68k_areg(regs,reg) + (uae_s16)disp16;
	break;
     case Ad8r:
	addr = m68k_areg (regs, reg);
     d8r_common:
	dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	disp8 = dp & 0xFF;
	r = (dp & 0x7000) >> 12;
	dispreg = dp & 0x8000 ? m68k_areg(regs,r) : m68k_dreg(regs,r);
	if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
	dispreg <<= (dp >> 9) & 3;

	if (dp & 0x100) {
	    uae_s32 outer = 0, disp = 0;
	    uae_s32 base = addr;
	    if (dp & 0x80) base = 0;
	    if (dp & 0x40) dispreg = 0;
	    if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
	    base += disp;

	    if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
	    if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

	    if (!(dp & 4)) base += dispreg;
	    if (dp & 3) base = get_long (base);
	    if (dp & 4) base += dispreg;

	    addr = base + outer;
	} else {
	  addr += (uae_s32)((uae_s8)disp8) + dispreg;
	}
	break;
     case PC16:
	addr = m68k_getpc () + m68kpc_offset;
	disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
	addr += (uae_s16)disp16;
	break;
     case PC8r:
	addr = m68k_getpc () + m68kpc_offset;
	goto d8r_common;
     case absw:
	addr = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	break;
     case absl:
	addr = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	break;
     case imm:
	switch (size){
	 case sz_byte:
	    *val = get_iword_1 (m68kpc_offset) & 0xff;
	    m68kpc_offset += 2;
	    break;
	 case sz_word:
	    *val = get_iword_1 (m68kpc_offset) & 0xffff;
	    m68kpc_offset += 2;
	    break;
	 case sz_long:
	    *val = get_ilong_1 (m68kpc_offset);
	    m68kpc_offset += 4;
	    break;
	 default:
	    break;
	}
	return 1;
     case imm0:
	*val = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm1:
	*val = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	return 1;
     case imm2:
	*val = get_ilong_1 (m68kpc_offset);
	m68kpc_offset += 4;
	return 1;
     case immi:
	*val = (uae_s32)(uae_s8)(reg & 0xff);
	return 1;
     default:
	addr = 0;
	break;
    }
    if ((addr & 1) == 0)
	return 1;

    last_addr_for_exception_3 = m68k_getpc () + m68kpc_offset;
    last_fault_for_exception_3 = addr;
    return 0;
}
#endif

uae_u32 get_disp_ea_020 (uae_u32 base, uae_u32 dp)
{
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    regd <<= (dp >> 9) & 3;
    if (dp & 0x100) {
	uae_s32 outer = 0;
	if (dp & 0x80) base = 0;
	if (dp & 0x40) regd = 0;

	if ((dp & 0x30) == 0x20) base += (uae_s32)(uae_s16)next_iword();
	if ((dp & 0x30) == 0x30) base += next_ilong();

	if ((dp & 0x3) == 0x2) outer = (uae_s32)(uae_s16)next_iword();
	if ((dp & 0x3) == 0x3) outer = next_ilong();

	if ((dp & 0x4) == 0) base += regd;
	if (dp & 0x3) base = get_long (base);
	if (dp & 0x4) base += regd;

	return base + outer;
    } else {
	return base + (uae_s32)((uae_s8)dp) + regd;
    }
}

uae_u32 get_disp_ea_000 (uae_u32 base, uae_u32 dp)
{
    int reg = (dp >> 12) & 15;
    uae_s32 regd = regs.regs[reg];
#if 1
    if ((dp & 0x800) == 0)
	regd = (uae_s32)(uae_s16)regd;
    return base + (uae_s8)dp + regd;
#else
    /* Branch-free code... benchmark this again now that
     * things are no longer inline.  */
    uae_s32 regd16;
    uae_u32 mask;
    mask = ((dp & 0x800) >> 11) - 1;
    regd16 = (uae_s32)(uae_s16)regd;
    regd16 &= mask;
    mask = ~mask;
    base += (uae_s8)dp;
    regd &= mask;
    regd |= regd16;
    return base + regd;
#endif
}

void MakeSR (void)
{
#if 0
    assert((regs.t1 & 1) == regs.t1);
    assert((regs.t0 & 1) == regs.t0);
    assert((regs.s & 1) == regs.s);
    assert((regs.m & 1) == regs.m);
    assert((XFLG & 1) == XFLG);
    assert((NFLG & 1) == NFLG);
    assert((ZFLG & 1) == ZFLG);
    assert((VFLG & 1) == VFLG);
    assert((CFLG & 1) == CFLG);
#endif
    regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
	       | (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
	       | (GET_XFLG << 4) | (GET_NFLG << 3) | (GET_ZFLG << 2) | (GET_VFLG << 1)
	       | GET_CFLG);
}

void MakeFromSR (void)
{
    int oldm = regs.m;
    int olds = regs.s;

    regs.t1 = (regs.sr >> 15) & 1;
    regs.t0 = (regs.sr >> 14) & 1;
    regs.s = (regs.sr >> 13) & 1;
    regs.m = (regs.sr >> 12) & 1;
    regs.intmask = (regs.sr >> 8) & 7;
    SET_XFLG ((regs.sr >> 4) & 1);
    SET_NFLG ((regs.sr >> 3) & 1);
    SET_ZFLG ((regs.sr >> 2) & 1);
    SET_VFLG ((regs.sr >> 1) & 1);
    SET_CFLG (regs.sr & 1);
	if (olds != regs.s) {
	    if (olds) {
		if (oldm)
		    regs.msp = m68k_areg(regs, 7);
		else
		    regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.usp;
	    } else {
		regs.usp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
	    }
	} else if (olds && oldm != regs.m) {
	    if (oldm) {
		regs.msp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.isp;
	    } else {
		regs.isp = m68k_areg(regs, 7);
		m68k_areg(regs, 7) = regs.msp;
	    }
	}

    // SPCFLAGS_SET( SPCFLAG_INT );
    if (regs.t1 || regs.t0)
	SPCFLAGS_SET( SPCFLAG_TRACE );
    else
	SPCFLAGS_CLEAR( SPCFLAG_TRACE );
}

/* for building exception frames */
static __inline__ void exc_push_word(uae_u16 w)
{
    m68k_areg(regs, 7) -= 2;
    put_word(m68k_areg(regs, 7), w);
}
static __inline__ void exc_push_long(uae_u32 l)
{
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), l);
}

static __inline__ void exc_make_frame(
		int format,
		uae_u16	sr,
		uae_u32 currpc,
		int nr,
		uae_u32 x0,
		uae_u32 x1
)
{
    switch(format) {
     case 4:
	exc_push_long(x1);
	exc_push_long(x0);
	break;
     case 3:
     case 2:
	exc_push_long(x0);
	break;
    }

    exc_push_word((format << 12) + (nr * 4));	/* format | vector */
    exc_push_long(currpc);
    exc_push_word(sr);
}

// MJ int in_exception_2 = 0;

void Exception(int nr, uaecptr oldpc)
{
    uae_u32 currpc = m68k_getpc ();
    MakeSR();
    if (!regs.s) {
	regs.usp = m68k_areg(regs, 7);
	m68k_areg(regs, 7) = regs.m ? regs.msp : regs.isp;
	regs.s = 1;
    }

#if 0
    if (nr == 2 && in_exception_2++)	{
	panicbug("HALT: Double Bus Error means bad news! PC: %08lx", m68k_getpc());
	abort();
    }
#endif

    if (nr == 2) {
#if 0
			if (currprefs.cpu_level == 5)	{
				/* 68060 */
				exc_make_frame(4,
						regs.sr,
						currpc,
						nr,
						regs.mmu_fault_addr, /* fault address */
						regs.mmu_fslw /* fault status long-word */
						);
			}
			else if (currprefs.cpu_level == 4)	{
#endif
	panicbug("Exception Nr. %d CPC: %08lx RPC: %08lx Addr: %08lx", nr, currpc, oldpc, regs.mmu_fault_addr);
	/* 68040 */
	exc_push_long(0);	/* PD3 */
	exc_push_long(0);	/* PD2 */
	exc_push_long(0);	/* PD1 */
	exc_push_long(0);	/* PD0/WB1D */
	exc_push_long(0);	/* WB1A */
	exc_push_long(0);	/* WB2D */
	exc_push_long(0);	/* WB2A */
	exc_push_long(0);	/* WB3D */
	exc_push_long(0);	/* WB3A */
	exc_push_long(regs.mmu_fault_addr);
	exc_push_word(0);	/* WB1S */
	exc_push_word(0);	/* WB2S */
	exc_push_word(0);	/* WB3S */
	exc_push_word(regs.mmu_ssw);
	exc_push_long(regs.mmu_fault_addr);	/* EA */
	exc_make_frame(7, regs.sr, currpc, 2, 0, 0);
    } else if (nr == 3) {
	exc_make_frame(2, regs.sr, last_addr_for_exception_3, nr,
			last_fault_for_exception_3 & 0xfffffffe, 0);
    } else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
	/* div by zero, CHK, TRAP or TRACE */
	exc_make_frame(2, regs.sr, currpc, nr, oldpc, 0);
    } else if (regs.m && nr >= 24 && nr < 32) {
	/* interrupts! */
	exc_make_frame(0, regs.sr, currpc, nr, 0, 0);
	regs.sr |= (1 << 13);
	regs.msp = m68k_areg(regs, 7);
	m68k_areg(regs, 7) = regs.isp;

	exc_make_frame(1,	/* throwaway */
			regs.sr, currpc, nr, 0, 0);
    } else {
	exc_make_frame(0, regs.sr, currpc, nr, 0, 0);
    }
    m68k_setpc (get_long (regs.vbr + 4*nr));
    fill_prefetch_0 ();
    regs.t1 = regs.t0 = regs.m = 0;
    SPCFLAGS_CLEAR(SPCFLAG_TRACE | SPCFLAG_DOTRACE);

#if 0  
	if (nr == 2 || nr == 3) {	// 16 words on stack
	   // internal register
		m68k_areg(regs, 7) -= 4;
		put_long (m68k_areg(regs, 7), 0);

		// data to write
		m68k_areg(regs, 7) -= 4;
		put_long (m68k_areg(regs, 7), 0);

	    	// internal register
		m68k_areg(regs, 7) -= 4;
		put_long (m68k_areg(regs, 7), 0);

		// data to write
		m68k_areg(regs, 7) -= 4;
		put_long (m68k_areg(regs, 7), last_fault_for_exception_3);

	    	// instruction B prefetch
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), get_word(regs.pc+2, false));

	    	// instruction C prefetch
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), get_word(regs.pc+4, false));

	    	// special status register ssw
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), 0x0100);

	    	// internal register
		m68k_areg(regs, 7) -= 2;
		put_word (m68k_areg(regs, 7), 0);

		// vector offset
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0xa000 + nr * 4);

		// PC
    	    m68k_areg(regs, 7) -= 4;
    	    put_long (m68k_areg(regs, 7), currpc);
	    goto kludge_me_do;
	} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
	    m68k_areg(regs, 7) -= 4;
	    put_long (m68k_areg(regs, 7), oldpc);
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0x2000 + nr * 4);
	} else if (regs.m && nr >= 24 && nr < 32) {
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), nr * 4);
	    m68k_areg(regs, 7) -= 4;
	    put_long (m68k_areg(regs, 7), currpc);
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), regs.sr);
	    regs.sr |= (1 << 13);
	    regs.msp = m68k_areg(regs, 7);
	    m68k_areg(regs, 7) = regs.isp;
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), 0x1000 + nr * 4);
	} else {
	    m68k_areg(regs, 7) -= 2;
	    put_word (m68k_areg(regs, 7), nr * 4);
	}
    m68k_areg(regs, 7) -= 4;
    put_long (m68k_areg(regs, 7), currpc);
kludge_me_do:
    m68k_areg(regs, 7) -= 2;
    put_word (m68k_areg(regs, 7), regs.sr);
    m68k_setpc (get_long (regs.vbr + 4*nr, false));
    fill_prefetch_0 ();
    regs.t1 = regs.t0 = regs.m = 0;
    regs.spcflags &= ~(SPCFLAG_TRACE | SPCFLAG_DOTRACE);
#endif
}

static void Interrupt(int nr)
{
    assert(nr < 8 && nr >= 0);
    lastint_regs = regs;
    lastint_no = nr;
    Exception(nr+24, 0);

    regs.intmask = nr;
    // why the hell the SPCFLAG_INT is to be set??? (joy)
    // regs.spcflags |= SPCFLAG_INT; (disabled by joy)
}

static void MFPInterrupt(int nr)
{
    // fprintf(stderr, "CPU: in MFPInterrupt\n");
    lastint_regs = regs;
    lastint_no = 6;
    Exception(nr, 0);

    regs.intmask = 6;
}

int m68k_move2c (int regno, uae_u32 *regp)
{
/* MJ   if ((CPUType == 1 && (regno & 0x7FF) > 1)
	|| (CPUType < 4 && (regno & 0x7FF) > 2)
	|| (CPUType == 4 && regno == 0x802))
    {
	op_illg (0x4E7B);
	return 0;
    } else {*/
	switch (regno) {
	 case 0: regs.sfc = *regp & 7; break;
	 case 1: regs.dfc = *regp & 7; break;
	 case 2: regs.cacr = *regp & 0x80008000;
#ifdef USE_JIT
		 set_cache_state((regs.cacr & 0x8000) || 0);
		 if (*regp & 0x08) {	/* Just to be on the safe side */
			flush_icache(2);
		 }
#endif
		 break;
	 case 3: mmu_set_tc(*regp & 0xc000); break;
	 case 4:
	 case 5:
	 case 6:
	 case 7: mmu_set_ttr(regno, *regp & 0xffffe364); break;
	 case 0x800: regs.usp = *regp; break;
	 case 0x801: regs.vbr = *regp; break;
	 case 0x802: regs.caar = *regp & 0xfc; break;
	 case 0x803: regs.msp = *regp; if (regs.m == 1) m68k_areg(regs, 7) = regs.msp; break;
	 case 0x804: regs.isp = *regp; if (regs.m == 0) m68k_areg(regs, 7) = regs.isp; break;
	 case 0x805: mmu_set_mmusr(*regp); break;
	 case 0x806: regs.urp = *regp & 0xffffff00; break;
	 case 0x807: regs.srp = *regp & 0xffffff00; break;
	 default:
	    op_illg (0x4E7B);
	    return 0;
	}
// MJ    }
    return 1;
}

int m68k_movec2 (int regno, uae_u32 *regp)
{
/* MJ    if ((CPUType == 1 && (regno & 0x7FF) > 1)
	|| (CPUType < 4 && (regno & 0x7FF) > 2)
	|| (CPUType == 4 && regno == 0x802))
    {
	op_illg (0x4E7A);
	return 0;
    } else {*/
	switch (regno) {
	 case 0: *regp = regs.sfc; break;
	 case 1: *regp = regs.dfc; break;
	 case 2: *regp = regs.cacr; break;
	 case 3: *regp = regs.tc; break;
	 case 4: *regp = regs.itt0; break;
	 case 5: *regp = regs.itt1; break;
	 case 6: *regp = regs.dtt0; break;
	 case 7: *regp = regs.dtt1; break;
	 case 0x800: *regp = regs.usp; break;
	 case 0x801: *regp = regs.vbr; break;
	 case 0x802: *regp = regs.caar; break;
	 case 0x803: *regp = regs.m == 1 ? m68k_areg(regs, 7) : regs.msp; break;
	 case 0x804: *regp = regs.m == 0 ? m68k_areg(regs, 7) : regs.isp; break;
	 case 0x805: *regp = regs.mmusr; break;
	 case 0x806: *regp = regs.urp; break;
	 case 0x807: *regp = regs.srp; break;
	 default:
	    op_illg (0x4E7A);
	    return 0;
	}
// MJ    }
    return 1;
}

static __inline__ int
div_unsigned(uae_u32 src_hi, uae_u32 src_lo, uae_u32 div, uae_u32 *quot, uae_u32 *rem)
{
	uae_u32 q = 0, cbit = 0;
	int i;

	if (div <= src_hi) {
	    return 1;
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) src_hi++;
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return 0;
}

void m68k_divl (uae_u32 opcode, uae_u32 src, uae_u16 extra, uaecptr oldpc)
{
#if defined(uae_s64)
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s64 a = (uae_s64)(uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_s64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_s64)(uae_s32)src;
	quot = a / (uae_s64)(uae_s32)src;
	if ((quot & UVAL64(0xffffffff80000000)) != 0
	    && (quot & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    if (((uae_s32)rem < 0) != ((uae_s64)a < 0)) rem = -rem;
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u64 a = (uae_u64)(uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u64 quot, rem;

	if (extra & 0x400) {
	    a &= 0xffffffffu;
	    a |= (uae_u64)m68k_dreg(regs, extra & 7) << 32;
	}
	rem = a % (uae_u64)src;
	quot = a / (uae_u64)src;
	if (quot > 0xffffffffu) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#else
    if (src == 0) {
	Exception (5, oldpc);
	return;
    }
    if (extra & 0x800) {
	/* signed variant */
	uae_s32 lo = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_s32 hi = lo < 0 ? -1 : 0;
	uae_s32 save_high;
	uae_u32 quot, rem;
	uae_u32 sign;

	if (extra & 0x400) {
	    hi = (uae_s32)m68k_dreg(regs, extra & 7);
	}
	save_high = hi;
	sign = (hi ^ src);
	if (hi < 0) {
	    hi = ~hi;
	    lo = -lo;
	    if (lo == 0) hi++;
	}
	if ((uae_s32)src < 0) src = -src;
	if (div_unsigned(hi, lo, src, &quot, &rem) ||
	    (sign & 0x80000000) ? quot > 0x80000000 : quot > 0x7fffffff) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    if (sign & 0x80000000) quot = -quot;
	    if (((uae_s32)rem < 0) != (save_high < 0)) rem = -rem;
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    } else {
	/* unsigned */
	uae_u32 lo = (uae_u32)m68k_dreg(regs, (extra >> 12) & 7);
	uae_u32 hi = 0;
	uae_u32 quot, rem;

	if (extra & 0x400) {
	    hi = (uae_u32)m68k_dreg(regs, extra & 7);
	}
	if (div_unsigned(hi, lo, src, &quot, &rem)) {
	    SET_VFLG (1);
	    SET_NFLG (1);
	    SET_CFLG (0);
	} else {
	    SET_VFLG (0);
	    SET_CFLG (0);
	    SET_ZFLG (((uae_s32)quot) == 0);
	    SET_NFLG (((uae_s32)quot) < 0);
	    m68k_dreg(regs, extra & 7) = rem;
	    m68k_dreg(regs, (extra >> 12) & 7) = quot;
	}
    }
#endif
}

static __inline__ void
mul_unsigned(uae_u32 src1, uae_u32 src2, uae_u32 *dst_hi, uae_u32 *dst_lo)
{
	uae_u32 r0 = (src1 & 0xffff) * (src2 & 0xffff);
	uae_u32 r1 = ((src1 >> 16) & 0xffff) * (src2 & 0xffff);
	uae_u32 r2 = (src1 & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 r3 = ((src1 >> 16) & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 lo;

	lo = r0 + ((r1 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r0 = lo;
	lo = r0 + ((r2 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r3 += ((r1 >> 16) & 0xffff) + ((r2 >> 16) & 0xffff);
	*dst_lo = lo;
	*dst_hi = r3;
}

void m68k_mull (uae_u32 opcode, uae_u32 src, uae_u16 extra)
{
#if defined(uae_s64)
    if (extra & 0x800) {
	/* signed variant */
	uae_s64 a = (uae_s64)(uae_s32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (uae_s64)(uae_s32)src;
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (a == 0);
	SET_NFLG (a < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff80000000)) != 0
		 && (a & UVAL64(0xffffffff80000000)) != UVAL64(0xffffffff80000000))
	{
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (uae_u32)a;
    } else {
	/* unsigned */
	uae_u64 a = (uae_u64)(uae_u32)m68k_dreg(regs, (extra >> 12) & 7);

	a *= (uae_u64)src;
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (a == 0);
	SET_NFLG (((uae_s64)a) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = a >> 32;
	else if ((a & UVAL64(0xffffffff00000000)) != 0) {
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = (uae_u32)a;
    }
#else
    if (extra & 0x800) {
	/* signed variant */
	uae_s32 src1,src2;
	uae_u32 dst_lo,dst_hi;
	uae_u32 sign;

	src1 = (uae_s32)src;
	src2 = (uae_s32)m68k_dreg(regs, (extra >> 12) & 7);
	sign = (src1 ^ src2);
	if (src1 < 0) src1 = -src1;
	if (src2 < 0) src2 = -src2;
	mul_unsigned((uae_u32)src1,(uae_u32)src2,&dst_hi,&dst_lo);
	if (sign & 0x80000000) {
		dst_hi = ~dst_hi;
		dst_lo = -dst_lo;
		if (dst_lo == 0) dst_hi++;
	}
	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (dst_hi == 0 && dst_lo == 0);
	SET_NFLG (((uae_s32)dst_hi) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if ((dst_hi != 0 || (dst_lo & 0x80000000) != 0)
		 && ((dst_hi & 0xffffffff) != 0xffffffff
		     || (dst_lo & 0x80000000) != 0x80000000))
	{
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    } else {
	/* unsigned */
	uae_u32 dst_lo,dst_hi;

	mul_unsigned(src,(uae_u32)m68k_dreg(regs, (extra >> 12) & 7),&dst_hi,&dst_lo);

	SET_VFLG (0);
	SET_CFLG (0);
	SET_ZFLG (dst_hi == 0 && dst_lo == 0);
	SET_NFLG (((uae_s32)dst_hi) < 0);
	if (extra & 0x400)
	    m68k_dreg(regs, extra & 7) = dst_hi;
	else if (dst_hi != 0) {
	    SET_VFLG (1);
	}
	m68k_dreg(regs, (extra >> 12) & 7) = dst_lo;
    }
#endif
}
static char* ccnames[] =
{ "T ","F ","HI","LS","CC","CS","NE","EQ",
  "VC","VS","PL","MI","GE","LT","GT","LE" };

// If value is greater than zero, this means we are still processing an EmulOp
// because the counter is incremented only in m68k_execute(), i.e. interpretive
// execution only
#ifdef USE_JIT
static int m68k_execute_depth = 0;
#endif

void m68k_reset (void)
{
    m68k_areg (regs, 7) = phys_get_long(0x00000000);
    m68k_setpc (phys_get_long(0x00000004));
    mmu_set_tc(regs.tc & ~0x8000); /* disable mmu */
    fill_prefetch_0 ();
    regs.s = 1;
    regs.m = 0;
    regs.stopped = 0;
    regs.t1 = 0;
    regs.t0 = 0;
    SET_ZFLG (0);
    SET_XFLG (0);
    SET_CFLG (0);
    SET_VFLG (0);
    SET_NFLG (0);
    SPCFLAGS_INIT( 0 );
    regs.intmask = 7;
    regs.vbr = regs.sfc = regs.dfc = 0;
    /* gb-- moved into {fpp,fpu_x86}.cpp::fpu_init()
    regs.fpcr = regs.fpsr = regs.fpiar = 0; */
    fpu_reset();
    // MMU
    mmu_set_root_pointer(0x806, 0);	// regs.urp = 0;
    mmu_set_root_pointer(0x807, 0);	// regs.srp = 0;
    mmu_set_ttr(TTR_D0, 0);
    mmu_set_ttr(TTR_D1, 0);
    mmu_set_ttr(TTR_I0, 0);
    mmu_set_ttr(TTR_I1, 0);
    mmu_set_mmusr(0);
    // Cache
    regs.cacr = 0;
    regs.caar = 0;
}

void m68k_emulop_return(void)
{
	SPCFLAGS_SET( SPCFLAG_BRK );
	quit_program = 1;
}

void m68k_emulop(uae_u32 opcode)
{
	struct M68kRegisters r;
	int i;
	
	for (i=0; i<8; i++) {
		r.d[i] = m68k_dreg(regs, i);
		r.a[i] = m68k_areg(regs, i);
	}
	MakeSR();
	r.sr = regs.sr;
	EmulOp(opcode, &r);
	for (i=0; i<8; i++) {
		m68k_dreg(regs, i) = r.d[i];
		m68k_areg(regs, i) = r.a[i];
	}
	regs.sr = r.sr;
	MakeFromSR();
}

void m68k_natfea_id(void)
{
	struct M68kRegisters r;
	int i;

	/* is it really necessary to save all registers? */
	for (i=0; i<8; i++) {
		r.d[i] = m68k_dreg(regs, i);
		r.a[i] = m68k_areg(regs, i);
	}
	MakeSR();
	r.sr = regs.sr;

	memptr stack = r.a[7] + 4;	/* skip return address */
	r.d[0] = nf_get_id(stack);

	for (i=0; i<8; i++) {
		m68k_dreg(regs, i) = r.d[i];
		m68k_areg(regs, i) = r.a[i];
	}
	regs.sr = r.sr;
	MakeFromSR();
}

void m68k_natfea_rcall(void)
{
	struct M68kRegisters r;
	int i;

	/* is it really necessary to save all registers? */
	for (i=0; i<8; i++) {
		r.d[i] = m68k_dreg(regs, i);
		r.a[i] = m68k_areg(regs, i);
	}
	MakeSR();
	r.sr = regs.sr;

	memptr stack = r.a[7] + 4;	/* skip return address */
	r.d[0] = nf_rcall(stack);

	for (i=0; i<8; i++) {
		m68k_dreg(regs, i) = r.d[i];
		m68k_areg(regs, i) = r.a[i];
	}
	regs.sr = r.sr;
	MakeFromSR();
}

#ifdef DISDIP
cpuop_rettype REGPARAM2 op_illg (uae_u32 opc)
{
	if (initial) {
		opcode = opc;
		goto op_illg_lab;
	}
	op_smalltbl_0_lab[opc] = &&op_illg_lab;
	return;
op_illg_lab:
#else
cpuop_rettype REGPARAM2 op_illg (uae_u32 opcode)
{		
#endif
	uaecptr pc = m68k_getpc ();

	if ((opcode & 0xF000) == 0xA000) {
	Exception(0xA,0);
	cpuop_return(CFLOW_TRAP);
	}

	if ((opcode & 0xF000) == 0xF000) {
	Exception(0xB,0);
	cpuop_return(CFLOW_TRAP);
	}

	D(bug("Illegal instruction: %04x at %08lx", opcode, pc));
#if USE_JIT && JIT_DEBUG
	compiler_dumpstate();
#endif

	Exception (4,0);
	cpuop_return(CFLOW_TRAP);
#ifdef DISDIP
	longjmp(loop_env, 0);
#endif
}

#ifndef FULLMMU
void mmu_op(uae_u32 opcode, uae_u16 extra)
{
    if ((opcode & 0xFF8) == 0x0500) { /* PFLUSHN instruction (An) */
	flush_internals();
    } else if ((opcode & 0xFF8) == 0x0508) { /* PFLUSH instruction (An) */
	flush_internals();
    } else if ((opcode & 0xFF8) == 0x0510) { /* PFLUSHAN instruction */
	flush_internals();
    } else if ((opcode & 0xFF8) == 0x0518) { /* PFLUSHA instruction */
	flush_internals();
    } else if ((opcode & 0xFF8) == 0x548) { /* PTESTW instruction */
    } else if ((opcode & 0xFF8) == 0x568) { /* PTESTR instruction */
    } else op_illg(opcode);
}
#endif

static uaecptr last_trace_ad = 0;

static void do_trace (void)
{
    if (regs.t0) {
       uae_u16 opcode;
       /* should also include TRAP, CHK, SR modification FPcc */
       /* probably never used so why bother */
       /* We can afford this to be inefficient... */
       m68k_setpc (m68k_getpc ());
       fill_prefetch_0 ();
       opcode = get_word(m68k_getpc());
       if (opcode == 0x4e72            /* RTE */
           || opcode == 0x4e74                 /* RTD */
           || opcode == 0x4e75                 /* RTS */
           || opcode == 0x4e77                 /* RTR */
           || opcode == 0x4e76                 /* TRAPV */
           || (opcode & 0xffc0) == 0x4e80      /* JSR */
           || (opcode & 0xffc0) == 0x4ec0      /* JMP */
           || (opcode & 0xff00) == 0x6100  /* BSR */
           || ((opcode & 0xf000) == 0x6000     /* Bcc */
               && cctrue((opcode >> 8) & 0xf))
           || ((opcode & 0xf0f0) == 0x5050 /* DBcc */
               && !cctrue((opcode >> 8) & 0xf)
               && (uae_s16)m68k_dreg(regs, opcode & 7) != 0))
      {
 	    last_trace_ad = m68k_getpc ();
	    SPCFLAGS_CLEAR( SPCFLAG_TRACE );
	    SPCFLAGS_SET( SPCFLAG_DOTRACE );
	}
    } else if (regs.t1) {
       last_trace_ad = m68k_getpc ();
       SPCFLAGS_CLEAR( SPCFLAG_TRACE );
       SPCFLAGS_SET( SPCFLAG_DOTRACE );
    }
}

#define SERVE_VBL_MFP(resetStop)							\
{															\
	if (SPCFLAGS_TEST( SPCFLAG_VBL|SPCFLAG_INT5|SPCFLAG_MFP )) {		\
		if (SPCFLAGS_TEST( SPCFLAG_VBL )) {					\
			if (4 > regs.intmask) {							\
				Interrupt(4);								\
				regs.stopped = 0;							\
				SPCFLAGS_CLEAR( SPCFLAG_VBL );				\
				if (resetStop)								\
					SPCFLAGS_CLEAR( SPCFLAG_STOP );			\
			}												\
		}													\
		if (SPCFLAGS_TEST( SPCFLAG_INT5 )) {					\
			if (5 > regs.intmask) {							\
				Interrupt(5);								\
				regs.stopped = 0;							\
				SPCFLAGS_CLEAR( SPCFLAG_INT5 );				\
				if (resetStop)								\
					SPCFLAGS_CLEAR( SPCFLAG_STOP );			\
			}												\
		}													\
		if (SPCFLAGS_TEST( SPCFLAG_MFP )) {					\
			if (6 > regs.intmask) {							\
				int vector_number = mfp.doInterrupt();		\
				if (vector_number) {						\
					MFPInterrupt(vector_number);			\
					regs.stopped = 0;						\
					if (resetStop)							\
						SPCFLAGS_CLEAR( SPCFLAG_STOP );		\
				}											\
				else										\
					SPCFLAGS_CLEAR( SPCFLAG_MFP );			\
			}												\
		}													\
	}														\
}

int m68k_do_specialties(void)
{
#if USE_JIT
	// Block was compiled
	SPCFLAGS_CLEAR( SPCFLAG_JIT_END_COMPILE );
	
	// Retain the request to get out of compiled code until
	// we reached the toplevel execution, i.e. the one that
	// can compile then run compiled code. This also means
	// we processed all (nested) EmulOps
	if ((m68k_execute_depth == 0) && SPCFLAGS_TEST( SPCFLAG_JIT_EXEC_RETURN ))
		SPCFLAGS_CLEAR( SPCFLAG_JIT_EXEC_RETURN );
#endif
	/*n_spcinsns++;*/
	if (SPCFLAGS_TEST( SPCFLAG_DOTRACE )) {
		Exception (9,last_trace_ad);
	}
	while (SPCFLAGS_TEST( SPCFLAG_STOP )) {
		usleep(1000);	// give unused time slices back to OS
		SERVE_VBL_MFP(true);
		if (SPCFLAGS_TEST( SPCFLAG_NMI ))
			break;
#ifndef USE_TIMERS
		innerCounter -= maxInnerCounter / 5;
		if (innerCounter <= 0) {
			innerCounter = maxInnerCounter;
			invoke200HzInterrupt();
	    }
#endif
	}
	if (SPCFLAGS_TEST( SPCFLAG_TRACE ))
		do_trace ();

	SERVE_VBL_MFP(false);

/*  
// do not understand the INT vs DOINT stuff so I disabled it (joy)
	if (regs.spcflags & SPCFLAG_INT) {
		regs.spcflags &= ~SPCFLAG_INT;
		regs.spcflags |= SPCFLAG_DOINT;
	}
*/
	if (SPCFLAGS_TEST( SPCFLAG_BRK | SPCFLAG_MODE_CHANGE )) {
		SPCFLAGS_CLEAR( SPCFLAG_BRK | SPCFLAG_MODE_CHANGE );
		return CFLOW_EXEC_RETURN;
	}

	if (SPCFLAGS_TEST( SPCFLAG_NMI ))
		return 1;

	return 0;
}

void m68k_do_execute (void)
{
    uae_u32 pc;
    uae_u32 opcode;
#ifdef DISDIP
    if (setjmp(loop_env)) return;
#endif	    
    for (;;) {
#ifdef DSP_EMULATION
	dsp56k_do_execute();
#endif
	pc = m68k_getpc();
#ifdef FULL_HISTORY
#ifdef NEED_TO_DEBUG_BADLY
	history[lasthist] = regs;
	historyf[lasthist] =  regflags;
#else
	history[lasthist] = m68k_getpc();
#endif
	if (++lasthist == MAX_HIST) lasthist = 0;
	if (lasthist == firsthist) {
	    if (++firsthist == MAX_HIST) firsthist = 0;
	}
#endif

#if ARAM_PAGE_CHECK
# if 0
	if (((pc ^ pc_page) > ARAM_PAGE_MASK)) {
	    check_ram_boundary(pc, 2, false);
//	    opcode = GET_OPCODE;
	    uae_u16* addr = (uae_u16*)get_real_address(pc, 0, sz_word);
	    pc_page = pc;
	    pc_offset = (uae_u32)addr - pc;
	}
#  ifdef HAVE_GET_WORD_UNSWAPPED
	opcode = do_get_mem_word_unswapped((uae_u16*)(pc + pc_offset));
#  else
	opcode = do_get_mem_word((uae_u16*)(pc + pc_offset));
#  endif
# else
	if (((pc ^ pc_page) > ARAM_PAGE_MASK)) {
	    check_ram_boundary(pc, 2, false);
	    pc_page = pc;
	    pc_offset = (uae_u32)get_real_address(pc, 0, sz_word) - pc;
	}
# endif
#else
	check_ram_boundary(pc, 2, false);
#endif
	opcode = GET_OPCODE;
#ifdef DISDIP
	printf("%lx\n",opcode);
#endif

// Seems to be faster without the assembly... - yes, it is "bugfix"
#if (0 && defined(X86_ASSEMBLY))
	__asm__ __volatile__ ("\tpushl %%ebp\n\tcall *%%ebx\n\tpopl %%ebp" /* FIXME */
                     : : "b" (cpufunctbl[opcode]), "a" (opcode)
                     : "%edx", "%ecx", "%esi", "%edi",  "%ebp", "memory", "cc");
#else
	(*cpufunctbl[opcode])(opcode);
#endif

#ifndef DISDIP
	if (SPCFLAGS_TEST(SPCFLAG_ALL_BUT_EXEC_RETURN)) {
	    if (m68k_do_specialties())
		return;
	}
#endif

#ifndef USE_TIMERS
	{
	    if (--innerCounter == 0) {
		innerCounter = maxInnerCounter;
		invoke200HzInterrupt();
	    }
	}
#endif
    }
}

#if USE_JIT
void m68k_compile_execute (void)
{
setjmpagain:
    int prb = setjmp(excep_env);
    if (prb != 0) {
	flush_icache(0);
	flush_internals();
        Exception(prb, 0);
    	goto setjmpagain;
    }
    for (;;) {
	if (quit_program > 0) {
	    if (quit_program == 1)
		break;
	    quit_program = 0;
	    m68k_reset ();
	}
	m68k_do_compile_execute();
    }
#if 0
    if (debugging) {
	uaecptr nextpc;
	m68k_dumpstate(&nextpc);
	exit(1);
    }
#endif
}
#endif

void m68k_execute (void)
{
#if USE_JIT
    m68k_execute_depth++;
#endif

setjmpagain:
    int prb = setjmp(excep_env);
    if (prb != 0) {
        Exception(prb, 0);
    	goto setjmpagain;
    }
    for (;;) {
	if (quit_program > 0) {
	    if (quit_program == 1)
		break;
	    quit_program = 0;
	    m68k_reset ();
	}
#ifdef DEBUGGER
	if (debugging) debug();
#endif
	m68k_do_execute();
    }

#if USE_JIT
    m68k_execute_depth--;
#endif
}

#if 0
static void m68k_verify (uaecptr addr, uaecptr *nextpc)
{
    uae_u32 opcode, val;
    struct instr *dp;

    opcode = get_iword_1(0);
    last_op_for_exception_3 = opcode;
    m68kpc_offset = 2;

    if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	opcode = 0x4AFC;
    }
    dp = table68k + opcode;

    if (dp->suse) {
	if (!verify_ea (dp->sreg, (amodes)dp->smode, (wordsizes)dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
    if (dp->duse) {
	if (!verify_ea (dp->dreg, (amodes)dp->dmode, (wordsizes)dp->size, &val)) {
	    Exception (3, 0);
	    return;
	}
    }
}
#endif

void m68k_disasm (uaecptr addr, uaecptr *nextpc, int cnt)
{
    uaecptr newpc = 0;
    m68kpc_offset = addr - m68k_getpc ();
    while (cnt-- > 0) {
	char instrname[20],*ccpt;
	int opwords;
	uae_u32 opcode;
	struct mnemolookup *lookup;
	struct instr *dp;
	printf ("%08lx: ", m68k_getpc () + m68kpc_offset);
	for (opwords = 0; opwords < 5; opwords++){
	    printf ("%04x ", get_iword_1 (m68kpc_offset + opwords*2));
	}
	opcode = get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
	    opcode = 0x4AFC;
	}
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
	    ;

	strcpy (instrname, lookup->name);
	ccpt = strstr (instrname, "cc");
	if (ccpt != 0) {
	    strncpy (ccpt, ccnames[dp->cc], 2);
	}
	printf ("%s", instrname);
	switch (dp->size){
	 case sz_byte: printf (".B "); break;
	 case sz_word: printf (".W "); break;
	 case sz_long: printf (".L "); break;
	 default: printf ("   "); break;
	}

	if (dp->suse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->sreg, (amodes)dp->smode, (wordsizes)dp->size, 0);
	}
	if (dp->suse && dp->duse)
	    printf (",");
	if (dp->duse) {
	    newpc = m68k_getpc () + m68kpc_offset;
	    newpc += ShowEA (dp->dreg, (amodes)dp->dmode, (wordsizes)dp->size, 0);
	}
	if (ccpt != 0) {
	    if (cctrue(dp->cc))
		printf (" == %08lx (TRUE)", (unsigned long)newpc);
	    else
		printf (" == %08lx (FALSE)", (unsigned long)newpc);
	} else if ((opcode & 0xff00) == 0x6100) /* BSR */
	    printf (" == %08lx", (unsigned long)newpc);
	printf ("\n");
    }
    if (nextpc)
	*nextpc = m68k_getpc () + m68kpc_offset;
}

#ifdef NEWDEBUG
void newm68k_disasm(FILE *f, uaecptr addr, uaecptr *nextpc, volatile unsigned int cnt)
{
    char *buffer = (char *)malloc(80 * sizeof(char));
    jmp_buf excep_env_old;
    excep_env_old = excep_env;
    strcpy(buffer,"");
    volatile uaecptr newpc = 0;
    m68kpc_offset = addr - m68k_getpc ();
    if (cnt == 0) {
        int prb = setjmp(excep_env);
        if (prb != 0) {
            goto setjmpagainx;
        }
        char instrname[20],*ccpt;
        int opwords;
        uae_u32 opcode;
        struct mnemolookup *lookup;
        struct instr *dp;
        for (opwords = 0; opwords < 5; opwords++) {
            get_iword_1 (m68kpc_offset + opwords*2);
        }
        opcode = get_iword_1 (m68kpc_offset);
        m68kpc_offset += 2;
        if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
            opcode = 0x4AFC;
        }
        dp = table68k + opcode;
        for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
            ;
        strcpy (instrname, lookup->name);
        ccpt = strstr (instrname, "cc");
        if (ccpt != 0) {
            strncpy (ccpt, ccnames[dp->cc], 2);
        }
        if (dp->suse) {
            newpc = m68k_getpc () + m68kpc_offset;
            newpc += ShowEA (dp->sreg, (amodes)dp->smode, (wordsizes)dp->size, buffer);
            strcpy(buffer,"");
        }
        if (dp->duse) {
            newpc = m68k_getpc () + m68kpc_offset;
            newpc += ShowEA (dp->dreg, (amodes)dp->dmode, (wordsizes)dp->size, buffer);
            strcpy(buffer,"");
        }
    } else {
setjmpagain:
        int prb = setjmp(excep_env);
        if (prb != 0) {
		fprintf (f, " unknown address\n");
                goto setjmpagain;
        }
	while (cnt-- > 0) {
		char instrname[20],*ccpt;
		int opwords;
		uae_u32 opcode;
		struct mnemolookup *lookup;
		struct instr *dp;
		fprintf (f, "%08lx: ", m68k_getpc () + m68kpc_offset);
		for (opwords = 0; opwords < 5; opwords++) {
		    fprintf (f, "%04x ", get_iword_1 (m68kpc_offset + opwords*2));
		}
		opcode = get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
			opcode = 0x4AFC;
		}
		dp = table68k + opcode;
		for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
		    ;
		strcpy (instrname, lookup->name);
		ccpt = strstr (instrname, "cc");
		if (ccpt != 0) {
		    strncpy (ccpt, ccnames[dp->cc], 2);
		}
		fprintf (f, "%s", instrname);
		switch (dp->size){
		 case sz_byte: fprintf (f, ".B "); break;
		 case sz_word: fprintf (f, ".W "); break;
		 case sz_long: fprintf (f, ".L "); break;
		 default: fprintf (f, "   "); break;
		}

		if (dp->suse) {
		    newpc = m68k_getpc () + m68kpc_offset;
		    newpc += ShowEA (dp->sreg, (amodes)dp->smode, (wordsizes)dp->size, buffer);
		    fprintf(f, "%s", buffer);
		    strcpy(buffer,"");
		}
		if (dp->suse && dp->duse)
		    fprintf (f, ",");
		if (dp->duse) {
		    newpc = m68k_getpc () + m68kpc_offset;
		    newpc += ShowEA (dp->dreg, (amodes)dp->dmode, (wordsizes)dp->size, buffer);
		    fprintf(f, "%s", buffer);
		    strcpy(buffer,"");
		}
		if (ccpt != 0) {
		    if (cctrue(dp->cc))
			fprintf (f, " == %08lx (TRUE)", (unsigned long)newpc);
		    else
			fprintf (f, " == %08lx (FALSE)", (unsigned long)newpc);
		} else if ((opcode & 0xff00) == 0x6100) /* BSR */
		    fprintf (f, " == %08lx", (unsigned long)newpc);
		fprintf (f, "\n");
	    }
    }
setjmpagainx:
    if (nextpc)
	*nextpc = m68k_getpc () + m68kpc_offset;
    free(buffer);
    excep_env = excep_env_old;
}

#ifdef FULL_HISTORY
void showDisasm(uaecptr addr) {
	char *buffer = (char *)malloc(80 * sizeof(char));
	strcpy(buffer, "");
	char *sbuffer[7];
	for (int i = 0; i < 7; i++) {
		sbuffer[i] = (char *)malloc(80 * sizeof(char));
		strcpy(sbuffer[i], "");
	}
	char *buff[5];
	for (int i = 0; i < 5; i++) {
		buff[i] = (char *)malloc(80 * sizeof(char));
		strcpy(buff[i],"");
	}
	jmp_buf excep_env_old;
	excep_env_old = excep_env;
	uaecptr newpc = 0;
	m68kpc_offset = addr - m68k_getpc ();
	int prb = setjmp(excep_env);
	if (prb != 0) {
		bug("%s%s%s%s%s%s%s%s%s%s%s%s unknown address", sbuffer[0], buff[0],  buff[1],  buff[2], buff[3], buff[4], sbuffer[1], sbuffer[2], sbuffer[3], sbuffer[4], sbuffer[5], sbuffer[6]);
		free(buffer);
		for (int i = 0; i < 7; i++) free(sbuffer[i]);
		for (int i = 0; i < 5; i++) free(buff[i]);
		return;
	}
	char instrname[20],*ccpt;
	int opwords;
	uae_u32 opcode;
	struct mnemolookup *lookup;
	struct instr *dp;
	sprintf(sbuffer[0], "%08lx: ", m68k_getpc () + m68kpc_offset);
	for (opwords = 0; opwords < 5; opwords++) {
		sprintf (buff[opwords], "%04x ", get_iword_1 (m68kpc_offset + opwords*2));
	}
	opcode = get_iword_1 (m68kpc_offset);
	m68kpc_offset += 2;
	if (cpufunctbl[cft_map (opcode)] == op_illg_1) {
		opcode = 0x4AFC;
	}
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
		    ;
	strcpy (instrname, lookup->name);
	ccpt = strstr (instrname, "cc");
	if (ccpt != 0) {
		strncpy (ccpt, ccnames[dp->cc], 2);
	}
	sprintf (sbuffer[1], "%s", instrname);
	switch (dp->size){
		 case sz_byte: sprintf (sbuffer[2], ".B "); break;
		 case sz_word: sprintf (sbuffer[2], ".W "); break;
		 case sz_long: sprintf (sbuffer[2], ".L "); break;
		 default: sprintf (sbuffer[2], "   "); break;
	}

	if (dp->suse) {
		newpc = m68k_getpc () + m68kpc_offset;
		newpc += ShowEA (dp->sreg, (amodes)dp->smode, (wordsizes)dp->size, buffer);
		sprintf(sbuffer[3], "%s", buffer);
		strcpy(buffer,"");
	}
	if (dp->suse && dp->duse) sprintf (sbuffer[4], ",");
	if (dp->duse) {
		newpc = m68k_getpc () + m68kpc_offset;
		newpc += ShowEA (dp->dreg, (amodes)dp->dmode, (wordsizes)dp->size, buffer);
		sprintf(sbuffer[5], "%s", buffer);
		strcpy(buffer,"");
	}
	if (ccpt != 0) {
		if (cctrue(dp->cc)) sprintf (sbuffer[6], " == %08lx (TRUE)", (unsigned long)newpc);
			else sprintf (sbuffer[6], " == %08lx (FALSE)", (unsigned long)newpc);
	} else if ((opcode & 0xff00) == 0x6100) /* BSR */
		sprintf (sbuffer[6], " == %08lx", (unsigned long)newpc);
	
	bug("%s%s%s%s%s%s%s%s%s%s%s%s", sbuffer[0], buff[0], buff[1], buff[2], buff[3],  buff[4], sbuffer[1], sbuffer[2], sbuffer[3], sbuffer[4], sbuffer[5], sbuffer[6]);
	free(buffer);
	for (int i = 0; i < 7; i++) free(sbuffer[i]);
	for (int i = 0; i < 5; i++) free(buff[i]);
	excep_env = excep_env_old;
}
#endif
#endif

void m68k_dumpstate (uaecptr *nextpc)
{
    int i;
    for (i = 0; i < 8; i++){
	printf ("D%d: %08lx ", i, (unsigned long)m68k_dreg(regs, i));
	if ((i & 3) == 3) printf ("\n");
    }
    for (i = 0; i < 8; i++){
	printf ("A%d: %08lx ", i, (unsigned long)m68k_areg(regs, i));
	if ((i & 3) == 3) printf ("\n");
    }
    if (regs.s == 0) regs.usp = m68k_areg(regs, 7);
    if (regs.s && regs.m) regs.msp = m68k_areg(regs, 7);
    if (regs.s && regs.m == 0) regs.isp = m68k_areg(regs, 7);
    printf ("USP=%08lx ISP=%08lx MSP=%08lx VBR=%08lx\n",
	    (unsigned long)regs.usp, (unsigned long)regs.isp,
	    (unsigned long)regs.msp, (unsigned long)regs.vbr);
    printf ("T=%d%d S=%d M=%d X=%d N=%d Z=%d V=%d C=%d IMASK=%d TCE=%d TCP=%d\n",
	    regs.t1, regs.t0, regs.s, regs.m,
	    GET_XFLG, GET_NFLG, GET_ZFLG, GET_VFLG, GET_CFLG, regs.intmask,
	    regs.mmu_enabled, regs.mmu_pagesize);
    printf ("CACR=%08lx CAAR=%08lx  URP=%08lx  SRP=%08lx\n",
            (unsigned long)regs.cacr,
	    (unsigned long)regs.caar,
	    (unsigned long)regs.urp,
	    (unsigned long)regs.srp);
    printf ("DTT0=%08lx DTT1=%08lx ITT0=%08lx ITT1=%08lx\n",
            (unsigned long)regs.dtt0,
	    (unsigned long)regs.dtt1,
	    (unsigned long)regs.itt0,
	    (unsigned long)regs.itt1);
    for (i = 0; i < 8; i++){
	printf ("FP%d: %g ", i, (double)fpu.registers[i]);
	if ((i & 3) == 3) printf ("\n");
    }
#if 0
    printf ("N=%d Z=%d I=%d NAN=%d\n",
		(regs.fpsr & 0x8000000) != 0,
		(regs.fpsr & 0x4000000) != 0,
		(regs.fpsr & 0x2000000) != 0,
		(regs.fpsr & 0x1000000) != 0);
#endif
    m68k_disasm(m68k_getpc (), nextpc, 1);
    if (nextpc)
	printf ("next PC: %08lx\n", (unsigned long)*nextpc);
}
