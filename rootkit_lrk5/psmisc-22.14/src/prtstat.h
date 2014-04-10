
typedef unsigned char opt_type;
#define OPT_RAW 1

struct proc_info
{
  char *comm;
  char state;
  int ppid, pgrp, session, tty_nr, tp_gid,
	  exit_signal, processor;
  unsigned int flags, rt_priority, policy;
  unsigned long minflt, cminflt, majflt, cmajflt,
				utime, stime, vsize, rsslim,
				startcode, endcode, startstack,
				kstesp, ksteip,
				wchan, nswap, cnswap, guest_time;
  long cutime, cstime, priority, nice, num_threads,
	   itrealvalue, rss, cguest_time;
  unsigned long long starttime, blkio;
};

