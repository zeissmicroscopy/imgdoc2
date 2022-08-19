#include "custom_functions.h"
#include <stdexcept> 
#include "custom_functions.h"

using namespace imgdoc2;

/*static*/const std::string SqliteCustomFunctions::queryFunctionName_rtree_linesegment2d = "LineThroughPoints2d";
/*static*/const std::string SqliteCustomFunctions::queryFunctionName_rtree_plane3d = "PlaneNormalDistance3d";
/*static*/const std::string SqliteCustomFunctions::queryFunctionName_scalar_doesintersectwithline = "IntersectsWithLine";

/*static*/const std::string& SqliteCustomFunctions::GetQueryFunctionName(Query q)
{
    switch (q)
    {
    case Query::RTree_LineSegment2D:
        return SqliteCustomFunctions::queryFunctionName_rtree_linesegment2d;
    case Query::RTree_PlaneAabb3D:
        return SqliteCustomFunctions::queryFunctionName_rtree_plane3d;
    case Query::Scalar_DoesIntersectWithLine:
        return SqliteCustomFunctions::queryFunctionName_scalar_doesintersectwithline;
    }

    throw std::invalid_argument("Unknown enumeration");
}

/*static*/void SqliteCustomFunctions::SetupCustomQueries(sqlite3* db)
{
    // TODO: 
    // * May consider this https://www.sqlite.org/c3ref/auto_extension.html instead of registering this stuff here by hand.
    // * It would also be nice to have a loadable extension with this functionality (https://www.sqlite.org/loadext.html).
    auto rc = sqlite3_rtree_query_callback(
        db,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::RTree_LineSegment2D).c_str(),
        SqliteCustomFunctions::LineThrough2Points2d_Query,
        nullptr,
        nullptr);

    rc = sqlite3_rtree_query_callback(
        db,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::RTree_PlaneAabb3D).c_str(),
        SqliteCustomFunctions::Plane3d_Query,
        nullptr,
        nullptr);
    rc = sqlite3_create_function_v2(
        db,
        SqliteCustomFunctions::GetQueryFunctionName(SqliteCustomFunctions::Query::Scalar_DoesIntersectWithLine).c_str(),
        8,
        SQLITE_UTF8 | SQLITE_DETERMINISTIC | SQLITE_DIRECTONLY,
        nullptr,
        SqliteCustomFunctions::ScalarFunctionDoesIntersectWithLine,
        nullptr,
        nullptr,
        nullptr);
}

/*static*/int SqliteCustomFunctions::LineThrough2Points2d_Query(sqlite3_rtree_query_info* info)
{
    LineThruTwoPointsD* pLine = static_cast<LineThruTwoPointsD*>(info->pUser);
    if (pLine == nullptr)
    {
        /* If pUser is still 0, then the parameter values have not been tested
        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */

        /* This geometry callback is for use with a 2-dimensional r-tree table.
        ** Return an error if the table does not have exactly 2 dimensions. */
        if (info->nCoord != 4)
        {
            return SQLITE_ERROR;
        }

        /* Test that the correct number of parameters (4) have been supplied,
        */
        if (info->nParam != 4)
        {
            return SQLITE_ERROR;
        }

        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
        ** if the allocation fails.*/
        pLine = static_cast<LineThruTwoPointsD*>(info->pUser = sqlite3_malloc(sizeof(LineThruTwoPointsD)));
        if (!pLine)
        {
            return SQLITE_NOMEM;
        }

        info->xDelUser = Free_LineThruTwoPointsD;
        pLine->a.x = info->aParam[0];
        pLine->a.y = info->aParam[1];
        pLine->b.x = info->aParam[2];
        pLine->b.y = info->aParam[3];
    }

    const RectangleD rect(info->aCoord[0], info->aCoord[2], info->aCoord[1] - info->aCoord[0], info->aCoord[3] - info->aCoord[2]);

    // check whether the start-/end-point is inside the rectangle
    const bool firstPointInside = rect.IsPointInside(pLine->a);
    const bool secondPointInside = rect.IsPointInside(pLine->b);
    if (firstPointInside && secondPointInside)
    {
        // if both are inside, we report "fully within"
        info->eWithin = FULLY_WITHIN;
    }
    else
    {
        if (firstPointInside || secondPointInside)
        {
            // if one of the start-/end-point is inside - then report "partly within"
            info->eWithin = PARTLY_WITHIN;
        }

        // now we determine whether the line-segment "pLine" intersects with the diagonals
        if (SqliteCustomFunctions::DoLinesIntersect(pLine->a, pLine->b, PointD(rect.x, rect.y), PointD(rect.x + rect.w, rect.y + rect.h)) ||
            SqliteCustomFunctions::DoLinesIntersect(pLine->a, pLine->b, PointD(rect.x, rect.y + rect.h), PointD(rect.x + rect.w, rect.y)))
        {
            info->eWithin = PARTLY_WITHIN;
        }

        else
        {
            info->eWithin = NOT_WITHIN;
        }
    }

    info->rScore = info->iLevel;
    return SQLITE_OK;
}

/*static*/int SqliteCustomFunctions::Plane3d_Query(sqlite3_rtree_query_info* info)
{
    Plane_NormalAndDistD* pPlane = static_cast<Plane_NormalAndDistD*>(info->pUser);
    if (pPlane == nullptr)
    {
        /* If pUser is still 0, then the parameter values have not been tested
        ** for correctness or stored into a "LineThruTwoPoints" structure yet. Do this now. */

        /* This geometry callback is for use with a 3-dimensional r-tree table.
        ** Return an error if the table does not have exactly 3 dimensions. */
        if (info->nCoord != 6)
        {
            return SQLITE_ERROR;
        }

        /* Test that the correct number of parameters (4) have been supplied */
        if (info->nParam != 4)
        {
            return SQLITE_ERROR;
        }

        /*Allocate a structure to cache parameter data in.Return SQLITE_NOMEM
        ** if the allocation fails.*/
        pPlane = static_cast<Plane_NormalAndDistD*>(info->pUser = sqlite3_malloc(sizeof(Plane_NormalAndDistD)));
        if (!pPlane)
        {
            return SQLITE_NOMEM;
        }

        info->xDelUser = Free_PlaneNormalAndDistD;
        pPlane->normal.x = info->aParam[0];
        pPlane->normal.y = info->aParam[1];
        pPlane->normal.z = info->aParam[2];
        pPlane->distance = info->aParam[3];
    }

    CuboidD aabb(info->aCoord[0], info->aCoord[2], info->aCoord[4],
        info->aCoord[1] - info->aCoord[0], info->aCoord[3] - info->aCoord[2], info->aCoord[5] - info->aCoord[4]);
    const bool doIntersect = SqliteCustomFunctions::DoAabbAndPlaneIntersect(aabb, *pPlane);
    if (doIntersect)
    {
        info->eWithin = PARTLY_WITHIN;
    }
    else
    {
        info->eWithin = NOT_WITHIN;
    }

    info->rScore = info->iLevel;
    return SQLITE_OK;
}

/*static*/void SqliteCustomFunctions::Free_LineThruTwoPointsD(void* p)
{
    sqlite3_free(p);
}

/*static*/void SqliteCustomFunctions::Free_PlaneNormalAndDistD(void* p)
{
    sqlite3_free(p);
}

/*static*/bool SqliteCustomFunctions::DoLinesIntersect(const imgdoc2::PointD& a1, const imgdoc2::PointD& a2, const imgdoc2::PointD& b1, const imgdoc2::PointD& b2)
{
    const PointD b(a2.x - a1.x, a2.y - a1.y);
    const PointD d(b2.x - b1.x, b2.y - b1.y);

    const double bDotDPerp = b.x * d.y - b.y * d.x;

    // if b dot d == 0, it means the lines are parallel so have infinite intersection points
    if (bDotDPerp == 0)
        return false;

    const PointD c(b1.x - a1.x, b1.y - a1.y);// = b1 - a1;
    const double t = (c.x * d.y - c.y * d.x) / bDotDPerp;
    if (t < 0 || t > 1)
    {
        return false;
    }

    const double u = (c.x * b.y - c.y * b.x) / bDotDPerp;
    if (u < 0 || u > 1)
    {
        return false;
    }

    return true;
}

/*static*/bool SqliteCustomFunctions::DoAabbAndPlaneIntersect(const imgdoc2::CuboidD& aabb, const imgdoc2::Plane_NormalAndDistD& plane)
{
    return aabb.DoesIntersectWith(plane);
}

/*static*/void SqliteCustomFunctions::ScalarFunctionDoesIntersectWithLine(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if (argc != 8)
    {
        return  sqlite3_result_null(context);
    }

    const double x = sqlite3_value_double(argv[0]);
    const double y = sqlite3_value_double(argv[1]);
    const double w = sqlite3_value_double(argv[2]);
    const double h = sqlite3_value_double(argv[3]);
    const double p1x = sqlite3_value_double(argv[4]);
    const double p1y = sqlite3_value_double(argv[5]);
    const double p2x = sqlite3_value_double(argv[6]);
    const double p2y = sqlite3_value_double(argv[7]);

    const RectangleD rect(x, y, w, h);
    const LineThruTwoPointsD twoPoints{ {p1x,p1y},{p2x,p2y} };

    bool doesIntersect = false;

    // check whether the start-/end-point is inside the rectangle
    if (rect.IsPointInside(twoPoints.a) || rect.IsPointInside(twoPoints.b))
    {
        // if at least one point is inside the rectangle, then we are done
        doesIntersect = true;
    }
    else
    {
        // now we determine whether the line-segment "twoPoints" intersects with the diagonals
        if (SqliteCustomFunctions::DoLinesIntersect(twoPoints.a, twoPoints.b, PointD(rect.x, rect.y), PointD(rect.x + rect.w, rect.y + rect.h)) ||
            SqliteCustomFunctions::DoLinesIntersect(twoPoints.a, twoPoints.b, PointD(rect.x, rect.y + rect.h), PointD(rect.x + rect.w, rect.y)))
        {
            doesIntersect = true;
        }
    }

    return sqlite3_result_int(context, doesIntersect ? 1 : 0);
}
