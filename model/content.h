/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, TCS Research & Innovation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 *          Suvrat Agrawal <suvrat.a@tcs.com>
 *
 */

#ifndef CONTENT_H
#define CONTENT_H

#include <stdint.h>
#include <string>
#include "ns3/type-name.h"

namespace ns3{
/**
 *\brief This class is used for creating different content name corresponding to index.
 *To create different content name we have assume a content name will be a combination of
 *a common string and corresponding index e.g. if index is 12, content name will be combination
 *of common string oicn://content/category/subcategory and index 12 i.e. oicn://content/category/subcategory1/2
 **/
class Content
{
public:
	/**
	 * \brief Create the content name corresponding to the index.
	 * \param index of the content
	 */
       static std::string GetContent(uint32_t index);
};

}
#endif
