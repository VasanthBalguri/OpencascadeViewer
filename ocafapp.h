#ifndef OCAFAPP_H
#define OCAFAPP_H

#include <TDocStd.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

class OCAFApp: public TDocStd_Application
{
public:
    OCAFApp();

    DEFINE_STANDARD_RTTIEXT(OCAFApp,TDocStd_Application);
};

#endif // OCAFAPP_H
