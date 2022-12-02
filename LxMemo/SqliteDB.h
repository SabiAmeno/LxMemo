#pragma once
#include "db.h"
#include "sqlite3.h"

class SqliteDB : public DB
{
public:
    SqliteDB();
    virtual ~SqliteDB();

    virtual bool Open(const std::string&) override;
    virtual void Close() override;
    virtual std::string ErrorString() override;
    virtual SharedQueryResult Query(const std::string& q) override;

private:
    sqlite3* db_{nullptr};

    std::string err_{};
};

//#define TEST
#ifdef TEST

class SqliteDBTest
{
public:
    SqliteDBTest();
    void Test();

private:
    DB* db_{ nullptr };
};

#endif