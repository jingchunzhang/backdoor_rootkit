/*
 *  Trimmed down Apache 1.3 header file
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

#include <netinet/in.h>

#define OK               0
#define DECLINED        -1
#define DONE            -2

#define COOKIE_AP13 0x41503133UL
#define COOKIE_EAPI 0x45415049UL

#define MODULE_MAGIC_NUMBER_MAJOR 19990320
#define MODULE_MAGIC_NUMBER_MINOR 16

#define STANDARD_MODULE_STUFF   MODULE_MAGIC_NUMBER_MAJOR, \
                                MODULE_MAGIC_NUMBER_MINOR, \
                                -1, \
                                __FILE__, \
                                NULL, \
                                NULL, \
                                MODULE_MAGIC_COOKIE

typedef struct pool pool;
typedef struct server_rec  server_rec;
typedef struct request_rec request_rec;
typedef struct command_struct command_rec;
typedef struct handler_struct handler_rec;

typedef struct module_struct
{
    int version;
    int minor_version;
    int module_index;
    const char *name;
    void *dynamic_load_handle;
    struct module_struct *next;
    unsigned long magic;

    void (*init) (server_rec *, pool *);
    void *(*create_dir_config) (pool *p, char *dir);
    void *(*merge_dir_config) (pool *p, void *base_conf, void *new_conf);
    void *(*create_server_config) (pool *p, server_rec *s);
    void *(*merge_server_config) (pool *p, void *base_conf, void *new_conf);

    const command_rec *cmds;
    const handler_rec *handlers;

    int (*translate_handler) (request_rec *);
    int (*ap_check_user_id) (request_rec *);
    int (*auth_checker) (request_rec *);
    int (*access_checker) (request_rec *);
    int (*type_checker) (request_rec *);
    int (*fixer_upper) (request_rec *);
    int (*logger) (request_rec *);
    int (*header_parser) (request_rec *);

    void (*child_init) (server_rec *, pool *);
    void (*child_exit) (server_rec *, pool *);
    int (*post_read_request) (request_rec *);
}
module;

typedef struct buff_struct BUFF;
typedef struct conn_rec conn_rec;
typedef struct pool ap_pool;
typedef struct table table;

struct buff_struct
{
    int flags;
    unsigned char *inptr;
    int incnt;
    int outchunk;
    int outcnt;
    unsigned char *inbase;
    unsigned char *outbase;
    int bufsiz;
    void (*error) (BUFF *fb, int op, void *data);
    void *error_data;
    long int bytes_sent;
    ap_pool *pool;
    int fd;
    int fd_in;
    void *t_handle;
    void *callback_data;
    void (*filter_callback) (BUFF *, const void *, int);
};

struct conn_rec
{
    ap_pool *pool;
    server_rec *server;
    server_rec *base_server;
    void *vhost_lookup_data;
    int child_num;
    BUFF *client;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    char *remote_ip;
    char *remote_host;
    char *remote_logname;
    char *user;
    char *ap_auth_type;
    unsigned aborted:1;
    signed int keepalive:2;
    unsigned keptalive:1;
    signed int double_reverse:2;
    int keepalives;
    char *local_ip;
    char *local_host;
};

struct request_rec
{
    ap_pool *pool;
    conn_rec *connection;
    server_rec *server;
    request_rec *next;
    request_rec *prev;
    request_rec *main;
    char *the_request;
    int assbackwards;
    int proxyreq;
    int header_only;
    char *protocol;
    int proto_num;
    const char *hostname;
    time_t request_time;
    const char *status_line;
    int status;
    const char *method;
    int method_number;
    int allowed;
    int sent_bodyct;
    long bytes_sent;
    time_t mtime;
    int chunked;
    int byterange;
    char *boundary;
    const char *range;
    long clength;
    long remaining;
    long read_length;
    int read_body;
    int read_chunked;
    unsigned expecting_100;
    table *headers_in;
    table *headers_out;
    table *err_headers_out;
    table *subprocess_env;
    table *notes;
    const char *content_type;
    const char *handler;
    const char *content_encoding;
    const char *content_language;
    void *content_languages;
    char *vlist_validator;
    int no_cache;
    int no_local_copy;
    char *unparsed_uri;
    char *uri;
    char *filename;
    char *path_info;
    char *args;

    /* whole bunch of other crap removed */
};
