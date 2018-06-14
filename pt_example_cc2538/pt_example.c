#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>

PROCESS(pt_example, "Protothread Example Process");
AUTOSTART_PROCESSES(&pt_example);
PROCESS_THREAD(pt_example, ev, data)
{
	/*Timer statisch deklariert, damit er nach Kontextwechsel noch verfuegbar ist*/
	static struct etimer et;
    static uint16_t runtime = 0; /*may wrap eventually*/
    
	PROCESS_BEGIN();
	etimer_set(&et, CLOCK_SECOND);
	
	while(1) 
	{
		/*Warte 1s, solange koennen andere Protothreads den Prozessor nutzen*/
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
        runtime++;
        leds_toggle(LEDS_YELLOW);
        printf("Runtime in Seconds: %i\n", runtime);
  	}
  	PROCESS_END();
}
