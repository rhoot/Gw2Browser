#ifndef GW2DATTOOLS_DLLMACROS_H
#define GW2DATTOOLS_DLLMACROS_H

#ifdef GW2DATTOOLS_DLL
#ifdef GW2DATTOOLS_EXPORTS
#define GW2DATTOOLS_API __declspec(dllexport)
#else
#define GW2DATTOOLS_API __declspec(dllimport)
#endif
#define GW2DATTOOLS_APIENTRY __stdcall
#else
#define GW2DATTOOLS_API
#define GW2DATTOOLS_APIENTRY
#endif

#endif // GW2RE_CORETOOLS_DLLMACROS_H
