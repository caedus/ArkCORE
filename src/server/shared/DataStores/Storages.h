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

#ifndef STORAGES_H
#define STORAGES_H

#include "StorageLoader.h"
#include "Logging/Log.h"
#include "Field.h"
#include "DatabaseWorkerPool.h"
#include "Implementation/WorldDatabase.h"
#include "DatabaseEnv.h"
#include <vector>

struct SqlDbc
{
    const std::string * formatString;
    const std::string * indexName;
    std::string sqlTableName;
    int32 indexPos;
    int32 sqlIndexPos;
    SqlDbc(const std::string * _filename, const std::string * _format, const std::string * _idname, const char * fmt)
        :formatString(_format), indexName (_idname), sqlIndexPos(0)
    {
        // Convert dbc file name to sql table name
        sqlTableName = *_filename;
        for (uint32 i = 0; i< sqlTableName.size(); ++i)
        {
            if (isalpha(sqlTableName[i]))
                sqlTableName[i] = tolower(sqlTableName[i]);
            else if (sqlTableName[i] == '.')
                sqlTableName[i] = '_';
        }

        // Get sql index position
        StorageLoader::GetFormatRecordSize(fmt, &indexPos);
        if (indexPos >= 0)
        {
            uint32 uindexPos = uint32(indexPos);
            for (uint32 x = 0; x < formatString->size(); ++x)
            {
                // Count only fields present in sql
                if ((*formatString)[x] == FT_SQL_PRESENT)
                {
                    if (x == uindexPos)
                        break;
                    ++sqlIndexPos;
                }
            }
        }
    }
};

template<class T>
class DataStorage
{
    typedef std::list<char*> StringPoolList;
    typedef std::vector<T*> DataTableEx;
    public:
        explicit DataStorage(const char *f) :
            fmt(f), nCount(0), fieldCount(0), dataTable(NULL)
        {
            indexTable.asT = NULL;
        }

        ~DataStorage() { Clear(); }

        T const* LookupEntry(uint32 id) const
        {
            return (id >= nCount) ? NULL : indexTable.asT[id];
        }
        uint32  GetNumRows() const { return nCount; }
        char const* GetFormat() const { return fmt; }
        uint32 GetFieldCount() const { return fieldCount; }

        bool LoadStorage(char const* fn, SqlDbc* sql)
        {
            StorageLoader dbc;
            // Check if load was successful, only then continue
            if (!dbc.LoadDBCStorage(fn, fmt) && !dbc.LoadDB2Storage(fn, fmt))
                return false;

            uint32 sqlRecordCount = 0;
            uint32 sqlHighestIndex = 0;
            Field* fields = NULL;
            QueryResult result = QueryResult(NULL);
            // Load data from sql
            if (sql)
            {
                std::string query = "SELECT * FROM " + sql->sqlTableName;
                if (sql->indexPos >= 0)
                    query +=" ORDER BY " + *sql->indexName + " DESC";
                query += ';';

                result = WorldDatabase.Query(query.c_str());
                if (result)
                {
                    sqlRecordCount = uint32(result->GetRowCount());
                    if (sql->indexPos >= 0)
                    {
                        fields = result->Fetch();
                        sqlHighestIndex = fields[sql->sqlIndexPos].GetUInt32();
                    }
                    // Check if sql index pos is valid
                    if (int32(result->GetFieldCount()-1) < sql->sqlIndexPos)
                    {
                        sLog->outError("Invalid index pos for dbc:'%s'", sql->sqlTableName.c_str());
                        return false;
                    }
                }
            }
            char* sqlDataTable;
            fieldCount = dbc.GetCols();

            // load raw non-string data
            dataTable = (T*)dbc.AutoProduceData(fmt, nCount, indexTable.asChar, sqlRecordCount, sqlHighestIndex, sqlDataTable);

            // create string holders for loaded string fields
            stringPoolList.push_back(dbc.AutoProduceStringsArrayHolders(fmt,(char*)dataTable));

            // load strings from dbc data
            stringPoolList.push_back(dbc.AutoProduceStrings(fmt,(char*)dataTable));

            // Insert sql data into arrays
            if (result)
            {
                if (indexTable.asT)
                {
                    uint32 offset = 0;
                    uint32 rowIndex = dbc.GetNumRows();
                    do
                    {
                        if (!fields)
                            fields = result->Fetch();

                        if (sql->indexPos >= 0)
                        {
                            uint32 id = fields[sql->sqlIndexPos].GetUInt32();
                            if (indexTable.asT[id])
                            {
                                sLog->outError("Index %d already exists in dbc:'%s'", id, sql->sqlTableName.c_str());
                                return false;
                            }
                            indexTable.asT[id]=(T*)&sqlDataTable[offset];
                        }
                        else
                            indexTable.asT[rowIndex]=(T*)&sqlDataTable[offset];
                        uint32 columnNumber = 0;
                        uint32 sqlColumnNumber = 0;

                        for (; columnNumber < sql->formatString->size(); ++columnNumber)
                        {
                            if ((*sql->formatString)[columnNumber] == FT_SQL_ABSENT)
                            {
                                switch (fmt[columnNumber])
                                {
                                    case FT_FLOAT:
                                        *((float*)(&sqlDataTable[offset]))= 0.0f;
                                        offset+=4;
                                        break;
                                    case FT_IND:
                                    case FT_INT:
                                        *((uint32*)(&sqlDataTable[offset]))=uint32(0);
                                        offset+=4;
                                        break;
                                    case FT_BYTE:
                                        *((uint8*)(&sqlDataTable[offset]))=uint8(0);
                                        offset+=1;
                                        break;
                                    case FT_STRING:
                                        // Beginning of the pool - empty string
                                        *((char**)(&sqlDataTable[offset]))=stringPoolList.back();
                                        offset+=sizeof(char*);
                                        break;
                                }
                            }
                            else if ((*sql->formatString)[columnNumber] == FT_SQL_PRESENT)
                            {
                                bool validSqlColumn = true;
                                switch (fmt[columnNumber])
                                {
                                    case FT_FLOAT:
                                        *((float*)(&sqlDataTable[offset]))=fields[sqlColumnNumber].GetFloat();
                                        offset+=4;
                                        break;
                                    case FT_IND:
                                    case FT_INT:
                                        *((uint32*)(&sqlDataTable[offset]))=fields[sqlColumnNumber].GetUInt32();
                                        offset+=4;
                                        break;
                                    case FT_BYTE:
                                        *((uint8*)(&sqlDataTable[offset]))=fields[sqlColumnNumber].GetUInt8();
                                        offset+=1;
                                        break;
                                    case FT_STRING:
                                        sLog->outError("Unsupported data type in table '%s' at char %d", sql->sqlTableName.c_str(), columnNumber);
                                        return false;
                                    case FT_SORT:
                                        break;
                                    default:
                                        validSqlColumn = false;
                                }
                                if (validSqlColumn && (columnNumber != (sql->formatString->size()-1)))
                                    sqlColumnNumber++;
                            }
                            else
                            {
                                sLog->outError("Incorrect sql format string '%s' at char %d", sql->sqlTableName.c_str(), columnNumber);
                                return false;
                            }
                        }
                        if (sqlColumnNumber != (result->GetFieldCount()-1))
                        {
                            sLog->outError("SQL and DBC format strings are not matching for table: '%s'", sql->sqlTableName.c_str());
                            return false;
                        }

                        fields = NULL;
                        ++rowIndex;
                    }while (result->NextRow());
                }
            }

            // error in dbc file at loading if NULL
            return indexTable.asT != NULL;
        }

        bool LoadStringsFrom(char const* fn)
        {
            // DBC must be already loaded using Load
            if (!indexTable.asT)
                return false;

            StorageLoader dbc;
            // Check if load was successful, only then continue
            if (!dbc.LoadDBCStorage(fn, fmt))
                return false;

            stringPoolList.push_back(dbc.AutoProduceStrings(fmt, (char*)dataTable));

            return true;
        }

        void Clear()
        {
            if (!indexTable.asT)
                return;

            delete [] ((char*)indexTable.asT);
            indexTable.asT = NULL;
            delete [] ((char*)dataTable);
            dataTable = NULL;

            while (!stringPoolList.empty())
            {
                delete [] stringPoolList.front();
                stringPoolList.pop_front();
            }
            nCount = 0;
        }

    private:
        char const* fmt;
        uint32 nCount;
        uint32 fieldCount;
        uint32 recordSize;

        union
        {
            T** asT;
            char** asChar;
        }
        indexTable;

        T* dataTable;
        DataTableEx dataTableEx;
        StringPoolList stringPoolList;
};

#endif
