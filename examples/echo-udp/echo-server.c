/**
 * \file
 *         A network application that listens on UDP port 50000 and echoes.
 * \author
 *         Clément Burin des Roziers <clement.burin-des-roziers@inrialpes.fr>
 */

#include "contiki.h"
#include "contiki-net.h"

#include "dev/leds.h"

#include <stdio.h>
#include <string.h>

#define PRINTF printf

#define UDP_DATA_LEN 120
#define UDP_HDR ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])


static struct uip_udp_conn *udpconn;
static uint8_t udpdata[UDP_DATA_LEN] = "rx=";

static void udphandler(process_event_t ev, process_data_t data)
{
    if (uip_newdata())
    {
        /* Set the last byte of the received data as 0 in order to print it. */
        int len = uip_datalen();
        ((char *)uip_appdata)[len] = 0;
        printf("Received from %u.%u.%u.%u:%u: '%s'\n", uip_ipaddr_to_quad(&UDP_HDR->srcipaddr), HTONS(UDP_HDR->srcport), (char*)uip_appdata);
        
        /* Prepare the response datagram in a local buffer */
        memcpy(udpdata, "rx=", 3);
        memcpy(udpdata+3, uip_appdata, len);
        
        /* Copy the information about the sender to the udpconn in order to reply */
        uip_ipaddr_copy(&udpconn->ripaddr , &UDP_HDR->srcipaddr); // ip address
        udpconn->rport = UDP_HDR->srcport; // UDP port
        
        /* Send the reply datagram */
        printf("sending back\n");
        uip_udp_packet_send(udpconn, udpdata, uip_datalen()+3);
        
        /* Restore the udpconn to previous setting in order to receive other packets */
        uip_ipaddr_copy(&udpconn->ripaddr , &uip_all_zeroes_addr);
        udpconn->rport = 0;
    }
}

/*---------------------------------------------------------------------------*/
/*
 * We declare the process.
 */
PROCESS(example_udp_server_process, "Example UDP server");
AUTOSTART_PROCESSES(&example_udp_server_process);
/*---------------------------------------------------------------------------*/
/*
 * The definition of the process.
 */
PROCESS_THREAD(example_udp_server_process, ev, data)
{
    
    PROCESS_BEGIN();
    printf("UDP Echo Server test\n");
    
    /* Create a UDP 'connection' with IP 0.0.0.0 and port 0 as remote host.
     * This means the stack will accepts UDP datagrams from any node. */
    udpconn = udp_new(NULL, HTONS(0), NULL);
    
    /* Bind the UDP 'connection' to the port 50000. That's the port we're listening on. */
    udp_bind(udpconn, HTONS(50000));

    printf("listening on UDP port %u\n", HTONS(udpconn->lport));
    
    while(1) {
        /* Wait until we have an event caused by tcpip interaction */
        PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
        /* Handle it */
        udphandler(ev, data);
    }
    
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
