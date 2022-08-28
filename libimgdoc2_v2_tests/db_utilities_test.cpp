#include <gtest/gtest.h>
#include "../libimgdoc2_a/inc/imgdoc2.h"
#include "../libimgdoc2_a/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

static void GetColumnNameForDimension(imgdoc2::Dimension d, std::string& columnName)
{
    columnName = "Dim_" + string(1, d);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness1)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1,3 });
    auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "([Dim_Z] > ? AND [Dim_Z] < ?)");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 2);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 3);
}

TEST(Db_Utilities, CreateConditionForDimQueryClauseCheckCorrectness2)
{
    CDimCoordinateQueryClause dim_query_clause;
    dim_query_clause.AddRangeClause('Z', IDimCoordinateQueryClause::RangeClause{ 1,3 });
    dim_query_clause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 5,8 });
    auto query_statement_and_binding_value = Utilities::CreateWhereConditionForDimQueryClause(&dim_query_clause, GetColumnNameForDimension);
    EXPECT_STREQ(get<0>(query_statement_and_binding_value).c_str(), "([Dim_T] > ? AND [Dim_T] < ?) AND ([Dim_Z] > ? AND [Dim_Z] < ?)");
    ASSERT_EQ(get<1>(query_statement_and_binding_value).size(), 4);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[0].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 1);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[1].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 3);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[2].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[0].value), 5);
    EXPECT_TRUE(holds_alternative<int>(get<1>(query_statement_and_binding_value)[3].value));
    EXPECT_EQ(get<int>(get<1>(query_statement_and_binding_value)[1].value), 8);

}