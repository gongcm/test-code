#include "dll.h"
#include <QtGlobal>
#include <QtGlobal>

#include "form.h"

DLL::DLL()
{
    obj = new Form();
    obj->show();
}

DLL::~DLL()
{
    Q_ASSERT(NULL != obj);
    obj->deleteLater();
}


void getobject(void)
{
    DLL *app = new DLL();
    Q_ASSERT(NULL != app);
}
