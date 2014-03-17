#pragma once

#ifndef _AnmarLib_netgeneral_H_
#define _AnmarLib_netgeneral_H_

#include "lib.h"

ns1(net)
	bool IsIPv4(char *str);

	struct addressinfo
	{
		char IPv4addresses[100][16];
		int IPCount;
		bool success;
	};

	addressinfo getaddressinfo(char *domainname);
	char *getaddressinfoOneIPv4(char *domainname);
ns1end

#endif