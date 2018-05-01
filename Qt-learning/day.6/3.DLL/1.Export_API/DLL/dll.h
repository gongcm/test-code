#ifndef DLL_H
#define DLL_H

#include "dll_global.h"

class DLLSHARED_EXPORT DLL
{

public:
    DLL();
    ~DLL();
};

extern "C" DLLSHARED_EXPORT int add(int x=0, int y=0);

#endif // DLL_H
