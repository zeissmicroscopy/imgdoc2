#pragma once

#pragma pack(push, 1)
struct DimensionQueryRangeClauseInterop
{
    imgdoc2::Dimension dimension;
    int start;
    int end;
};

struct DimensionQueryClauseInterop
{
    int element_count;
    DimensionQueryRangeClauseInterop query_range_clause;
};
#pragma pack(pop)
