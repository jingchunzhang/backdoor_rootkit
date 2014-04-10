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

    if( argc != 3 )
    {
        printf( "\nusage: client <hostname> [port]\n" );
        return( 1 );
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
