#ifndef DLL_H
#define DLL_H

#include "dll_global.h"

class Form;

class DLLSHARED_EXPORT DLL
{

public:
    DLL();
    ~DLL();

private:
    Form *obj;
};

extern "C" DLLSHARED_EXPORT void getobject(void);

#endif // DLL_H
