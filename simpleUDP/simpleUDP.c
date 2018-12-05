/**
 * \file
 *         A very simple Contiki UDP application
 * \author
 *         Thomas Scheffler, Sven Zehl
 */
 
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "net/netstack.h"
#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
#define UDP_IP_BUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
 
/*---------------------------------------------------------------------------*/
PROCESS(simpleUDP_process, "Simple UDP process");
AUTOSTART_PROCESSES(&simpleUDP_process);
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *udpconn;
#define MAX_PAYLOAD_LEN 120
 
 
void bsp_udphandler(void)
{
  char buf[MAX_PAYLOAD_LEN];
  if(uip_newdata()) 
  {
    /*Zeige Benutzer den Inhalt der empfangenen Nachricht*/
    /*Setze letztes Byte zu Null, fuer String Ende*/
    ((char *)uip_appdata)[uip_datalen()] = 0;
    printf("Server received: %s\n", (char *)uip_appdata);
 
    /*Verwende die Quell- als Zieladresse fuer Antwort */
    uip_ipaddr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
    /*IPv6*/
    //uip_ip6addr_copy(&udpconn->ripaddr, &UDP_IP_BUF->srcipaddr);
    udpconn->rport = UDP_IP_BUF->srcport;
 
    /*Schreibe Antwort-Daten in Buffer*/
    sprintf(buf, "Hello from the server");
 
    /*Versende das Antwort-Packet*/
    uip_udp_packet_send(udpconn, buf, strlen(buf));
 
    /*Setze Adresse/Port in Verbindungsstruktur auf Null,*/
    /*um von jedem Absender Daten empfangen zu koennen*/
    memset(&udpconn->ripaddr, 0, sizeof(udpconn->ripaddr));
    udpconn->rport = 0;
  }
}
 
PROCESS_THREAD(simpleUDP_process, ev, data)
{
  PROCESS_BEGIN();
 
  printf("UDP Echo Server\n");
 
  /*Starte neue UDP Verbindung mit IP 0.0.0.0 und Port 0, */
  /* d.h. akzeptiere jede ankommende Verbindung*/
  udpconn = udp_new(NULL, UIP_HTONS(0), NULL); 
 
  /*Setze den Port auf dem gelauscht wird auf 49999*/
  /*HTONS() uebersetzt zur Network Byte Order*/
  udp_bind(udpconn, UIP_HTONS(49999)); 
  printf("listening on UDP port %u\n", UIP_HTONS(udpconn->lport));
  while(1) {
    /* Warte bis ein TCP/IP Event eintrifft */
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
 
    /*Rufe die Handler-Funktion auf*/
    bsp_udphandler();
  }
 
  PROCESS_END();
}
