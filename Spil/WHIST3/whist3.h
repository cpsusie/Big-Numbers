#pragma once

#include "resource.h"       // main symbols
#include "GameTypes.h"

class CWhist3App : public CWinApp {
public:
    CWhist3App();
    Options m_options;
public:
    virtual BOOL InitInstance();

    DECLARE_MESSAGE_MAP()
};

extern CWhist3App theApp;
