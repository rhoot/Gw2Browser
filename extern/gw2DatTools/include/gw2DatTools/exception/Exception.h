#ifndef GW2DATTOOLS_EXCEPTION_EXCEPTION_H
#define GW2DATTOOLS_EXCEPTION_EXCEPTION_H

#include <exception>

#include "gw2DatTools/dllMacros.h"

namespace gw2dt
{
namespace exception
{

class GW2DATTOOLS_API Exception: public std::exception
{
    public:
        Exception(const char* iReason);
        virtual ~Exception();
};

}
}

#endif // GW2DATTOOLS_EXCEPTION_EXCEPTION_H
