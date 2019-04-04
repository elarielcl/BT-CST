/* SequenceRepair.h
 * Copyright (C) 2014-current_year, Alberto Ordóñez, all rights reserved.
 *
 * Sequence encoded with Repair which stores counters to solve rank/select/access operations
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef _CARTESIAN_TREE_H
 #define _CARTESIAN_TREE_H
#include <utils/BinaryTree.h> 
#include <utils/libcdsBasics.h>

 using namespace cds_utils;
 
 namespace cds_static{

 	class CartesianTree{
 		public:
 			static void obtainCartesianTree(uint *input, size_t len, uint *&tree, size_t &treeLength);
 		protected:
 			static void obtainCartesianTreeRec(uint *input, size_t len, size_t i, size_t j, uint *tree,size_t &p);
 			static void obtainCartesianTreeRec(uint *input, size_t len, uint *treeBmp, size_t &treeLength);
 			static void testCartesianTree(uint *input, size_t len);
 	};
 };
 #endif
