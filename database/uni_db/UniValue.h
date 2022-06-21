// -------------------------------------------------------------------------
// -----                          UniValue header file                 -----
// -----                  Created 03/05/17 by K. Gertsenberger         -----
// -------------------------------------------------------------------------

/** UniValue.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** abstract base pure virtual structure for value storing
 **/

#ifndef UNIVALUE_H
#define UNIVALUE_H 1

#include "uni_db_settings.h"

#include <boost/endian/conversion.hpp>

#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
using namespace std;

enum enumValueType : unsigned int;
struct UniValue
{
    virtual ~UniValue(){} // Destructor
    virtual enumValueType GetType() = 0;
    virtual size_t GetStorageSize() = 0;
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
        boost::endian::little_to_native_inplace(value);
        source += 4;
    }
    void Write(unsigned char*& destination, int32_t& value)
    {
        boost::endian::native_to_little_inplace(value);
        memcpy(destination, &value, 4);
        destination += 4;
    }

    void Read(unsigned char*& source, uint32_t& value)
    {
        memcpy(&value, source, 4);
        boost::endian::little_to_native_inplace(value);
        source += 4;
    }
    void Write(unsigned char*& destination, uint32_t& value)
    {
        boost::endian::native_to_little_inplace(value);
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

    void Read(unsigned char*& source, unsigned char*& value, uint64_t& size)
    {
        memcpy(&size, source, 8);
        boost::endian::little_to_native_inplace(size);
        source += 8;
        value = new unsigned char[size];
        memcpy(value, source, size);
        source += size;
    }
    void Write(unsigned char*& destination, unsigned char* value, uint64_t size)
    {
        if (size > 0)
        {
            uint64_t size_little = boost::endian::native_to_little(size);
            memcpy(destination, &size_little, 8);
            destination += 8;
            memcpy(destination, value, size);
            destination += size;
        }
        else cout<<"ERROR: count of bytes for the value should be greater than zero. The value was not written to the database!"<<endl;
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

    template <size_t rows, size_t cols>
    void Read(unsigned char*& source, double (&value)[rows][cols])
    {
        memcpy(value, source, rows*cols*8);
        source += rows*cols*8;
    }
    template <size_t rows, size_t cols>
    void Write(unsigned char*& destination, double (&value)[rows][cols])
    {
        memcpy(destination, value, rows*cols*8);
        destination += rows*cols*8;
    }

    void Read(unsigned char*& source, double*** value, uint8_t& size1, uint8_t& size2, uint8_t& size3)
    {
        memcpy(&size1, source, 1);
        boost::endian::little_to_native_inplace(size1);
        source += 1;
        memcpy(&size2, source, 1);
        boost::endian::little_to_native_inplace(size2);
        source += 1;
        memcpy(&size3, source, 1);
        boost::endian::little_to_native_inplace(size3);
        source += 1;

        value = new double**[size1];
        for (int i = 0; i < size1; i++)
        {
            value[i] = new double*[size2];
            for (int j = 0; j < size2; j++)
                value[i][j] = new double[size3];
        }
        uint32_t full_size = size1*size2*size3*8;
        memcpy(value, source, full_size);
        source += full_size;
    }
    void Write(unsigned char*& destination, double*** value, uint8_t size1, uint8_t size2, uint8_t size3)
    {
        uint32_t full_size = size1*size2*size3*8;
        if (full_size > 0)
        {
            uint8_t size1_little = boost::endian::native_to_little(size1);
            memcpy(destination, &size1_little, 1);
            destination += 1;
            uint8_t size2_little = boost::endian::native_to_little(size2);
            memcpy(destination, &size2_little, 1);
            destination += 1;
            uint8_t size3_little = boost::endian::native_to_little(size3);
            memcpy(destination, &size3_little, 1);
            destination += 1;

            memcpy(destination, value, full_size);
            destination += full_size;
        }
        else cout<<"ERROR: count of bytes for the value should be greater than zero. The value was not written to the database!"<<endl;
    }

    void Read(unsigned char*& source, vector<double>& value)
    {
        uint64_t size;
        memcpy(&size, source, 8);
        boost::endian::little_to_native_inplace(size);
        source += 8;
        value.assign((double*)source, (double*)(source + size));
        source += size;
    }
    void Write(unsigned char*& destination, vector<double>& value)
    {
        uint64_t size = value.size() * 8;
        if (size > 0)
        {
            uint64_t size_little = boost::endian::native_to_little(size);
            memcpy(destination, &size_little, 8);
            destination += 8;
            memcpy(destination, value.data(), size);
            destination += size;
        }
        else cout<<"ERROR: vector size of the value should be greater than zero. The value was not written to the database!"<<endl;
    }
};

#endif
