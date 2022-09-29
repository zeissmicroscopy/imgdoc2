#pragma once

struct QueryResultInterop
{
    int element_count;
    bool more_results_available;
    imgdoc2::dbIndex indices[];
};