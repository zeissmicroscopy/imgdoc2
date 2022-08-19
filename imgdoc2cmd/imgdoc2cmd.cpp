#include <imgdoc2.h>

using namespace  std;
using namespace imgdoc2;

int main(int argc, char** argv)
{
    auto create_options = ClassFactory::CreateCreateOptions();
    //create_options->SetFilename(":memory:");
    create_options->SetFilename("d:\\test.db");

    auto doc = ClassFactory::CreateNew(create_options);

    return 0;
}