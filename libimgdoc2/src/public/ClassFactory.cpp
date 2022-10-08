#include "ClassFactory.h"
#include "../doc/document.h"
#include "../src/db/DbFactory.h"
#include "../src/db/database_creator.h"
#include "../src/db/database_discovery.h"
#include <cstdlib>

using namespace std;
using namespace imgdoc2;

class StandardHostingEnvironment : public IHostingEnvironment
{
public:
    void Log(int level, const char* message) override
    {

    }

    bool IsLogLevelActive(int level) override
    {
        return true;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {
        abort();
    }
};

class NullHostingEnvironment : public IHostingEnvironment
{
public:
    void Log(int level, const char* message) override
    {
    }

    bool IsLogLevelActive(int level) override
    {
        return false;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {
        abort();
    }
};

class HostingEnvironmentOnFunctionPointers : public IHostingEnvironment
{
private:
    std::intptr_t userparam_;
    void (*pfnLog_)(std::intptr_t userparam, int level, const char* szMessage);
    bool (*pfnIsLevelActive_)(std::intptr_t userparam, int level);
    void (*pfnReportFatalErrorAndExit_)(std::intptr_t userparam, const char* szMessage);
public:
    explicit HostingEnvironmentOnFunctionPointers(std::intptr_t userparam,
        void (*pfnLog)(std::intptr_t userparam, int level, const char* szMessage),
        bool (*pfnIsLevelActive)(std::intptr_t userparam, int level),
        void (*pfnReportFatalErrorAndExit)(std::intptr_t userparam, const char* szMessage))
        : userparam_(userparam),
        pfnLog_(pfnLog),
        pfnIsLevelActive_(pfnIsLevelActive),
        pfnReportFatalErrorAndExit_(pfnReportFatalErrorAndExit)
    {}

    void Log(int level, const char* message) override
    {
        if (this->pfnLog_ != nullptr)
        {
            this->pfnLog_(this->userparam_, level, message);
        }
    }

    bool IsLogLevelActive(int level) override
    {
        if (this->pfnIsLevelActive_ != nullptr)
        {
            return this->pfnIsLevelActive_(this->userparam_, level);
        }

        return false;
    }

    void ReportFatalErrorAndExit(const char* message) override
    {

        if (this->pfnReportFatalErrorAndExit_ != nullptr)
        {
            this->pfnReportFatalErrorAndExit_(this->userparam_, message);
        }

        abort();
    }
};

//------------------------------------------------------------------------

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::CreateNew(imgdoc2::ICreateOptions* create_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(create_options->GetFilename().c_str(), environment);

    // check pre-conditions
    // TODO(JBL)

    // tweak settings

    DbCreator db_creator(db_connection);
    const auto configuration = db_creator.CreateTables(create_options);

    const auto database_configuration_2d = std::dynamic_pointer_cast<DatabaseConfiguration2D>(configuration);
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    // TODO(JBL): 3D version should follow here

    return {};
}

/*static*/std::shared_ptr<imgdoc2::IDoc> imgdoc2::ClassFactory::OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options, std::shared_ptr<IHostingEnvironment> environment)
{
    // TODO(JBL): here would be the place where we'd allow for "other databases than Sqlite", for the time being,
    //            we just deal with Sqlite here
    auto db_connection = DbFactory::SqliteCreateNewDatabase(open_existing_options->GetFilename().c_str(), environment);

    DbDiscovery database_discovery{ db_connection };
    database_discovery.DoDiscovery();

    const auto database_configuration_2d = std::dynamic_pointer_cast<DatabaseConfiguration2D>(database_discovery.GetDatabaseConfiguration());
    if (database_configuration_2d)
    {
        return make_shared<Document>(db_connection, database_configuration_2d);
    }

    // TODO(JBL): 3d version should follow here

    return {};
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateStandardHostingEnvironment()
{
    return make_shared<StandardHostingEnvironment>();
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateNullHostingEnvironment()
{
    return make_shared<NullHostingEnvironment>();
}

/*static*/std::shared_ptr<IHostingEnvironment> imgdoc2::ClassFactory::CreateHostingEnvironmentForFunctionPointers(
    std::intptr_t userparam,
    void (*pfnLog)(std::intptr_t userparam, int level, const char* szMessage),
    bool (*pfnIsLevelActive)(std::intptr_t userparam, int level),
    void (*pfnReportFatalErrorAndExit)(std::intptr_t userparam, const char* szMessage))
{
    return make_shared<HostingEnvironmentOnFunctionPointers>(
        userparam,
        pfnLog,
        pfnIsLevelActive,
        pfnReportFatalErrorAndExit);
}
