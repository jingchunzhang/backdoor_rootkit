/*
 *  Trimmed down Apache 2.2 header file
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

#define MODULE_MAGIC_NUMBER_MAJOR 20020903
#define MODULE_MAGIC_NUMBER_MINOR 7

#define MODULE_MAGIC_COOKIE 0x41503230UL

#define STANDARD20_MODULE_STUFF MODULE_MAGIC_NUMBER_MAJOR, \
                                MODULE_MAGIC_NUMBER_MINOR, \
                                -1, \
                                __FILE__, \
                                NULL, \
                                NULL, \
                                MODULE_MAGIC_COOKIE, \
                                NULL

#define APR_HOOK_FIRST  0

typedef struct module_struct module;
typedef struct apr_pool_t apr_pool_t;
typedef struct server_rec  server_rec;
typedef struct process_rec process_rec;
typedef struct command_struct command_rec; 

struct module_struct
{
    int version;
    int minor_version;
    int module_index;
    const char *name;
    void *dynamic_load_handle;
    struct module_struct *next;
    unsigned long magic;

    void (*rewrite_args) (process_rec *process);
    void *(*create_dir_config) (apr_pool_t *p, char *dir);
    void *(*merge_dir_config) (apr_pool_t *p, void *base_conf, void *new_conf);
    void *(*create_server_config) (apr_pool_t *p, server_rec *s);
    void *(*merge_server_config) (apr_pool_t *p, void *base_conf, 
                                  void *new_conf);
    const command_rec *cmds;
    void (*register_hooks) (apr_pool_t *p);
};

typedef struct ap_conf_vector_t ap_conf_vector_t;

extern void ap_hook_post_config( void *pf, const char *aszPre,
                                 const char *aszSucc, int nOrder );

extern void ap_hook_post_read_request( void *pf, const char *aszPre,
                                       const char *aszSucc, int nOrder );

extern void *ap_get_module_config( const ap_conf_vector_t *cv,
                                   const module *m );

extern module core_module;

typedef int apr_int32_t;
typedef long apr_off_t;
typedef long long apr_int64_t;
typedef apr_int64_t apr_time_t;
typedef apr_int64_t apr_interval_time_t;

typedef struct conn_rec conn_rec;
typedef struct request_rec request_rec;
typedef struct apr_table_t apr_table_t;
typedef struct apr_sockaddr_t apr_sockaddr_t;
typedef struct ap_method_list_t ap_method_list_t;
typedef struct apr_array_header_t apr_array_header_t;
typedef struct sock_userdata_t sock_userdata_t;

typedef struct apr_socket_t
{
    apr_pool_t *cntxt;
    int socketdes;
    int type;
    int protocol;
    apr_sockaddr_t *local_addr;
    apr_sockaddr_t *remote_addr;
    apr_interval_time_t timeout; 
    int local_port_unknown;
    int local_interface_unknown;
    int remote_addr_unknown;
    apr_int32_t netmask;
    apr_int32_t inherit;
    sock_userdata_t *userdata;
}
apr_socket_t;

struct conn_rec
{
    apr_pool_t *pool;
    server_rec *base_server;
    void *vhost_lookup_data;
    apr_sockaddr_t *local_addr;
    apr_sockaddr_t *remote_addr;
    char *remote_ip;
    char *remote_host;
    char *remote_logname;
    unsigned aborted:1;
    int keepalive;
    signed int double_reverse:2;
    int keepalives;
    char *local_ip;
    char *local_host;
    long id; 
    struct ap_conf_vector_t *conn_config;
    apr_table_t *notes;
    struct ap_filter_t *input_filters;
    struct ap_filter_t *output_filters;
    void *sbh;
    struct apr_bucket_alloc_t *bucket_alloc;
};

struct request_rec
{
    apr_pool_t *pool;
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
    apr_time_t request_time;
    const char *status_line;
    int status;
    const char *method;
    int method_number;
    apr_int64_t allowed;
    apr_array_header_t *allowed_xmethods; 
    ap_method_list_t *allowed_methods; 
    apr_off_t sent_bodyct;
    apr_off_t bytes_sent;
    apr_time_t mtime;
    int chunked;
    const char *range;
    apr_off_t clength;
    apr_off_t remaining;
    apr_off_t read_length;
    int read_body;
    int read_chunked;
    unsigned expecting_100;
    apr_table_t *headers_in;
    apr_table_t *headers_out;
    apr_table_t *err_headers_out;
    apr_table_t *subprocess_env;
    apr_table_t *notes;
    const char *content_type;
    const char *handler;
    const char *content_encoding;
    apr_array_header_t *content_languages;
    char *vlist_validator;
    char *user;
    char *ap_auth_type;
    int no_cache;
    int no_local_copy;
    char *unparsed_uri;
    char *uri;
    char *filename;
    char *canonical_filename;
    char *path_info;
    char *args;

    /* rest of the brain damage removed */
};
