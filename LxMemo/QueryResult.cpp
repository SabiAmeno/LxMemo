#include "QueryResult.h"

std::any QueryResult::Get(int row, size_t ind)
{
    return query_result_[row][ind];
}

Row& QueryResult::NewRow()
{
    rows_++;

    return query_result_.emplace_back(Row());
}

//void QueryResult::Set(std::any&& data)
//{
//    query_result_[rows_ - 1].emplace_back(std::move(data));
//}
