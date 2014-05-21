#ifndef __USER_MIWI__
#define __USER_MIWI__

#include "MRF24J40.h"
#include "UserTrame.h"
#include "UserAckMiwi.h"
#include "radioAddress.h"
#include "radiohardware.h"

void InitMiwi(void);
void RadioInitP2P(void);
void Envoi(char destinataire, Trame trame);
void EnvoiAck(char destinataire, Trame trame);
void EnvoiDebug(char a, char b, char c, char d);
int CalculChecksum(Trame trame, char checksumPresent);

#define MY_CHANNEL 23
#define	DEFAULT_PAYLOAD 30
#define DEST_SHORT_ADDR 0xC200

#endif
