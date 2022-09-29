#pragma once

/// This struct is used to return the result of are IDocRead2d-query call.
#pragma pack(push, 4)
struct QueryResultInterop
{
    int element_count;
    bool more_results_available;
    imgdoc2::dbIndex indices[];
};
#pragma  pack(pop)
