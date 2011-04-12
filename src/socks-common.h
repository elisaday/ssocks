/*
 *      socks5-common.h
 *
 *      Created on: 2011-04-11
 *      Author:     Hugo Caron
 *      Email:      <h.caron@codsec.com>
 *
 * Copyright (C) 2011 by Hugo Caron
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef SOCKS_COMMON__H
#define SOCKS_COMMON__H

#include <netinet/ip.h> /* socket sockaddr_in */
#include <unistd.h>		/* read and write */

#include <config.h>
#include <stdint.h>

#include "output-util.h"


#ifdef HAVE_LIBSSL
#include "ssl-util.h"
#endif

#define SOCKS4_V 0x04
#define SOCKS5_V 0x05
#define SOCKS5_SSL_V 0x06

/* Socks mode */
enum{
	M_SERVER, /* Server mode used by ssocksd */

	M_CLIENT, /* Client mode used by nsocks and ssocks */

	M_DYNAMIC, /* Run a server and listen, when it receive data
				* it transmit it to a another socks server,
				* used by ssocks */
};

/* Socks state */
enum {
	S_R_VER,		/* Read version */
	S_W_VER,		/* Write version */
	S_R_VER_ACK,	/* Read version ACK */
	S_W_VER_ACK,	/* Write version ACK */

	S_R_AUTH,		/* Read authentication */
	S_W_AUTH,		/* Write authentication ACK */
	S_R_AUTH_ACK,	/* Read authentication */
	S_W_AUTH_ACK,	/* Write authentication ACK */

	S_R_REQ,		/* Read authentication */
	S_W_REQ,		/* Write authentication */
	S_R_REQ_ACK,	/* Read authentication ACK */
	S_W_REQ_ACK,	/* Write authentication ACK */

	S_REPLY,		/* Read on a socket, write a another */

	S_WAIT			/* Nothing to do just wait */
};

/* Configuration type enumeration */
enum {
	CONFIG_SERVER,
	CONFIG_SERVER_DYNA,
	CONFIG_CLIENT
};

/* Socket structure need to used socket and ssl
 * side by side in the same code
 */
typedef struct {
	int soc;  /* Socket */
#ifdef HAVE_LIBSSL
	SSL *ssl; /* SSL socket */
#endif
	struct sockaddr_in adrS; /* Socket server info */
	struct sockaddr_in adrC; /* Socket client info */
} s_socket;

/* Socks5 information used to build packet
 */
typedef struct {
	int id;
	int mode;	 	/* Socks mode */
	int state;   	/* Socks state */
	int version; 	/* Version choose, default -1 */
	int method;	 	/* Authentication method choose, default -1 */
	int auth; 	 	/* Authenticate flag, default 0 */
	int connected;	/* Connected flag, default 0 */
	int listen;		/* Listen flag in bind mode, default 0, 
					 * if -1 error when accept */
	int cmd;		/* Socks command request */
}s_socks;


/* Socks5 server configuration */
/*typedef struct {

}s_socks_serv_config;*/

/* Socks5 client configuration */
typedef struct {
	char *host;
	int port;
	char *sockshost;				/* Socks5 destination host */
	int socksport;				/* Socks5 port host */
	int version;
	int loop;				/* Client loop */
	char *username;			/* Socks5 username */
	char *password;			/* Socks5 password */
}s_socks_serv_cli_config;

/* Socks5 server dynamic configuration */
typedef struct {
	s_socks_serv_cli_config client; /* Client configuration
									 * of dynamic server */
}s_socks_serv_dyna_config;

/* Union of configuration to have all in one */
typedef union {
	s_socks_serv_dyna_config dyna;
	s_socks_serv_cli_config cli;
}s_socks_config;

/* Socks5 configuration
 */
typedef struct {
	int type; /* CONFIG_SERVER, CONFIG_SERVER_DYNA, CONFIG_CLIENT */
	char *allowed_version; /* Version accepted,
							 *  last need to be NULL */
	
	char *allowed_method;  /* Accepted method,
							 * last need to be NULL */
	size_t n_allowed_method;
	
	s_socks_config config;
}s_socks_conf;


/* Buffer definition
 */
typedef struct {
	char data[4096];
	size_t pos, a, b;
} s_buffer;

/* Macro to read and write properly
 */
#define WRITE_DISP(k, soc, buf, client) \
	({ \
		k = write_socks(soc, buf); \
		if (k < 0){ disconnection(client); break; } /* Error */ \
		if (k == 0) { break; } /* Need to write again */	\
		init_buffer(buf); \
	})
	
#define READ_DISP(k, soc, buf, minsize, client) \
	({ \
		k = read_socks(soc, buf, minsize); \
		if (k < 0){ disconnection(client); break; } /* Error */ \
		if (k == 0) { break; } /* Need to read again */ \
	})

/* Prototypes
 */
int buf_empty(s_buffer *buf);
int buf_size(s_buffer *buf);
int buf_free(s_buffer *buf);

int read_socks(s_socket *s, s_buffer *buf, size_t minsize);
int write_socks(s_socket *s, s_buffer *buf);


void init_buffer(s_buffer *buf);
void init_socket(s_socket *s);
void init_socks(s_socks *s, int id, int mode);

void close_socket(s_socket *s);

/* From here packet definition used to manipulate packet 
 * ---------------------------------------------------------------------
 */
 
 /* Socks5 version packet */
typedef struct {
	char ver;
	char nmethods;
	char methods[5];
} Socks5Version;

/* Socks5 version packet ACK */
typedef struct {
	char ver;
	char method;
} Socks5VersionACK;

/* Socks5 authentication packet */
typedef struct {
	char ver;
	char ulen;
	char uname[256];
	char plen;
	char passwd[256];
} Socks5Auth;

/* Socks5 authentication packet ACK */
typedef struct {
	char ver;
	char status;
} Socks5AuthACK;

/* Socks5 request packet */
typedef struct {
	char ver;
	char cmd;
	char rsv;
	char atyp;
	/*char dstadr;
	unsigned short dstport;*/
} Socks5Req;

/* Socks5 request packet ACK
 * Need to change alignment 4 -> 2  else sizeof 12 instead of 10 */
#pragma pack(push, 2) /* Change alignment 4 -> 2 */
typedef struct {
	char ver;
	char rep;
	char rsv;
	char atyp;
	struct in_addr bndaddr; /* uint32_t */
	uint16_t  bndport;
} Socks5ReqACK;
#pragma pack(pop) /* End of change alignment */


#endif /* SOCKS5_COMMON__H */
