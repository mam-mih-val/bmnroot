// -------------------------------------------------------------------------
// -----                     UniDbParameterValue header file           -----
// -----                  Created 03/05/17 by K. Gertsenberger         -----
// -------------------------------------------------------------------------

/** UniDbParameterValue.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** abstract base pure virtual structure for parameter storing
 **/

#ifndef UNIDBPARAMETERVALUE_H
#define UNIDBPARAMETERVALUE_H 1

#include "db_settings.h"

#include <map>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

enum enumParameterTypeNew : unsigned int;
struct UniDbParameterValue
{
    virtual enumParameterTypeNew GetType() = 0;
    virtual size_t GetLength() = 0;
    virtual void ReadValue(unsigned char* source) = 0;
    virtual void WriteValue(unsigned char* destination) = 0;

 protected:
    void Read(unsigned char*& source, uint8_t& value)
    {
        memcpy(&value, source, 1);
        source++;
    }
    void Write(unsigned char*& destination, uint8_t& value)
    {
        memcpy(destination, &value, 1);
        destination++;
    }

    void Read(unsigned char*& source, int32_t& value)
    {
        memcpy(&value, source, 4);
        source += 4;
    }
    void Write(unsigned char*& destination, int32_t& value)
    {
        memcpy(destination, &value, 4);
        destination += 4;
    }

    void Read(unsigned char*& source, uint32_t& value)
    {
        memcpy(&value, source, 4);
        source += 4;
    }
    void Write(unsigned char*& destination, uint32_t& value)
    {
        memcpy(destination, &value, 4);
        destination += 4;
    }

    void Read(unsigned char*& source, double& value)
    {
        memcpy(&value, source, 8);
        source += 8;
    }
    void Write(unsigned char*& destination, double& value)
    {
        memcpy(destination, &value, 8);
        destination += 8;
    }

    void Read(unsigned char*& source, string& value)
    {
        value.assign((char*)source);
        source += value.length() + 1;
    }
    void Write(unsigned char*& destination, string& value)
    {
        memcpy(destination, value.c_str(), value.length() + 1);
        destination += value.length() + 1;
    }

    void Read(unsigned char*& source, unsigned char* value, uint64_t& count)
    {
        memcpy(&count, source, 8);
        source += 8;
        memcpy(value, source, count);
        source += count;
    }
    void Write(unsigned char*& destination, unsigned char* value, uint64_t count)
    {
        if (count > 0)
        {
            memcpy(destination, &count, 8);
            destination += 8;
            memcpy(destination, value, count);
            destination += count;
        }
        else cout<<"ERROR: The count of bytes for parameter value should be greater than zero. The parameter value was not written to the database!"<<endl;
    }

    void Read(unsigned char*& source, double* value, const uint64_t count)
    {
        memcpy(value, source, count*8);
        source += count*8;
    }
    void Write(unsigned char*& destination, double* value, const uint64_t count)
    {
        memcpy(destination, value, count*8);
        destination += count*8;
    }
};

#endif
