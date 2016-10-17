/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#ifndef CONTENT_H
#define CONTENT_H

#include <stdint.h>
#include <string>
#include "ns3/type-name.h"

namespace ns3{
/**
 *\brief This class is used for creating different content name corresponding to index
 *To create different content name we have assume a content name will be a combination of
 *a common string and corresponding index e.g. if index is 12, content name will be combination
 *of common string oicn://content/category/subcategory and index 12 i.e. oicn://content/category/subcategory1/2
 **/
class Content
{
public:
	/**
	 * \brief Create the content name corresponding to the index.
	 * This function create content name corresponding to the index
	 * \param index of the content
	 */
       static std::string GetContent(uint32_t index);
};

}
#endif
