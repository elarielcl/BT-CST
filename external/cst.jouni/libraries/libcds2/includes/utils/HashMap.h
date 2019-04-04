/*  
 * Copyright (C) 2012, Andres Abeliuk, all rights reserved.
 *
 * Andres Abeliuk <aabeliuk@dcc.uchile.cl>
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

#ifndef _HASHMAP_H
#define	_HASHMAP_H


class HashMap {
private:
      int *table;
      int TABLE_SIZE;
     
public:
       int used;
       
      HashMap() {
            used = 0;
            TABLE_SIZE = 1024*1024;
            table = new int[TABLE_SIZE];
            for (int i = 0; i < TABLE_SIZE; i++)
                  table[i] = -1;
      }

      int get(int key) {
            int hash = (key % TABLE_SIZE);
            while (table[hash] != -1 && table[hash] != key)
                  hash = (hash + 1) % TABLE_SIZE;

            return table[hash];
      }

      void erase (int key) {
            --used;
            int hash = (key % TABLE_SIZE);
            while (table[hash] != -1 && table[hash] != key)
                  hash = (hash + 1) % TABLE_SIZE;

             table[hash]= -1;
      }

      void put(int key) {
            //asuming key is not in table
            if(used > TABLE_SIZE*0.75)
                resize();
            ++used;
            int hash = (key % TABLE_SIZE);
            while (table[hash] != -1 && table[hash] != key)
                  hash = (hash + 1) % TABLE_SIZE;
            table[hash] = key;
      }

      void resize(){
            int i;
            int old_size = TABLE_SIZE;
            TABLE_SIZE = (int)TABLE_SIZE*2;
            int *aux =new int[TABLE_SIZE];
            for (i=0; i<TABLE_SIZE; ++i)
                aux[i] = -1;

            int *aux2 = table;
            table = aux;
            used = 0;
            for (i=0; i<old_size; ++i){
                if(aux2[i] > -1 )
                    put(aux2[i]);
            }

     }

      ~HashMap() {
            used = 0;
            delete[] table;
      }
};

#endif	/* _HASHMAP_H */

