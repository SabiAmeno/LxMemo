#pragma once

#include "db.h"

class DBFactory
{
public:
    static DB* Create(const std::string& db_type);
private:
    DBFactory() = default;
};

