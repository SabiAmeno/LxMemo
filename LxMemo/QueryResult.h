#pragma once

#include <string>
#include <any>
#include <vector>

using Row = std::vector<std::any>;
using MulRow = std::vector<Row>;

class QueryResult
{
public:
    QueryResult() = default;

    size_t RowCounts() { return rows_; }
    std::any Get(int row, size_t ind);
    Row& Get(int row) { return query_result_[row]; }

    Row& NewRow();
private:
    size_t rows_{ 0 };
    MulRow query_result_{};
};

inline const std::string& any2string(const std::any& val)
{
    return std::any_cast<const std::string&>(val);
}

inline const char* any2chars(const std::any& val)
{
    return any2string(val).c_str();
}