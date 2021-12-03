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
    std::vector<int> DijkstrasConnect(const unsigned int user, const unsigned int target);

public:
    cFrame();
    ~cFrame();

public:
    wxButton * FPBtn = nullptr;
    wxButton * LDBtn = nullptr;

    wxTextCtrl * FrUser = nullptr;
    wxStaticText * FrUseLbl = nullptr;
    wxTextCtrl * TrgUser = nullptr;
    wxStaticText * TrgUseLbl = nullptr;
    wxStaticText * TotCon = nullptr;

    wxStaticText * PthLbl = nullptr;
    wxStaticText * TotConLbl = nullptr;

    wxListBox * PthList = nullptr;

    void OnButtonClicked(wxCommandEvent &evt);
    void OnButtonClickedSec(wxCommandEvent &evt);

    wxDECLARE_EVENT_TABLE();
};


#endif //UNTITLED1_CFRAME_H
