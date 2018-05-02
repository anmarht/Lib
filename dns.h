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

#pragma once

#ifndef _AnmarLib_dns_H_
#define _AnmarLib_dns_H_

#include "lib.h"
#include "general.h"
#include "smartandvardataarray.h"
#include "str.h"

#if defined(_usewindows_)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#if defined(_useposix_)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
#endif

#if defined(_useposix_)
	#define SOCKET int
	#define INVALID_SOCKET -1
#endif

ns1(net)

	class DNS
	{
		#if defined(_usewindows_)
			WSADATA wsaData;
		#endif
		SOCKET m_socket;
		sockaddr_in con;

		char serverIP[40];
		int maxretries;
		long timeout_ms;

		struct Question
		{
			char name[256];
			unsigned short type;
			unsigned short _class;
		};

		#pragma pack(1)
		struct DNSData
		{
			unsigned short TransactionID;
			unsigned short Flags;
			unsigned short Questions;
			unsigned short AnswerRRs;
			unsigned short AuthorityRRs;
			unsigned short AdditionalRRs;
			// Question
			// Answer
		};

		libroot::general::smartbuf buf, questions, receivebuf;
		unsigned short totalquestions;
		unsigned short transid;

		void decodeandstoreanswer(unsigned short anscategory, char *&currentpos);		
		int readfield(char *origin, char *currentposition, bool canhavepointers, int fixedlength, libroot::general::smartbuf &targetbuf);
		void dnsstringtonormal(char *dnsstring);
		void constructpacket();
		unsigned short makeBE(unsigned short val);
		unsigned long makeBElong(unsigned long val);
	public:

		struct Response_SOA
		{
			libroot::general::str PrimaryNameServer;
			libroot::general::str ResponsibleMailAddr;
			unsigned long SerialNumber;
			unsigned long RefreshInterval;
			unsigned long RetryInterval;
			unsigned long ExpirationLimit;
			unsigned long MinimumTTL;
		};

		struct Response_MX
		{
			unsigned short Preference;
			libroot::general::str MailExchange;
		};

		struct ResultAnswer
		{
			libroot::general::str name;
			//char name[512];
			unsigned short _type;
			unsigned short _class;
			unsigned long TTL;
			unsigned short DataLength;
			libroot::general::smartbuf data;
			//char data[1024];
			unsigned short answercategory;				// Lookup in the below enum AnswerCategory
		};

		libroot::general::SmartArray<ResultAnswer> Results;

		#if defined(_usewindows_)
			#define AnswerCategory_NonAuthoritativeAnswer AnswerCategory::NonAuthoritativeAnswer
			#define AnswerCategory_AuthoritativeAnswer AnswerCategory::AuthoritativeAnswer
			#define AnswerCategory_AdditionalAnswer AnswerCategory::AdditionalAnswer
		#endif
		#if defined(_useposix_)
			#define AnswerCategory_NonAuthoritativeAnswer NonAuthoritativeAnswer
			#define AnswerCategory_AuthoritativeAnswer AuthoritativeAnswer
			#define AnswerCategory_AdditionalAnswer AdditionalAnswer
		#endif
		enum AnswerCategory
		{
			NonAuthoritativeAnswer=1,
			AuthoritativeAnswer=2,
			AdditionalAnswer=3
		};

		#if defined(_usewindows_)
			#define Qclasses_IN Qclasses::_IN
		#endif
		#if defined(_useposix_)
			#define Qclasses_IN _IN
		#endif
		enum Qclasses 
		{
			_IN=0x01
		};

		#if defined(_usewindows_)
			#define Qtypes_HostA Qtypes::_HostA
			#define Qtypes_NameServer Qtypes::_NameServer
			#define Qtypes_Alias_CNAME Qtypes::_Alias_CNAME
			#define Qtypes_ReverseLookup_PTR Qtypes::_ReverseLookup_PTR
			#define Qtypes_MailExchance_MX Qtypes::_MailExchance_MX
			#define Qtypes_Service_SRV Qtypes::_Service_SRV
			#define Qtypes_IncrimentalZoneTransfer_IXFR Qtypes::_IncrimentalZoneTransfer_IXFR
			#define Qtypes_StandardZoneTransfer_AXFR Qtypes::_StandardZoneTransfer_AXFR
			#define Qtypes_AllRecords Qtypes::_AllRecords
			#define Qtypes_Text_TXT Qtypes::_Text_TXT
			#define Qtypes_StartOfAuthority_SOA Qtypes::_StartOfAuthority_SOA
			#define Qtypes_IPv6Address_AAAA Qtypes::_IPv6Address_AAAA
		#endif
		#if defined(_useposix_)
			#define Qtypes_HostA _HostA
			#define Qtypes_NameServer _NameServer
			#define Qtypes_Alias_CNAME _Alias_CNAME
			#define Qtypes_ReverseLookup_PTR _ReverseLookup_PTR
			#define Qtypes_MailExchance_MX _MailExchance_MX
			#define Qtypes_Service_SRV _Service_SRV
			#define Qtypes_IncrimentalZoneTransfer_IXFR _IncrimentalZoneTransfer_IXFR
			#define Qtypes_StandardZoneTransfer_AXFR _StandardZoneTransfer_AXFR
			#define Qtypes_AllRecords _AllRecords
			#define Qtypes_Text_TXT _Text_TXT
			#define Qtypes_StartOfAuthority_SOA _StartOfAuthority_SOA
			#define Qtypes_IPv6Address_AAAA _IPv6Address_AAAA
		#endif
		enum Qtypes
		{
			_HostA=							0x01,
			_NameServer=					0x02,
			_Alias_CNAME=					0x05,
			_ReverseLookup_PTR=				0x0c,
			_MailExchance_MX=				0x0f,
			_Service_SRV=					0x21,
			_IncrimentalZoneTransfer_IXFR=	0xfb,
			_StandardZoneTransfer_AXFR=		0xfc,
			_AllRecords=					0xff,

			_Text_TXT=						0x10,
			_StartOfAuthority_SOA=			0x06,
			_IPv6Address_AAAA=				0x1c

		};

		DNS();
		~DNS();

		// Sets DNS server IP to the specified serverIPv4.
		void setserverIP(char *serverIPv4);

		// Resets back server to 4.2.2.2, clears questions, transaction id to 0, timeout to 2 seconds and maxretries to 16.
		void resettodefault();

		// Sets the timeout by seconds and microseconds. The default is 2 seconds.
		void SetTimeout(unsigned long milliseconds);

		// Clears questions.
		void reset();

		// Sets the total number of retries when the server isn't responsive, the default is 16 times.
		void SetTotalRetries(int maxretrycount);

		// Adds a question, this should be followed by a call to Query(). The recommended is using the other parameterized query functions.
		void addquestion(char *name, Qtypes _type, Qclasses _class=_IN);

		// Executes the query against the DNS server according to the supplied question
		// If no question is added, function will fail. The recommended is using the other parameterized query functions.
		// Returns true on success, false on failure.
		bool Query();

		// Executes the query against the DNS server according to the type parameter.
		// supply the second parameter by typing DNS::Qtypes::
		// Returns true on success, false on failure.
		bool Query(char *name, Qtypes _type, Qclasses _class=_IN);

		// Executes a HostA query.
		// Returns true on success, false on failure.
		bool QueryHostA(char *name);

		// Executes a Host AAAA (IPv6) query.
		// Returns true on success, false on failure.
		bool QueryIPv6(char *name);

		// Executes an MX query.
		// Returns true on success, false on failure.
		bool QueryMX(char *name);

		// Executes a CNAME query.
		// Returns true on success, false on failure.
		bool QueryCNAME(char *name);

		// Executes an All Records query.
		// Returns true on success, false on failure.
		bool QueryAll(char *name);

		// Executes a Host A query, and returns the first found IP address, otherwise NULL.
		// Returns true on success, false on failure.
		char *QuickQueryHostIPv4(char *name);
	};

ns1end

#endif