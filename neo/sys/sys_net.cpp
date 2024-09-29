/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

This file is part of the Beato idTech 4  GPL Source Code (?Beato idTech 4  Source Code?).

Beato idTech 4  Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato idTech 4  Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato idTech 4  Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#include "idlib/precompiled.h"
#pragma hdrstop

// TODO: use SDL_net 

#if defined( _WIN32 ) || defined( _WIN64 )

#include <iptypes.h>
#include <iphlpapi.h>

//#include "win_local.h"

static WSADATA	winsockdata;
static bool	winsockInitialized = false;
static bool usingSocks = false;

idCVar net_ip( "net_ip", "localhost", CVAR_SYSTEM, "local IP address" );
idCVar net_port( "net_port", "0", CVAR_SYSTEM | CVAR_INTEGER, "local IP port number" );
idCVar net_forceLatency( "net_forceLatency", "0", CVAR_SYSTEM | CVAR_INTEGER, "milliseconds latency" );
idCVar net_forceDrop( "net_forceDrop", "0", CVAR_SYSTEM | CVAR_INTEGER, "percentage packet loss" );

idCVar net_socksEnabled( "net_socksEnabled", "0", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_BOOL, "" );
idCVar net_socksServer( "net_socksServer", "", CVAR_SYSTEM | CVAR_ARCHIVE, "" );
idCVar net_socksPort( "net_socksPort", "1080", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_INTEGER, "" );
idCVar net_socksUsername( "net_socksUsername", "", CVAR_SYSTEM | CVAR_ARCHIVE, "" );
idCVar net_socksPassword( "net_socksPassword", "", CVAR_SYSTEM | CVAR_ARCHIVE, "" );


static struct sockaddr	socksRelayAddr;

static SOCKET	ip_socket;
static SOCKET	socks_socket;
static char		socksBuf[4096];

typedef struct 
{
	unsigned long ip;
	unsigned long mask;
} net_interface;

#define 		MAX_INTERFACES	32
int				num_interfaces = 0;
net_interface	netint[MAX_INTERFACES];

//=============================================================================


/*
====================
NET_ErrorString
====================
*/
const char *NET_ErrorString( void ) 
{
	int		code;

	code = WSAGetLastError();
	switch( code ) {
	case WSAEINTR: return "WSAEINTR";
	case WSAEBADF: return "WSAEBADF";
	case WSAEACCES: return "WSAEACCES";
	case WSAEDISCON: return "WSAEDISCON";
	case WSAEFAULT: return "WSAEFAULT";
	case WSAEINVAL: return "WSAEINVAL";
	case WSAEMFILE: return "WSAEMFILE";
	case WSAEWOULDBLOCK: return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS: return "WSAEINPROGRESS";
	case WSAEALREADY: return "WSAEALREADY";
	case WSAENOTSOCK: return "WSAENOTSOCK";
	case WSAEDESTADDRREQ: return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE: return "WSAEMSGSIZE";
	case WSAEPROTOTYPE: return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT: return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN: return "WSAENETDOWN";
	case WSAENETUNREACH: return "WSAENETUNREACH";
	case WSAENETRESET: return "WSAENETRESET";
	case WSAECONNABORTED: return "WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: return "WSAECONNRESET";
	case WSAENOBUFS: return "WSAENOBUFS";
	case WSAEISCONN: return "WSAEISCONN";
	case WSAENOTCONN: return "WSAENOTCONN";
	case WSAESHUTDOWN: return "WSAESHUTDOWN";
	case WSAETOOMANYREFS: return "WSAETOOMANYREFS";
	case WSAETIMEDOUT: return "WSAETIMEDOUT";
	case WSAECONNREFUSED: return "WSAECONNREFUSED";
	case WSAELOOP: return "WSAELOOP";
	case WSAENAMETOOLONG: return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN: return "WSAEHOSTDOWN";
	case WSASYSNOTREADY: return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: return "WSATRY_AGAIN";
	case WSANO_RECOVERY: return "WSANO_RECOVERY";
	case WSANO_DATA: return "WSANO_DATA";
	default: return "NO ERROR";
	}
}

/*
====================
Net_NetadrToSockadr
====================
*/
void Net_NetadrToSockadr( const netadr_t *a, struct sockaddr *s ) {
	memset( s, 0, sizeof(*s) );

	if( a->type == NA_BROADCAST ) {
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_addr.s_addr = INADDR_BROADCAST;
	}
	else if( a->type == NA_IP || a->type == NA_LOOPBACK ) {
		((struct sockaddr_in *)s)->sin_family = AF_INET;
		((struct sockaddr_in *)s)->sin_addr.s_addr = *(int *)&a->ip;
	}

	((struct sockaddr_in *)s)->sin_port = htons( (short)a->port );
}


/*
====================
Net_SockadrToNetadr
====================
*/
void Net_SockadrToNetadr( struct sockaddr *s, netadr_t *a ) {
	unsigned int ip;
	if (s->sa_family == AF_INET) {
		ip = ((struct sockaddr_in *)s)->sin_addr.s_addr;
		*(unsigned int *)&a->ip = ip;
		a->port = htons( ((struct sockaddr_in *)s)->sin_port );
		// we store in network order, that loopback test is host order..
		ip = ntohl( ip );
		if ( ip == INADDR_LOOPBACK ) {
			a->type = NA_LOOPBACK;
		} else {
			a->type = NA_IP;
		}
	}
}

/*
=============
Net_ExtractPort
=============
*/
static bool Net_ExtractPort( const char *src, char *buf, int bufsize, int *port ) {
	char *p;
	strncpy( buf, src, bufsize );
	p = buf; p += min( bufsize - 1, (int)strlen( src ) ); *p = '\0';
	p = strchr( buf, ':' );
	if ( !p ) {
		return false;
	}
	*p = '\0';
	*port = strtol( p+1, NULL, 10 );
	if ( errno == ERANGE ) {
		return false;
	}
	return true;
}

/*
=============
Net_StringToSockaddr
=============
*/
static bool Net_StringToSockaddr( const char *s, struct sockaddr *sadr, bool doDNSResolve ) {
	struct hostent	*h;
	char buf[256];
	int port;
	
	memset( sadr, 0, sizeof( *sadr ) );

	((struct sockaddr_in *)sadr)->sin_family = AF_INET;
	((struct sockaddr_in *)sadr)->sin_port = 0;

	if( s[0] >= '0' && s[0] <= '9' ) {
		unsigned long ret = inet_addr(s);
		if ( ret != INADDR_NONE ) {
			*(int *)&((struct sockaddr_in *)sadr)->sin_addr = ret;
		} else {
			// check for port
			if ( !Net_ExtractPort( s, buf, sizeof( buf ), &port ) ) {
				return false;
			}
			ret = inet_addr( buf );
			if ( ret == INADDR_NONE ) {
				return false;
			}
			*(int *)&((struct sockaddr_in *)sadr)->sin_addr = ret;
			((struct sockaddr_in *)sadr)->sin_port = htons( port );
		}
	} else if ( doDNSResolve ) {
		// try to remove the port first, otherwise the DNS gets confused into multiple timeouts
		// failed or not failed, buf is expected to contain the appropriate host to resolve
		if ( Net_ExtractPort( s, buf, sizeof( buf ), &port ) ) {
			((struct sockaddr_in *)sadr)->sin_port = htons( port );			
		}
		h = gethostbyname( buf );
		if ( h == 0 ) {
			return false;
		}
		*(int *)&((struct sockaddr_in *)sadr)->sin_addr = *(int *)h->h_addr_list[0];
	}
	
	return true;
}

/*
====================
NET_IPSocket
====================
*/
int NET_IPSocket( const char *net_interface, int port, netadr_t *bound_to ) {
	SOCKET				newsocket;
	struct sockaddr_in	address;
	unsigned long		_true = 1;
	int					i = 1;
	int					err;

	if( net_interface ) {
		common->DPrintf( "Opening IP socket: %s:%i\n", net_interface, port );
	} else {
		common->DPrintf( "Opening IP socket: localhost:%i\n", port );
	}

	if( ( newsocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET ) {
		err = WSAGetLastError();
		if( err != WSAEAFNOSUPPORT ) {
			common->Printf( "WARNING: UDP_OpenSocket: socket: %s\n", NET_ErrorString() );
		}
		return 0;
	}

	// make it non-blocking
	if( ioctlsocket( newsocket, FIONBIO, &_true ) == SOCKET_ERROR ) {
		common->Printf( "WARNING: UDP_OpenSocket: ioctl FIONBIO: %s\n", NET_ErrorString() );
		return 0;
	}

	// make it broadcast capable
	if( setsockopt( newsocket, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i) ) == SOCKET_ERROR ) {
		common->Printf( "WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString() );
		return 0;
	}

	if( !net_interface || !net_interface[0] || !idStr::Icmp( net_interface, "localhost" ) ) {
		address.sin_addr.s_addr = INADDR_ANY;
	}
	else {
		Net_StringToSockaddr( net_interface, (struct sockaddr *)&address, true );
	}

	if( port == PORT_ANY ) {
		address.sin_port = 0;
	}
	else {
		address.sin_port = htons( (short)port );
	}

	address.sin_family = AF_INET;

	if( bind( newsocket, (const struct sockaddr *)&address, sizeof(address) ) == SOCKET_ERROR ) {
		common->Printf( "WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString() );
		closesocket( newsocket );
		return 0;
	}

	// if the port was PORT_ANY, we need to query again to know the real port we got bound to
	// ( this used to be in idPort::InitForPort )
	if ( bound_to ) {
		int len = sizeof( address );
		getsockname( newsocket, (sockaddr *)&address, &len );
		Net_SockadrToNetadr( (sockaddr *)&address, bound_to );
	}

	return newsocket;
}

/*
====================
NET_OpenSocks
====================
*/
void NET_OpenSocks( int port ) {
	struct sockaddr_in	address;
	int					err;
	struct hostent		*h;
	int					len;
	bool			rfc1929;
	unsigned char		buf[64];

	usingSocks = false;

	common->Printf( "Opening connection to SOCKS server.\n" );

	if ( ( socks_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) == INVALID_SOCKET ) {
		err = WSAGetLastError();
		common->Printf( "WARNING: NET_OpenSocks: socket: %s\n", NET_ErrorString() );
		return;
	}

	h = gethostbyname( net_socksServer.GetString() );
	if ( h == NULL ) {
		err = WSAGetLastError();
		common->Printf( "WARNING: NET_OpenSocks: gethostbyname: %s\n", NET_ErrorString() );
		return;
	}
	if ( h->h_addrtype != AF_INET ) {
		common->Printf( "WARNING: NET_OpenSocks: gethostbyname: address type was not AF_INET\n" );
		return;
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = *(int *)h->h_addr_list[0];
	address.sin_port = htons( (short)net_socksPort.GetInteger() );

	if ( connect( socks_socket, (struct sockaddr *)&address, sizeof( address ) ) == SOCKET_ERROR ) {
		err = WSAGetLastError();
		common->Printf( "NET_OpenSocks: connect: %s\n", NET_ErrorString() );
		return;
	}

	// send socks authentication handshake
	if ( *net_socksUsername.GetString() || *net_socksPassword.GetString() ) {
		rfc1929 = true;
	}
	else {
		rfc1929 = false;
	}

	buf[0] = 5;		// SOCKS version
	// method count
	if ( rfc1929 ) {
		buf[1] = 2;
		len = 4;
	}
	else {
		buf[1] = 1;
		len = 3;
	}
	buf[2] = 0;		// method #1 - method id #00: no authentication
	if ( rfc1929 ) {
		buf[2] = 2;		// method #2 - method id #02: username/password
	}
	if ( send( socks_socket, (const char *)buf, len, 0 ) == SOCKET_ERROR ) {
		err = WSAGetLastError();
		common->Printf( "NET_OpenSocks: send: %s\n", NET_ErrorString() );
		return;
	}

	// get the response
	len = recv( socks_socket, (char *)buf, 64, 0 );
	if ( len == SOCKET_ERROR ) {
		err = WSAGetLastError();
		common->Printf( "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
		return;
	}
	if ( len != 2 || buf[0] != 5 ) {
		common->Printf( "NET_OpenSocks: bad response\n" );
		return;
	}
	switch( buf[1] ) {
	case 0:	// no authentication
		break;
	case 2: // username/password authentication
		break;
	default:
		common->Printf( "NET_OpenSocks: request denied\n" );
		return;
	}

	// do username/password authentication if needed
	if ( buf[1] == 2 ) {
		int		ulen;
		int		plen;

		// build the request
		ulen = strlen( net_socksUsername.GetString() );
		plen = strlen( net_socksPassword.GetString() );

		buf[0] = 1;		// username/password authentication version
		buf[1] = ulen;
		if ( ulen ) {
			memcpy( &buf[2], net_socksUsername.GetString(), ulen );
		}
		buf[2 + ulen] = plen;
		if ( plen ) {
			memcpy( &buf[3 + ulen], net_socksPassword.GetString(), plen );
		}

		// send it
		if ( send( socks_socket, (const char *)buf, 3 + ulen + plen, 0 ) == SOCKET_ERROR ) {
			err = WSAGetLastError();
			common->Printf( "NET_OpenSocks: send: %s\n", NET_ErrorString() );
			return;
		}

		// get the response
		len = recv( socks_socket, (char *)buf, 64, 0 );
		if ( len == SOCKET_ERROR ) {
			err = WSAGetLastError();
			common->Printf( "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
			return;
		}
		if ( len != 2 || buf[0] != 1 ) {
			common->Printf( "NET_OpenSocks: bad response\n" );
			return;
		}
		if ( buf[1] != 0 ) {
			common->Printf( "NET_OpenSocks: authentication failed\n" );
			return;
		}
	}

	// send the UDP associate request
	buf[0] = 5;		// SOCKS version
	buf[1] = 3;		// command: UDP associate
	buf[2] = 0;		// reserved
	buf[3] = 1;		// address type: IPV4
	*(int *)&buf[4] = INADDR_ANY;
	*(short *)&buf[8] = htons( (short)port );		// port
	if ( send( socks_socket, (const char *)buf, 10, 0 ) == SOCKET_ERROR ) {
		err = WSAGetLastError();
		common->Printf( "NET_OpenSocks: send: %s\n", NET_ErrorString() );
		return;
	}

	// get the response
	len = recv( socks_socket, (char *)buf, 64, 0 );
	if( len == SOCKET_ERROR ) {
		err = WSAGetLastError();
		common->Printf( "NET_OpenSocks: recv: %s\n", NET_ErrorString() );
		return;
	}
	if( len < 2 || buf[0] != 5 ) {
		common->Printf( "NET_OpenSocks: bad response\n" );
		return;
	}
	// check completion code
	if( buf[1] != 0 ) {
		common->Printf( "NET_OpenSocks: request denied: %i\n", buf[1] );
		return;
	}
	if( buf[3] != 1 ) {
		common->Printf( "NET_OpenSocks: relay address is not IPV4: %i\n", buf[3] );
		return;
	}
	((struct sockaddr_in *)&socksRelayAddr)->sin_family = AF_INET;
	((struct sockaddr_in *)&socksRelayAddr)->sin_addr.s_addr = *(int *)&buf[4];
	((struct sockaddr_in *)&socksRelayAddr)->sin_port = *(short *)&buf[8];
	memset( ((struct sockaddr_in *)&socksRelayAddr)->sin_zero, 0, 8 );

	usingSocks = true;
}

/*
==================
Net_WaitForUDPPacket
==================
*/
bool Net_WaitForUDPPacket( int netSocket, int timeout ) {
	int					ret;
	fd_set				set;
	struct timeval		tv;

	if ( !netSocket ) {
		return false;
	}

	if ( timeout <= 0 ) {
		return true;
	}

	FD_ZERO( &set );
	FD_SET( netSocket, &set );

	tv.tv_sec = 0;
	tv.tv_usec = timeout * 1000;

	ret = select( netSocket + 1, &set, NULL, NULL, &tv );

	if ( ret == -1 ) {
		common->DPrintf( "Net_WaitForUPDPacket select(): %s\n", strerror( errno ) );
		return false;
	}

	// timeout with no data
	if ( ret == 0 ) {
		return false;
	}

	return true;
}

/*
==================
Net_GetUDPPacket
==================
*/
bool Net_GetUDPPacket( int netSocket, netadr_t &net_from, char *data, int &size, int maxSize ) {
	int 			ret;
	struct sockaddr	from;
	int				fromlen;
	int				err;

	if( !netSocket ) {
		return false;
	}

	fromlen = sizeof(from);
	ret = recvfrom( netSocket, data, maxSize, 0, (struct sockaddr *)&from, &fromlen );
	if ( ret == SOCKET_ERROR ) {
		err = WSAGetLastError();

		if( err == WSAEWOULDBLOCK || err == WSAECONNRESET ) {
			return false;
		}
		char	buf[1024];
		sprintf( buf, "Net_GetUDPPacket: %s\n", NET_ErrorString() );
		OutputDebugString( buf );
		return false;
	}

	if ( netSocket == ip_socket ) {
		memset( ((struct sockaddr_in *)&from)->sin_zero, 0, 8 );
	}

	if ( usingSocks && netSocket == ip_socket && memcmp( &from, &socksRelayAddr, fromlen ) == 0 ) {
		if ( ret < 10 || data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 1 ) {
			return false;
		}
		net_from.type = NA_IP;
		net_from.ip[0] = data[4];
		net_from.ip[1] = data[5];
		net_from.ip[2] = data[6];
		net_from.ip[3] = data[7];
		net_from.port = *(short *)&data[8];
		memmove( data, &data[10], ret - 10 );
	} else {
		Net_SockadrToNetadr( &from, &net_from );
	}

	if( ret == maxSize ) {
		char	buf[1024];
		sprintf( buf, "Net_GetUDPPacket: oversize packet from %s\n", Sys_NetAdrToString( net_from ) );
		OutputDebugString( buf );
		return false;
	}

	size = ret;

	return true;
}

/*
==================
Net_SendUDPPacket
==================
*/
void Net_SendUDPPacket( int netSocket, int length, const void *data, const netadr_t to ) {
	int				ret;
	struct sockaddr	addr;

	if( !netSocket ) {
		return;
	}

	Net_NetadrToSockadr( &to, &addr );

	if( usingSocks && to.type == NA_IP ) {
		socksBuf[0] = 0;	// reserved
		socksBuf[1] = 0;
		socksBuf[2] = 0;	// fragment (not fragmented)
		socksBuf[3] = 1;	// address type: IPV4
		*(int *)&socksBuf[4] = ((struct sockaddr_in *)&addr)->sin_addr.s_addr;
		*(short *)&socksBuf[8] = ((struct sockaddr_in *)&addr)->sin_port;
		memcpy( &socksBuf[10], data, length );
		ret = sendto( netSocket, socksBuf, length+10, 0, &socksRelayAddr, sizeof(socksRelayAddr) );
	} else {
		ret = sendto( netSocket, (const char *)data, length, 0, &addr, sizeof(addr) );
	}
	if( ret == SOCKET_ERROR ) {
		int err = WSAGetLastError();

		// wouldblock is silent
		if( err == WSAEWOULDBLOCK ) {
			return;
		}

		// some PPP links do not allow broadcasts and return an error
		if( ( err == WSAEADDRNOTAVAIL ) && ( to.type == NA_BROADCAST ) ) {
			return;
		}

		char	buf[1024];
		sprintf( buf, "Net_SendUDPPacket: %s\n", NET_ErrorString() );
		OutputDebugString( buf );
	}
}

/*
====================
Sys_InitNetworking
====================
*/
void Sys_InitNetworking( void ) {
	int		r;

	r = WSAStartup( MAKEWORD( 1, 1 ), &winsockdata );
	if( r ) {
		common->Printf( "WARNING: Winsock initialization failed, returned %d\n", r );
		return;
	}

	winsockInitialized = true;
	common->Printf( "Winsock Initialized\n" );

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	PIP_ADDR_STRING pIPAddrString;
	ULONG ulOutBufLen;
	bool foundloopback;

	num_interfaces = 0;
	foundloopback = false;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc( sizeof( IP_ADAPTER_INFO ) );
	if( !pAdapterInfo ) {
		common->FatalError( "Sys_InitNetworking: Couldn't malloc( %d )", sizeof( IP_ADAPTER_INFO ) );
	}
	ulOutBufLen = sizeof( IP_ADAPTER_INFO );

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if( GetAdaptersInfo( pAdapterInfo, &ulOutBufLen ) == ERROR_BUFFER_OVERFLOW ) {
		free( pAdapterInfo );
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc( ulOutBufLen ); 
		if( !pAdapterInfo ) {
			common->FatalError( "Sys_InitNetworking: Couldn't malloc( %ld )", ulOutBufLen );
		}
	}

	if( ( dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) ) != NO_ERROR ) {
		// happens if you have no network connection
		common->Printf( "Sys_InitNetworking: GetAdaptersInfo failed (%ld).\n", dwRetVal );
	} else {
		pAdapter = pAdapterInfo;
		while( pAdapter ) {
			common->Printf( "Found interface: %s %s - ", pAdapter->AdapterName, pAdapter->Description );
			pIPAddrString = &pAdapter->IpAddressList;
			while( pIPAddrString ) {
				unsigned long ip_a, ip_m;
				if( !idStr::Icmp( "127.0.0.1", pIPAddrString->IpAddress.String ) ) {
					foundloopback = true;
				}
				ip_a = ntohl( inet_addr( pIPAddrString->IpAddress.String ) );
				ip_m = ntohl( inet_addr( pIPAddrString->IpMask.String ) );
				//skip null netmasks
				if( !ip_m ) {
					common->Printf( "%s NULL netmask - skipped\n", pIPAddrString->IpAddress.String );
					pIPAddrString = pIPAddrString->Next;
					continue;
				}
				common->Printf( "%s/%s\n", pIPAddrString->IpAddress.String, pIPAddrString->IpMask.String );
				netint[num_interfaces].ip = ip_a;
				netint[num_interfaces].mask = ip_m;
				num_interfaces++;
				if( num_interfaces >= MAX_INTERFACES ) {
					common->Printf( "Sys_InitNetworking: MAX_INTERFACES(%d) hit.\n", MAX_INTERFACES );
					free( pAdapterInfo );
					return;
				}
				pIPAddrString = pIPAddrString->Next;
			}
			pAdapter = pAdapter->Next;
		}
	}
	// for some retarded reason, win32 doesn't count loopback as an adapter...
	if( !foundloopback && num_interfaces < MAX_INTERFACES ) {
		common->Printf( "Sys_InitNetworking: adding loopback interface\n" );
		netint[num_interfaces].ip = ntohl( inet_addr( "127.0.0.1" ) );
		netint[num_interfaces].mask = ntohl( inet_addr( "255.0.0.0" ) );
		num_interfaces++;
	}
	free( pAdapterInfo );
}


/*
====================
Sys_ShutdownNetworking
====================
*/
void Sys_ShutdownNetworking( void ) {
	if ( !winsockInitialized ) {
		return;
	}
	WSACleanup();
	winsockInitialized = false;
}

/*
=============
Sys_StringToNetAdr
=============
*/
bool Sys_StringToNetAdr( const char *s, netadr_t *a, bool doDNSResolve ) {
	struct sockaddr sadr;
	
	if ( !Net_StringToSockaddr( s, &sadr, doDNSResolve ) ) {
		return false;
	}
	
	Net_SockadrToNetadr( &sadr, a );
	return true;
}

/*
=============
Sys_NetAdrToString
=============
*/
const char *Sys_NetAdrToString( const netadr_t a ) {
	static int index = 0;
	static char buf[ 4 ][ 64 ];	// flip/flop
	char *s;

	s = buf[index];
	index = (index + 1) & 3;

	if ( a.type == NA_LOOPBACK ) {
		if ( a.port ) {
			idStr::snPrintf( s, 64, "localhost:%i", a.port );
		} else {
			idStr::snPrintf( s, 64, "localhost" );
		}
	} else if ( a.type == NA_IP ) {
		idStr::snPrintf( s, 64, "%i.%i.%i.%i:%i", a.ip[0], a.ip[1], a.ip[2], a.ip[3], a.port );
	}
	return s;
}

/*
==================
Sys_IsLANAddress
==================
*/
bool Sys_IsLANAddress( const netadr_t adr ) {
#if ID_NOLANADDRESS
	common->Printf( "Sys_IsLANAddress: ID_NOLANADDRESS\n" );
	return false;
#endif
	if( adr.type == NA_LOOPBACK ) {
		return true;
	}

	if( adr.type != NA_IP ) {
		return false;
	}

	if( num_interfaces ) {
		int i;
		unsigned long *p_ip;
		unsigned long ip;
		p_ip = (unsigned long *)&adr.ip[0];
		ip = ntohl( *p_ip );
                
		for( i=0; i < num_interfaces; i++ ) {
			if( ( netint[i].ip & netint[i].mask ) == ( ip & netint[i].mask ) ) {
				return true;
			}
		} 
	}	
	return false;
}

/*
===================
Sys_CompareNetAdrBase

Compares without the port
===================
*/
bool Sys_CompareNetAdrBase( const netadr_t a, const netadr_t b ) {
	if ( a.type != b.type ) {
		return false;
	}

	if ( a.type == NA_LOOPBACK ) {
		return true;
	}

	if ( a.type == NA_IP ) {
		if ( a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] ) {
			return true;
		}
		return false;
	}

	common->Printf( "Sys_CompareNetAdrBase: bad address type\n" );
	return false;
}

//=============================================================================


#define MAX_UDP_MSG_SIZE	1400

typedef struct udpMsg_s {
	byte				data[MAX_UDP_MSG_SIZE];
	netadr_t			address;
	int					size;
	int					time;
	struct udpMsg_s *	next;
} udpMsg_t;

class idUDPLag {
public:
						idUDPLag( void );
						~idUDPLag( void );

	udpMsg_t *			sendFirst;
	udpMsg_t *			sendLast;
	udpMsg_t *			recieveFirst;
	udpMsg_t *			recieveLast;
	idBlockAlloc<udpMsg_t, 64> udpMsgAllocator;
};

idUDPLag::idUDPLag( void ) {
	sendFirst = sendLast = recieveFirst = recieveLast = NULL;
}

idUDPLag::~idUDPLag( void ) {
	udpMsgAllocator.Shutdown();
}

idUDPLag *udpPorts[65536];

/*
==================
idPort::idPort
==================
*/
idPort::idPort() {
	netSocket = 0;
	memset( &bound_to, 0, sizeof( bound_to ) );
}

/*
==================
idPort::~idPort
==================
*/
idPort::~idPort() {
	Close();
}

/*
==================
InitForPort
==================
*/
bool idPort::InitForPort( int portNumber ) {
	int len = sizeof( struct sockaddr_in );

	netSocket = NET_IPSocket( net_ip.GetString(), portNumber, &bound_to );
	if ( netSocket <= 0 ) {
		netSocket = 0;
		memset( &bound_to, 0, sizeof( bound_to ) );
		return false;
	}

#if 0
	if ( net_socksEnabled.GetBool() ) {
		NET_OpenSocks( portNumber );
	}
#endif

	udpPorts[ bound_to.port ] = new idUDPLag;

	return true;
}

/*
==================
idPort::Close
==================
*/
void idPort::Close() {
	if ( netSocket ) {
		if ( udpPorts[ bound_to.port ] ) {
			delete udpPorts[ bound_to.port ];
			udpPorts[ bound_to.port ] = NULL;
		}
		closesocket( netSocket );
		netSocket = 0;
		memset( &bound_to, 0, sizeof( bound_to ) );
	}
}

/*
==================
idPort::GetPacket
==================
*/
bool idPort::GetPacket( netadr_t &from, void *data, int &size, int maxSize ) {
	udpMsg_t *msg;
	bool ret;

	while( 1 ) {

		ret = Net_GetUDPPacket( netSocket, from, (char *)data, size, maxSize );
		if ( !ret ) {
			break;
		}

		if ( net_forceDrop.GetInteger() > 0 ) {
			if ( rand() < net_forceDrop.GetInteger() * RAND_MAX / 100 ) {
				continue;
			}
		}

		packetsRead++;
		bytesRead += size;

		if ( net_forceLatency.GetInteger() > 0 ) {

			assert( size <= MAX_UDP_MSG_SIZE );
			msg = udpPorts[ bound_to.port ]->udpMsgAllocator.Alloc();
			memcpy( msg->data, data, size );
			msg->size = size;
			msg->address = from;
			msg->time = Sys_Milliseconds();
			msg->next = NULL;
			if ( udpPorts[ bound_to.port ]->recieveLast ) {
				udpPorts[ bound_to.port ]->recieveLast->next = msg;
			} else {
				udpPorts[ bound_to.port ]->recieveFirst = msg;
			}
			udpPorts[ bound_to.port ]->recieveLast = msg;
		} else {
			break;
		}
	}

	if ( net_forceLatency.GetInteger() > 0 || ( udpPorts[ bound_to.port] && udpPorts[ bound_to.port ]->recieveFirst ) ) {

		msg = udpPorts[ bound_to.port ]->recieveFirst;
		if ( msg && msg->time <= Sys_Milliseconds() - net_forceLatency.GetInteger() ) {
			memcpy( data, msg->data, msg->size );
			size = msg->size;
			from = msg->address;
			udpPorts[ bound_to.port ]->recieveFirst = udpPorts[ bound_to.port ]->recieveFirst->next;
			if ( !udpPorts[ bound_to.port ]->recieveFirst ) {
				udpPorts[ bound_to.port ]->recieveLast = NULL;
			}
			udpPorts[ bound_to.port ]->udpMsgAllocator.Free( msg );
			return true;
		}
		return false;

	} else {
		return ret;
	}
}

/*
==================
idPort::GetPacketBlocking
==================
*/
bool idPort::GetPacketBlocking( netadr_t &from, void *data, int &size, int maxSize, int timeout ) {

	Net_WaitForUDPPacket( netSocket, timeout );

	if ( GetPacket( from, data, size, maxSize ) ) {
		return true;
	}

	return false;
}

/*
==================
idPort::SendPacket
==================
*/
void idPort::SendPacket( const netadr_t to, const void *data, int size ) {
	udpMsg_t *msg;

	if ( to.type == NA_BAD ) {
		common->Warning( "idPort::SendPacket: bad address type NA_BAD - ignored" );
		return;
	}

	packetsWritten++;
	bytesWritten += size;

	if ( net_forceDrop.GetInteger() > 0 ) {
		if ( rand() < net_forceDrop.GetInteger() * RAND_MAX / 100 ) {
			return;
		}
	}

	if ( net_forceLatency.GetInteger() > 0 || ( udpPorts[ bound_to.port ] && udpPorts[ bound_to.port ]->sendFirst ) ) {

		assert( size <= MAX_UDP_MSG_SIZE );
		msg = udpPorts[ bound_to.port ]->udpMsgAllocator.Alloc();
		memcpy( msg->data, data, size );
		msg->size = size;
		msg->address = to;
		msg->time = Sys_Milliseconds();
		msg->next = NULL;
		if ( udpPorts[ bound_to.port ]->sendLast ) {
			udpPorts[ bound_to.port ]->sendLast->next = msg;
		} else {
			udpPorts[ bound_to.port ]->sendFirst = msg;
		}
		udpPorts[ bound_to.port ]->sendLast = msg;

		for ( msg = udpPorts[ bound_to.port ]->sendFirst; msg && msg->time <= Sys_Milliseconds() - net_forceLatency.GetInteger(); msg = udpPorts[ bound_to.port ]->sendFirst ) {
			Net_SendUDPPacket( netSocket, msg->size, msg->data, msg->address );
			udpPorts[ bound_to.port ]->sendFirst = udpPorts[ bound_to.port ]->sendFirst->next;
			if ( !udpPorts[ bound_to.port ]->sendFirst ) {
				udpPorts[ bound_to.port ]->sendLast = NULL;
			}
			udpPorts[ bound_to.port ]->udpMsgAllocator.Free( msg );
		}

	} else {
		Net_SendUDPPacket( netSocket, size, data, to );
	}
}


//=============================================================================

/*
==================
idTCP::idTCP
==================
*/
idTCP::idTCP() {
	fd = 0;
	memset( &address, 0, sizeof( address ) );
}

/*
==================
idTCP::~idTCP
==================
*/
idTCP::~idTCP() {
	Close();
}

/*
==================
idTCP::Init
==================
*/
bool idTCP::Init( const char *host, short port ) {
	unsigned long	_true = 1;
	struct sockaddr sadr;

	if ( !Sys_StringToNetAdr( host, &address, true ) ) {
		common->Printf( "Couldn't resolve server name \"%s\"\n", host );
		return false;
	}
	address.type = NA_IP;
	if ( !address.port ) {
		address.port = port;
	}
	common->Printf( "\"%s\" resolved to %i.%i.%i.%i:%i\n", host, 
					address.ip[0], address.ip[1], address.ip[2], address.ip[3], address.port );
	Net_NetadrToSockadr( &address, &sadr );

	if ( fd ) {
		common->Warning( "idTCP::Init: already initialized?" );
	}
		
	if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == INVALID_SOCKET ) {
		fd = 0;
		common->Printf( "ERROR: idTCP::Init: socket: %s\n", NET_ErrorString() );
		return false;
	}
	
	if ( connect( fd, &sadr, sizeof(sadr)) == SOCKET_ERROR ) {
		common->Printf( "ERROR: idTCP::Init: connect: %s\n", NET_ErrorString() );
		closesocket( fd );
		fd = 0;
		return false;
	}
	
	// make it non-blocking
	if( ioctlsocket( fd, FIONBIO, &_true ) == SOCKET_ERROR ) {
		common->Printf( "ERROR: idTCP::Init: ioctl FIONBIO: %s\n", NET_ErrorString() );
		closesocket( fd );
		fd = 0;
		return false;
	}
	
	common->DPrintf( "Opened TCP connection\n" );
	return true;
}

/*
==================
idTCP::Close
==================
*/
void idTCP::Close() {
	if ( fd ) {
		closesocket( fd );
	}
	fd = 0;
}

/*
==================
idTCP::Read
==================
*/
int idTCP::Read( void *data, int size ) {
	int nbytes;
	
	if ( !fd ) {
		common->Printf("idTCP::Read: not initialized\n");
		return -1;
	}
	
	if ( ( nbytes = recv( fd, (char *)data, size, 0 ) ) == SOCKET_ERROR ) {
		if ( WSAGetLastError() == WSAEWOULDBLOCK ) {
			return 0;
		}
		common->Printf( "ERROR: idTCP::Read: %s\n", NET_ErrorString() );
		Close();
		return -1;
	}

	// a successful read of 0 bytes indicates remote has closed the connection
	if ( nbytes == 0 ) {
		common->DPrintf( "idTCP::Read: read 0 bytes - assume connection closed\n" );
		return -1;
	}

	return nbytes;
}

/*
==================
idTCP::Write
==================
*/
int idTCP::Write( void *data, int size ) {
	int nbytes;
	
	if ( !fd ) {
		common->Printf("idTCP::Write: not initialized\n");
		return -1;
	}

	if ( ( nbytes = send( fd, (char *)data, size, 0 ) ) == SOCKET_ERROR ) {
		common->Printf( "ERROR: idTCP::Write: %s\n", NET_ErrorString() );
		Close();
		return -1;
	}
	
	return nbytes;
}

#elif defined(__linux__)

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include <sys/select.h>
#include <net/if.h>

idPort clientPort, serverPort;

idCVar net_ip( "net_ip", "localhost", CVAR_SYSTEM, "local IP address" );
idCVar net_port( "net_port", "", CVAR_SYSTEM | CVAR_INTEGER, "local IP port number" );

typedef struct {
	unsigned long ip;
	unsigned long mask;
} net_interface;

#define 		MAX_INTERFACES	32
int				num_interfaces = 0;
net_interface	netint[MAX_INTERFACES];

/*
=============
NetadrToSockadr
=============
*/
static void NetadrToSockadr( const netadr_t * a, struct sockaddr_in *s ) {
	memset(s, 0, sizeof(*s));

	if ( a->type == NA_BROADCAST ) {
		s->sin_family = AF_INET;

		s->sin_port = htons( (short)a->port );
		*(int *) &s->sin_addr = -1;
	} else if ( a->type == NA_IP || a->type == NA_LOOPBACK ) {
		s->sin_family = AF_INET;

		*(int *) &s->sin_addr = *(int *) &a->ip;
		s->sin_port = htons( (short)a->port );
	}
}

/*
=============
SockadrToNetadr
=============
*/
static void SockadrToNetadr(struct sockaddr_in *s, netadr_t * a) {
	unsigned int ip = *(int *)&s->sin_addr;
	*(int *)&a->ip = ip;
	a->port = ntohs( s->sin_port );
	// we store in network order, that loopback test is host order..
	ip = ntohl( ip );
	if ( ip == INADDR_LOOPBACK ) {
		a->type = NA_LOOPBACK;
	} else {
		a->type = NA_IP;
	}
}

/*
=============
ExtractPort
=============
*/
static bool ExtractPort( const char *src, char *buf, int bufsize, int *port ) {
	char *p;
	strncpy( buf, src, bufsize );
	p = buf; p += min( bufsize - 1, (int)strlen( src ) ); *p = '\0';
	p = strchr( buf, ':' );
	if ( !p ) {
		return false;
	}
	*p = '\0';
	*port = strtol( p+1, NULL, 10 );
	if ( ( *port == 0 && errno == EINVAL ) ||
		 ( ( *port == LONG_MIN || *port == LONG_MAX ) && errno == ERANGE ) ) {
		return false;
	}
	return true;
}

/*
=============
StringToSockaddr
=============
*/
static bool StringToSockaddr( const char *s, struct sockaddr_in *sadr, bool doDNSResolve ) {
	struct hostent *h;
	char buf[256];
	int port;

	memset( sadr, 0, sizeof( *sadr ) );
	sadr->sin_family = AF_INET;

	sadr->sin_port = 0;

	if (s[0] >= '0' && s[0] <= '9') {
		if ( !inet_aton( s, &sadr->sin_addr ) ) {
			// check for port
			if ( !ExtractPort( s, buf, sizeof( buf ), &port ) ) {
				return false;
			}
			if ( !inet_aton( buf, &sadr->sin_addr ) ) {
				return false;
			}
			sadr->sin_port = htons( port );
		}
	} else if ( doDNSResolve ) {
		// try to remove the port first, otherwise the DNS gets confused into multiple timeouts
		// failed or not failed, buf is expected to contain the appropriate host to resolve
		if ( ExtractPort( s, buf, sizeof( buf ), &port ) ) {
			sadr->sin_port = htons( port );			
		}		
		if ( !( h = gethostbyname( buf ) ) ) {
			return false;
		}
		*(int *) &sadr->sin_addr =
			*(int *) h->h_addr_list[0];
	}

	return true;
}

/*
=============
Sys_StringToAdr
=============
*/
bool Sys_StringToNetAdr( const char *s, netadr_t * a, bool doDNSResolve ) {
	struct sockaddr_in sadr;

	if ( !StringToSockaddr( s, &sadr, doDNSResolve ) ) {
		return false;
	}

	SockadrToNetadr( &sadr, a );
	return true;
}

/*
=============
Sys_NetAdrToString
=============
*/
const char *Sys_NetAdrToString( const netadr_t a ) {
	static char s[64];

	if ( a.type == NA_LOOPBACK ) {
		if ( a.port ) {
			idStr::snPrintf( s, sizeof(s), "localhost:%i", a.port );
		} else {
			idStr::snPrintf( s, sizeof(s), "localhost" );
		}
	} else if ( a.type == NA_IP ) {
		idStr::snPrintf( s, sizeof(s), "%i.%i.%i.%i:%i",
			a.ip[0], a.ip[1], a.ip[2], a.ip[3], a.port );
	}
	return s;
}

/*
==================
Sys_IsLANAddress
==================
*/
bool Sys_IsLANAddress( const netadr_t adr ) {
	int i;
	unsigned long *p_ip;
	unsigned long ip;

#if ID_NOLANADDRESS
	common->Printf( "Sys_IsLANAddress: ID_NOLANADDRESS\n" );
	return false;
#endif

	if ( adr.type == NA_LOOPBACK ) {
		return true;
	}

	if ( adr.type != NA_IP ) {
		return false;
	}

	if ( !num_interfaces ) {
		return false;	// well, if there's no networking, there are no LAN addresses, right
	}

	for ( i = 0; i < num_interfaces; i++ ) {
		p_ip = (unsigned long *)&adr.ip[0];
		ip = ntohl( *p_ip );
		if( ( netint[i].ip & netint[i].mask ) == ( ip & netint[i].mask ) ) {
			return true;
		}
	}

	return false;
}

/*
===================
Sys_CompareNetAdrBase

Compares without the port
===================
*/
bool Sys_CompareNetAdrBase( const netadr_t a, const netadr_t b ) {
	if ( a.type != b.type ) {
		return false;
	}

	if ( a.type == NA_LOOPBACK ) {
		return true;
	}

	if ( a.type == NA_IP ) {
		if ( a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] ) {
			return true;
		}
		return false;
	}

	common->Printf( "Sys_CompareNetAdrBase: bad address type\n" );
	return false;
}

/*
====================
NET_InitNetworking
====================
*/
void Sys_InitNetworking(void)
{
	// haven't been able to clearly pinpoint which standards or RFCs define SIOCGIFCONF, SIOCGIFADDR, SIOCGIFNETMASK ioctls
	// it seems fairly widespread, in Linux kernel ioctl, and in BSD .. so let's assume it's always available on our targets

#if MACOS_X
	unsigned int ip, mask;
	struct ifaddrs *ifap, *ifp;
	
	num_interfaces = 0;
	
	if( getifaddrs( &ifap ) < 0 ) {
		common->FatalError( "InitNetworking: SIOCGIFCONF error - %s\n", strerror( errno ) );
		return;
	}
	
	for( ifp = ifap; ifp; ifp = ifp->ifa_next ) {
		if ( ifp->ifa_addr->sa_family != AF_INET )
			continue;

		if ( !( ifp->ifa_flags & IFF_UP ) )
			continue;

		if ( !ifp->ifa_addr )
			continue;

		if ( !ifp->ifa_netmask )
			continue;
		
		ip = ntohl( *( unsigned long *)&ifp->ifa_addr->sa_data[2] );
		mask = ntohl( *( unsigned long *)&ifp->ifa_netmask->sa_data[2] );
		
		if ( ip == INADDR_LOOPBACK ) {
			common->Printf( "loopback\n" );
		} else {
			common->Printf( "IP: %d.%d.%d.%d\n",
							(unsigned char)ifp->ifa_addr->sa_data[2],
							(unsigned char)ifp->ifa_addr->sa_data[3],
							(unsigned char)ifp->ifa_addr->sa_data[4],
							(unsigned char)ifp->ifa_addr->sa_data[5] );
			common->Printf( "NetMask: %d.%d.%d.%d\n",
							(unsigned char)ifp->ifa_netmask->sa_data[2],
							(unsigned char)ifp->ifa_netmask->sa_data[3],
							(unsigned char)ifp->ifa_netmask->sa_data[4],
							(unsigned char)ifp->ifa_netmask->sa_data[5] );
		}
		netint[ num_interfaces ].ip = ip;
		netint[ num_interfaces ].mask = mask;
		num_interfaces++;
	}
#else
	int		s;
	char	buf[ MAX_INTERFACES*sizeof( ifreq ) ];
	ifconf	ifc;
	ifreq	*ifr;
	int		ifindex;
	unsigned int ip, mask;

	num_interfaces = 0;

	s = socket( AF_INET, SOCK_DGRAM, 0 );
	ifc.ifc_len = MAX_INTERFACES*sizeof( ifreq );
	ifc.ifc_buf = buf;
	if ( ioctl( s, SIOCGIFCONF, &ifc ) < 0 ) {
		common->FatalError( "InitNetworking: SIOCGIFCONF error - %s\n", strerror( errno ) );
		return;
	}
	ifindex = 0;
	while ( ifindex < ifc.ifc_len ) {
		common->Printf( "found interface %s - ", ifc.ifc_buf + ifindex );
		// find the type - ignore interfaces for which we can find we can't get IP and mask ( not configured )
		ifr = (ifreq*)( ifc.ifc_buf + ifindex );
		if ( ioctl( s, SIOCGIFADDR, ifr ) < 0 ) {
			common->Printf( "SIOCGIFADDR failed: %s\n", strerror( errno ) );			
		} else {
			if ( ifr->ifr_addr.sa_family != AF_INET ) {
				common->Printf( "not AF_INET\n" );
			} else {
				ip = ntohl( *( unsigned long *)&ifr->ifr_addr.sa_data[2] );
				if ( ip == INADDR_LOOPBACK ) {
					common->Printf( "loopback\n" );
				} else {
					common->Printf( "%d.%d.%d.%d",
									(unsigned char)ifr->ifr_addr.sa_data[2],
									(unsigned char)ifr->ifr_addr.sa_data[3],
									(unsigned char)ifr->ifr_addr.sa_data[4],
									(unsigned char)ifr->ifr_addr.sa_data[5] );
				}
				if ( ioctl( s, SIOCGIFNETMASK, ifr ) < 0 ) {
					common->Printf( " SIOCGIFNETMASK failed: %s\n", strerror( errno ) );
				} else {
					mask = ntohl( *( unsigned long *)&ifr->ifr_addr.sa_data[2] );
					if ( ip != INADDR_LOOPBACK ) {
						common->Printf( "/%d.%d.%d.%d\n",
										(unsigned char)ifr->ifr_addr.sa_data[2],
										(unsigned char)ifr->ifr_addr.sa_data[3],
										(unsigned char)ifr->ifr_addr.sa_data[4],
										(unsigned char)ifr->ifr_addr.sa_data[5] );
					}
					netint[ num_interfaces ].ip = ip;
					netint[ num_interfaces ].mask = mask;
					num_interfaces++;
				}
			}
		}
		ifindex += sizeof( ifreq );
	}
#endif
}

/*
====================
IPSocket
====================
*/
static int IPSocket( const char *net_interface, int port, netadr_t *bound_to = NULL ) {
	int newsocket;
	struct sockaddr_in address;
	int i = 1;

	if ( net_interface ) {
		common->Printf( "Opening IP socket: %s:%i\n", net_interface, port );
	} else {
		common->Printf( "Opening IP socket: localhost:%i\n", port );
	}

	if ( ( newsocket = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == -1 ) {
		common->Printf( "ERROR: IPSocket: socket: %s", strerror( errno ) );
		return 0;
	}
	// make it non-blocking
	int on = 1;
	if ( ioctl( newsocket, FIONBIO, &on ) == -1 ) {
		common->Printf( "ERROR: IPSocket: ioctl FIONBIO:%s\n",
				   strerror( errno ) );
		return 0;
	}
	// make it broadcast capable
	if ( setsockopt( newsocket, SOL_SOCKET, SO_BROADCAST, (char *) &i, sizeof(i) ) == -1 ) {
		common->Printf( "ERROR: IPSocket: setsockopt SO_BROADCAST:%s\n", strerror( errno ) );
		return 0;
	}

	if ( !net_interface || !net_interface[ 0 ]
		|| !idStr::Icmp( net_interface, "localhost" ) ) {
		address.sin_addr.s_addr = INADDR_ANY;
	} else {
		StringToSockaddr( net_interface, &address, true );
	}

	if ( port == PORT_ANY ) {
		address.sin_port = 0;
	} else {
		address.sin_port = htons((short) port);
	}

	address.sin_family = AF_INET;

	if ( bind( newsocket, (const struct sockaddr *)&address, sizeof( address ) ) == -1 ) {
		common->Printf( "ERROR: IPSocket: bind: %s\n", strerror( errno ) );
		close( newsocket );
		return 0;
	}

	if ( bound_to ) {
		unsigned int len = sizeof( address );
		if ( (unsigned int)(getsockname( newsocket, (struct sockaddr *)&address, (socklen_t*)&len )) == -1 ) {
			common->Printf( "ERROR: IPSocket: getsockname: %s\n", strerror( errno ) );
			close( newsocket );
			return 0;
		}
		SockadrToNetadr( &address, bound_to );
	}

	return newsocket;
}

/*
==================
idPort::idPort
==================
*/
idPort::idPort() {
	netSocket = 0;
	memset( &bound_to, 0, sizeof( bound_to ) );
}

/*
==================
idPort::~idPort
==================
*/
idPort::~idPort() {
	Close();
}

/*
==================
idPort::Close
==================
*/
void idPort::Close() {
	if ( netSocket ) {
		close(netSocket);
		netSocket = 0;
		memset( &bound_to, 0, sizeof( bound_to ) );
	}
}

/*
==================
idPort::GetPacket
==================
*/
bool idPort::GetPacket( netadr_t &net_from, void *data, int &size, int maxSize ) {
	int ret;
	struct sockaddr_in from;
	int fromlen;
	
	if ( !netSocket ) {
		return false;
	}
	
	fromlen = sizeof( from );
	ret = recvfrom( netSocket, data, maxSize, 0, (struct sockaddr *) &from, (socklen_t *) &fromlen );

	if ( ret == -1 ) {
		if (errno == EWOULDBLOCK || errno == ECONNREFUSED) {
			// those commonly happen, don't verbose
			return false;
		}
		common->DPrintf( "idPort::GetPacket recvfrom(): %s\n", strerror( errno ) );
		return false;
	}

	assert( ret < maxSize );

	SockadrToNetadr( &from, &net_from );
	size = ret;
	return true;
}

/*
==================
idPort::GetPacketBlocking
==================
*/
bool idPort::GetPacketBlocking( netadr_t &net_from, void *data, int &size, int maxSize, int timeout ) {
	fd_set				set;
	struct timeval		tv;
	int					ret;
	
	if ( !netSocket ) {
		return false;
	}

	if ( timeout < 0 ) {
		return GetPacket( net_from, data, size, maxSize );
	}

	FD_ZERO( &set );
	FD_SET( netSocket, &set );

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = ( timeout % 1000 ) * 1000;
	ret = select( netSocket+1, &set, NULL, NULL, &tv );
	if ( ret == -1 ) {
		if ( errno == EINTR ) {
			common->DPrintf( "idPort::GetPacketBlocking: select EINTR\n" );
			return false;
		} else {
			common->Error( "idPort::GetPacketBlocking: select failed: %s\n", strerror( errno ) );
		}
	}

	if ( ret == 0 ) {
		// timed out
		return false;
	}
	struct sockaddr_in from;
	int fromlen;
	fromlen = sizeof( from );
	ret = recvfrom( netSocket, data, maxSize, 0, (struct sockaddr *)&from, (socklen_t *)&fromlen );
	if ( ret == -1 ) {
		// there should be no blocking errors once select declares things are good
		common->DPrintf( "idPort::GetPacketBlocking: %s\n", strerror( errno ) );
		return false;
	}
	assert( ret < maxSize );
	SockadrToNetadr( &from, &net_from );
	size = ret;
	return true;
}

/*
==================
idPort::SendPacket
==================
*/
void idPort::SendPacket( const netadr_t to, const void *data, int size ) {
	int ret;
	struct sockaddr_in addr;

	if ( to.type == NA_BAD ) {
		common->Warning( "idPort::SendPacket: bad address type NA_BAD - ignored" );
		return;
	}

	if ( !netSocket ) {
		return;
	}

	NetadrToSockadr( &to, &addr );

	ret = sendto( netSocket, data, size, 0, (struct sockaddr *) &addr, sizeof(addr) );
	if ( ret == -1 ) {
		common->Printf( "idPort::SendPacket ERROR: to %s: %s\n", Sys_NetAdrToString( to ), strerror( errno ) );
	}
}

/*
==================
idPort::InitForPort
==================
*/
bool idPort::InitForPort( int portNumber ) {
	netSocket = IPSocket( net_ip.GetString(), portNumber, &bound_to );
	if ( netSocket <= 0 ) {
		netSocket = 0;
		memset( &bound_to, 0, sizeof( bound_to ) );
		return false;
	}
	return true;
}

//=============================================================================

/*
==================
idTCP::idTCP
==================
*/
idTCP::idTCP() {
	fd = 0;
	memset(&address, 0, sizeof(address));
}

/*
==================
idTCP::~idTCP
==================
*/
idTCP::~idTCP() {
	Close();
}

/*
==================
idTCP::Init
==================
*/
bool idTCP::Init( const char *host, short port ) {
	struct sockaddr_in sadr;
	if ( !Sys_StringToNetAdr( host, &address, true ) ) {
		common->Printf( "Couldn't resolve server name \"%s\"\n", host );
		return false;
	}
	address.type = NA_IP;
	if (!address.port) {
		address.port = port;
	}
	common->Printf( "\"%s\" resolved to %i.%i.%i.%i:%i\n", host, 
		address.ip[0], address.ip[1], address.ip[2], address.ip[3],  address.port );
	NetadrToSockadr(&address, &sadr);

	if (fd) {
		common->Warning("idTCP::Init: already initialized?\n");
	}
		
	if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		fd = 0;
		common->Printf("ERROR: idTCP::Init: socket: %s\n", strerror(errno));
		return false;
	}
	
	if ( connect( fd, (const sockaddr *)&sadr, sizeof( sadr ) ) == -1 ) {
		common->Printf( "ERROR: idTCP::Init: connect: %s\n", strerror( errno ) );		
		close( fd );
		fd = 0;
		return false;
	}
	
	int status;
	if ((status = fcntl(fd, F_GETFL, 0)) != -1) {
	    status |= O_NONBLOCK; /* POSIX */
	    status = fcntl(fd, F_SETFL, status);
	}
	if (status == -1) {
		common->Printf("ERROR: idTCP::Init: fcntl / O_NONBLOCK: %s\n", strerror(errno));
		close(fd);
		fd = 0;
		return false;
	}
	
	common->DPrintf("Opened TCP connection\n");
	return true;
}

/*
==================
idTCP::Close
==================
*/
void idTCP::Close() {
	if (fd) {
		close(fd);
	}
	fd = 0;
}

/*
==================
idTCP::Read
==================
*/
int idTCP::Read(void *data, int size) {
	int nbytes;
	
	if (!fd) {
		common->Printf("idTCP::Read: not initialized\n");
		return -1;
	}

#if defined(_GNU_SOURCE)
	// handle EINTR interrupted system call with TEMP_FAILURE_RETRY -  this is probably GNU libc specific
	if ( ( nbytes = TEMP_FAILURE_RETRY( read( fd, data, size ) ) ) == -1 ) {
#else
	do {
	  nbytes = read( fd, data, size );
	} while ( nbytes == -1 && errno == EINTR );
	if ( nbytes == -1 ) {
#endif
		if (errno == EAGAIN) {
			return 0;
		}
		common->Printf("ERROR: idTCP::Read: %s\n", strerror(errno));
		Close();
		return -1;
	}
	
	// a successful read of 0 bytes indicates remote has closed the connection
	if ( nbytes == 0 ) {
		common->DPrintf( "idTCP::Read: read 0 bytes - assume connection closed\n" );
		return -1;
	}
	
	return nbytes;
}

/*
==================
idTCP::Write
==================
*/

static void got_SIGPIPE( int signum ) {
	common->Printf( "idTCP: SIGPIPE\n" );
}

int	idTCP::Write(void *data, int size) 
{
	int nbytes;
	
	if ( !fd ) 
	{
		common->Printf( "idTCP::Write: not initialized\n");
		return -1;
	}
	
	struct sigaction bak_action;
	struct sigaction action;

	action.sa_handler = got_SIGPIPE;
	sigemptyset( &action.sa_mask );
	action.sa_flags = 0;
	
	if ( sigaction( SIGPIPE, &action, &bak_action ) != 0 ) {
		common->Printf( "ERROR: idTCP::Write: failed to set temporary SIGPIPE handler\n" );
		Close();
		return -1;
	}

#if defined(_GNU_SOURCE)	
	// handle EINTR interrupted system call with TEMP_FAILURE_RETRY -  this is probably GNU libc specific
	if ( ( nbytes = TEMP_FAILURE_RETRY ( write( fd, data, size ) ) ) == -1 ) 
	{
#else
	  do 
	  {
	    nbytes = write( fd, data, size );
	  } while ( nbytes == -1 && errno == EINTR );
	  if ( nbytes == -1 ) 
	  {
#endif
		common->Printf( "ERROR: idTCP::Write: %s\n", strerror( errno ) );
		Close();
		return -1;
	}
	
	if ( sigaction( SIGPIPE, &bak_action, NULL ) != 0 ) {
		common->Printf( "ERROR: idTCP::Write: failed to reset SIGPIPE handler\n" );
		Close();
		return -1;
	}
	
	return nbytes;	
}

#endif
