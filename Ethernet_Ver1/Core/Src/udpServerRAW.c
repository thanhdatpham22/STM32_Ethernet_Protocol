/*
  ***************************************************************************************************************
  ***************************************************************************************************************
  ***************************************************************************************************************

  File:		  udpServerRAW.c
  Author:     ControllersTech.com
  Updated:    Jul 23, 2021

  ***************************************************************************************************************
  Copyright (C) 2017 ControllersTech.com

  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.

  ***************************************************************************************************************
*/


#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "stdio.h"
#include "udpServerRAW.h"
#include <string.h>
int counter_udp;
struct udp_pcb *upcb;
char buffer[100];
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void udpClient_send(void);

/* IMPLEMENTATION FOR UDP Server :   source:https://www.geeksforgeeks.org/udp-server-client-implementation-c/

1. Create UDP socket.
2. Bind the socket to server address.
3. Wait until datagram packet arrives from client.
4. Process the datagram packet and send a reply to client.
5. Go back to Step 3.
*/

void udpServer_init(void)
{
	// UDP Control Block structure
   struct udp_pcb *upcb;
   err_t err;

   /* 1. Create a new UDP control block  */
   upcb = udp_new();

   /* 2. Bind the upcb to the local port */
   ip_addr_t myIPADDR;
   IP_ADDR4(&myIPADDR, 192, 168, 165, 41);

   err = udp_bind(upcb, &myIPADDR, 7);  // 7 is the server UDP port


   /* 3. Set a receive callback for the upcb */
   if(err == ERR_OK)
   {
	   udp_recv(upcb, udp_receive_callback, NULL);
   }
   else
   {
	   udp_remove(upcb);
   }
}
void udpClient_connect(void)
{

	 err_t err;
	 upcb = udp_new();
	 ip_addr_t myIPADDR;
	 IP_ADDR4(&myIPADDR, 192, 168, 165, 41);
	 err = udp_bind(upcb, &myIPADDR, 8);

	 /* configure destination IP address and port */
	 ip_addr_t DestIPaddr;
	 IP_ADDR4(&DestIPaddr, 192, 168, 165, 125);
	 err = udp_connect(upcb, &DestIPaddr, 7);
	 if (err == ERR_OK)
	 {
		 /* 2. Send message to server */
		 udpClient_send();

		 /* 3. Set a receive callback for the upcb */
		 udp_recv(upcb, udp_receive_callback, NULL);
	 }

}

static void udpClient_send(void)
{
    struct pbuf *txBuf;
    char data[100];

    int len = sprintf(data, "sending UDP client message %d \r\n", counter_udp);

    /* allocate pbuf from pool */
    txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

    if (txBuf != NULL)
    {
        /* copy data to pbuf */
        pbuf_take(txBuf, data, len);

        /* send udp data */
        udp_send(upcb, txBuf);

        /* free pbuf */
        pbuf_free(txBuf);
    }
}
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
////    udpClient_send();
//	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13);
//}

// udp_receive_callback will be called, when the client sends some data to the server
/* 4. Process the datagram packet and send a reply to client. */

//void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
//{
//	struct pbuf *txBuf;
//
//	/* Get the IP of the Client */
//	char *remoteIP = ipaddr_ntoa(addr);
//
//	char buf[100];
//
//
//	int len = sprintf (buf,"Hello %s From UDP SERVER\n", (char*)p->payload);
//
//	/* allocate pbuf from RAM*/
//	txBuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
//
//	/* copy the data into the buffer  */
//	pbuf_take(txBuf, buf, len);
//
//	/* Connect to the remote client */
//	udp_connect(upcb, addr, port);
//
//	/* Send a Reply to the Client */
//	udp_send(upcb, txBuf);
//
//	/* free the UDP connection, so we can accept new clients */
//	udp_disconnect(upcb);
//
//	/* Free the p_tx buffer */
//	pbuf_free(txBuf);
//
//	/* Free the p buffer */
//	pbuf_free(p);
//}
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,
                           const ip_addr_t *addr, u16_t port)
{
	 struct pbuf *txBuf;
    /* Copy the data from the pbuf */
    strncpy(buffer, (char *)p->payload, p->len);

    /* increment message count */
    counter_udp++;
    int len = sprintf(buffer, "Received %s From UDP Server! \r\n", (char*)p->payload);
    txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

    if (txBuf != NULL)
      {
          /* copy data to pbuf */
          pbuf_take(txBuf, buffer, len);

          /* send udp data */
          udp_send(upcb, txBuf);

          /* free pbuf */
          pbuf_free(txBuf);
      }
    /* Free receive pbuf */
    pbuf_free(p);
}
