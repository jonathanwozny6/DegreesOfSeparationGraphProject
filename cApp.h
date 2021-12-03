#ifndef UNTITLED1_CAPP_H
#define UNTITLED1_CAPP_H

#include "wx/wx.h"
#include "cFrame.h"

class cApp : public wxApp
{
public:
    cApp();
    ~cApp();

public:
    virtual bool OnInit();
};


#endif //UNTITLED1_CAPP_H
