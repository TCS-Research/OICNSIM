/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include "ns3/type-name.h"
#include "ns3/log.h"
#include "content.h"

NS_LOG_COMPONENT_DEFINE ("Content");

namespace ns3{


std::string Content::GetContent(uint32_t index)
{
	std::string name = "oicn://content/category/subcategory" ;
	std::string suffix;
	uint32_t digit[6];
	std::memset(digit, 0, sizeof(digit));
	if (index>999999)
	{
		NS_LOG_DEBUG (" "<<index<<" has more than six digits "<<std::endl<<" Set indexes that are not more than six digits ");
	}
	else if (index>99999)
	{
		NS_LOG_DEBUG ("index>99999 "<<index<<std::endl);
		digit[0] = (index/100000)+48;
		digit[1] = ((index%100000)/10000)+48;
		digit[2] = ((index%10000)/1000)+48;
		digit[3] = ((index%1000)/100)+48;
		digit[4] = ((index%100)/10)+48;
		digit[5] = (index%10)+48;
		suffix.append((char*)&digit[0]);
		suffix.append((char*)&digit[1]);
		suffix.append((char*)&digit[2]);
		suffix.append("/");
		suffix.append((char*)&digit[3]);
		suffix.append((char*)&digit[4]);
		suffix.append((char*)&digit[5]);

	}
	else if (index>9999)
	{
		NS_LOG_DEBUG ("index>9999 "<<index<<std::endl);
		digit[0] = (index/10000)+48;
		digit[1] = ((index%10000)/1000)+48;
		digit[2] = ((index%1000)/100)+48;
		digit[3] = ((index%100)/10)+48;
		digit[4] = (index%10)+48;
		suffix.append((char*)&digit[0]);
		suffix.append((char*)&digit[1]);
		suffix.append("/");
		suffix.append((char*)&digit[2]);
		suffix.append((char*)&digit[3]);
		suffix.append((char*)&digit[4]);
	 }
	else if (index>999)
	{
		NS_LOG_DEBUG ("index>999 "<<index<<std::endl);
		digit[0] = (index/1000)+48;
		digit[1] = ((index%1000)/100)+48;
		digit[2] = ((index%100)/10)+48;
		digit[3] = (index%10)+48;
		suffix.append((char*)&digit[0]);
		suffix.append((char*)&digit[1]);
		suffix.append("/");
		suffix.append((char*)&digit[2]);
		suffix.append((char*)&digit[3]);
 	}
	else if (index>99)
	{
		NS_LOG_DEBUG ("index>99 "<<index<<std::endl);
		digit[0] = (index/100)+48;
		digit[1] = ((index%100)/10)+48;
		digit[2] = (index%10)+48;
    	suffix.append((char*)&digit[0]);
    	suffix.append("/");
    	suffix.append((char*)&digit[1]);
		suffix.append((char*)&digit[2]);
	}
	else if (index>9)
    {
		NS_LOG_DEBUG ("index>9 "<<index<<std::endl);
		digit[0] = (index/10)+48;
		digit[1] = (index%10)+48;
		suffix.append((char*)&digit[0]);
	   	suffix.append("/");
	   	suffix.append((char*)&digit[1]);
    }
	else if (index>0)
	{
		NS_LOG_DEBUG ("index>0 "<<index<<std::endl);
		digit[0] = index+48;
		suffix.append("0/");
	   	suffix.append((char*)&digit[0]);
	}
	else
	{
		suffix.append("0/0");
	}

	name.append(suffix);
	return (name);
}
}
