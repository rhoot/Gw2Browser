#include "gw2DatTools/exception/Exception.h"

namespace gw2dt
{
namespace exception
{

Exception::Exception(const char* iReason) :
    std::exception(iReason)
{
}

Exception::~Exception()
{
}

}
}
