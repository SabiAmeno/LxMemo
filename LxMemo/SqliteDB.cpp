#include "SqliteDB.h"
#include "QueryResult.h"
#include <cassert>
#include "DBFactory.h"

SqliteDB::SqliteDB()
{

}

SqliteDB::~SqliteDB()
{
    Close();
}

bool SqliteDB::Open(const std::string& dbname)
{
    int ret = sqlite3_open_v2(dbname.c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (ret) {
        err_ = sqlite3_errmsg(db_);
    }
    return ret == SQLITE_OK;
}

void SqliteDB::Close()
{
    if (db_) {
        sqlite3_close_v2(db_);
        db_ = nullptr;
    }
}

std::string SqliteDB::ErrorString()
{
    return err_;
}

int query_call(void* p, int rows, char** row_data, char** col_name)
{
     auto qr = (QueryResult*)p;
     auto& row = qr->NewRow();

     for (int i = 0; i < rows; i++) {
         std::string d;
         if (row_data[i])
             d.assign(row_data[i]);
         row.emplace_back(d);
     }

     return 0;
}

SharedQueryResult SqliteDB::Query(const std::string& q)
{
    SharedQueryResult qr(new QueryResult());
    char* error = nullptr;

    int ret = sqlite3_exec(db_, q.c_str(), query_call, qr.get(), &error);
    if (0 != ret) {
        err_ = sqlite3_errmsg(db_);
        return nullptr;
    }

    return qr;
}

#ifdef TEST

SqliteDBTest::SqliteDBTest()
{
    auto create_db_sql = R"(
    CREATE TABLE IF NOT EXISTS Test(
            ID BIGINT PRIMARY KEY NOT NULL,
            TYPE TEXT);
    )";
    
    auto insert_data_sql = R"(
    INSERT INTO Test VALUES(0, "FILE");
    )";

    auto get_data_sql = R"(
    select * from Test;
    )";
    
    auto get_no_data_sql = R"(select * from Test where ID=1;)";

    //db_ = new SqliteDB();
    db_ = DBFactory::Create("sqlite3");
    auto isopen = db_->Open("test.db");
    if (isopen) {
        db_->Query(create_db_sql);
        db_->Query(insert_data_sql);
        auto r = db_->Query(get_data_sql);
        if (r) {
            assert(r->RowCounts() == 1);
            auto sid = std::any_cast<std::string>(r->Get(0, 0));
            auto stype = std::any_cast<std::string>(r->Get(0, 1));

            assert(sid == "0");
            assert(stype == "FILE");
        }

        r = db_->Query(get_no_data_sql);
        if (r) {
            assert(r->RowCounts() == 0);
        }
    }
}

void SqliteDBTest::Test()
{

}

#endif