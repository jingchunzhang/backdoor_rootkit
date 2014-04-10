/*
 * peekfd.c - Intercept file descriptor read and writes
 *
 * Copyright (C) 2007 Trent Waddington <trent.waddington@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <asm/ptrace.h>
#include <byteswap.h>
#include <endian.h>
#include <sys/user.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>

#include "i18n.h"

#ifdef I386
	#define REG_ORIG_ACCUM orig_eax
	#define REG_ACCUM eax
	#define REG_PARAM1 ebx
	#define REG_PARAM2 ecx
	#define REG_PARAM3 edx
#elif X86_64
	#define REG_ORIG_ACCUM orig_rax
	#define REG_ACCUM rax
	#define REG_PARAM1 rdi
	#define REG_PARAM2 rsi
	#define REG_PARAM3 rdx
#elif PPC
	#define REG_ORIG_ACCUM gpr[0]
	#define REG_ACCUM gpr[3]
	#define REG_PARAM1 orig_gpr3
	#define REG_PARAM2 gpr[4]
	#define REG_PARAM3 gpr[5]
#ifndef PT_ORIG_R3
	#define PT_ORIG_R3 34
#endif
#elif defined(ARM)
#ifndef __ARM_EABI__
#error arm oabi not supported
#endif
	#define REG_ORIG_ACCUM ARM_r7
	#define REG_ACCUM ARM_r0
	#define REG_PARAM1 ARM_ORIG_r0
	#define REG_PARAM2 ARM_r1
	#define REG_PARAM3 ARM_r2
#elif defined(MIPS)
#ifndef MIPSEL
#error only little endian supported
#endif
	#define REG_ORIG_ACCUM regs[3]
	#define REG_ACCUM regs[2]
	#define REG_PARAM1 regs[4]
	#define REG_PARAM2 regs[5]
	#define REG_PARAM3 regs[6]
#endif

#define MAX_ATTACHED_PIDS 1024
int num_attached_pids = 0;
pid_t attached_pids[MAX_ATTACHED_PIDS];

void detach(void) {
	int i;
	for (i = 0; i < num_attached_pids; i++)	
		ptrace(PTRACE_DETACH, attached_pids[i], 0, 0);
}

void attach(pid_t pid) {
	if (num_attached_pids >= MAX_ATTACHED_PIDS)
		return;
	attached_pids[num_attached_pids] = pid;
	if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
		fprintf(stderr, _("Error attaching to pid %i\n"), pid);
		return;
	}
	num_attached_pids++;
}

void print_version()
{
  fprintf(stderr, _("peekfd (PSmisc) %s\n"), VERSION);
  fprintf(stderr, _(
    "Copyright (C) 2007 Trent Waddington\n\n"));
  fprintf(stderr, _(
    "PSmisc comes with ABSOLUTELY NO WARRANTY.\n"
    "This is free software, and you are welcome to redistribute it under\n"
    "the terms of the GNU General Public License.\n"
    "For more information about these matters, see the files named COPYING.\n"));
}

void usage() {
	fprintf(stderr, _(
      "Usage: peekfd [-8] [-n] [-c] [-d] [-V] [-h] <pid> [<fd> ..]\n"
	  "    -8 output 8 bit clean streams.\n"
	  "    -n don't display read/write from fd headers.\n"
	  "    -c peek at any new child processes too.\n"
	  "    -d remove duplicate read/writes from the output.\n"
	  "    -V prints version info.\n"
	  "    -h prints this help.\n"
	  "\n"
	  "  Press CTRL-C to end output.\n"));
}

int bufdiff(pid_t pid, unsigned char *lastbuf, unsigned int addr, unsigned int len) {
	int i;
	for (i = 0; i < len; i++)
		if (lastbuf[i] != (ptrace(PTRACE_PEEKTEXT, pid, addr + i, 0) & 0xff))
			return 1;
	return 0;
}

int main(int argc, char **argv)
{
	int eight_bit_clean = 0;
	int no_headers = 0;
	int follow_forks = 0;
	int remove_duplicates = 0;
	int optc;
    int target_pid = 0;
    int numfds = 0;
    int *fds = NULL;
    int i;

    struct option options[] = {
      {"eight-bit-clean", 0, NULL, '8'},
      {"no-headers", 0, NULL, 'n'},
      {"follow", 0, NULL, 'c'},
      {"duplicates-removed", 0, NULL, 'd'},
      {"help", 0, NULL, 'h'},
      {"version", 0, NULL, 'V'},
    };

  /* Setup the i18n */
#ifdef ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
#endif

	if (argc < 2) {
		usage();
		return 1;
	}

	while ((optc = getopt_long(argc, argv, "8ncdhV",options, NULL)) != -1) {
		switch(optc) {
			case '8':
				eight_bit_clean = 1;
				break;
			case 'n':
				no_headers = 1;
				break;
			case 'c':
				follow_forks = 1;
				break;
			case 'd':
				remove_duplicates = 1;
				break;
			case 'V':
				print_version();
				return 1;
			case 'h':
			case '?':
				usage();
				return 1;
		}
	}
    /* First arg off the options is the PID to see */
    if (optind >= argc) {
      usage();
      return -1;
    }
    target_pid = atoi(argv[optind++]);

    if (optind < argc) {
      numfds = argc - optind;
      fds = malloc(sizeof(int) * numfds);
	  for (i = 0; i < numfds; i++)
		fds[i] = atoi(argv[optind + i]);
    }

	attach(target_pid);
	if (num_attached_pids == 0)
		return 1;

	atexit(detach);

	ptrace(PTRACE_SYSCALL, attached_pids[0], 0, 0);

	/*int count = 0;*/
	int lastfd = numfds > 0 ? fds[0] : 0;
	int lastdir = 3;
	unsigned char *lastbuf = NULL;
	int last_buf_size=-1;

	for(;;) {
		int status;
		pid_t pid = wait(&status);
		if (WIFSTOPPED(status)) {
#ifdef PPC
			struct pt_regs regs;
			regs.gpr[0] = ptrace(PTRACE_PEEKUSER, pid, __WORDSIZE/8 * PT_R0, 0);
			regs.gpr[3] = ptrace(PTRACE_PEEKUSER, pid, __WORDSIZE/8 * PT_R3, 0);
			regs.gpr[4] = ptrace(PTRACE_PEEKUSER, pid, __WORDSIZE/8 * PT_R4, 0);
			regs.gpr[5] = ptrace(PTRACE_PEEKUSER, pid, __WORDSIZE/8 * PT_R5, 0);
			regs.orig_gpr3 = ptrace(PTRACE_PEEKUSER, pid, __WORDSIZE/8 * PT_ORIG_R3, 0);
#elif defined(ARM)
			struct pt_regs regs;
			ptrace(PTRACE_GETREGS, pid, 0, &regs);
#elif defined(MIPS)
			struct pt_regs regs;
			long pc = ptrace(PTRACE_PEEKUSER, pid, 64, 0);
			regs.regs[2] = ptrace(PTRACE_PEEKUSER,pid,2,0);
			regs.regs[3] = ptrace(PTRACE_PEEKTEXT, pid, pc - 8, 0) & 0xffff;
			regs.regs[4] = ptrace(PTRACE_PEEKUSER,pid,4,0);
			regs.regs[5] = ptrace(PTRACE_PEEKUSER,pid,5,0);
			regs.regs[6] = ptrace(PTRACE_PEEKUSER,pid,6,0);
#else
			struct user_regs_struct regs;
			ptrace(PTRACE_GETREGS, pid, 0, &regs);
#endif		
			/*unsigned int b = ptrace(PTRACE_PEEKTEXT, pid, regs.eip, 0);*/
			if (follow_forks && (regs.REG_ORIG_ACCUM == SYS_fork || regs.REG_ORIG_ACCUM == SYS_clone)) {
				if (regs.REG_ACCUM > 0)
					attach(regs.REG_ACCUM);					
			}
			if ((regs.REG_ORIG_ACCUM == SYS_read || regs.REG_ORIG_ACCUM == SYS_write) && (regs.REG_PARAM3 == regs.REG_ACCUM)) {
				for (i = 0; i < numfds; i++)
					if (fds[i] == regs.REG_PARAM1)
						break;
				if (i != numfds || numfds == 0) {
					if (regs.REG_PARAM1 != lastfd || regs.REG_ORIG_ACCUM != lastdir) {
						lastfd = regs.REG_PARAM1;
						lastdir = regs.REG_ORIG_ACCUM;
						if (!no_headers)
							printf("\n%sing fd %i:\n", regs.REG_ORIG_ACCUM == SYS_read ? "read" : "writ", lastfd);
					}
					if (!remove_duplicates || lastbuf == NULL
							||  last_buf_size != regs.REG_PARAM3 || 
							bufdiff(pid, lastbuf, regs.REG_PARAM2, regs.REG_PARAM3)) {

						if (remove_duplicates) {
							if (lastbuf)
								free(lastbuf);
							lastbuf = malloc(regs.REG_PARAM3);
							last_buf_size = regs.REG_PARAM3;
						}

						for (i = 0; i < regs.REG_PARAM3; i++) {
#ifdef _BIG_ENDIAN
#if __WORDSIZE == 64
							unsigned int a = bswap_64(ptrace(PTRACE_PEEKTEXT, pid, regs.REG_PARAM2 + i, 0));
#else
							unsigned int a = bswap_32(ptrace(PTRACE_PEEKTEXT, pid, regs.REG_PARAM2 + i, 0));
#endif
#else
							unsigned int a = ptrace(PTRACE_PEEKTEXT, pid, regs.REG_PARAM2 + i, 0);
#endif
							if (remove_duplicates)
								lastbuf[i] = a & 0xff;

							if (eight_bit_clean)
								putchar(a & 0xff);
							else {
								if (isprint(a & 0xff) || (a & 0xff) == '\n')
									printf("%c", a & 0xff);
								else if ((a & 0xff) == 0x0d)
									printf("\n");
								else if ((a & 0xff) == 0x7f)
									printf("\b");
								else if (a & 0xff)
									printf(" [%02x] ", a & 0xff);
							}
						}
					}
					fflush(stdout);
				}
			}

			ptrace(PTRACE_SYSCALL, pid, 0, 0);
		}
	}

	return 0;
}
