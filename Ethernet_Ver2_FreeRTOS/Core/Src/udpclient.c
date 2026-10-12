/*
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************

  File:		  udpclient.c
  Author:     ControllersTech.com
  Updated:    15 April 2022

  ***************************************************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ***************************************************************************************************************
*/


#include "cmsis_os.h"
#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "udpclient.h"
#include "string.h"

static struct netconn *conn;
static struct netbuf *buf, *rxbuf;

int indx = 0;
char smsg[200];

void udpsend(char *data);


static void udpinit_thread(void *arg)
{
	err_t err, recv_err;
	ip_addr_t dest_addr;
	/* Create a new connection identifier */
	conn = netconn_new(NETCONN_UDP);

	if (conn!= NULL)
	{
		/* Bind connection to the port 7 */
		err = netconn_bind(conn, IP_ADDR_ANY, 7);
		if (err == ERR_OK)
		{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 192, 168, 165, 125);
			/* connect to the destination (server) at port 8 */
			err = netconn_connect(conn, &dest_addr, 8);
			if (err == ERR_OK)
			{

				/* Got into infinite loop after initialisation */
				for (;;)
				{
						/* Receive data from the server */
						recv_err = netconn_recv(conn, &rxbuf);
						if (recv_err == ERR_OK)
						{
							/* copy the data into our buffer (smsg) */
							sprintf (smsg, "\"%s\" was sent by the Server\n", (char *) rxbuf->p->payload);
							udpsend(smsg);  // send the message to the server
							netbuf_delete(rxbuf);  // delete the netbuf
						}
				}
			}
		}
	}

	else
	{
		netconn_delete(conn);
	}
}

/*-------UDP Send function to send the data to the server-------------*/
void udpsend (char *data)
{
	buf = netbuf_new();   // Create a new netbuf
	netbuf_ref(buf, data, strlen(data));  // refer the netbuf to the data to be sent 
	netconn_send(conn,buf);  // send the netbuf to the client
	netbuf_delete(buf);  // delete the netbuf
}

/* UDP send Thread will send data every 500ms */
static void udpsend_thread(void *arg)
{
	for (;;)
	{
		sprintf (smsg, "index value = %d\n", indx++);
		udpsend(smsg);
		osDelay(500);
	}
}


void udpclient_init(void)
{
	sys_thread_new("udpsend_thread", udpsend_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	sys_thread_new("udpinit_thread", udpinit_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}


