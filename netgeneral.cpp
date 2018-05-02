/**************************************************************************
 *  Copyright 2008 Anmar Al-Tamimi <anmarhasan@gmail.com>
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 *  AHCLIB 2008
 */

#include "lib.h"
#include "general.h"
#include "netgeneral.h"
#include "stdlib.h"
#if defined(_usewindows_)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#if defined(_useposix_)
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include "debugtools.h"
ns1(net)

bool IsIPv4(char *str) {
	int i,dotcnt=0;long v;
	bool IPNum,InRange;
	char buf[100];
	for (i=0;i<strlen(str);i++) 
	if (str[i]=='.') dotcnt++;
	if (dotcnt!=3) return FALSE;
	IPNum=TRUE;
	InRange=TRUE;	
	libroot::general::SplitStr(str,".",0,buf);IPNum=IPNum&&libroot::general::IsNumeral(buf);if (IPNum==TRUE) {v=atol(buf);if ((v<0)||(v>255)) InRange=InRange&&FALSE;}
	libroot::general::SplitStr(str,".",1,buf);IPNum=IPNum&&libroot::general::IsNumeral(buf);if (IPNum==TRUE) {v=atol(buf);if ((v<0)||(v>255)) InRange=InRange&&FALSE;}
	libroot::general::SplitStr(str,".",2,buf);IPNum=IPNum&&libroot::general::IsNumeral(buf);if (IPNum==TRUE) {v=atol(buf);if ((v<0)||(v>255)) InRange=InRange&&FALSE;}
	libroot::general::SplitStr(str,".",3,buf);IPNum=IPNum&&libroot::general::IsNumeral(buf);if (IPNum==TRUE) {v=atol(buf);if ((v<0)||(v>255)) InRange=InRange&&FALSE;}
	return IPNum&&InRange;
}

char *getaddressinfoOneIPv4(char *domainname)
{
	addressinfo ai= getaddressinfo(domainname);

	if (ai.IPCount>0)
	{
		return ai.IPv4addresses[0];
	}
	return NULL;
}

addressinfo getaddressinfo(char *domainname)
{
#if defined(_usewindows_)
	WSADATA wsaData;
#endif
    int iResult;
    struct addrinfo *result = NULL;
    struct addrinfo *ptr = NULL;
    struct addrinfo hints;
    struct sockaddr_in  *sockaddr_ipv4;

    DWORD dwRetval;

    int i = 1;
    
	addressinfo myai;

    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;

#if defined(_usewindows_)
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        print("WSAStartup failed: %d\n", iResult);
        return myai;
    }
    ZeroMemory( &hints, sizeof(hints) );
#endif
#if defined(_useposix_)
    memset(&hints,0,sizeof(hints));
#endif
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    dwRetval = getaddrinfo(domainname, NULL, &hints, &result);
    if ( dwRetval != 0 ) {
        print("getaddrinfo failed with error: %d\n", dwRetval);
		#if defined(_usewindows_)
				WSACleanup();
		#endif
    }
	myai.success=FALSE;
	myai.IPCount=0;
    int count=0;
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        switch (ptr->ai_family) {
            case AF_INET:
                sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;				  
				if (count<100) 
				{
				  strcpy(myai.IPv4addresses[myai.IPCount],inet_ntoa(sockaddr_ipv4->sin_addr));
				  myai.success=TRUE;
				  myai.IPCount++;
				}
                break;         
            default:
                break;
        }      
    }

    freeaddrinfo(result);
#if defined(_usewindows_)
    WSACleanup();
#endif
	return myai;
}

ns1end

