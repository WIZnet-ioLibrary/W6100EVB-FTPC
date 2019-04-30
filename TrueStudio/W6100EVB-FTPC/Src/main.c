#include <stdio.h>
#include "wizchip_conf.h"
#include "loopback.h"
#include "board_init.h"
#include "ftpc.h"
#include "Application.h"



wiz_NetInfo gWIZNETINFO = { .mac = {0x00,0x08,0xdc,0xff,0xff,0xff},
                            .ip = {192,168,100,25},
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 100, 1},
                            .dns = {168, 126, 63, 1},
                            //.dhcp = NETINFO_STATIC,
                            .lla={0xfe,0x80,0x00,0x00,
                                    0x00,0x00, 0x00,0x00,
                                    0x02,0x08, 0xdc,0xff,
                                    0xfe,0xff, 0xff,0xff},   ///< Source Link Local Address
                            .gua={0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00},   ///< Source Global Unicast Address
                            .sn6={0xff,0xff,0xff,0xff,
                                    0xff,0xff,0xff,0xff,
                                    0x00,0x00,0x00, 0x00,
                                    0x00,0x00,0x00,0x00 },   ///< IPv6 Prefix
                            .gw6={0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00}   ///< Gateway IPv6 Address

};



uint8_t gFTPBUF[_FTP_MAX_MSG_SIZE_];


void print_network_information(void);

int main(void)
{

    uint8_t syslock = SYS_NET_LOCK;
    uint8_t FTP_destip6[16] = {0xfe,0x80, 0x00,0x00,  // For FTP IPv6 client examples; destination network info
                            0x00,0x00, 0x00,0x00,
                            0x31,0x71,0x98,0x05,
                            0x70,0x24,0x4b,0xb1
                                                    };

    uint8_t FTP_destip4[4]={192,168,0,232};


    BoardInitialze();

    ctlwizchip(CW_SYS_UNLOCK,& syslock);
    ctlnetwork(CN_SET_NETINFO,&gWIZNETINFO);

    printf("Register value after W6100 initialize!\r\n");

    print_network_information();

    ftpc_init(FTP_destip6, 21, AS_IPV6);

    while(1)
    {
        ftpc_run(gFTPBUF);
   }
}



void delay(unsigned int count)
{
    int temp;
    temp = count + TIM2_gettimer();
    while(temp > TIM2_gettimer()){}
}

void print_network_information(void)
{
    wizchip_getnetinfo(&gWIZNETINFO);

    printf("Mac address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",gWIZNETINFO.mac[0],gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
    printf("IP address : %d.%d.%d.%d\n\r",gWIZNETINFO.ip[0],gWIZNETINFO.ip[1],gWIZNETINFO.ip[2],gWIZNETINFO.ip[3]);
    printf("SM Mask    : %d.%d.%d.%d\n\r",gWIZNETINFO.sn[0],gWIZNETINFO.sn[1],gWIZNETINFO.sn[2],gWIZNETINFO.sn[3]);
    printf("Gate way   : %d.%d.%d.%d\n\r",gWIZNETINFO.gw[0],gWIZNETINFO.gw[1],gWIZNETINFO.gw[2],gWIZNETINFO.gw[3]);
    printf("DNS Server : %d.%d.%d.%d\n\r",gWIZNETINFO.dns[0],gWIZNETINFO.dns[1],gWIZNETINFO.dns[2],gWIZNETINFO.dns[3]);

    print_ipv6_addr("GW6 ", gWIZNETINFO.gw6);
    print_ipv6_addr("LLA ", gWIZNETINFO.lla);
    print_ipv6_addr("GUA ", gWIZNETINFO.gua);
    print_ipv6_addr("SUB6", gWIZNETINFO.sn6);

    printf("\r\nNETCFGLOCK : %x\r\n", getNETLCKR());
}

void print_ipv6_addr(uint8_t* name, uint8_t* ip6addr)
{
    printf("%s : ", name);
    printf("%04X:%04X", ((uint16_t)ip6addr[0] << 8) | ((uint16_t)ip6addr[1]), ((uint16_t)ip6addr[2] << 8) | ((uint16_t)ip6addr[3]));
    printf(":%04X:%04X", ((uint16_t)ip6addr[4] << 8) | ((uint16_t)ip6addr[5]), ((uint16_t)ip6addr[6] << 8) | ((uint16_t)ip6addr[7]));
    printf(":%04X:%04X", ((uint16_t)ip6addr[8] << 8) | ((uint16_t)ip6addr[9]), ((uint16_t)ip6addr[10] << 8) | ((uint16_t)ip6addr[11]));
    printf(":%04X:%04X\r\n", ((uint16_t)ip6addr[12] << 8) | ((uint16_t)ip6addr[13]), ((uint16_t)ip6addr[14] << 8) | ((uint16_t)ip6addr[15]));
}
