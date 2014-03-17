#include "lib.h"
#include "dns.h"
#if defined(_useposix_)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <string.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <errno.h>
#endif

#include "debugtools.h"

ns1(net)

	DNS::DNS()
	{
		resettodefault();
	}

	DNS::~DNS()
	{
		// Delete all resultanswers
		int i;
		for (i=0;i<Results.count;i++)
		{
			Results[i]->data.dispose();
			Results[i]->name.dispose();
		}

	}

	void DNS::setserverIP(char *serverIPv4)
	{
		strcpy(serverIP,serverIPv4);
	}
	
	void DNS::resettodefault()
	{
		setserverIP("4.2.2.2");
		questions.resize(0);
		totalquestions=0;
		transid=0;
		maxretries=16;
		timeout_ms=2000;
	}

	void DNS::SetTimeout(unsigned long milliseconds)
	{
		timeout_ms=milliseconds;
	}
		
	void DNS::reset()
	{
		questions.resize(0);
		totalquestions=0;
	}

	void DNS::SetTotalRetries(int maxretrycount)
	{
		maxretries=maxretrycount;
	}

	void DNS::addquestion(char *name, Qtypes _type, Qclasses _class)
	{
		int i,cnt=0, lastsrcpos=0, dstpos=0;
		char dnsname[256];
		memset(dnsname,0,255);
		for (i=0;i<=strlen(name);i++)
		{
			if ((name[i]=='.')||(name[i]==0))
			{
				dstpos=strlen(dnsname);
				dnsname[dstpos]=cnt;
				strncpy(dnsname+dstpos+1,name+lastsrcpos,cnt);
				cnt=0;lastsrcpos=i+1;
				if (name[i]==0) break;
				continue;
			}
			cnt++;
		}		

		questions.push(dnsname, strlen(dnsname)+1);

		unsigned short _typ=makeBE(_type);
		unsigned short _cls=makeBE(_class);

		questions.push((char *)&_typ,2);
		questions.push((char *)&_cls,2);
		totalquestions++;
	}

	unsigned short DNS::makeBE(unsigned short val)
	{
		unsigned char high=val>>8;
		unsigned char low=val & 0xff;
		unsigned short result=(low << 8 ) | high;
		return result;
	}

	unsigned long DNS::makeBElong(unsigned long val)
	{
		unsigned char highest=val>>24;
		unsigned char high=(val & 0x00ffffff)>>16;
		unsigned char low=(val & 0x0000ffff)>>8;
		unsigned char lowest=val & 0xff;
		unsigned long result=(lowest << 24) | (low << 16) | (high << 8) | highest;
		return result;
	}
	void DNS::constructpacket()
	{
		if (totalquestions==0) return;
		transid++;
		buf.resize(0);
		unsigned short _tid, _f, _tq, _z;
		_tid=makeBE(transid);
		_f=makeBE(0x0100);
		_tq=makeBE(totalquestions);
		_z=0;

		buf.push((char *)&_tid,2);
		buf.push((char *)&_f,2);
		buf.push((char *)&_tq,2);
		buf.push((char *)&_z,2);
		buf.push((char *)&_z,2);
		buf.push((char *)&_z,2);
		buf.push((char *)questions.buffer,questions.len);
	}

	char *DNS::QuickQueryHostIPv4(char *name)
	{
		bool rv=QueryHostA(name);
		if (!rv) return NULL;

		if (Results.count==0) return NULL;
		int i;
		for (i=0;i<Results.count;i++)
		{
			ResultAnswer *ans=Results[i];
			if (ans->_type==Qtypes_HostA)
			{
				return (char *)ans->data.buffer;
			}
		}
		return NULL;
	}

	bool DNS::Query(char *name, Qtypes _type, Qclasses _class)
	{
		reset();
		addquestion(name,_type,_class);
		return Query();
	}

	bool DNS::QueryHostA(char *name)
	{
		reset();
		addquestion(name,Qtypes_HostA);
		return Query();
	}

	bool DNS::QueryIPv6(char *name)
	{
		reset();
		addquestion(name,Qtypes_IPv6Address_AAAA);
		return Query();
	}

	bool DNS::QueryMX(char *name)
	{
		reset();
		addquestion(name,Qtypes_MailExchance_MX);
		return Query();
	}

	bool DNS::QueryCNAME(char *name)
	{
		reset();
		addquestion(name,Qtypes_Alias_CNAME);
		return Query();
	}

	bool DNS::QueryAll(char *name)
	{
		reset();
		addquestion(name,Qtypes_AllRecords);
		return Query();
	}

	// Returns true on success, false on failure.
	bool DNS::Query()
	{
		if (totalquestions==0) return false;
		#if defined(_usewindows_)
			int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
			if ( iResult != NO_ERROR ) {_print("Error at WSAStartup()\n"); return false;}
		#endif
		Results.DeleteAllItems();
		constructpacket();
		int totalretries=maxretries;
		int nbytes=0, currenttry=0;
		do 
		{
			m_socket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
			if ( m_socket == INVALID_SOCKET ) {
				#if defined(_usewindows_)
				_print( "\nError at socket(): %ld\n", WSAGetLastError() );WSACleanup(); 
				#endif
				#if defined(_useposix_)
				_print( "\nError creating socket: %s", strerror(errno));
				#endif
				return false;}

			con.sin_family = AF_INET;
			con.sin_addr.s_addr = inet_addr(serverIP);
			con.sin_port = htons(53);

			sendto(m_socket,(char *)buf.buffer,buf.len,0,(sockaddr*)&con,sizeof(con));
			receivebuf.resize(2048);
			int fromlen=sizeof(con);

			#if defined(_useposix_)
				struct timeval timevalue;
				timevalue.tv_sec = timeout_ms/1000;
				timevalue.tv_usec = (timeout_ms%1000)*1000;
				setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timevalue,sizeof(timevalue));
				socklen_t flen=(socklen_t)fromlen;
				nbytes=recvfrom(m_socket,(char *)receivebuf.buffer,receivebuf.len,0,(sockaddr*)&con,&flen);
				close(m_socket);
			#endif
			#if defined(_usewindows_)
				setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout_ms,sizeof(timeout_ms));
				nbytes=recvfrom(m_socket,(char *)receivebuf.buffer,receivebuf.len,0,(sockaddr*)&con,&fromlen);
				closesocket(m_socket);
			#endif			
			currenttry++;
//			_print("\n TRY %d",currenttry);
		} while ((nbytes<0)&&(currenttry<totalretries));
		
		if (nbytes<0) return false;
		// Decode response
		DNSData *dnsdata=(DNSData *)receivebuf.buffer;

		// start walking the questions, then the answers
		unsigned short questions=makeBE(dnsdata->Questions);
		unsigned short answers=makeBE(dnsdata->AnswerRRs);
		unsigned short authorityanswers=makeBE(dnsdata->AuthorityRRs);
		unsigned short additionalanswers=makeBE(dnsdata->AdditionalRRs);

		// bypass question
		char *currentpos=(char *)receivebuf.buffer+buf.len;			// Now starting at the Answers

		int i;						

		for (i=0;i<answers;i++)
		{
			decodeandstoreanswer(AnswerCategory_NonAuthoritativeAnswer, currentpos);
		}
		for (i=0;i<authorityanswers;i++)
		{
			decodeandstoreanswer(AnswerCategory_AuthoritativeAnswer, currentpos);
		}
		for (i=0;i<additionalanswers;i++)
		{
			decodeandstoreanswer(AnswerCategory_AdditionalAnswer, currentpos);
		}
		return TRUE;
	}

	void DNS::decodeandstoreanswer(unsigned short anscategory, char *&currentpos)
	{
		ResultAnswer *resultanswer= new ResultAnswer;			
		libroot::general::smartbuf tmp;
		int len=0;
		libroot::general::str rdata;

		resultanswer->answercategory=anscategory;

		len=readfield((char *)receivebuf.buffer, currentpos,TRUE,0,tmp);		// Name
		currentpos+=len;dnsstringtonormal((char *)tmp.buffer);
//			_print("\n\tname=%s",(char *)tmp.buffer);
		resultanswer->name=(char *)tmp.buffer;
		//strcpy(resultanswer->name, (char *)tmp.buffer);

		len=readfield((char *)receivebuf.buffer, currentpos,FALSE,2,tmp);		// Type
		currentpos+=len;
		unsigned short _type=makeBE(*(unsigned short *)tmp.buffer);
//			_print("\n\ttype=%x",_type);
		resultanswer->_type=_type;

		len=readfield((char *)receivebuf.buffer, currentpos,FALSE,2,tmp);		// Class
		currentpos+=len;
		unsigned short _class=makeBE(*(unsigned short *)tmp.buffer);
//			_print("\n\tclass=%x",_class);
		resultanswer->_class=_class;

		len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		// TTL
		currentpos+=len;
		unsigned long _ttl=makeBElong(*(unsigned long *)tmp.buffer);
//			_print("\n\tTTL=%x",_ttl);
		resultanswer->TTL=_ttl;

		len=readfield((char *)receivebuf.buffer, currentpos,FALSE,2,tmp);		// RDLength
		currentpos+=len;
		// Get RDlength
		unsigned short _rdlength=makeBE(*(unsigned short *)tmp.buffer);
//			_print("\n\tRDLength=%d",_rdlength);
		resultanswer->DataLength=_rdlength;

		rdata="";
		switch (_type)
		{
			case Qtypes_HostA:										// IP address, 4 bytes, parse
				len=readfield((char *)receivebuf.buffer, currentpos,FALSE,_rdlength,tmp);		
				currentpos+=len;
				char tmpbuf[20];
				sprintf(tmpbuf,"%d.%d.%d.%d",((unsigned char *)tmp.buffer)[0],((unsigned char *)tmp.buffer)[1],((unsigned char *)tmp.buffer)[2],((unsigned char *)tmp.buffer)[3]);
//					_print("\n\tData: IP Address=%s",tmpbuf);
				resultanswer->data.push(tmpbuf,strlen(tmpbuf)+1);
			break;

			case Qtypes_NameServer:
				len=readfield((char *)receivebuf.buffer, currentpos,TRUE,_rdlength,tmp);		
				currentpos+=len;
				dnsstringtonormal((char *)tmp.buffer);
//					_print ("\n\_NameServer=%s",(char *)tmp.buffer);
				rdata=(char *)tmp.buffer;
				resultanswer->data.push((char *)tmp.buffer,strlen(tmpbuf)+1);
			break;

			case Qtypes_Alias_CNAME:

			break;

			case Qtypes_ReverseLookup_PTR:

			break;

			case Qtypes_MailExchance_MX:
				{
					Response_MX *rmx=new Response_MX;
					// Preference
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,2,tmp);		
					currentpos+=len;
					rmx->Preference=makeBE(*(unsigned short *)tmp.buffer);

					// Mail Exchange
					len=readfield((char *)receivebuf.buffer, currentpos,TRUE,0,tmp);		
					currentpos+=len;
					dnsstringtonormal((char *)tmp.buffer);
					rmx->MailExchange=(char *)tmp.buffer;

					resultanswer->data.push((char *)rmx,sizeof(Response_MX));
					delete rmx;									
				}

			break;

			case Qtypes_Service_SRV:

			break;

			case Qtypes_IncrimentalZoneTransfer_IXFR:

			break;

			case Qtypes_StandardZoneTransfer_AXFR:

			break;

			case Qtypes_AllRecords:

			break;

			case Qtypes_Text_TXT:
				len=readfield((char *)receivebuf.buffer, currentpos,FALSE,_rdlength,tmp);		
				currentpos+=len;

				resultanswer->data.push((char *)tmp.buffer,strlen(tmpbuf));
				resultanswer->data.push((char *)"\0",1);
			break;

			case Qtypes_StartOfAuthority_SOA:
				{
					Response_SOA *rsoa=new Response_SOA;
					// Primary Name Server
					len=readfield((char *)receivebuf.buffer, currentpos,TRUE,0,tmp);		
					currentpos+=len;
					dnsstringtonormal((char *)tmp.buffer);
					rsoa->PrimaryNameServer=(char *)tmp.buffer;

					// Responsible mail addr
					len=readfield((char *)receivebuf.buffer, currentpos,TRUE,0,tmp);		
					currentpos+=len;
					dnsstringtonormal((char *)tmp.buffer);
					rsoa->ResponsibleMailAddr=(char *)tmp.buffer;

					// Serial Number
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		
					currentpos+=len;
					rsoa->SerialNumber=makeBElong(*(unsigned long *)tmp.buffer);

					// Refresh Interval
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		
					currentpos+=len;
					rsoa->RefreshInterval=makeBElong(*(unsigned long *)tmp.buffer);

					// Retry Interval
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		
					currentpos+=len;
					rsoa->RetryInterval=makeBElong(*(unsigned long *)tmp.buffer);

					// Expiration Limit
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		
					currentpos+=len;
					rsoa->ExpirationLimit=makeBElong(*(unsigned long *)tmp.buffer);

					// Minimum TTL
					len=readfield((char *)receivebuf.buffer, currentpos,FALSE,4,tmp);		
					currentpos+=len;
					rsoa->MinimumTTL=makeBElong(*(unsigned long *)tmp.buffer);

					resultanswer->data.push((char *)rsoa,sizeof(Response_SOA));
					delete rsoa;
				}
			break;

			case Qtypes_IPv6Address_AAAA:
				len=readfield((char *)receivebuf.buffer, currentpos,FALSE,_rdlength,tmp);		// IPv6 should be 16 bytes
				currentpos+=len;
				char tmpbufv6[40];
				sprintf(tmpbufv6,"%x:%x:%x:%x:%x:%x:%x:%x",makeBE(((unsigned short *)tmp.buffer)[0]),makeBE(((unsigned short *)tmp.buffer)[1]),
															makeBE(((unsigned short *)tmp.buffer)[2]),makeBE(((unsigned short *)tmp.buffer)[3]),
															makeBE(((unsigned short *)tmp.buffer)[4]),makeBE(((unsigned short *)tmp.buffer)[5]),
															makeBE(((unsigned short *)tmp.buffer)[6]),makeBE(((unsigned short *)tmp.buffer)[7]));
				resultanswer->data.push(tmpbufv6,strlen(tmpbufv6)+1);
			break;


			default:
				break;
		}			
			
		Results.Additem(*resultanswer);
		//delete resultanswer;
	}

	void DNS::dnsstringtonormal(char *dnsstring)
	{
		int i;
		for (i=0;i<strlen(dnsstring);i++)
		{
			if (dnsstring[i]<20) dnsstring[i]='.';
		}
		memmove(dnsstring, dnsstring+1,strlen(dnsstring)+1);
	}

	int DNS::readfield(char *origin, char *currentposition, bool canhavepointers, int fixedlength, libroot::general::smartbuf &targetbuf)
	{
		bool ispointer=false;
		char *curpos=currentposition;
		int count=0;
		targetbuf.resize(0);
		
		if (canhavepointers)
		{
			unsigned char curbyte=0;
			bool mainsearchdone=false;
			int maincount=0;
			char *newpos=curpos;
			do {
				curbyte=*newpos;
				if ((curbyte & 0xc0)==0xc0)
				{
					// read pointer
					unsigned short pointer= *((unsigned short *)newpos);
					pointer=makeBE(pointer) & 0x3fff;

					newpos=origin+pointer;

					if (!mainsearchdone)
					{
						mainsearchdone=true;
						count+=2;
						maincount=count;
					}
				}
				targetbuf.push((char *)newpos,1);
				newpos++;
				count++;
			} while (curbyte!=0);
			curpos+= mainsearchdone ? maincount : count;
			return mainsearchdone ? maincount : count;
		}
		else
		{
			targetbuf.push(curpos,fixedlength);
			return fixedlength;
		}

	}

ns1end