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