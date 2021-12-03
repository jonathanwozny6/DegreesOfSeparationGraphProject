#ifndef UNTITLED1_CFRAME_H
#define UNTITLED1_CFRAME_H

#include "wx/wx.h"
//#include "Graph.h"
#include <string>
#include <vector>
#include <set>

class cFrame : public wxFrame
{
private:
    std::vector<std::set<int>> adjList;
    unsigned int numUsers;

public:
    void Graph(const unsigned int numberOfUsers, const std::string& fileName);
    void Graph(const std::string& fileName);
    void ReadInput(const std::string& fileName, const bool knownSize);
    void Follow(const unsigned int user, const unsigned int following);
    void Unfollow(const unsigned int user, const unsigned int following);
    unsigned int GetNumFollowing(const unsigned int user);
    unsigned int GetNumUsers();
    unsigned int BFSConnect(const unsigned int user, const unsigned int target);
    bool CheckConnected(const unsigned int user, const unsigned int target);
    unsigned int DijkstrasConnect(const unsigned int user, const unsigned int target);

public:
    cFrame();
    ~cFrame();

public:
    wxButton * m_btn1 = nullptr;
    wxButton * m_btn2 = nullptr;

    wxTextCtrl * m_txt1 = nullptr;
    wxStaticText * m_txtLbl1 = nullptr;
    wxTextCtrl * m_txt2 = nullptr;
    wxStaticText * m_txtLbl2 = nullptr;
    wxStaticText * m_txtLbl3 = nullptr;

    wxStaticText * m_txtLblPath = nullptr;
    wxStaticText * m_txtLblTotCon = nullptr;

    wxListBox * m_list1 = nullptr;

    void OnButtonClicked(wxCommandEvent &evt);
    void OnButtonClickedSec(wxCommandEvent &evt);

    wxDECLARE_EVENT_TABLE();
};


#endif //UNTITLED1_CFRAME_H
