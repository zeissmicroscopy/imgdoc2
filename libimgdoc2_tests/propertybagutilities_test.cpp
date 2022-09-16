#include <gtest/gtest.h>
#include "../libimgdoc2/inc/imgdoc2.h"
#include "../libimgdoc2/src/db/utilities.h"
#include "../libimgdoc2/src/db/DbFactory.h"

using namespace std;

// Tests concerned with the "Property-Bag-Utilities" (Utilities::WriteStringIntoPropertyBag, Utilities::TryReadStringFromPropertyBag, ...)
// are found here.

TEST(PropertyBagUtilities, WriteStringAndReadItCompareResult)
{
    const char* item_value = "TestValue123";

    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");

    dbConnection->Execute("CREATE TABLE [TESTTABLE]([Key] TEXT(40) UNIQUE,[ValueString] TEXT) ");

    Utilities::WriteStringIntoPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        item_value);

    string value_from_propertybag;
    bool b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        &value_from_propertybag);

    EXPECT_TRUE(b);
    EXPECT_STREQ(value_from_propertybag.c_str(), item_value);
}

TEST(PropertyBagUtilities, WriteStringAndOverwriteItAndReadItCompareResult)
{
    const char* item_value_first = "TestValue123";
    const char* item_value_second = "TestValue1234567";

    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");

    dbConnection->Execute("CREATE TABLE [TESTTABLE]([Key] TEXT(40) UNIQUE,[ValueString] TEXT) ");

    Utilities::WriteStringIntoPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        item_value_first);

    string value_from_propertybag;
    bool b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        &value_from_propertybag);

    EXPECT_TRUE(b);
    EXPECT_STREQ(value_from_propertybag.c_str(), item_value_first);

    // here we overwrite the value we put in above
    Utilities::WriteStringIntoPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        item_value_second);

    // ...and expect to find the overwritten value now
    b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        &value_from_propertybag);

    EXPECT_TRUE(b);
    EXPECT_STREQ(value_from_propertybag.c_str(), item_value_second);
}

TEST(PropertyBagUtilities, ReadNonExistingKeyAndCheckError)
{
    const char* item_value = "TestValue123";

    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");

    dbConnection->Execute("CREATE TABLE [TESTTABLE]([Key] TEXT(40) UNIQUE,[ValueString] TEXT) ");

    Utilities::WriteStringIntoPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        item_value);

    string value_from_propertybag;
    bool b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey2",
        &value_from_propertybag);

    EXPECT_FALSE(b);
    EXPECT_TRUE(value_from_propertybag.empty());
}

TEST(PropertyBagUtilities, AddItemAndDeleteItAndCheckThatItIsGone)
{
    const char* item_value = "TestValue123";

    auto dbConnection = DbFactory::SqliteCreateNewDatabase(":memory:");

    dbConnection->Execute("CREATE TABLE [TESTTABLE]([Key] TEXT(40) UNIQUE,[ValueString] TEXT) ");

    Utilities::WriteStringIntoPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        item_value);

    string value_from_propertybag;
    bool b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        &value_from_propertybag);

    EXPECT_TRUE(b);
    EXPECT_STREQ(value_from_propertybag.c_str(), item_value);

    Utilities::DeleteItemFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1");

    b = Utilities::TryReadStringFromPropertyBag(
        dbConnection.get(),
        "TESTTABLE",
        "Key",
        "ValueString",
        "TestKey1",
        &value_from_propertybag);

    EXPECT_FALSE(b);
}

