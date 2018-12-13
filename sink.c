#include "contiki.h"
#include "net/rime/rime.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Sink");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;



/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;
 // holds the values of the nodes allows me to specify the parent and position in the tree
 struct key_values
{
int hopcount;
int sequencenumber;
};
// allows me to access the variabls which hold the node information
struct key_values values;
 
 
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
  PROCESS_BEGIN();
// i open a broadcast band connection at 129
  broadcast_open(&broadcast, 129, &broadcast_call);
while (1){


    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4+ random_rand() % (CLOCK_SECOND * 4));
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    // sets the data copied within the range that the struct takes up memory for the node information
    packetbuf_copyfrom(&values, sizeof(struct key_values));
    static int sequenceNumber =0;
    // setting the sequence number and the hopcount to 0 and are allowed to increase per node.
    values.sequencenumber = sequenceNumber; 
    values.hopcount=0;
   
    broadcast_send(&broadcast);
 sequenceNumber++;
    printf("broadcast message sent \n");
 
// if the sequence limit is reached the values are reset
  if (values.sequencenumber>21)
{
  values.sequencenumber = 0;
  values.hopcount = 0;
  packetbuf_copyfrom(&values, sizeof(struct key_values));
  printf("Broadcast message sent From Sink\n");
  broadcast_send(&broadcast);

}}
PROCESS_END();
}
/*---------------------------------------------------------------------------*/