#include "document.h"
#include "documentRead2d.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/std::shared_ptr<imgdoc2::IDocWrite2d> Document::GetWriter2d()
{
    return std::shared_ptr<imgdoc2::IDocWrite2d>();
}

/*virtual*/std::shared_ptr<imgdoc2::IDocRead2d> Document::GetReader2d()
{
    return std::make_shared<DocumentRead2d>(shared_from_this());
}