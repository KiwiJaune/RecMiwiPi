#ifndef __USER_ANALYSE_TRAME__
#define __USER_ANALYSE_TRAME__

#include "UserMiwi.h"
#include "UserTrame.h"
#include "UserAckMiwi.h"
#include "UserUdp.h"

#define CMD_ENVOI_MIWI 0xA0
#define CMD_TEST_CONNEXION 0xF0

#define CMD_ACK_UDP 0xA1
#define CMD_ENVOI_MIWI_TO_UDP 0xA2

void AnalyseTrameUdp(Trame trame);
void MiwiToUdp(Trame trame);

#endif
