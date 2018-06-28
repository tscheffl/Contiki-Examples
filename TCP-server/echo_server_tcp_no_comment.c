/*
 * Example from http://senstools.gforge.inria.fr/doku.php?id=contiki:examples#echo_server_tcp
 */

#include "contiki.h"
#include "contiki-net.h"

#include "dev/leds.h"

#include <stdio.h>
#include <string.h>

static struct psock ps;
/*
 * Incoming data usees a 50 byte buffer.
 */
static char buffer[50];


static PT_THREAD(handle_connection(struct psock *p))
{
  PSOCK_BEGIN(p);

  PSOCK_SEND_STR(p, "Welcome, please type something and press return.\n");
  PSOCK_READTO(p, '\n');

  PSOCK_SEND_STR(p, "Got the following data: ");
  PSOCK_SEND(p, buffer, PSOCK_DATALEN(p));
  PSOCK_SEND_STR(p, "Good bye!\r\n");

  PSOCK_CLOSE(p);
  PSOCK_END(p);
}

PROCESS(example_psock_server_process, "Example protosocket server");
AUTOSTART_PROCESSES(&example_psock_server_process);

PROCESS_THREAD(example_psock_server_process, ev, data)
{
  PROCESS_BEGIN();

  tcp_listen(UIP_HTONS(12345));
  while(1)
  {

  PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
  if(uip_connected()) {
      PSOCK_INIT(&ps, buffer, sizeof(buffer));

      /* We loop until the connection is aborted, closed, or times out. */
      while(!(uip_aborted() || uip_closed() || uip_timedout()))
      {
          PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
          handle_connection(&ps);
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
