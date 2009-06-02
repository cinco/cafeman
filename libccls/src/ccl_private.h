#ifndef CCL_PRIVATE_H
#define CCL_PRIVATE_H

#include <time.h>
#include <glib.h>
#include <sqlite3.h>

#ifndef G_OS_WIN32
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#else
#include <windows.h>
#include <winuser.h>
#include <winsock2.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#ifndef G_OS_WIN32
#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#else
typedef gint socklen_t;
#endif

struct _Events_list
{
  fd_set		readfds;
  gint			maxfd;
  gint			listenfd;
  CCL_on_event_cb	on_event;
  void		       *on_event_data;
  CCL_on_connect_cb	on_connect;
  void		       *on_connect_data;
  CCL_on_disconnect_cb	on_disconnect;
  void		       *on_disconnect_data;
  SSL_CTX	       *ssl_ctx;
  BIO		       *listenbio;
  gchar		       *certpass;
};
typedef struct _Events_list Events_list;

/* App instance struct */
struct _CCL
{
  sqlite3	       *db;
  struct sockaddr_in	server_addr;
  gint			tarif_id;
  GSList	       *tarif;
  GData		       *tarifs;
  GData		       *clients;
  GData		       *members;
  gint			perminafter;
  Events_list		events;
};
typedef struct _CCL CCL;

/* Tarifpart struct */
struct _CCL_tarifpart
{
  gint	  id;
  guint	  hprice;
  gint	  stime;
  GData **prices;
};
typedef struct _CCL_tarifpart CCL_tarifpart;

/* Time interval struct */
struct _CCL_interval
{
  time_t stime;
  time_t etime;
};
typedef struct _CCL_interval CCL_interval;

/* Member struct */
struct _CCL_member
{
  gchar	  *name;
  gint	  tarif;
  gchar	  *email;
  gchar	  *other;
  gint32  flags;
  gpointer data;
};
typedef struct _CCL_member CCL_member;

/* Client struct */
struct _CCL_client
{
  gint	     sockfd;
  gchar	    *name;
  time_t     timeout;
  GPtrArray *intervals;
  GSList    *products;
  guint8     status;
  gint	     member;
  gint32     flags;
  gpointer   data;
  BIO	    *bio;
};
typedef struct _CCL_client CCL_client;

gint _recvall(BIO * bio, void * buf, gint len);
gint _sendall(BIO * bio, const void * buf, gint len);
gint _TarifCompareFunc(gconstpointer a, gconstpointer b);
gboolean _CCL_member_restore(gint id);
gboolean _CCL_client_restore(gint id);
void _CCL_tarif_clear(void);
void _destroy_member(gpointer data);
void _shutdown_connection(CCL_client * client);
void _destroy_client(gpointer data);
void _CCL_member_init(CCL_member * member, const gchar * name);
void _CCL_client_init(CCL_client * client, const gchar * name);

#endif
