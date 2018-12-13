#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(broadcast_process, "sensor process");
AUTOSTART_PROCESSES(&broadcast_process);
/*---------------------------------------------------------------------------*/
// struct which holds the values of the nodes
struct key_values
{
int hopcount;
int sequencenumber;
};
// the struct variable is called pointer
struct key_values pointer;
static struct broadcast_conn broadcast;
// address of the parent
static linkaddr_t parent;
static int sequencenumber = -1;
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
struct key_values *values;
values = packetbuf_dataptr(); 
// set current value of the struct
pointer.sequencenumber = values->sequencenumber;
  pointer.hopcount = values->hopcount ;
// if the values of the nodes hop count == zero then acknowledge this and set the info of values to pointer and this node is the parent this is the root also
if (values -> hopcount == 0 ){
      printf("Hop Count: %d\nSequence Number: %d\n", pointer.hopcount, pointer.sequencenumber);
// the node that is sending the broadcast is now the new parent     
      parent.u8[0] = from-> u8[0];
      parent.u8[1] = from-> u8[0];
// sets the node information inside the struct
      pointer.hopcount = values->hopcount+1;
      pointer.sequencenumber = values->sequencenumber;
// copys the data the size of struct and sends broacast
      packetbuf_copyfrom(&pointer, sizeof(struct key_values));
      broadcast_send(&broadcast);
}
else {
// if there is a parent sets the values of the struct makes the broadcat node the parent and then broadcast the information of the struct
    if(pointer.sequencenumber > sequencenumber && ((from->u8[0] == parent.u8[0]) && (from->u8[1] == parent.u8[1]))){
         printf("Hop Count: %d\nSequence Number: %d\n", values->hopcount, values->sequencenumber);
         pointer.hopcount = values->hopcount +1;
         pointer.sequencenumber = values->sequencenumber;
         parent.u8[0] = from->u8[0];
         parent.u8[1] = from->u8[1];
         pointer.hopcount = values -> hopcount + 1;

// copys the data the size of struct and sends broacast
        packetbuf_copyfrom(&pointer, sizeof(struct key_values));
        broadcast_send(&broadcast);
    }
}
    
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(broadcast_process, ev, data)
{
  
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    
  PROCESS_BEGIN();
  broadcast_open(&broadcast, 129, &broadcast_call);

 
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/