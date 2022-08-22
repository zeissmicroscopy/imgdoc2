#include <imgdoc2.h>
#include "document.h"

class DocumentRead2d : public imgdoc2::IDocRead2d
{
private:
    std::shared_ptr < Document> document_;
public:
    DocumentRead2d(std::shared_ptr < Document> document) : document_(std::move(document))
    {}


};