/*
 * Copyright (C) 2011      ArkCORE <http://www.arkania.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "StorageLoader.h"
#include "Errors.h"

StorageLoader::StorageLoader()
{
    data = NULL;
    fieldsOffset = NULL;
}

bool StorageLoader::LoadDBCStorage(const char* filename, const char* fmt)
{
    uint32 header;
    if (data)
    {
        delete [] data;
        data = NULL;
    }

    FILE* f = fopen(filename, "rb");
    if (!f)
        return false;

    if (fread(&header, 4, 1, f) != 1)                        // Number of records
    {
        fclose(f);
        return false;
    }

    EndianConvert(header);

    if (header != 0x43424457)                                //'WDBC'
    {
        fclose(f);
        return false;
    }

    if (fread(&recordCount, 4, 1, f) != 1)                   // Number of records
    {
        fclose(f);
        return false;
    }

    EndianConvert(recordCount);

    if (fread(&fieldCount, 4, 1, f) != 1)                    // Number of fields
    {
        fclose(f);
        return false;
    }

    EndianConvert(fieldCount);

    if (fread(&recordSize, 4, 1, f) != 1)                    // Size of a record
    {
        fclose(f);
        return false;
    }

    EndianConvert(recordSize);

    if (fread(&stringSize, 4, 1, f) != 1)                    // String size
    {
        fclose(f);
        return false;
    }

    EndianConvert(stringSize);

    fieldsOffset = new uint32[fieldCount];
    fieldsOffset[0] = 0;
    for (uint32 i = 1; i < fieldCount; ++i)
    {
        fieldsOffset[i] = fieldsOffset[i - 1];
        if (fmt[i - 1] == 'b' || fmt[i - 1] == 'X')         // byte fields
            fieldsOffset[i] += sizeof(uint8);
        else                                                // 4 byte fields (int32/float/strings)
            fieldsOffset[i] += sizeof(uint32);
    }

    data = new unsigned char[recordSize * recordCount + stringSize];
    stringTable = data + recordSize*recordCount;

    if (fread(data, recordSize * recordCount + stringSize, 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fclose(f);

    return true;
}

bool StorageLoader::LoadDB2Storage(const char* filename, const char* fmt)
{
    uint32 header = 48;
    if (data)
    {
        delete [] data;
        data = NULL;
    }

    FILE * f = fopen(filename, "rb");
    if (!f)
        return false;

    if (fread(&header, 4, 1, f) != 1)                        // Signature
    {
        fclose(f);
        return false;
    }

    EndianConvert(header);

    if (header != 0x32424457)
    {
        fclose(f);
        return false;                                       //'WDB2'
    }

    if (fread(&recordCount, 4, 1, f) != 1)                       // Number of records
    {
        fclose(f);
        return false;
    }

    EndianConvert(recordCount);

    if (fread(&fieldCount, 4, 1, f) != 1)                         // Number of fields
    {
        fclose(f);
        return false;
    }

    EndianConvert(fieldCount);

    if (fread(&recordSize, 4, 1, f) != 1)                         // Size of a record
    {
        fclose(f);
        return false;
    }

    EndianConvert(recordSize);

    if (fread(&stringSize, 4, 1, f) != 1)                         // String size
    {
        fclose(f);
        return false;
    }

    EndianConvert(stringSize);

    /* NEW WDB2 FIELDS*/
    if (fread(&tableHash, 4, 1, f) != 1)                          // Table hash
    {
        fclose(f);
        return false;
    }

    EndianConvert(tableHash);

    if (fread(&build, 4, 1, f) != 1)                              // Build
    {
        fclose(f);
        return false;
    }

    EndianConvert(build);

    if (fread(&unk1, 4, 1, f) != 1)                                // Unknown WDB2
    {
        fclose(f);
        return false;
    }
    EndianConvert(unk1);

    if (build > 12880)
    {
        if (fread(&unk2, 4, 1, f) != 1)                               // Unknown WDB2
        {
            fclose(f);
            return false;
        }
        EndianConvert(unk2);

        if (fread(&maxIndex, 4, 1, f) != 1)                           // MaxIndex WDB2
        {
            fclose(f);
            return false;
        }
        EndianConvert(maxIndex);

        if (fread(&locale, 4, 1, f) != 1)                             // Locales
        {
            fclose(f);
            return false;
        }
        EndianConvert(locale);

        if (fread(&unk5, 4, 1, f) != 1)                               // Unknown WDB2
        {
            fclose(f);
            return false;
        }
        EndianConvert(unk5);

        if (maxIndex)
            fseek(f, maxIndex * 6 - 48*3, SEEK_CUR);
    }

    fieldsOffset = new uint32[fieldCount];
    fieldsOffset[0] = 0;
    for (uint32 i = 1; i < fieldCount; i++)
    {
        fieldsOffset[i] = fieldsOffset[i - 1];
        if (fmt[i - 1] == 'b' || fmt[i - 1] == 'X')         // byte fields
            fieldsOffset[i] += 1;
        else                                                // 4 byte fields (int32/float/strings)
            fieldsOffset[i] += 4;
    }

    data = new unsigned char[recordSize*recordCount+stringSize];
    stringTable = data + recordSize*recordCount;

    if (fread(data, recordSize * recordCount + stringSize, 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

StorageLoader::~StorageLoader()
{
    if (data)
        delete [] data;

    if (fieldsOffset)
        delete [] fieldsOffset;
}

StorageLoader::Record StorageLoader::getRecord(size_t id)
{
    assert(data);
    return Record(*this, data + id * recordSize);
}

uint32 StorageLoader::GetFormatRecordSize(const char* format, int32* index_pos)
{
    uint32 recordsize = 0;
    int32 i = -1;
    for (uint32 x = 0; format[x]; ++x)
    {
        switch (format[x])
        {
            case FT_FLOAT:
                recordsize += sizeof(float);
                break;
            case FT_INT:
                recordsize += sizeof(uint32);
                break;
            case FT_STRING:
                recordsize += sizeof(char*);
                break;
            case FT_SORT:
                i = x;
                break;
            case FT_IND:
                i = x;
                recordsize += sizeof(uint32);
                break;
            case FT_BYTE:
                recordsize += sizeof(uint8);
                break;
            case FT_NA:
            case FT_NA_BYTE:
                break;
            case FT_LOGIC:
                ASSERT(false && "Attempted to load DBC files that do not have field types that match what is in the core. Check DataFormat.h or your DBC files.");
                break;
            default:
                ASSERT(false && "Unknown field format character in DataFormat.h");
                break;
        }
    }

    if (index_pos)
        *index_pos = i;

    return recordsize;
}

uint32 StorageLoader::GetFormatStringsFields(const char * format)
{
    uint32 stringfields = 0;
    for (uint32 x=0; format[x]; ++x)
        if (format[x] == FT_STRING)
            ++stringfields;

    return stringfields;
}

char* StorageLoader::AutoProduceData(const char* format, uint32& records, char**& indexTable, uint32 sqlRecordCount, uint32 sqlHighestIndex, char*& sqlDataTable)
{
    typedef char* ptr;
    if (strlen(format) != fieldCount)
        return NULL;

    // get struct size and index pos
    int32 i;
    uint32 recordsize = GetFormatRecordSize(format, &i);

    if (i >= 0)
    {
        uint32 maxi = 0;
        // find max index
        for (uint32 y = 0; y < recordCount; ++y)
        {
            uint32 ind = getRecord(y).getUInt(i);
            if (ind > maxi)
                maxi = ind;
        }

        // If higher index avalible from sql - use it instead of dbcs
        if (sqlHighestIndex > maxi)
            maxi = sqlHighestIndex;

        ++maxi;
        records = maxi;
        indexTable = new ptr[maxi];
        memset(indexTable, 0, maxi * sizeof(ptr));
    }
    else
    {
        records = recordCount + sqlRecordCount;
        indexTable = new ptr[recordCount + sqlRecordCount];
    }

    char* dataTable = new char[(recordCount + sqlRecordCount) * recordsize];

    uint32 offset = 0;

    for (uint32 y = 0; y < recordCount; ++y)
    {
        if (i >= 0)
            indexTable[getRecord(y).getUInt(i)] = &dataTable[offset];
        else
            indexTable[y] = &dataTable[offset];

        for (uint32 x=0; x < fieldCount; ++x)
        {
            switch (format[x])
            {
                case FT_FLOAT:
                    *((float*)(&dataTable[offset])) = getRecord(y).getFloat(x);
                    offset += sizeof(float);
                    break;
                case FT_IND:
                case FT_INT:
                    *((uint32*)(&dataTable[offset])) = getRecord(y).getUInt(x);
                    offset += sizeof(uint32);
                    break;
                case FT_BYTE:
                    *((uint8*)(&dataTable[offset])) = getRecord(y).getUInt8(x);
                    offset += sizeof(uint8);
                    break;
                case FT_STRING:
                    *((char**)(&dataTable[offset])) = NULL;   // will replace non-empty or "" strings in AutoProduceStrings
                    offset += sizeof(char*);
                    break;
                case FT_LOGIC:
                    ASSERT(false && "Attempted to load DBC files that do not have field types that match what is in the core. Check DataFormat.h or your DBC files.");
                    break;
                case FT_NA:
                case FT_NA_BYTE:
                case FT_SORT:
                    break;
                default:
                    ASSERT(false && "Unknown field format character in DataFormat.h");
                    break;
            }
        }
    }

    sqlDataTable = dataTable + offset;

    return dataTable;
}

static char const* const nullStr = "";

char* StorageLoader::AutoProduceStringsArrayHolders(const char* format, char* dataTable)
{
    if (strlen(format) != fieldCount)
        return NULL;

    // we store flat holders pool as single memory block
    size_t stringFields = GetFormatStringsFields(format);
    // each string field at load have array of string for each locale
    size_t stringHolderSize = sizeof(char*) * TOTAL_LOCALES;
    size_t stringHoldersRecordPoolSize = stringFields * stringHolderSize;
    size_t stringHoldersPoolSize = stringHoldersRecordPoolSize * recordCount;

    char* stringHoldersPool = new char[stringHoldersPoolSize];

    // DB2 strings expected to have at least empty string
    for (size_t i = 0; i < stringHoldersPoolSize / sizeof(char*); ++i)
        ((char const**)stringHoldersPool)[i] = nullStr;

    uint32 offset=0;

    // assign string holders to string field slots
    for (uint32 y = 0; y < recordCount; y++)
    {
        uint32 stringFieldNum = 0;

        for (uint32 x = 0; x < fieldCount; x++)
            switch (format[x])
            {
                case FT_FLOAT:
                case FT_IND:
                case FT_INT:
                    offset += 4;
                    break;
                case FT_BYTE:
                    offset += 1;
                    break;
                case FT_STRING:
                {
                    // init db2 string field slots by pointers to string holders
                    char const*** slot = (char const***)(&dataTable[offset]);
                    *slot = (char const**)(&stringHoldersPool[stringHoldersRecordPoolSize * y + stringHolderSize*stringFieldNum]);
                    ++stringFieldNum;
                    offset += sizeof(char*);
                    break;
                }
                case FT_NA:
                case FT_NA_BYTE:
                case FT_SORT:
                    break;
                default:
                    assert(false && "unknown format character");
        }
    }

    //send as char* for store in char* pool list for free at unload
    return stringHoldersPool;
}

char* StorageLoader::AutoProduceStrings(const char* format, char* dataTable)
{
    if (strlen(format) != fieldCount)
        return NULL;

    char* stringPool = new char[stringSize];
    memcpy(stringPool, stringTable, stringSize);

    uint32 offset = 0;

    for (uint32 y = 0; y < recordCount; ++y)
    {
        for (uint32 x = 0; x < fieldCount; ++x)
        {
            switch (format[x])
            {
                case FT_FLOAT:
                    offset += sizeof(float);
                    break;
                case FT_IND:
                case FT_INT:
                    offset += sizeof(uint32);
                    break;
                case FT_BYTE:
                    offset += sizeof(uint8);
                    break;
                case FT_STRING:
                {
                    // fill only not filled entries
                    char** slot = (char**)(&dataTable[offset]);
                    if (!*slot || !**slot)
                    {
                        const char * st = getRecord(y).getString(x);
                        *slot=stringPool+(st-(const char*)stringTable);
                    }
                    offset += sizeof(char*);
                    break;
                 }
                 case FT_LOGIC:
                     ASSERT(false && "Attempted to load DBC files that does not have field types that match what is in the core. Check DataFormat.h or your DBC files.");
                     break;
                 case FT_NA:
                 case FT_NA_BYTE:
                 case FT_SORT:
                     break;
                 default:
                     ASSERT(false && "Unknown field format character in DataFormat.h");
                     break;
            }
        }
    }

    return stringPool;
}
