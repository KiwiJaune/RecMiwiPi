#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UserUdp.h"
#include "UserMiwi.h"
#include "UserAckMiwi.h"
#include "MRF24J40.h"
#include "UserTrame.h"
#include "radiohardware.h"
#include "UserAnalyseTrame.h"

//Clock configuration
//Clock set on OSC1, primary external oscillator (8MHz)
_FOSCSEL(FNOSC_PRI)                          	//Primary osc
_FOSC(POSCMD_EC & FCKSM_CSECMD & OSCIOFNC_ON)  //Primary Oscillator Source (EC Oscillator Mode)
_FPOR(FPWRT_PWR1)
_FWDT(FWDTEN_OFF)								
_FICD(ICS_PGD1 & JTAGEN_OFF)

//--UDP---
APP_CONFIG AppConfig;
static void InitAppConfig(void);
void InitializeBoard(void);

void delay(BYTE mult){
    long i = 400000/mult;
    while(i--);
}

int main(int argc, char** argv) 
{	
	char expediteurAck = 0;
	
	
	//Trame Test Pingpong
	Trame trameRecuMiwi;	
	
	//Envoi Ack vers PC pour les messages bloquant
	Trame trameAckUdp;
	static BYTE messAckUdp[3];
	// Pour RecMiwiPi les messages viennent toujours de RecPi = 0xC3
	messAckUdp[0] = 0xC3;
	messAckUdp[1] = CMD_ACK_UDP;
	trameAckUdp.message = messAckUdp;
	trameAckUdp.nbChar = 3;	
	
	//Trame d'initialisation
	Trame trameInit;
	static BYTE mess[3];
	mess[0]=0xC3;
	mess[1]=0xF0;
	mess[2]=0xCA;
	trameInit.message = mess;
	trameInit.nbChar = 3;	
		
	//Gestion des messages UDP
    Trame trameUdp;
	
    //Ethernet init
    InitializeBoard();
    InitAppConfig();
    UDPInit();
    StackInit();
    InitUserUdp();
    
    //Miwi init
	InitMiwi();
	
	//Gestion LED Carte Miwi
	//(A enlever sur les balises)	
	TRISCbits.TRISC7 = 0;
	LED = 1;
	
	//EnvoiMiwi(0xE2,BUFFER,trameInit);
	DelayMs(10);
	EnvoiUserUdp(trameInit);
	DelayMs(10);
	EnvoiUserUdp(trameInit);
	DelayMs(10);
	EnvoiUserUdp(trameInit);
	DelayMs(10);
	EnvoiUserUdp(trameInit);
	DelayMs(10);
	EnvoiUserUdp(trameInit);
	
	while(1)	
	{				
			
		//--UDP Reception
	    StackTask();   
        trameUdp = ReceptionUserUdp();        
        if(trameUdp.nbChar != 0)
        {
	        //Analyse de la trame recu par UDP
	        AnalyseTrameUdp(trameUdp);
        }
        StackApplications();        
		        
        //--Miwi Reception
        MiwiTasks();        
		if(MiwiIsDataReady())
		{
			//Recupere la trame Miwi et envoi en UDP 
			EnvoiUserUdp(MiwiGetData());
			trameRecuMiwi = MiwiGetData();
		}
		//--Acquittement des msgs bloquant vers PC
		expediteurAck = MiwiIsAck();		

		if(expediteurAck)
		{
			// RecPi n'a pas besoin des acquittements du PC
			/*trameAckUdp.message[2] = expediteurAck;
			delay(100);
			EnvoiUserUdp(trameAckUdp);
			delay(100);*/
			
		}
		
	}
	
    return (EXIT_SUCCESS);
}

void InitializeBoard(void){

    AD1PCFGL = 0xFFFF;
    RPOR1bits.RP3R    = 0b01000;         // SCK1 RP3
    RPOR2bits.RP4R    = 0b00111;         // SDO1 RP4
    RPINR20bits.SDI1R = 0b00010;         // SDI1 RP2
}

static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
static void InitAppConfig(void){
	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;
	memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
//	{
//		_prog_addressT MACAddressAddress;
//		MACAddressAddress.next = 0x157F8;
//		_memcpy_p2d24((char*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
//	}
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;


	// SNMP Community String configuration
	#if defined(STACK_USE_SNMP_SERVER)
	{
		BYTE i;
		static ROM char * ROM cReadCommunities[] = SNMP_READ_COMMUNITIES;
		static ROM char * ROM cWriteCommunities[] = SNMP_WRITE_COMMUNITIES;
		ROM char * strCommunity;

		for(i = 0; i < SNMP_MAX_COMMUNITY_SUPPORT; i++)
		{
			// Get a pointer to the next community string
			strCommunity = cReadCommunities[i];
			if(i >= sizeof(cReadCommunities)/sizeof(cReadCommunities[0]))
				strCommunity = "";

			// Ensure we don't buffer overflow.  If your code gets stuck here,
			// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h
			// is either too small or one of your community string lengths
			// (SNMP_READ_COMMUNITIES) are too large.  Fix either.
			if(strlenpgm(strCommunity) >= sizeof(AppConfig.readCommunity[0]))
				while(1);

			// Copy string into AppConfig
			strcpypgm2ram((char*)AppConfig.readCommunity[i], strCommunity);

			// Get a pointer to the next community string
			strCommunity = cWriteCommunities[i];
			if(i >= sizeof(cWriteCommunities)/sizeof(cWriteCommunities[0]))
				strCommunity = "";

			// Ensure we don't buffer overflow.  If your code gets stuck here,
			// it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h
			// is either too small or one of your community string lengths
			// (SNMP_WRITE_COMMUNITIES) are too large.  Fix either.
			if(strlenpgm(strCommunity) >= sizeof(AppConfig.writeCommunity[0]))
				while(1);

			// Copy string into AppConfig
			strcpypgm2ram((char*)AppConfig.writeCommunity[i], strCommunity);
		}
	}
	#endif

	// Load the default NetBIOS Host Name
	memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
	FormatNetBIOSName(AppConfig.NetBIOSName);

	#if defined(ZG_CS_TRIS)
		// Load the default SSID Name
		if (sizeof(MY_DEFAULT_SSID_NAME) > sizeof(AppConfig.MySSID))
		{
		    ZGSYS_DRIVER_ASSERT(5, (ROM char *)"AppConfig.MySSID[] too small.\n");
		}
		memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
	#endif

	#if defined(EEPROM_CS_TRIS)
	{
		BYTE c;

	    // When a record is saved, first byte is written as 0x60 to indicate
	    // that a valid record was saved.  Note that older stack versions
		// used 0x57.  This change has been made to so old EEPROM contents
		// will get overwritten.  The AppConfig() structure has been changed,
		// resulting in parameter misalignment if still using old EEPROM
		// contents.
		XEEReadArray(0x0000, &c, 1);
	    if(c == 0x60u)
		    XEEReadArray(0x0001, (BYTE*)&AppConfig, sizeof(AppConfig));
	    else
	        SaveAppConfig();
	}
	#elif defined(SPIFLASH_CS_TRIS)
	{
		BYTE c;

		SPIFlashReadArray(0x0000, &c, 1);
		if(c == 0x60u)
			SPIFlashReadArray(0x0001, (BYTE*)&AppConfig, sizeof(AppConfig));
		else
			SaveAppConfig();
	}
	#endif
}
