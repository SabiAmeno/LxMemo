#include "DBFactory.h"
#include <map>
#include <functional>
#include "SqliteDB.h"

using DbCreator = std::function<DB*()>;
using SupportDB = std::map<std::string, DbCreator>;
static SupportDB kSupportDB = { 
    { "sqlite3", [] {return DB::New<SqliteDB>(); }},
};

DB* DBFactory::Create(const std::string& db_type)
{
    if (kSupportDB.contains(db_type))
        return kSupportDB[db_type]();

    return nullptr;
}
