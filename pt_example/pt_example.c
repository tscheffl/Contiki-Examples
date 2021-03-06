#include "contiki.h"
#include <stdio.h>

PROCESS(pt_example, "Protothread Example Process");
AUTOSTART_PROCESSES(&pt_example);
PROCESS_THREAD(pt_example, ev, data)
{
	/*Timer statisch deklariert, damit er nach Kontextwechsel noch verfuegbar ist*/
	static struct etimer et;

	PROCESS_BEGIN();
	etimer_set(&et, (CLOCK_SECOND*10));
	
	while(1) 
	{
		/*Warte 10s, solange koennen andere Protothreads den Prozessor nutzen*/
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
		printf("Timer abgelaufen, starte Timer neu...\n");
  	}
  	PROCESS_END();
}
