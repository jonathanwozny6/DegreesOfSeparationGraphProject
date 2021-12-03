#include "cApp.h"

cApp::cApp()
{

}

cApp::~cApp()
{

}

bool cApp::OnInit()
{
    cFrame * m_frame1 = new cFrame();
    m_frame1->Show(true);
    return true;
}