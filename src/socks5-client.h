/*
 *      socks5_client.h
 *      
 *      Created on: 2011-03-31
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

#ifndef SOCKS5_CLIENT__H
#define SOCKS5_CLIENT__H

#include "client.h"

#ifdef HAVE_LIBSSL
	#include "ssl-util.h"
#endif

typedef struct {
	char *host;		/* Host destination */
	int port;		/* Port destination */
	
	char *uname; 	/* Socks username */
	char *passwd;	/* Socks password */
	
	int loop;	  	/* Stop the client loop */
	int naskbind; 	/* Count number of ask send for bind */

	int version;  	/* Version wanted */
	int bind;     	/* BIND mode not connected */

	int soc;		/* If connection OK is != -1 */
#ifdef HAVE_LIBSSL
	SSL *socSsl;	/* If connection OK is != NULL */
#endif
} ConfigClient;

#ifdef HAVE_LIBSSL
	extern SSL *sslCli;
#endif

void dispatch_client (Client *c);

void write_version(Client *c);
void read_version_ack(Client *c);

void write_auth(Client *c);
void read_auth_ack(Client *c);

void write_request(Client *c);
void read_request_ack(Client *c);

int new_socket_with_socks(char *sockshost, int socksport,
		ConfigClient *config);



#endif /* SOCKS5_CLIENT__H */
