/* MJ 2001 */

#ifndef NDEBUG_H
#define NDEBUG_H

#include "sysdeps.h"
#include "identify.h"

#ifndef CONFGUI
#include "memory.h"
#include "cpu_emulation.h"
#else
#undef NEWDEBUG
#endif

#ifdef NEWDEBUG

class ndebug {
  static unsigned int rowlen;
  static const unsigned int dbsize = 1000;
  static unsigned int warnlen;
  static char **dbbuffer;
  static unsigned int dbstart;
  static unsigned int dbend;
  static unsigned int dbfull;
  static unsigned int aktualrow;
  static unsigned int tp;
  static uaecptr skipaddr;
  static char old_debug_cmd[80];
 
  static void reset_aktualrow();
  static void set_aktualrow(signed int);

  static unsigned int get_len(); // { return 25; }
  static unsigned int get_warnlen() {
	return warnlen = get_len() - 12;
  }

  static unsigned int get_rowlen() { return rowlen; }
  static void warn_print(FILE *);
  static void m68k_print(FILE *);
  static void instr_print(FILE *);
  static void show(FILE *);
  static void showHelp(FILE *);
  static void set_Ax(char **);
  static void set_Dx(char **);
  static void set_Px(char **);
  static void set_Sx(char **);
  static void saveintofile(FILE *, char **);
  static void loadintomemory(FILE *, char **);
  static void convertNo(char **);
  static uae_u32 readhex(char, char **);
  static uae_u32 readoct(char, char **);
  static uae_u32 readdec(char, char **);
  static uae_u32 readbin(char, char **);
  static char *dectobin(uae_u32);

  static void pressenkey(FILE *f) {
    char input[80];
    fprintf(f, "Press ENTER\n");
    fflush(f);
    if (fgets(input, 80, stdin) == NULL) {
        fprintf(stderr, "Internal error!\n");
        return;
    }
  }
  
  static char next_char(char **c) {
    ignore_ws (c);
    return *(*c)++;
  }

  static void ignore_ws(char **c);

  static int more_params(char **c) {
    ignore_ws (c);
    return (**c) != 0;
  }
  static uae_u32 readhex(char **c) {
    uae_u32 val = 0;
    char nc;

    ignore_ws (c);

    while (isxdigit(nc = **c)) {
	(*c)++;
	val *= 16;
	nc = toupper(nc);
	if (isdigit(nc)) {
	    val += nc - '0';
	} else {
	    val += nc - 'A' + 10;
	}
    }
    return val;
  }

  static void dump_traps(FILE *f) {
    for (int i = 0; trap_labels[i].name; i++) {
      fprintf(f, "$%02x: %s\t $%08x\n", trap_labels[i].adr,
        trap_labels[i].name, ReadAtariInt32(trap_labels[i].adr));
    }
  }

  static void dump_ints(FILE *f) {
    for (int i = 0; int_labels[i].name; i++) {
      fprintf(f, "$%02x: %s\t $%08x\n", int_labels[i].adr,
        int_labels[i].name, ReadAtariInt32(int_labels[i].adr));
    }
  }

  static void dumpmem(FILE *, uaecptr, uaecptr *, unsigned int);
  static void writeintomem(FILE *, char **);
  static void backtrace(FILE *, unsigned int);
  static void log2phys(FILE *, uaecptr);

  static void showTypes();
  static int canon(FILE *, bool, uaecptr, uaecptr &, uaecptr &);
  static int icanon(FILE *, bool, uaecptr, uaecptr &, uaecptr &);
  static int dm(FILE *, bool, uaecptr, uaecptr &, uaecptr &);
 
#endif

#ifdef NEWDEBUG
public:
  static
#endif
  int dbprintf(char *, ...);

#ifdef NEWDEBUG
  static bool do_skip;
  static void run();
  static void init();
  static void nexit();
#ifdef FULL_HISTORY
  static void showHistory(unsigned int);
#endif
};
#endif

#endif
