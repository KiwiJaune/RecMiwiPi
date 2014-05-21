#include "UserAnalyseTrame.h"

void AnalyseTrameUdp(Trame trame)
{
	unsigned char destinataire;
	unsigned char typeMessage;
	
		
	if(trame.message[0]!= MY_SHORT_ADDRESS)
		return;
	
	switch (trame.message[1])
	{
		case CMD_ENVOI_MIWI : 
			//Type : Bloquant ou Buffer
			typeMessage = trame.message[2];
			destinataire = 0xC2;//trame.message[3];
			//Envoi miwi de la trame recu qui suit
			//les 2 premiers octects
			trame.message = &trame.message[3];
			trame.nbChar -= 3;		
			EnvoiMiwi(destinataire,typeMessage,trame);
			

		break;
		case CMD_TEST_CONNEXION :
			//Echo de la trame recu vers UDP
			EnvoiUserUdp(trame);   
		break;		
	}
}


void MiwiToUdp(Trame t)
{
	int i = 0;
		
	Trame trame;
	static BYTE mess[50];
	mess[0] = MY_SHORT_ADDRESS;
	mess[1] = CMD_ENVOI_MIWI_TO_UDP;
	trame.message = mess;
	trame.nbChar = t.nbChar+2;	
	
	for(i = 0; i < t.nbChar ; i++)
		trame.message[i+2] = t.message[i];
		
	//Send to udp
	EnvoiUserUdp(trame);
}
