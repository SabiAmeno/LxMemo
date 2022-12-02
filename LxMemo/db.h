#pragma once

#include <string>
#include <memory>
//#include "sqlite3.h"

//sqlite3* NewSqlite3Handle(const char* dbname);
//bool ExecSQL(sqlite3* s, const char* sql, int(*cb)(void*, int, char**, char**) = 0, void* arg = 0);

class QueryResult;
using SharedQueryResult = std::shared_ptr<QueryResult>;

class DB
{
public:
    virtual ~DB() {}
    virtual bool Open(const std::string&) = 0;
    virtual void Close() = 0;
    virtual std::string ErrorString() = 0;
    virtual SharedQueryResult Query(const std::string& q) = 0;

    template<typename T>
    static DB* New()
    {
        return new T();
    }
};