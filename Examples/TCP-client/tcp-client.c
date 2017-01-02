/*
 * Copyright (c) 2017, Thomas Scheffler.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         A simple Contiki TCP client application
 * \author
 *         Thomas Scheffler <scheffler@beuth-hochschule.de>
 */


#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#define SEND_INTERVAL		CLOCK_SECOND
#define PORT_NUMBER		1234
#define MAX_PAYLOAD_LEN		40

static struct tcp_socket socket;
#define BUFSIZE 400
static uint8_t in_buf[BUFSIZE];
static uint8_t out_buf[BUFSIZE];

static struct etimer et;
static uip_ipaddr_t addr;
static int status;

#define STATUS_TCP_UNCONNECTED 0
#define STATUS_TCP_CONNECTING 1
#define STATUS_TCP_CONNECTED 2
#define STATUS_MESSAGE_SENT 3
#define STATUS_MSG_ACK_RECEIVED 4


/*--------------Prototypes---------------------------------------------------*/
static void connect_tcp();
static void abort_connection();
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS(tcp_client_process, "TCP client process");
AUTOSTART_PROCESSES(&tcp_client_process);
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
static void
periodic_message(void)
{
  //if (status == STATUS_TCP_CONNECTED)
  {
    printf("Client sending message...\n");
    tcp_socket_send_str(&socket, "Hello World.\n");
  }
  status = STATUS_MESSAGE_SENT; 
}
/*---------------------------------------------------------------------------*/
static void
event(struct tcp_socket *s, void *ptr,
      tcp_socket_event_t ev)
{
  printf("  event %d - ", ev);
  if(ev == TCP_SOCKET_CONNECTED) 
  {
    printf("connected\n");
    status = STATUS_TCP_CONNECTED;
  }
  else if (ev == TCP_SOCKET_CLOSED || ev == TCP_SOCKET_TIMEDOUT || ev == TCP_SOCKET_ABORTED)
  {
    printf("aborting\n");
    abort_connection();
  }
  else if (ev == TCP_SOCKET_DATA_SENT)
  {
    printf("ack rcvd.\n");
    status = STATUS_MSG_ACK_RECEIVED;  
  }
  else 
  {
    printf("other event\n");
  }
}
/*---------------------------------------------------------------------------*/
static int
input(struct tcp_socket *s, void *ptr,
      const uint8_t *inputptr, int inputdatalen)
{
  printf("input %d bytes '%s'\n", inputdatalen, inputptr);
    /* Discard everything */
    return 0; /* all data consumed */
}
/*---------------------------------------------------------------------------*/
static void
abort_connection()
{
  tcp_socket_close(&socket);
  tcp_socket_unregister(&socket);

  status = STATUS_TCP_UNCONNECTED;
  printf("Connection aborted.\n");
}
/*---------------------------------------------------------------------------*/
static void
connect_tcp()
{
  status = STATUS_TCP_CONNECTING;

  if (tcp_socket_register(&socket, NULL,
               in_buf, BUFSIZE,
               out_buf, BUFSIZE,
               input, event) < 0)
  {
    printf("Error: tcp_socket_register \n");
//    PROCESS_END();
  }

  printf("Connecting...\n\r");
  uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x1);
  if (tcp_socket_connect(&socket, &addr, PORT_NUMBER) < 0)
  {
    printf("Error: tcp_socket_connect \n");
//    PROCESS_END();
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_client_process, ev, data)
{
  PROCESS_BEGIN();
  PROCESS_PAUSE();
  PRINTF("\n# # # # # # #\n");
  PRINTF("TCP client process started. \n  Configure TAP-interface and press key to continue... \n");
  getchar();

  printf("Clock system time: %lu sec\n",(unsigned long)clock_seconds());
  etimer_set(&et, CLOCK_SECOND*1);

  while(1) 
  {
    PROCESS_WAIT_EVENT();
    
    if(status == STATUS_TCP_UNCONNECTED) 
    {
      printf("1st Connect system time: %lu sec\n",(unsigned long)clock_seconds());
      connect_tcp();
    }
    else if(status == STATUS_TCP_CONNECTING)
    {
      printf("Connecing: %lu sec\n",(unsigned long)clock_seconds());
    }
    else if(etimer_expired(&et)&&(status==STATUS_TCP_CONNECTED||status==STATUS_MSG_ACK_RECEIVED))
    {
      printf("Periodic system time: %lu sec - ",(unsigned long)clock_seconds());
      etimer_set(&et, CLOCK_SECOND*5);
      periodic_message();
    }
    etimer_restart(&et);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
