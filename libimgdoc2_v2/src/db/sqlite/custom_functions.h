#pragma once

#include <string>
#include <sqlite3.h>
#include <types.h>

/// Custom Queries are found here - here we register custom extensions.
/// Currently, we extend R-Tree queries (https://sqlite.org/rtree.html).
class SqliteCustomFunctions
{
private:
  friend class CustomQueriesTest;
  static const std::string queryFunctionName_rtree_linesegment2d;
  static const std::string queryFunctionName_rtree_plane3d;
  static const std::string queryFunctionName_scalar_doesintersectwithline;
public:
  enum class Query
  {
    RTree_LineSegment2D,
    RTree_PlaneAabb3D,
    Scalar_DoesIntersectWithLine
  };

  static void SetupCustomQueries(sqlite3* db);

  static const std::string& GetQueryFunctionName(Query q);
private:
  static int LineThrough2Points2d_Query(sqlite3_rtree_query_info* info);
  static int Plane3d_Query(sqlite3_rtree_query_info* info);

  static void Free_LineThruTwoPointsD(void* p);
  static void Free_PlaneNormalAndDistD(void* p);

  static bool DoLinesIntersect(const imgdoc2::PointD& a1, const imgdoc2::PointD& a2, const imgdoc2::PointD& b1, const imgdoc2::PointD& b2);
  static bool DoAabbAndPlaneIntersect(const imgdoc2::CuboidD& aabb, const imgdoc2::Plane_NormalAndDistD& plane);
  static void ScalarFunctionDoesIntersectWithLine(sqlite3_context* context, int argc, sqlite3_value** argv);
};
