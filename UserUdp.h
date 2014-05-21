/************************************************
*
* UserUdp.h
* Fichier permettant de g�rer une communication UDP
* Couche interm�diaire entre le main et le fichier udp.c
*
*
*****************************************************/

#ifndef __USER_UDP__
#define __USER_UDP__

#include "TCPIP Stack/TCPIP.h"
#include "UserTrame.h"

#define portReception 12323
#define portEmission 12313

void InitUserUdp();
Trame ReceptionUserUdp();
void EnvoiUserUdp();
void memclr(void * dest, WORD size);

#endif
