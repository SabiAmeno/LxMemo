#include "db.h"

//sqlite3* NewSqlite3Handle(const char* dbname)
//{
//    sqlite3* s = nullptr;
//    int ret = sqlite3_open_v2("memo.db", &s, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
//    if (ret != 0) {
//        return nullptr;
//    }
//
//    return s;
//}
//
//bool ExecSQL(sqlite3* s, const char* sql, int(*cb)(void*, int, char**, char**), void* arg)
//{
//    char* error = nullptr;
//    int ret = sqlite3_exec(s, sql, cb, arg, &error);
//
//    return ret == 0;
//}