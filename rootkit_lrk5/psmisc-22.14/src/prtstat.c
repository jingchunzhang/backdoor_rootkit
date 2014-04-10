/*
 * prtstat.c - Print a processes stat file
 *
 * Copyright (C) 2009 Craig Small
 * Based upon a shell script pstat by martin f. krafft <madduck@madduck.net>
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
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "i18n.h"
#include "prtstat.h"

#define NORETURN __attribute__((__noreturn__))

static long sc_clk_tck;

static void usage(const char *errormsg) NORETURN;

static void usage(const char *errormsg)
{
  if (errormsg != NULL)
	fprintf(stderr, "%s\n", errormsg);
  fprintf(stderr,
	  _
	  ("Usage: prtstat [options] PID ...\n"
	   "       prtstat -V\n"
	   "Print information about a process\n"
	   "    -r,--raw       Raw display of information\n"
	   "    -V,--version   Display version information and exit\n"
	  ));
  exit(1);
}

static void print_version(void)
{
  fprintf(stderr, _("prtstat (PSmisc) %s\n"), VERSION);
  fprintf(stderr, _( "Copyright (C) 2009 Craig Small\n\n"));
  fprintf(stderr, _(
		"PSmisc comes with ABSOLUTELY NO WARRANTY.\n"
		"This is free software, and you are welcome to redistribute it under\n"
		"the terms of the GNU General Public License.\n"
		"For more information about these matters, see the files named COPYING.\n"));
}

static char *print_state(const char state)
{
  switch(state) {
	case 'R':
	  return _("running");
	case 'S':
	  return _("sleeping");
	case 'D':
	  return _("disk sleep");
	case 'Z':
	  return _("zombie");
	case 'T':
	  return _("traced");
	case 'W':
	  return _("paging");
  }
  return _("unknown");
}

#define RAW_STAT(afmt,aname, aval, bfmt, bname, bval) \
  printf("%12.11s: %-15"afmt"\t%22.21s: %"bfmt"\n",(aname),(aval),(bname),(bval))

static double convert_time(const unsigned long ticks)
{
  assert(sc_clk_tck > 0);
  return (float)ticks / (float)sc_clk_tck;
}

static void convert_bytes(char *buf, unsigned long bytes)
{
  if (bytes > (10000000))
	sprintf(buf, "%lu MB",bytes/1000000L);
  else if (bytes > (10000))
	sprintf(buf, "%lu kB", bytes/1000L);
  else
	sprintf(buf, "%lu B", bytes);
}

/* comes from SCHED_* from linux/sched.h */
static char *convert_policy(const unsigned int policy)
{
  static char *policy_names[] = { "normal", "fifo","rr", "batch", "iso", "idle" };
  if (policy < 6)
	return policy_names[policy];
  return "unknown";
}

/* minor is bits 31-20 and 7-0, major is 15-8 */
static char *convert_tty(int tty_nr)
{
  static char buf[20];
  sprintf(buf, "%d:%d",(tty_nr & 0xff00)>>8,(tty_nr & 0xff)|((tty_nr & 0xfff00000)>>20));
  return buf;
}


static void print_raw_stat(const int pid,struct proc_info *pr)
{
  RAW_STAT("d","pid",pid,"s","comm",pr->comm);
  RAW_STAT("c","state",pr->state, "d","ppid",pr->ppid);
  RAW_STAT("d","pgrp",pr->pgrp, "d","session",pr->session);
  RAW_STAT("d","tty_nr",pr->tty_nr, "d","tpgid",pr->tp_gid);
  RAW_STAT("x","flags",pr->flags, "lu","minflt",pr->minflt);
  RAW_STAT("lu","cminflt",pr->cminflt, "lu","majflt",pr->majflt);
  RAW_STAT("lu","cmajflt",pr->cmajflt, "lu","utime",pr->utime);
  RAW_STAT("lu","stime",pr->stime, "ld","cutime",pr->cutime);
  RAW_STAT("ld","cstime",pr->cstime, "ld","priority",pr->priority);
  RAW_STAT("ld","nice",pr->nice, "ld","num_threads",pr->num_threads);
  RAW_STAT("ld","itrealvalue",pr->itrealvalue, "llu","starttime",pr->starttime);
  RAW_STAT("lu","vsize",pr->vsize, "ld","rss",pr->rss);
  RAW_STAT("lu","rsslim",pr->rsslim, "lu","startcode",pr->startcode);
  RAW_STAT("lu","endcode",pr->endcode, "lu","startstack",pr->startstack);
  RAW_STAT("lX","kstkesp",pr->kstesp, "lX","kstkeip",pr->ksteip);
  RAW_STAT("lu","wchan",pr->wchan, "lu","nswap",pr->nswap);
  RAW_STAT("lu","cnswap",pr->wchan, "d","exit_signal",pr->exit_signal);
  RAW_STAT("d","processor",pr->processor, "u","rt_priority",pr->rt_priority);
  RAW_STAT("u","policy",pr->policy, "llu","delayaccr_blkio_ticks",pr->blkio);
  RAW_STAT("lu","guest_time",pr->guest_time, "ld","cguest_time",pr->cguest_time);
}
static void print_formated_stat(const int pid,struct proc_info *pr)
{
  char buf_vsize[100];
  char buf_rss[100];
  char buf_rsslim[100];
  long page_size;

  page_size = sysconf(_SC_PAGESIZE);
  assert(page_size>1);

  printf(_(
		"Process: %-14s\t\tState: %c (%s)\n"
		"  CPU#:  %-3d\t\tTTY: %s\tThreads: %ld\n"),
	  pr->comm, pr->state, print_state(pr->state),
	  pr->processor, convert_tty(pr->tty_nr), pr->num_threads);
  printf(_(
		"Process, Group and Session IDs\n"
		"  Process ID: %d\t\t  Parent ID: %d\n"
		"    Group ID: %d\t\t Session ID: %d\n"
	    "  T Group ID: %d\n\n"),
	  pid, pr->ppid, pr->pgrp, pr->session, pr->tp_gid);
  printf(_(
		"Page Faults\n"
		"  This Process    (minor major): %8lu  %8lu\n"
		"  Child Processes (minor major): %8lu  %8lu\n"),
	  pr->minflt, pr->majflt, pr->cminflt, pr->cmajflt);
  printf(_(
		"CPU Times\n"
		"  This Process    (user system guest blkio): %6.2f %6.2f %6.2f %6.2f\n"
		"  Child processes (user system guest):       %6.2f %6.2f %6.2f\n"),
	  convert_time(pr->utime), convert_time(pr->stime), convert_time(pr->guest_time), convert_time(pr->blkio),
	  convert_time(pr->cutime), convert_time(pr->cstime), convert_time(pr->cguest_time));
  convert_bytes(buf_vsize, pr->vsize);
  convert_bytes(buf_rss, pr->rss*page_size);
  convert_bytes(buf_rsslim, pr->rsslim);
  printf(_(
		"Memory\n"
		"  Vsize:       %-10s\n"
		"  RSS:         %-10s \t\t RSS Limit: %s\n"
		"  Code Start:  %#-10lx\t\t Code Stop:  %#-10lx\n"
		"  Stack Start: %#-10lx\n"
		"  Stack Pointer (ESP): %#10lx\t Inst Pointer (EIP): %#10lx\n"),
	  buf_vsize, buf_rss, buf_rsslim,
	  pr->startcode, pr->endcode, 
	  pr->startstack, pr->kstesp, pr->ksteip);
  printf(_(
		"Scheduling\n"
		"  Policy: %s\n"
		"  Nice:   %ld \t\t RT Priority: %ld %s\n"),
	  convert_policy(pr->policy),
	  pr->nice, (pr->priority>0?pr->priority-20:1-pr->priority),
	  (pr->priority>0?"(non RT)":""));




}
static void print_stat(const int pid, const opt_type options)
{
  char *pathname;
  char buf[BUFSIZ];
  char *bptr;
  FILE *fp;

  struct proc_info *pr;
  pr = malloc(sizeof(struct proc_info));

  if ( (asprintf(&pathname, "/proc/%d/stat",(int)pid)) < 0) {
	perror(_("asprintf in print_stat failed.\n"));
	exit(1);
  }
  if ( (fp = fopen(pathname,"r")) == NULL) {
	if (errno == ENOENT) 
	  fprintf(stderr, _("Process with pid %d does not exist.\n"), pid);
	else
	  fprintf(stderr, _("Unable to open stat file for pid %d (%s)\n"),(int)pid,strerror(errno));
	free(pathname);
	return;
  }
  free(pathname);

  fgets(buf,BUFSIZ,fp);
  bptr = strchr(buf, '(');
  if (bptr == NULL) return;
  bptr++;
  sscanf(bptr,
	  "%a[^)]) "
	  "%c "
	  "%d %d %d %d %d %d"
	  "%lu %lu %lu %lu " /*flts*/
	  "%lu %lu %lu %lu " /*times */
	  "%ld %ld %ld %ld " /* nice, priority, threads, itreal*/
	  "%llu " /*startime*/
	  "%lu %ld %lu " /* vsize, rss, rslim */
	  "%lu %lu %lu " /* startcode endcode startstack */
	  "%lu %lu " /* stack and ip */
	  "%*s %*s %*s %*s " /* signals - ignore as they are obsolete */
	  "%lu %lu %lu " /* wchan nswap cnswap */
      "%d %d %u"
	  "%u %llu " /* policy blkio */
	  "%lu %lu ", /* guest time cguest time */
	   &pr->comm,
	   &pr->state,
	   &pr->ppid, &pr->pgrp, &pr->session, &pr->tty_nr, &pr->tp_gid, &pr->flags,
	   &pr->minflt, &pr->cminflt, &pr->majflt, &pr->cmajflt,
	   &pr->utime, &pr->stime, &pr->cutime, &pr->cstime,
	   &pr->priority, &pr->nice, &pr->num_threads, &pr->itrealvalue,
	   &pr->starttime,
	   &pr->vsize, &pr->rss, &pr->rsslim,
	   &pr->startcode, &pr->endcode, &pr->startstack,
	   &pr->kstesp, &pr->ksteip,
	   &pr->wchan, &pr->nswap, &pr->cnswap,
	   &pr->exit_signal, &pr->processor, &pr->rt_priority,
	   &pr->policy, &pr->blkio, 
	   &pr->guest_time, &pr->cguest_time
		 );
  if (options & OPT_RAW) {
	print_raw_stat(pid, pr);
	return;
  }
  print_formated_stat(pid, pr);


}

int main(int argc, char *argv[])
{
  int optc;
  struct stat st;
  int pptr;
  int pid;
  opt_type opt_flags = 0;

  struct option options[] = {
	{"raw"		,0, NULL, 'r' },
	{"version", 0, NULL, 'V'},
	{ 0, 0, 0, 0}
  };

#ifdef ENABLE_NLS
  /* Set up the i18n */
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
#endif

  while ((optc = getopt_long(argc, argv, "rV", options, NULL)) != -1) {
	switch(optc) {
	  case 'r':
		opt_flags |= OPT_RAW;
		break;
	  case 'V':
		print_version();
		return 0;
	  case '?':
		usage(_("Invalid option"));
		break;
	}
  } /* while */
  if (argc <= optind)
	usage(_("You must provide at least one PID."));

  if (stat("/proc/self/stat", &st) == -1)
  {
	fprintf(stderr, _("/proc is not mounted, cannot stat /proc/self/stat.\n"));
	exit(1);
  }
  sc_clk_tck = sysconf(_SC_CLK_TCK);
  for(pptr = optind; pptr < argc; pptr++)
  {
	pid = atoi(argv[pptr]);
	print_stat(pid, opt_flags);
  }

  return 0;
}



