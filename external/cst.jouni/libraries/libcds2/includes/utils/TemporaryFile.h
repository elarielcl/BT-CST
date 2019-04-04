/**
* Copyright (C) 2014, Alberto Ordóñez, all rights reserved.
*
* Implements an object that encapsultes a temporary file. The destructor
* is in charge of deleting the file from the fileSystem, while the constructor
* to create it.
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

#ifndef  _TEMPORARY_FILE_UTILS_H_
#define _TEMPORARY_FILE_UTILS_H_
#include <cstdio>
#include <string>

class TemporaryFile{
public:
    TemporaryFile(){
        char *b = new char[L_tmpnam];
        b = tmpnam(b);
        fileName = string(b);
        delete [] b;
    }

    string getFileName() const{
        return fileName;
    }

    ~TemporaryFile(){
        if (!fileName.empty()){
            remove(fileName.c_str());
        }
    }

protected:
    string fileName;
};

#endif