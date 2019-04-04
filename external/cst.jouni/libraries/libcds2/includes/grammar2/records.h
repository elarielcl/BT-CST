
/*

Repair -- an implementation of Larsson and Moffat's compression and
decompression algorithms.
Copyright (C) 2010-current_year Gonzalo Navarro

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Author's contact: Gonzalo Navarro, Dept. of Computer Science, University of
Chile. Blanco Encalada 2120, Santiago, Chile. gnavarro@dcc.uchile.cl

*/

	// extendible array for records

#ifndef RECORDS_REPAIR_INCLUDED
#define RECORDS_REPAIR_INCLUDED

#include <grammar/basicsRepair.h>


namespace cds_static{
	

typedef struct
   { long prev,next;
   } Tlist_repair; // list of prev next equal char

typedef struct
   { Tpair_repair pair; // pair content
     long freq; // frequency
     long cpos; // 1st position in C
     int hpos; // position in heap
     int kpos; // position in hash
   } Trecord_repair;

typedef struct
   { Trecord_repair *records; 
     int maxsize;  
     int size;
     float factor;
     int minsize;
     void *Hash;  // Thash_repair *
     void *Heap; // Theap_repair *
     void *List; // Tlist_repair *
   } Trarray_repair;


// contents can be accessed as Rec.records[0..Rec.size-1]

int insertRecord (Trarray_repair *Rec, Tpair_repair pair); 
			// inserts pair in Rec, returns id, links to/from
			// Hash and Heap, not List. sets freq = 1

void deleteRecord (Trarray_repair *Rec); // deletes last cell in Rec

Trarray_repair createRecords (float factor, int minsize); // creates empty array

void assocRecords (Trarray_repair *Rec, void *Hash, void *Heap, void *List); 
						// associates structures

void destroyRecords (Trarray_repair *Rec); // destroys Rec
  
void removeRecord (Trarray_repair *Rec, int id);// delete record, freq <= 1
                                       // due to freq 0 or purgue (freq 1)
                                       // already deleted from heap
};

#include <grammar/heap.h>
#include <grammar/hash.h>
#endif
