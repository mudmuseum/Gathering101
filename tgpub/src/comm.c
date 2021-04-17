/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif

/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>	/* for RLIMIT_NOFILE */
#endif

#if defined(apollo)
#undef __attribute
#endif

/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
#endif

#include <sys/socket.h>
#include <sys/types.h>

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
// int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
// int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined(linux)
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, socklen_t *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, socklen_t *namelen ) );
// int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

// int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
// int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
// int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well.
 */
#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, const struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
// int	gettimeofday	args( ( struct timeval *tp, void *) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, void *, size_t nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, const char *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, const void *, size_t nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
// int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif



/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
char		    crypt_pwd[MAX_INPUT_LENGTH];
time_t		    current_time;	/* Time of this pulse		*/

/* Colour scale char list - Calamar */

char *scale[SCALE_COLS] = {
	L_RED,
	L_BLUE,
	L_GREEN,
	YELLOW
};

/*
 * OS-dependent local functions.
 */

#if defined(unix)
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
bool	write_to_descriptor	args( ( int desc, char *txt, int length ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_kickoff		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void	bust_a_prompt		args( ( DESCRIPTOR_DATA *d ) );
void 	check_stats		args( ( CHAR_DATA *ch ) );
void	upgrade_demon		args( ( CHAR_DATA *ch ) );
void 	motd			args( ( CHAR_DATA *ch) );
int	nice			( int );

int main( int argc, char **argv )
{
    struct timeval now_time;
    int port;

#if defined(unix)
    int control;
#endif

    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

#ifdef RLIMIT_NOFILE
#ifndef min
# define min(a,b)     (((a) < (b)) ? (a) : (b))
#endif
        { 
        struct  rlimit rlp;
        (void)getrlimit(RLIMIT_NOFILE, &rlp);
        rlp.rlim_cur=min(rlp.rlim_max,FD_SETSIZE);
        (void)setrlimit(RLIMIT_NOFILE, &rlp);
	}
#endif

    /*
     * Init time and encryption.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );
    strcpy( crypt_pwd, "Don't bother." );

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
    port = 9002;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

    int someval = nice(-10);
    if (someval > 1) ;

    /*
     * Run the game.
     */

#if defined(unix)
    control = init_socket( port );
    boot_db( );
    sprintf( log_buf, "God Wars is ready to rock on port %d.", port );
    log_string( log_buf, 0 );
    game_loop_unix( control );
    close( control );
#endif

    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game.", 0 );
    exit( 0 );
    return 0;
}



#if defined(unix)
int init_socket( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x;
    int fd;

    if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
	perror( "Init_socket: socket" );
	exit( 1 );
    }

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof(x) ) < 0 )
    {
	perror( "Init_socket: SO_REUSEADDR" );
	close( fd );
	exit( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
	(char *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
	    close( fd );
	    exit( 1 );
	}
    }
#endif

    sa		    = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port	    = htons( port );

    if ( bind( fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
	perror( "Init_socket: bind" );
	close( fd );
	exit( 1 );
    }

    if ( listen( fd, 3 ) < 0 )
    {
	perror( "Init_socket: listen" );
	close( fd );
	exit( 1 );
    }

    return fd;
}
#endif


#if defined(unix)

void excessive_cpu(int blx)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next;

	if ( !IS_NPC(vch) )
	{
	    send_to_char("Mud frozen: Autosave and quit.  The mud will reboot in 2 seconds.\n\r",vch);
	    interpret( vch, "quit" );
	}
    }
    exit(1);
}

void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		    interpret( d->character, d->incomm );
		else
		    nanny( d, d->incomm );

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix)
void new_descriptor( int control )
{
    static DESCRIPTOR_DATA d_zero;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    BAN_DATA *pban;
    struct sockaddr_in sock;
/*    struct hostent *from; */
    int desc;
    unsigned int size;

    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
	perror( "New_descriptor: accept" );
	return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
	perror( "New_descriptor: fcntl: FNDELAY" );
	return;
    }

    /*
     * Cons a new descriptor.
     */
    if ( descriptor_free == NULL )
    {
	dnew		= alloc_perm( sizeof(*dnew) );
    }
    else
    {
	dnew		= descriptor_free;
	descriptor_free	= descriptor_free->next;
    }

    *dnew		= d_zero;
    dnew->descriptor	= desc;
    dnew->connected	= CON_GET_NAME;
    dnew->outsize	= 2000;
    dnew->outbuf	= alloc_mem( dnew->outsize );

    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    {
	perror( "New_descriptor: getpeername" );
	dnew->host = str_dup( "(unknown)" );
    }
    else
    {
	/*
	 * Would be nice to use inet_ntoa here but it takes a struct arg,
	 * which ain't very compatible between gcc and system libraries.
	 */
	int addr;

	addr = ntohl( sock.sin_addr.s_addr );
	sprintf( buf, "%d.%d.%d.%d",
	    ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	    ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	    );
	sprintf( log_buf, "Sock.sinaddr:  %s", buf );
	log_string( log_buf,  dnew->descriptor);
/* Commented out to cut out gethostbyaddr lag. It seems this function can 
 * halt a process mid-execution if trying to get an invalid/masqueraded 
 * address.  Wizcommand gethost can be used to find this information on demand.  * - Puck
	from = gethostbyaddr( (char *) &sock.sin_addr,
	    sizeof(sock.sin_addr), AF_INET );
	dnew->host = str_dup( from ? from->h_name : buf ); */
	dnew->host = str_dup( buf );
    }
	
    /*
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
	if ( !str_suffix( pban->name, dnew->host ) )
	{
	    write_to_descriptor( desc,
		"Your site has been banned from this Mud.\n\r", 0 );
	    close( desc );
	    free_string( dnew->host );
	    free_string( dnew->host2 );
	    free_mem( dnew->outbuf, dnew->outsize );
	    dnew->next		= descriptor_free;
	    descriptor_free	= dnew;
	    return;
	}
    }

    /*
     * Init descriptor data.
     */
    dnew->next			= descriptor_list;
    descriptor_list		= dnew;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( dnew, help_greeting+1, 0 );
	else
	    write_to_buffer( dnew, help_greeting  , 0 );
    }

    return;
}
#endif



void close_socket( DESCRIPTOR_DATA *dclose )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    if ( dclose->character != NULL && dclose->connected == CON_PLAYING &&
	IS_NPC(dclose->character) ) do_return(dclose->character,"");
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }


    if ( ( ch = dclose->character ) != NULL )
    {
	sprintf( log_buf, "Closing link to %s.", ch->name );
	log_string( log_buf, dclose->descriptor );

 	/* If ch is writing note or playing, just lose link otherwise clear char */
	if (( dclose->connected == CON_PLAYING ) ||
 	   ((dclose->connected >= CON_NOTE_TO) && 
 	    (dclose->connected <= CON_NOTE_FINISH)))
	{
	    if (IS_NPC(ch) || ch->pcdata->obj_vnum == 0)
	    	act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    close( dclose->descriptor );
    free_string( dclose->host );
    free_string( dclose->host2 );

    /* RT socket leak fix */
    free_mem( dclose->outbuf, dclose->outsize );

    dclose->next	= descriptor_free;
    descriptor_free	= dclose;
    return;
}

/* For a better kickoff message :) KaVir */
void close_socket2( DESCRIPTOR_DATA *dclose, bool kickoff )
{
    CHAR_DATA *ch;

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by != NULL )
    {
	write_to_buffer( dclose->snoop_by,
	    "Your victim has left the game.\n\r", 0 );
    }

    if ( dclose->character != NULL && dclose->connected == CON_PLAYING &&
	IS_NPC(dclose->character) ) do_return(dclose->character,"");
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) != NULL )
    {
	if ( dclose->connected == CON_PLAYING )
	{
	    if (kickoff)
		act( "$n doubles over in agony and $s eyes roll up into $s head.", ch, NULL, NULL, TO_ROOM );
 	    save_char_obj( ch );
	    ch->desc = NULL;
	}
	else
	{
	    free_char( dclose->character );
	}
    }

    if ( d_next == dclose )
	d_next = d_next->next;   

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d != NULL )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }
    close( dclose->descriptor );
    free_string( dclose->host );
    free_string( dclose->host2 );
    dclose->next	= descriptor_free;
    descriptor_free	= dclose;
    return;
}



bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	if (d != NULL && d->character != NULL)
	    sprintf( log_buf, "%s input overflow!", d->character->lasthost );
	else
	    sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf, d->descriptor );

	write_to_descriptor( d->descriptor,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined(MSDOS) || defined(unix)
    for ( ; ; )
    {
	int nRead;

	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read.", d->descriptor );
	    return FALSE;
	}
	else if ( errno == EWOULDBLOCK )
	    break;
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d->descriptor, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii((int)d->inbuf[i]) && isprint((int)d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     *//*
    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 40 )
	    {
		if (d != NULL && d->character != NULL)
		    sprintf( log_buf, "%s input spamming!", d->character->lasthost );
		else
		    sprintf( log_buf, "%s input spamming!", d->host );
		log_string( log_buf, d->descriptor );
		write_to_descriptor( d->descriptor,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		strcpy( d->incomm, "quit" );
	    }
	}
    }*/

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && d->connected == CON_PLAYING )
    {
	CHAR_DATA *ch;
	CHAR_DATA *victim;

	ch = d->original ? d->original : d->character;
	if ( IS_SET(ch->act, PLR_BLANK) )
	    write_to_buffer( d, "\n\r", 2 );

	if (IS_SET(ch->act, PLR_PROMPT) && IS_EXTRA(ch, EXTRA_PROMPT))
	    bust_a_prompt( d );
	else if ( IS_SET(ch->act, PLR_PROMPT) )
	{
	    char buf[MAX_STRING_LENGTH];
	    char cond[MAX_INPUT_LENGTH];
	    char hit_str[MAX_INPUT_LENGTH];
	    char mana_str[MAX_INPUT_LENGTH];
	    char move_str[MAX_INPUT_LENGTH];
	    char exp_str[MAX_INPUT_LENGTH];

	    ch = d->character;
	    if (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH))
	    {
		sprintf(exp_str, "%d", ch->exp);
		COL_SCALE(exp_str, ch, ch->exp, 1000);
	        sprintf( buf, "<[%sX] [?H ?M ?V]> ",exp_str );
	    }
	    else if (ch->position == POS_FIGHTING)
	    {
	        victim = ch->fighting;
		if ((victim->hit*100/victim->max_hit) < 25)
		{
		    strcpy(cond, "Awful");
		    ADD_COLOUR(ch, cond, L_RED);
		}
		else if ((victim->hit*100/victim->max_hit) < 50)
		{
		    strcpy(cond, "Poor");
		    ADD_COLOUR(ch, cond, L_BLUE);
		}
		else if ((victim->hit*100/victim->max_hit) < 75)
		{
		    strcpy(cond, "Fair");
		    ADD_COLOUR(ch, cond, L_GREEN);
		}
		else if ((victim->hit*100/victim->max_hit) < 100)
		{
		    strcpy(cond, "Good");
		    ADD_COLOUR(ch, cond, YELLOW);
		}
		else if ((victim->hit*100/victim->max_hit) >= 100)
		{
		    strcpy(cond, "Perfect");
		    ADD_COLOUR(ch, cond, L_CYAN);
		}

		sprintf(hit_str, "%d", ch->hit);
		COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
		sprintf(mana_str, "%d", ch->mana);
		COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
		sprintf(move_str, "%d", ch->move);
		COL_SCALE(move_str, ch, ch->move, ch->max_move);

		sprintf( buf, "<[%s] [%sH %sM %sV]> ", cond, hit_str, mana_str, move_str );
	    }
	    else
	    {
		sprintf(hit_str, "%d", ch->hit);
		COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
		sprintf(mana_str, "%d", ch->mana);
		COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
		sprintf(move_str, "%d", ch->move);
		COL_SCALE(move_str, ch, ch->move, ch->max_move);
		sprintf(exp_str, "%d", ch->exp);
		COL_SCALE(exp_str, ch, ch->exp, 1000);

	        sprintf( buf, "<[%s] [%sH %sM %sV]> ",exp_str, hit_str, mana_str, move_str );
	    }
	    write_to_buffer( d, buf, 0 );
	}

	if ( IS_SET(ch->act, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d->descriptor, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}



/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    char tb[MAX_STRING_LENGTH], ccode;
    int i,j;
    bool ansi, grfx = FALSE;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen(txt);

    if ( length >= MAX_STRING_LENGTH )
    {
	bug( "Write_to_buffer: Way too big. Closing.", 0 );
	return;
    }

    if (d->character == NULL || d->connected != CON_PLAYING)
	ansi = FALSE;
    else ansi = (IS_SET(d->character->act, PLR_ANSI)) ? TRUE : FALSE;

    /*
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
	d->outbuf[0]    = '\n';
	d->outbuf[1]    = '\r';
	d->outtop       = 2;
    }

    /*
     * Expand d->outbuf for ansi info
     */

    j = 0;
    ccode = '3';

    tb[0] = '\0';
    for (i = 0; i < length ; i++)
    {
	if (txt[i] == '\n')
	{
/* added to fix stupid PC telnet incompatibility with the rest of the world */
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '7'; tb[j++] = 'm';
                    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
                    tb[j++] = '4'; tb[j++] = '0'; tb[j++] = 'm';

/* end. */
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = 'm';
	}
	if (txt[i] == '#' || txt[i] == '@') //&& txt[i+1] != 'I' && txt[i+1] != 'N')
	{
	    if (txt[i] == '#')
		ccode = '3';
	    else
		ccode = '4';
	    i++;

	    switch (txt[i])
	    {
		default : break;
		case '#': tb[j++] = '#'; break;
		case '@': tb[j++] = '@'; break;
		case '-': tb[j++] = '~'; break;
		case '+': tb[j++] = '%'; break;
		case 'I':
		case 'i': tb[j++] = 27; tb[j++] = '['; tb[j++] = '7';
			  tb[j++] = 'm'; break;
 /* Underline */       case 'u':
               case 'U': if (!ansi) continue;
                         tb[j++] = 27; tb[j++] = '['; tb[j++] = '4';
                         tb[j++] = 'm'; break;
 /* Flash */   case 'f':
               case 'F': if (!ansi) continue;
                         tb[j++] = 27; tb[j++] = '['; tb[j++] = '5';
			 tb[j++] = 'm'; break;
/* White */	case 'W': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '7'; tb[j++] = 'm';
		    break;
/* Grey */	case 'w': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '0'; tb[j++] = 'm';
		    break;
/* Red */	case 'R': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '1'; tb[j++] = 'm';
		    break;
		case 'r': if (!ansi) continue;
                    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';';
                    tb[j++] = ccode; tb[j++] = '1'; tb[j++] = 'm';
		    break;
/* Green */	case 'G': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '2'; tb[j++] = 'm';
		    break;
		case 'g': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '2'; tb[j++] = 'm';
		    break;
/* Yellow */	case 'Y': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '3'; tb[j++] = 'm';
		    break;
		case 'y': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '3'; tb[j++] = 'm';
		    break;
/* Blue */	case 'B': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';'; 
		    tb[j++] = ccode; tb[j++] = '4'; tb[j++] = 'm';
		    break;
		case 'b': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';'; 
		    tb[j++] = ccode; tb[j++] = '4'; tb[j++] = 'm';
		    break;
/* Purple */	case 'P': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '5'; tb[j++] = 'm';
		    break;
		case 'p': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '5'; tb[j++] = 'm';
		    break;
/* Cyan */	case 'C': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '6'; tb[j++] = 'm';
		    break;
		case 'c': if (!ansi) continue;
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = ';';
		    tb[j++] = ccode; tb[j++] = '6'; tb[j++] = 'm';
		    break;
		case 'N':
		case 'n':
/* added to fix stupid PC telnet incompatibility with the rest of the world */
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
		    tb[j++] = '3'; tb[j++] = '7'; tb[j++] = 'm';
                    tb[j++] = 27; tb[j++] = '['; tb[j++] = '1'; tb[j++] = ';';
                    tb[j++] = '4'; tb[j++] = '0'; tb[j++] = 'm';
/* end. */
		    tb[j++] = 27; tb[j++] = '['; tb[j++] = '0'; tb[j++] = 'm';
		    break;
	    }
	}
	else tb[j++] = txt[i];
    }

    {
    	tb[j++] = 27;
    	tb[j++] = '[';
    	tb[j++] = '0';
    	tb[j++] = 'm';
	if (grfx)
	{
		tb[j++] = 27;
		tb[j++]='('; 
		tb[j++]='B';
	}
    }
    tb[j]   = '\0';

    length = j;

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

	if (d->outsize >= 32000)
	{
	    bug("Buffer overflow. Closing.\n\r",0);
	    close_socket(d);
	    return;
	}
	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    /*
     * Copy.
     */
    strncpy( d->outbuf + d->outtop, tb, length);
    d->outtop += length;
    return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

    if ( length <= 0 )
	length = strlen(txt);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
	    { perror( "Write_to_descriptor" ); return FALSE; }
    } 

    return TRUE;
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char kav[MAX_STRING_LENGTH];
    char letter[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    char *pwdnew;
    char *p;
    char *strtime;
    int char_age = 17;
    bool fOld; 

    while ( isspace((int)*argument) )
    argument++;

    ch = d->character;

    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_GET_NAME:
	argument[0] = UPPER(argument[0]);
	if ( !check_parse_name( argument ) )
	{
	    write_to_buffer( d, "Illegal name, try another.\n\rName: ", 0 );
	    return;
	}

	sprintf(kav,"%s trying to connect.", argument);
	log_string( kav, d->descriptor );
	fOld = load_char_short( d, argument );
        log_string( "Finished load_char_short.", d->descriptor );
	ch   = d->character;
        log_string( "Finished assigning ch as d->character.", d->descriptor );
	if ( IS_SET(ch->act, PLR_DENY) )
	{
	    sprintf( log_buf, "Denying access to %s@%s.", argument, ch->lasthost );
	    log_string( log_buf, d->descriptor );
	    write_to_buffer( d, "You are denied access.\n\r", 0 );
	    close_socket( d );
	    return;
	}
	else if ( IS_EXTRA(ch, EXTRA_BORN) && char_age < 15 )
	{
	    char agebuf [MAX_INPUT_LENGTH];
	    if (char_age == 14)
		sprintf(agebuf, "You cannot play for another year.\n\r" );
	    else
		sprintf(agebuf, "You cannot play for another %d years.\n\r", 
		(15 - years_old(ch)) );
	    write_to_buffer(d, agebuf, 0);
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    /* Check max number of players - KaVir */

	    DESCRIPTOR_DATA *dcheck;
	    DESCRIPTOR_DATA *dcheck_next;
	    int countdesc = 0;
	    int max_players = 150;

	    for (dcheck = descriptor_list; dcheck != NULL; dcheck = dcheck_next)
	    {
		dcheck_next = dcheck->next;
		countdesc++;
	    }

	    if ( countdesc > max_players && !IS_IMMORTAL(ch) )
	    {
		write_to_buffer( d, "Too many players connected, please try again in a couple of minutes.\n\r", 0 );
		close_socket( d );
		return;
	    }

	    if ( wizlock && !IS_IMMORTAL(ch) )
	    {
		write_to_buffer( d, "The game is wizlocked.\n\r", 0 );
		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
	    write_to_buffer( d, "Please enter password: ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */
	    sprintf( buf, "You want %s engraved on your tombstone (Y/N)? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif


	if ( ch == NULL || (!IS_EXTRA(ch,EXTRA_NEWPASS) &&
	    strcmp( argument, ch->pcdata->pwd ) &&
	    strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd )))
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}
	else if ( ch == NULL || (IS_EXTRA(ch,EXTRA_NEWPASS) &&
	    strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd )))
	{
	    write_to_buffer( d, "Wrong password.\n\r", 0 );
	    close_socket( d );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	if ( check_reconnect( d, ch->name, TRUE ) )
	    return;

	if ( check_playing( d, ch->name ) )
	    return;

	if (ch->level > 1)
	{
	    sprintf(kav, "%s", ch->name);
	    free_char(d->character);
	    d->character = NULL;
	    fOld = load_char_obj( d, kav );
	    ch   = d->character;
	     if ( fOld && ch->lasthost != NULL && strlen(ch->lasthost) > 1 &&
                ch->lasttime != NULL && strlen(ch->lasttime) > 1 )
        {
            sprintf(kav,"Last connected from %s at %s\n\r",ch->lasthost,ch->lasttime);
            write_to_buffer( d, kav, 0 );
        }
        else if ( fOld && ch->lasthost != NULL && strlen(ch->lasthost) > 1 )
        {
            sprintf(kav,"Last connected from %s.\n\r",ch->lasthost);
            write_to_buffer( d, kav, 0 );
        }
	}

	if ( !IS_EXTRA(ch,EXTRA_NEWPASS) && strlen(argument) > 1) {
		sprintf(kav,"%s %s",argument,argument);
		do_password(ch,kav);}

	if (ch->lasthost != NULL) free_string(ch->lasthost);
      if (ch->lasthost != NULL)
        ch->lasthost = str_dup(ch->desc->host);
	else
	    ch->lasthost = str_dup("(unknown)");
	strtime = ctime( &current_time );
	strtime[strlen(strtime)-1] = '\0';
	free_string(ch->lasttime);
	ch->lasttime = str_dup( strtime );
	sprintf( log_buf, "%s@%s has connected.", ch->name, ch->lasthost );
	log_string( log_buf, d->descriptor );

	/* In case we have level 4+ players from another merc mud, or 
	 * players who have somehow got file access and changed their pfiles.
	 */
	if ( ch->level > 3 && ch->trust == 0)
	    ch->level = 3;
	else
	{
	    if ( ch->level > MAX_LEVEL )
		ch->level = MAX_LEVEL;
	    if ( ch->trust > MAX_LEVEL)
		ch->trust = MAX_LEVEL;
	}

	if ( IS_HERO(ch) )
	    do_help( ch, "imotd" );
	do_help( ch, "motd" );
	d->connected = CON_PLAYING;
	motd(ch);
//	d->connected = CON_READ_MOTD;
	break;

    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

        char sha512_salt[MAX_STRING_LENGTH];
        int  rounds = 20000;

        sprintf(sha512_salt, "$6$rounds=%d$%s$", rounds, ch->name);
	pwdnew = crypt( argument, ch->name );

	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
		    0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );

	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer( d, "What is your sex (M/F)? ", 0 );
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}
        ch->pcdata->perm_str=number_range(10,16);
        ch->pcdata->perm_int=number_range(10,16);
        ch->pcdata->perm_wis=number_range(10,16);
        ch->pcdata->perm_dex=number_range(10,16);
	ch->pcdata->perm_con=number_range(10,16);
	sprintf( log_buf, "%s@%s new player.", ch->name, d->host );
	log_string( log_buf, d->descriptor );
	write_to_buffer( d, "\n\r", 2 );
	do_help( ch, "motd" );
//	d->connected = CON_READ_MOTD;
	motd(ch);
        d->connected = CON_PLAYING;
	break;

    case CON_READ_MOTD:
	write_to_buffer( d,     "\n\r\n\rWelcome to The Gathering.\n\r\n\rGreat nations built from the bones of the dead,\n\rWith mud and straw, blood and sweat,\n\rYou know your worth when your enemies\n\rPraise your architecture of aggression!\n\r\n\r",
	    0 );
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;

	ch->position = POS_STANDING;
	if (IS_CLASS(ch, CLASS_VAMPIRE))
	{
	    int ch_age = 	((get_age(ch) - 17) * 2 );
	    if      (ch_age >= 400) ch->pcdata->rank = AGE_METHUSELAH;
	    else if (ch_age >= 200 ) ch->pcdata->rank = AGE_ELDER;
	    else if (ch_age >= 100 ) ch->pcdata->rank = AGE_ANCILLA;
	    else                    ch->pcdata->rank = AGE_NEONATE;
	}

	if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->class > 0 &&
		ch->pcdata->stats[WOLF_AUSPICE] == 0)
	{
	    ch->pcdata->stats[WOLF_AUSPICE] = number_range(1, 28);
	    sprintf(buf, "%s assigned lunar birthday: %d.", ch->name,
		ch->pcdata->stats[WOLF_AUSPICE]);
	    log_string(buf, 0);
	}


	if ( ch->level == 0 )
	{
	    ch->level	= 1;
	    ch->exp	= 0;
	    ch->max_hit = 3000;
	    ch->max_mana= 1500;
	    ch->max_move= 1500;
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
	    ch->class   = 0;
            ch->special = 0;
	    set_title( ch, "the mortal" );
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    send_to_char("If you need help, try talking to the spirit of mud school!\n\r",ch);
	    send_to_char("--------------------------------------------------------------------------------\n\r",ch);
	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    do_look( ch, "auto" );
	}
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	{
	    if (ch->in_room != NULL) char_to_room( ch, ch->in_room );
	    else char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    bind_char(ch);
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
    	    do_board (ch, ""); /* Show board status */
	    do_look( ch, "auto" );
	}
	else if ( IS_IMMORTAL(ch) )
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
	    do_look( ch, "auto" );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	    do_look( ch, "auto" );
	}

        if ( IS_EXTRA(ch, EXTRA_OSWITCH) && IS_CLASS(ch, CLASS_DEMON) )
	{
	    do_humanform(ch,"");
	    do_clearstats(ch,"");
	    check_stats(ch);
	    break;
	}
	else if (ch->level >=7)
	    sprintf(buf,"#Y%s has entered The Gathering.", ch->name);
	else 
	    sprintf(buf,"#Y%s %s", ch->name, (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
            	? ch->pcdata->bamfin : "has entered The Gathering.");

	do_info(ch,buf);
	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
	room_text(ch,">ENTER<");
	if ( IS_EXTRA(ch, EXTRA_OSWITCH) && IS_CLASS(ch, CLASS_DEMON) )    
	    do_humanform (ch,"");
	do_clearstats (ch,"");
	check_stats(ch);
	if ( ch->in_room->sector_type == SECT_HQ && ch->level < 7) do_recall(ch,"");
	break;

    case CON_NOTE_TO:
	    if (!ch->pcdata->in_progress)
	    {
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_TO, but no note in progress",0);
		break;
	    }

	    strcpy (buf, argument);
	    smash_tilde (buf); /* change ~ to - as we save this field as a string later */

	    switch (ch->pcdata->board->force_type)
	    {
	 	case DEF_NORMAL: /* default field */
		    if (!buf[0]) /* empty string? */
		    {
			ch->pcdata->in_progress->to_list = str_dup (ch->pcdata->board->names);
			sprintf (buf, "Assumed default recipient: %s\n\r", ch->pcdata->board->names);
			write_to_buffer (d, buf, 0);
		    }
		    else
			ch->pcdata->in_progress->to_list = str_dup (buf);
		    break;
			
		case DEF_INCLUDE: /* forced default */
		    if (!is_name (ch->pcdata->board->names, buf))
		    {
			strcat (buf, " ");
			strcat (buf, ch->pcdata->board->names);
			ch->pcdata->in_progress->to_list = str_dup(buf);
			sprintf (buf, "\n\rYou did not specify %s as recipient, so it was automatically added.\n\r"
			    "New To :  %s\n\r",
			    ch->pcdata->board->names, ch->pcdata->in_progress->to_list);
			write_to_buffer (d, buf, 0);
		    }
		    else
			ch->pcdata->in_progress->to_list = str_dup (buf);
		    break;
			
		case DEF_EXCLUDE: /* forced exclude */
		    if (is_full_name (ch->pcdata->board->names, buf))
		    {
			sprintf (buf, "You are not allowed to send notes to %s on this board. Try again.\n\r"
		             "To:      ", ch->pcdata->board->names);
			write_to_buffer (d, buf, 0);
			return; /* return from nanny, not changing to the next state! */
		    }
		    else
			ch->pcdata->in_progress->to_list = str_dup (buf);
		    break;
		case DEF_CLAN:
		    if (IS_CLASS(ch, CLASS_MAGE))
		    {
			char color[5];
			sprintf(color, "%d", ch->pcdata->powers[MPOWER_RUNE0]);
			ch->pcdata->in_progress->to_list = str_dup(color);
		    }
		    else
                    	ch->pcdata->in_progress->to_list = str_dup(ch->clan);
		    break;
	    }		

	    write_to_buffer (d, "\n\rSubject: ", 0);
	    d->connected = CON_NOTE_SUBJECT;
	
	    break; /* to */
		
	case CON_NOTE_SUBJECT:
	    if (!ch->pcdata->in_progress)
	    {
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_SUBJECT, but no note in progress",0);
		break;
	    }

	    strcpy (buf, argument);
	    smash_tilde (buf);
		
	    /* Do not allow empty subjects */
		
	    if (!buf[0])		
	    {
		write_to_buffer (d, "Please find a meaningful subject!\n\r",0);
		write_to_buffer (d, "Subject: ", 0);
	    }
	    else  if (strlen(buf) > 60)
	    {
		write_to_buffer (d, "No, no. This is just the Subject. You're note writing the note yet. Twit.\n\r",0);
	    }
	    else
	    /* advance to next stage */
	    {
		ch->pcdata->in_progress->subject = str_dup(buf);
		if (IS_IMMORTAL(ch)) /* immortals get to choose number of expire days */
		{
		    sprintf (buf,"\n\rHow many days do you want this note to expire in?\n\r"
		        "Press Enter for default value for this board, %d days.\n\r"
          		"Expire:  ",
			 ch->pcdata->board->purge_days);
		    write_to_buffer (d, buf, 0);				               
		    d->connected = CON_NOTE_EXPIRE;
		}
		else
		{
		    ch->pcdata->in_progress->expire = 
			current_time + ch->pcdata->board->purge_days * 24L * 3600L;				
		    write_to_buffer (d, "\n\rEnter text. Type ~ or END on an empty line to end note.\n\r"
			"=======================================================\n\r",0);
		    d->connected = CON_NOTE_TEXT;
		}
	    }
		
	    break; /* subject */
	
	case CON_NOTE_EXPIRE:
	{
	    time_t expire;
	    int days;

	    if (!ch->pcdata->in_progress)
	    {
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_EXPIRE, but no note in progress",0);
		break;
	    }
		
	    /* Numeric argument. no tilde smashing */
	    strcpy (buf, argument);
	    if (!buf[0]) /* assume default expire */
		days = 	ch->pcdata->board->purge_days;
	    else /* use this expire */
	    if (!is_number(buf))
	    {
		write_to_buffer (d,"Write the number of days!\n\r",0);
		write_to_buffer (d,"Expire:  ",0);
		return;
	    }
	    else
	    {
		days = atoi (buf);
		if (days <= 0)
		{
		    write_to_buffer (d, "This is a positive MUD. Use positive numbers only! :)\n\r",0);
		    write_to_buffer (d,"Expire:  ",0);
		    return;
		}
	    }
				
	    expire = current_time + (days*24L*3600L); /* 24 hours, 3600 seconds */
	    ch->pcdata->in_progress->expire = expire;
		
	    /* note that ctime returnx XXX\n so we only need to add an \r */
	    sprintf (buf, "This note will expire %s\r",ctime(&expire));
	    write_to_buffer (d,buf,0);

	    write_to_buffer (d, "\n\rEnter text. Type ~ or END on an empty line to end note.\n\r"
		"=======================================================\n\r",0);
	    d->connected = CON_NOTE_TEXT;
		
	    break;

	} /* expire */
		
	case CON_NOTE_TEXT:
	{
		
	    if (!ch->pcdata->in_progress)
	    {
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_TEXT, but no note in progress",0);
		break;
	    }

	    /* First, check for EndOfNote marker */

	    strcpy (buf, argument);
	    if ((!str_cmp(buf, "~")) || (!str_cmp(buf, "END")))
	    {
		write_to_buffer (d, "\n\r(C)ontinue, (V)iew, (P)ost or (F)orget it?\n\r",0);
		d->connected = CON_NOTE_FINISH;
		return;
	    }
		
	    smash_tilde (buf); /* smash it now */

	    /* Check for too long lines. Do not allow lines longer than 75 chars */
		
	    if (strlen (buf) > MAX_LINE_LENGTH)
	    {
		write_to_buffer (d, "Too long line rejected. Do NOT go over 75 characters!\n\r",0);
		return;
	    }
		
	    /* Not end of note. Copy current text into temp buffer, add new line, and copy back */

	    /* How would the system react to strcpy( , NULL) ? */		
	    if (ch->pcdata->in_progress->text)
	    {
		strcpy (letter, ch->pcdata->in_progress->text);
		free_string (ch->pcdata->in_progress->text);
		ch->pcdata->in_progress->text = NULL; /* be sure we don't free it twice */
	    }
	    else
		strcpy (letter, "");
			
	    /* Check for overflow */
		
	    if ((strlen(letter) + strlen (buf)) > MAX_NOTE_TEXT)
	    { /* Note too long, take appropriate steps */
		write_to_buffer (d, "Note too long. Autosplitting <TO BE IMPLEMENTED\n\r", 0);
		free_note (ch->pcdata->in_progress);
		d->connected = CON_PLAYING;
		return;			
	    }
		
	    /* Add new line to the buffer */
		
	    strcat (letter, buf);
	    strcat (letter, "\r\n"); /* new line. \r first to make note files better readable */
	    /* allocate dynamically */		
	    ch->pcdata->in_progress->text = str_dup (letter);
		
	    break;
	} /* con_note_text */
		
	case CON_NOTE_FINISH:

	    if (!ch->pcdata->in_progress)
	    {
		d->connected = CON_PLAYING;
		bug ("nanny: In CON_NOTE_FINISH, but no note in progress",0);
		break;
	    }
		
	    switch (tolower(argument[0]))
	    {
		case 'c': /* keep writing */
		    write_to_buffer (d,"Continuing note...\n\r",0);
		    d->connected = CON_NOTE_TEXT;
		    break;

		case 'v': /* view note so far */
		    if (ch->pcdata->in_progress->text)
		    {
			write_to_buffer (d,"Text of your note so far:\n\r",0);
			write_to_buffer (d, ch->pcdata->in_progress->text, 0);
		    }
		    else
			write_to_buffer (d,"You haven't written a thing!\n\r",0);
		    write_to_buffer (d, "\n\r(C)ontinue, (V)iew, (P)ost or (F)orget it?\n\r",0);
			
		    break;

		case 'p': /* post note */
		    if (!ch->pcdata->in_progress->text)
		    {
			free_note(ch->pcdata->in_progress);
			d->connected = CON_PLAYING;
			write_to_buffer(d,"Blank note.  Nothing posted.\r\n",0);
			break;
		    }
		    finish_note (ch->pcdata->board, ch->pcdata->in_progress);
		    write_to_buffer (d, "Note posted.\n\r",0);
		    d->connected = CON_PLAYING;
		    /* remove AFK status */
		    ch->pcdata->in_progress = NULL;
		    break;
				
		case 'f':
		    write_to_buffer (d, "Note cancelled!\n\r",0);
		    free_note (ch->pcdata->in_progress);
		    ch->pcdata->in_progress = NULL;
		    d->connected = CON_PLAYING;
		    /* remove afk status */
		    break;
			
		default: /* invalid response */
		    write_to_buffer (d, "Huh? Valid answers are:\n\r",0);
		    write_to_buffer (d, "\n\r(C)ontinue, (V)iew, (P)ost or (F)orget it?\n\r",0);
				
		}
	      break;
 
      
      case CON_HQ_PART2:
      {
	  strcpy (buf, argument);


	  if (!str_cmp(buf, "END"))
        {
          send_to_char( "Editing room...\n\r", ch );
          ch->in_room->description = str_dup( ch->pcdata->hqinfo->room_description );
          send_to_char( "Done.\n\r", ch );
          send_to_char( "Saving...\n\r", ch );
          save_new_creation( ch );
          send_to_char( "Purchases are final - NO refunds, so I hope you got it right.\n\r", ch );
          send_to_char( "Ok.\n\r", ch );
          ch->pcdata->quest -= 150;
          ch->practice -= 25;
	    d->connected = CON_PLAYING;
          return;
	  }

        smash_tilde( buf );

	  if (strlen (buf) > MAX_LINE_LENGTH)
	  {
          write_to_buffer (d, "Too long line rejected. Do NOT go over 75 characters!\n\r",0);
          return;
	  }
		
	  if (ch->pcdata->hqinfo->room_description)
	  {
          strcpy (letter, ch->pcdata->hqinfo->room_description);
          free_string (ch->pcdata->hqinfo->room_description);
          ch->pcdata->hqinfo->room_description = NULL; 
        }
	  else
          strcpy (letter, "");

	  strcat (letter, buf);
        strcat (letter, "\r\n");		
        ch->pcdata->hqinfo->room_description = str_dup(letter);
        return;
       

        d->connected = CON_PLAYING;
        break;
      }   
    
  }
  return;
}


/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if ( is_full_name( name, "all auto immortal self kavir someone gaia pestilence aazaroth elric" ) )
	return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;


#if defined(macintosh) || defined(unix)
    if ( strlen(name) > 12 )
	return FALSE;
#endif

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha((int)*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_full_name( name, pMobIndex->player_name ) )
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch) && !IS_EXTRA(ch, EXTRA_SWITCH)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
	    	if (IS_NPC(ch) || ch->pcdata->obj_vnum == 0)
		    act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected.", ch->name, ch->lasthost );
		log_string( log_buf, d->descriptor );
		d->connected = CON_PLAYING;

	/* Inform the character of a note in progress and the possbility 
	 * of continuation! 
	 */		
		if (ch->pcdata->in_progress)
 			send_to_char 
	("You have a note in progress. Type \'note write\' to continue it.\n\r",ch);
	    }
	    return TRUE;
	}
    }

    return FALSE;
}

/*
 * Kick off old connection.  KaVir.
 */
bool check_kickoff( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "You take over your body, which was already in use.\n\r", ch );
		act( "...$n's body has been taken over by another spirit!", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s kicking off old link.", ch->name, ch->lasthost );
		log_string( log_buf, d->descriptor );
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing - KaVir.
 * Using kickoff code from Malice, as mine is v. dodgy.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold != NULL; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
	&&   !str_cmp( name, dold->original
	         ? dold->original->name : dold->character->name ) )
	{
	    char	buf [MAX_STRING_LENGTH];
	    if ( d->character != NULL )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    send_to_char("This body has been taken over!\n\r",dold->character);
	    d->connected = CON_PLAYING;
	    d->character = dold->character;
	    d->character->desc = d;
	    send_to_char( "You take over your body, which was already in use.\n\r", d->character );
	    act( "$n doubles over in agony and $s eyes roll up into $s head.", d->character, NULL, NULL, TO_ROOM );
	    act( "...$n's body has been taken over by another spirit!", d->character, NULL, NULL, TO_ROOM );
	    dold->character=NULL;
	    
	    sprintf(buf,"Kicking off old connection %s@%s",d->character->name,d->host);
	    log_string(buf, d->descriptor);
	    close_socket(dold);	/*Slam the old connection into the ether*/
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   ch->desc->connected != CON_PLAYING
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );

    if (ch->pcdata->chobj)
    {
	obj_from_room( ch->pcdata->chobj );
	obj_to_room( ch->pcdata->chobj, ch->was_in_room );
	act( "$p has returned from the void.", ch, ch->pcdata->chobj, NULL, TO_ROOM );
    }
    else
    	act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );

    ch->was_in_room	= NULL;
    return;
}


/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    if( !txt || !ch->desc )
        return;


    write_to_buffer( ch->desc, txt, strlen( txt ) );


    return;
}

/*
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
	write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };

    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;

    CHAR_DATA *vch = (CHAR_DATA *) arg2;

    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;

    const char *str;
    const char *i;
    char *point;

    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
	return;

    to = (ch ? ch->in_room->people : obj1->in_room->people);

    if ( type == TO_VICT )
    {
	if ( vch == NULL || vch->in_room == NULL)
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    return;
	}
	to = vch->in_room->people;
    }

    if ( type == TO_OBJ )
    {
	if ( (obj1 == NULL || obj1->in_room == NULL) && ch->in_room != NULL)
	    bug( "Act: null room with TO_OBJ.", 0 );
	if ( ch == NULL ) ch = to;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {
	if ( type == TO_CHAR && to != ch ) continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) ) continue;
	if ( type == TO_ROOM && to == ch ) continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) ) continue;

	if ( to->desc == NULL || !IS_AWAKE(to) )
	{
	    continue;
	}

    	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    	{
	    is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_room != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_room != NULL &&
	    	ch->in_room == to->in_room)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_obj != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_obj != NULL &&
	    	ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!is_ok)
	    {
		continue;
	    }
    	}

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  i = " <@@@> ";				break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'N': i = PERS( vch, to  );				break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];	break;
		case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];	break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'M': i = him_her [URANGE(0, vch ->sex, 2)];	break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'S': i = his_her [URANGE(0, vch ->sex, 2)];	break;

		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? ( (obj1->chobj != NULL && obj1->chobj == to)
			    ? "you" : obj1->short_descr)
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
			    ? ( (obj2->chobj != NULL && obj2->chobj == to)
			    ? "you" : obj2->short_descr)
			    : "something";
		    break;

		case 'd':
		    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
		    {
			i = "door";
		    }
		    else
		    {
			one_argument( (char *) arg2, fname );
			i = fname;
		    }
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';

	buf[0]   = UPPER(buf[0]);

	if (to->desc && (to->desc->connected == CON_PLAYING))
	    write_to_buffer( to->desc, buf, point - buf );
    }
    return;
}


void act2( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *to;

    CHAR_DATA *vch = (CHAR_DATA *) arg2;

    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( vch == NULL || vch->in_room == NULL)
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {
	if ( type == TO_CHAR && to != ch ) continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) ) continue;
	if ( type == TO_ROOM && to == ch ) continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) ) continue;
	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

    	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    	{
	    is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_room != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_room != NULL &&
	    	ch->in_room == to->in_room)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_obj != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_obj != NULL &&
	    	ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!is_ok)
		continue;
    	}

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
		i = " <@@@> ";
	    else
	    {
		switch ( *str )
		{
		default:  i = " "; break;
		case 'n': if ( ch != NULL ) i = PERS( ch,  to  );
			  else i = " ";
			  break;
		case 'N': if ( vch != NULL ) i = PERS( vch,  to  );
			  else i = " ";
			  break;
		case 'e': if ( ch != NULL ) i=he_she  [URANGE(0, ch  ->sex, 2)];
			  else i = " ";
			  break;
		case 'E': if (vch != NULL ) i=he_she  [URANGE(0, vch  ->sex,2)];
			  else i = " ";
			  break;
		case 'm': if ( ch != NULL ) i=him_her [URANGE(0, ch  ->sex, 2)];
			  else i = " ";
			  break;
		case 'M': if (vch != NULL ) i=him_her [URANGE(0, vch  ->sex,2)];
			  else i = " ";
			  break;
		case 's': if ( ch != NULL ) i=his_her [URANGE(0, ch  ->sex, 2)];
			  else i = " ";
			  break;
		case 'S': if (vch != NULL ) i=his_her [URANGE(0, vch  ->sex,2)];
			  else i = " ";
			  break;
		case 'p':
		    if (obj1 != NULL)
		    {
			i = can_see_obj( to, obj1 )
			    ? ( (obj1->chobj != NULL && obj1->chobj == to)
			    ? "you" : obj1->short_descr)
			    : "something";
		    }
		    else i = " ";
		    break;

		case 'P':
		    if (obj2 != NULL)
		    {
			i = can_see_obj( to, obj2 )
			    ? ( (obj2->chobj != NULL && obj2->chobj == to)
			    ? "you" : obj2->short_descr)
			    : "something";
		    }
		    else i = " ";
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';

	buf[0]   = UPPER(buf[0]);
	write_to_buffer( to->desc, buf, point - buf );
    }
    return;
}



void kavitem( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };

    char buf[MAX_STRING_LENGTH];
    char kav[MAX_INPUT_LENGTH];
    CHAR_DATA *to;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    const char *str;
    const char *i;
    char *point;
    bool is_ok;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
	return;

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( vch == NULL )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {
	if ( to->desc == NULL || !IS_AWAKE(to) )
	    continue;

    	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    	{
	    is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_room != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_room != NULL &&
	    	ch->in_room == to->in_room)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_obj != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_obj != NULL &&
	    	ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!is_ok) continue;
    	}
	if ( type == TO_CHAR && to != ch )
	    continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) )
	    continue;
	if ( type == TO_ROOM && to == ch )
	    continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) )
	    continue;

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
		i = "";
	    else
	    {
		switch ( *str )
		{
		default:  i = "";					break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];	break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];	break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? ( (obj1->chobj != NULL && obj1->chobj == to)
			    ? "you" : obj1->short_descr)
			    : "something";
		    break;

		case 'o':
		    if (obj1 != NULL) sprintf(kav,"%s's",obj1->short_descr);
		    i = can_see_obj( to, obj1 )
			    ? ( (obj1->chobj != NULL && obj1->chobj == to)
			    ? "your" : kav)
			    : "something's";
		    break;

		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';
	buf[0]   = UPPER(buf[0]);
	write_to_buffer( to->desc, buf, point - buf );
    }

    return;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
         CHAR_DATA *ch;
         CHAR_DATA *victim;
         CHAR_DATA *tank;
   const char      *str;
   const char      *i;
         char      *point;
         char       buf  [ MAX_STRING_LENGTH ];
         char       buf2 [ MAX_STRING_LENGTH ];
	 bool       is_fighting = TRUE;

   if ( ( ch = d->character ) == NULL ) return;
   if ( ch->pcdata == NULL )
   {
      send_to_char( "\n\r\n\r", ch );
      return;
   }
   if ( ch->position == POS_FIGHTING && ch->cprompt[0] == '\0' )
   {
      if ( ch->prompt[0] == '\0' )
      {
         send_to_char( "\n\r\n\r", ch );
         return;
      }
      is_fighting = FALSE;
   }
   else if ( ch->position != POS_FIGHTING && ch->prompt[0] == '\0' )
   {
      send_to_char( "\n\r\n\r", ch );
      return;
   }

   point = buf;
   if ( ch->position == POS_FIGHTING && is_fighting )
      str = d->original ? d->original->cprompt : d->character->cprompt;
   else
      str = d->original ? d->original->prompt : d->character->prompt;
   while( *str != '\0' )
   {
      if( *str != '%' )
      {
         *point++ = *str++;
         continue;
      }
      ++str;
      switch( *str )
      {
         default :
            i = " "; break;
         case 'h' :
            sprintf( buf2, "%d", ch->hit );
            COL_SCALE(buf2, ch, ch->hit, ch->max_hit);
            i = buf2; break;
         case 'H' :
            sprintf( buf2, "%d", ch->max_hit );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'm' :
            sprintf( buf2, "%d", ch->mana                              );
            COL_SCALE(buf2, ch, ch->mana, ch->max_mana);
            i = buf2; break;
         case 'M' :
            sprintf( buf2, "%d", ch->max_mana                          );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'v' :
            sprintf( buf2, "%d", ch->move                              ); 
            COL_SCALE(buf2, ch, ch->move, ch->max_move);
            i = buf2; break;
         case 'V' :
            sprintf( buf2, "%d", ch->max_move                          );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'x' :
            sprintf( buf2, "%d", ch->exp                               );
            COL_SCALE(buf2, ch, ch->exp, 1000);
            i = buf2; break;
         case 'g' :
            sprintf( buf2, "%d", ch->gold                              );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'q' :
            sprintf( buf2, "%d", ch->pcdata->quest                     );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'f' :
            if ( ( victim = ch->fighting ) == NULL )
            {
	       strcpy( buf2, "N/A" );
               ADD_COLOUR(ch, buf2, L_CYAN);
            }
            else
            {
               if ((victim->hit*100/victim->max_hit) < 25)
               {strcpy(buf2, "Awful");ADD_COLOUR(ch, buf2, L_RED);}
               else if ((victim->hit*100/victim->max_hit) < 50)
               {strcpy(buf2, "Poor");ADD_COLOUR(ch, buf2, L_BLUE);}
               else if ((victim->hit*100/victim->max_hit) < 75)
               {strcpy(buf2, "Fair");ADD_COLOUR(ch, buf2, L_GREEN);}
               else if ((victim->hit*100/victim->max_hit) < 100)
               {strcpy(buf2, "Good");ADD_COLOUR(ch, buf2, YELLOW);}
               else if ((victim->hit*100/victim->max_hit) >= 100)
               {strcpy(buf2, "Perfect");ADD_COLOUR(ch, buf2, L_CYAN);}
            }
            i = buf2; break;
         case 'F' :
            if ( ( victim = ch->fighting ) == NULL )
            {
	       strcpy( buf2, "N/A" );
               ADD_COLOUR(ch, buf2, L_CYAN);
            }
            else if ( ( tank = victim->fighting ) == NULL )
            {
	       strcpy( buf2, "N/A" );
               ADD_COLOUR(ch, buf2, L_CYAN);
            }
            else
            {
               if ((tank->hit*100/tank->max_hit) < 25)
               {strcpy(buf2, "Awful");ADD_COLOUR(ch, buf2, L_RED);}
               else if ((tank->hit*100/tank->max_hit) < 50)
               {strcpy(buf2, "Poor");ADD_COLOUR(ch, buf2, L_BLUE);}
               else if ((tank->hit*100/tank->max_hit) < 75)
               {strcpy(buf2, "Fair");ADD_COLOUR(ch, buf2, L_GREEN);}
               else if ((tank->hit*100/tank->max_hit) < 100)
               {strcpy(buf2, "Good");ADD_COLOUR(ch, buf2, YELLOW);}
               else if ((tank->hit*100/tank->max_hit) >= 100)
               {strcpy(buf2, "Perfect");ADD_COLOUR(ch, buf2, L_CYAN);}
            }
            i = buf2; break;
         case 'n' :
            if ( ( victim = ch->fighting ) == NULL )
	       strcpy( buf2, "N/A" );
            else
            {
               if ( IS_AFFECTED(victim, AFF_POLYMORPH) )
                  strcpy(buf2, victim->morph);
               else if ( IS_NPC(victim) )
                  strcpy(buf2, victim->short_descr);
               else
                  strcpy(buf2, victim->name);
               buf2[0] = UPPER(buf2[0]);
            }
            i = buf2; break;
         case 'N' :
            if ( ( victim = ch->fighting ) == NULL )
	       strcpy( buf2, "N/A" );
            else if ( ( tank = victim->fighting ) == NULL )
	       strcpy( buf2, "N/A" );
            else
            {
               if ( ch == tank )
                  strcpy(buf2, "You");
               else if ( IS_AFFECTED(tank, AFF_POLYMORPH) )
                  strcpy(buf2, tank->morph);
               else if ( IS_NPC(victim) )
                  strcpy(buf2, tank->short_descr);
               else
                  strcpy(buf2, tank->name);
               buf2[0] = UPPER(buf2[0]);
            }
            i = buf2; break;
         case 'a' :
            sprintf( buf2, "%s", IS_GOOD( ch ) ? "good"
		                  : IS_EVIL( ch ) ? "evil" : "neutral" );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'A' :
            sprintf( buf2, "%d", ch->alignment                      );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'r' :
            if( ch->in_room )
               sprintf( buf2, "%s", ch->in_room->name                  );
            else
               sprintf( buf2, " "                                      );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'R' :
            if (!IS_NPC(ch) && (IS_CLASS(ch, CLASS_WEREWOLF) || IS_CLASS(ch, CLASS_VAMPIRE)))
            {
               sprintf( buf2, "%d", ch->pcdata->stats[UNI_RAGE]);
               ADD_COLOUR(ch, buf2, D_RED);
            }
            else strcpy( buf2, "0" );
            i = buf2; break;
         case 'b' :
            sprintf( buf2, "%d", ch->beast );
            ADD_COLOUR(ch, buf2, L_CYAN);
            i = buf2; break;
         case 'B' :
            if (!IS_NPC(ch) && IS_CLASS(ch, CLASS_VAMPIRE))
            {
               sprintf( buf2, "%d", ch->pcdata->condition[COND_THIRST] );
               ADD_COLOUR(ch, buf2, D_RED);
            }
            else strcpy( buf2, "0" );
            i = buf2; break;
         case 'c' :
            sprintf( buf2, "%d", char_ac(ch) );
            i = buf2; break;
         case 'p' :
            sprintf( buf2, "%d", char_hitroll(ch) );
            COL_SCALE(buf2, ch, char_hitroll(ch), 200);
            i = buf2; break;
         case 'P' :
            sprintf( buf2, "%d", char_damroll(ch) );
            COL_SCALE(buf2, ch, char_damroll(ch), 200);
            i = buf2; break;
         case 's' :
                 if (ch->race <= 0 ) strcpy(buf2,"Avatar");
            else if (ch->race <= 4 ) strcpy(buf2,"Immortal");
            else if (ch->race <= 9 ) strcpy(buf2,"Godling");
            else if (ch->race <= 14) strcpy(buf2,"Demigod");
            else if (ch->race <= 19) strcpy(buf2,"Lesser God");
            else if (ch->race <= 24) strcpy(buf2,"Greater God");
            else                     strcpy(buf2,"Supreme God");
            i = buf2; break;
         case 'S' :
            sprintf( buf2, "%d", ch->race );
            i = buf2; break;
         case 'o' :
            if (!IS_NPC(ch) && ch->pcdata->stage[2]+25 >= ch->pcdata->stage[1]
		&& ch->pcdata->stage[1] > 0)
            {
               sprintf( buf2, "yes" );
               ADD_COLOUR(ch, buf2, WHITE);
            }
            else strcpy( buf2, "no" );
            i = buf2; break;
         case 'O' :
            if ( ( victim = ch->pcdata->partner ) == NULL )
	       strcpy( buf2, "no" );
            else if (!IS_NPC(victim) && victim != NULL && victim->pcdata->stage[1] > 0
		&& victim->pcdata->stage[2]+25 >= victim->pcdata->stage[1])
            {
               sprintf( buf2, "yes" );
               ADD_COLOUR(ch, buf2, WHITE);
            }
            else strcpy( buf2, "no" );
            i = buf2; break;
         case 'l' :
            if ( ( victim = ch->pcdata->partner ) == NULL )
	       strcpy( buf2, "Nobody" );
            else
            {
               if ( IS_AFFECTED(victim, AFF_POLYMORPH) )
                  strcpy(buf2, victim->morph);
               else if ( IS_NPC(victim) )
                  strcpy(buf2, victim->short_descr);
               else
                  strcpy(buf2, victim->name);
               buf2[0] = UPPER(buf2[0]);
            }
            i = buf2; break;
        case '%' :
            sprintf( buf2, "%%"                                        );
            i = buf2; break;
      } 
      ++str;
      while( ( *point = *i ) != '\0' )
         ++point, ++i;      
   }
   write_to_buffer( d, buf, point - buf );
   return;
}

/*
 * Checks weapon, stance, and spell levels and caps them at appropriate
 * levels if they're too high, and generates log message if done.
 * Called in nanny function.
 */
void check_stats(CHAR_DATA *ch)
{
    int max_wpn = 200;
    int max_spell = 200;
    int max_stance = 200;
    sh_int max_stat = 18;

    int count;
    char buf[MAX_STRING_LENGTH];

    if (IS_IMMORTAL(ch)) return; 

    if (IS_CLASS(ch, CLASS_WEREWOLF))
        max_wpn = 240;

    if (IS_CLASS(ch, CLASS_HIGHLANDER))
    {
        max_wpn = 1000;
	ch->pcdata->powers[HPOWER_MASTERED] = 0;

	max_stat = 25;
    }

    if (IS_CLASS(ch, CLASS_MAGE))
    {
        if (ch->trust == LEVEL_ARCHMAGE)
            max_spell = 300;
        else if (ch->trust == LEVEL_MAGE)
            max_spell = 275;
        else
            max_spell = 240;
    }

    for (count = 0; count < 5; count++)
	if (ch->spl[count] > max_spell)
        {
	    sprintf(buf, "%s: Spell level %d set from %d to %d", 
		ch->name, count, ch->spl[count], max_spell);
            log_string(buf, LOG_CHECK_STATS);
            ch->spl[count] = max_spell;
         }


    for (count = 0; count < 13; count++)
    {
	if (IS_CLASS(ch, CLASS_HIGHLANDER) )
	{
	    if (ch->wpn[count] >= 500)			// Count up weapons the
		ch->pcdata->powers[HPOWER_MASTERED]++;	// HL has mastered.
	}

        if (ch->wpn[count] > max_wpn)
        {
	    sprintf(buf, "%s: Weapon level %d set from %d to %d", 
		ch->name, count, ch->wpn[count], max_wpn);
            log_string(buf, LOG_CHECK_STATS);
            ch->wpn[count] = max_wpn;
         }
    }

    for (count = 1; count < 11; count++)
        if (ch->stance[count] > max_stance)
        {
	    sprintf(buf, "%s: Stance level %d set from %d to %d", 
		ch->name, count, ch->stance[count], max_stance);
            log_string(buf,LOG_CHECK_STATS);
            ch->stance[count] = max_stance;
        }

    if ( ch->pcdata->perm_str > max_stat )
    {
	sprintf(buf,"%s str changed from %d to %d.",
	    ch->name, ch->pcdata->perm_str, max_stat);
	log_string(buf, LOG_CHECK_STATS);
	ch->pcdata->perm_str = max_stat;
    }
    if ( ch->pcdata->perm_wis > max_stat )
    {
        sprintf(buf,"%s wis changed from %d to %d.",
            ch->name, ch->pcdata->perm_wis, max_stat);
        log_string(buf,  LOG_CHECK_STATS);
	ch->pcdata->perm_wis = max_stat;
    }
    if ( ch->pcdata->perm_con > max_stat )
    {
        sprintf(buf,"%s con changed from %d to %d.",
            ch->name, ch->pcdata->perm_con, max_stat);
        log_string(buf,  LOG_CHECK_STATS);
	ch->pcdata->perm_con = max_stat;
    }
    if ( ch->pcdata->perm_dex > max_stat )
    {
        sprintf(buf,"%s dex changed from %d to %d.",
            ch->name, ch->pcdata->perm_dex, max_stat);
        log_string(buf,  LOG_CHECK_STATS);
	ch->pcdata->perm_dex = max_stat;
    }
    if ( ch->pcdata->perm_int > max_stat )
    {
        sprintf(buf,"%s int changed from %d to %d.",
            ch->name, ch->pcdata->perm_int, max_stat);
        log_string(buf,  LOG_CHECK_STATS);
	ch->pcdata->perm_int = max_stat;
    }

    if (IS_SET(ch->act, PLR_WIZINVIS) && !IS_CLASS(ch, CLASS_WEREWOLF))
    {
	REMOVE_BIT(ch->act, PLR_WIZINVIS);
	sprintf(buf, "%s: WIZINVIS removed.", ch->name);
	log_string(buf,  LOG_CHECK_STATS);
    }

    if (IS_IMMUNE(ch, IMM_SHIELDED) && !IS_CLASS(ch, CLASS_WEREWOLF)
	&& !IS_CLASS(ch, CLASS_VAMPIRE))
    {
	REMOVE_BIT(ch->immune, IMM_SHIELDED);
	sprintf(buf, "%s: IMM_SHIELDED removed.", ch->name);
	log_string(buf,  LOG_CHECK_STATS);
    }

    if (!str_cmp(ch->clan, "The Fallen") )
    {
	free_string(ch->clan);
	ch->clan = str_dup( "" );
        if ( get_room_index(ch->home) == NULL ||
            get_room_index(ch->home)->sector_type == SECT_HQ )
            ch->home = ROOM_VNUM_TEMPLE;

    }

    return;
}

void upgrade_demon( CHAR_DATA *ch)
{
}

void motd ( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];

        send_to_char("\n\r\n\rWelcome to The Gathering.\n\r\n\rGreat nations built from the bones of the dead,\n\rWith mud and straw, blood and sweat,\n\rYou know your worth when your enemies\n\rPraise your architecture of aggression!\n\r\n\r",
            ch );
        ch->next        = char_list;
        char_list       = ch;
        ch->desc->connected    = CON_PLAYING;

        ch->position = POS_STANDING;
        if (IS_CLASS(ch, CLASS_VAMPIRE))
        {
            int ch_age =        ((get_age(ch) - 17) * 2 );
            if      (ch_age >= 400) ch->pcdata->rank = AGE_METHUSELAH;
            else if (ch_age >= 200 ) ch->pcdata->rank = AGE_ELDER;
            else if (ch_age >= 100 ) ch->pcdata->rank = AGE_ANCILLA;
            else                    ch->pcdata->rank = AGE_NEONATE;
        }

        if (IS_CLASS(ch, CLASS_WEREWOLF) && ch->class > 0 &&
                ch->pcdata->stats[WOLF_AUSPICE] == 0)
        {
            ch->pcdata->stats[WOLF_AUSPICE] = number_range(1, 28);
            sprintf(buf, "%s assigned lunar birthday: %d.", ch->name,
                ch->pcdata->stats[WOLF_AUSPICE]);
            log_string(buf, 0);
        }

        if ( ch->level == 0 )
        {
            ch->level   = 1;
            ch->exp     = 0;
            ch->max_hit = 3000;
            ch->max_mana= 1500;
            ch->max_move= 1500;
            ch->hit     = ch->max_hit;
            ch->mana    = ch->max_mana;
            ch->move    = ch->max_move;
            ch->class   = 0;
            ch->special = 0;
            set_title( ch, "the mortal" );
            send_to_char("--------------------------------------------------------------------------------\n\r",ch);
            send_to_char("If you need help, try talking to the spirit of mud school!\r\n", ch);
            send_to_char("--------------------------------------------------------------------------------\n\r",ch);
            char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
            do_look( ch, "auto" );
        }
        else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
        {
            if (ch->in_room != NULL) char_to_room( ch, ch->in_room );
            else char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
            bind_char(ch);
        }
        else if ( ch->in_room != NULL )
        {
            char_to_room( ch, ch->in_room );
            do_board (ch, ""); /* Show board status */
            do_look( ch, "auto" );
        }
        else if ( IS_IMMORTAL(ch) )
        {
            char_to_room( ch, get_room_index( ROOM_VNUM_CHAT ) );
            do_look( ch, "auto" );
        }
        else
        {
            char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
            do_look( ch, "auto" );
        }

        if ( IS_EXTRA(ch, EXTRA_OSWITCH) && IS_CLASS(ch, CLASS_DEMON) )
        {
            do_humanform(ch,"");
            do_clearstats(ch,"");
            check_stats(ch);
            return;
        }
        else if (ch->level >=7)
            sprintf(buf,"#Y%s has entered The Gathering.", ch->name);
        else
	{
            sprintf(buf,"#Y%s %s", ch->name, 
		(ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0') ?
                    ch->pcdata->bamfin : "has entered The Gathering.");
	}
        do_info(ch,buf);
        act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
        room_text(ch,">ENTER<");
        if ( IS_EXTRA(ch, EXTRA_OSWITCH) && IS_CLASS(ch, CLASS_DEMON) )
            do_humanform (ch,"");
        do_clearstats (ch,"");
        check_stats(ch);
        if ( ch->in_room->sector_type == SECT_HQ && ch->level < 7) 
	    do_recall(ch, "");

     return;

}

