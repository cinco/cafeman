/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * mca_network.h
 * Copyright (C) Bernard Owuor 2010 <owuor@unwiretechnologies.net>
 * 
 * mkahawa-client-applet is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * mkahawa-client-applet is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MCA_NETWORK_H
#define MCA_NETWORK_H

#include <gtk/gtk.h>

#include "mca_types.h"

#define MAX_RCV_BUF_SIZE   1024

//#define USE_IO_STREAM
typedef struct _MCACnxn{
	guint16              port;
	gint                 uses_ssl;
	gchar               *server_name;
	gchar               *client_name;
	gchar               *user_name;
	gchar               *cert_passwd;
	cmd_callback_t       cmd_cb;
	status_callback_t    status_cb;
	gboolean             connected;
	gint                 retrysec;
	GThread             *cnxn_thread;
	gint                 cmd;
	gchar                cmd_data[MAX_RCV_BUF_SIZE];
	guint32              cmd_data_len;
	//	guint32              rcv_cmd;
	//	gchar                rcv_cmd_data[MAX_RCV_BUF_SIZE];
	//	guint32              rcv_cmd_data_len;
	gint                 sock_fd;
	gpointer             conn_priv;
} MCACnxn;
	
gint32 mca_network_init (MCACnxn *cnxn, cmd_callback_t exec_net_cmd, status_callback_t status_cb);
gint32 mca_network_start (MCACnxn *cnxn);
gint32 mca_network_wait_for_cmd (MCACnxn *cnxn);
gint32 mca_network_get_cmd_data (MCACnxn *cnxn);
gint32 mca_network_send_cmd (MCACnxn *cnxn, guint32 cmd, void *data, gint32 datalen);
gint32 mca_network_get_status (MCACnxn *cnxn);
cnxn_status_t mca_network_cnxn_thread (MCACnxn *cnxn);
void mca_network_close_connection(MCACnxn *cnxn);

#endif
