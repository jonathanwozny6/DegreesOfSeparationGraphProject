#ifndef UNTITLED1_CFRAME_H
#define UNTITLED1_CFRAME_H

#include "wx/wx.h"
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

// these libraries are for opening a link online
#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>

class cFrame : public wxFrame
{
private:
    int clicks;

    std::vector<std::set<unsigned int>> adjList; // adjacency list to hold number of users
    unsigned int numUsers; // total number of users in graph
    std::unordered_map<unsigned int, std::string> userMap; // map between node number and userName
    int oldFrUser; // the purpose of this is to keep track of the previous searched user
                   // if the value is not changed in the from user text box, then BFSConnect will not be
                   // called again. If it is different, then BFSConnect will be called to get the new
                   // maximum degree of separation

    void OnButtonClicked(wxCommandEvent &evt);  // event button for Finding the Path
    void OnStatButtonClicked(wxCommandEvent &evt); // event button for clicking the user stats button
    void URLButtonClicked(wxCommandEvent &evt); // event button for opening url of double clicked user
    void openURL(const std::string &webURL);    // helper function to open url --> ONLY FOR MAC
                                                // https://www.codegrepper.com/code-examples/cpp/c%2B%2B+open+website+in+default+browser
                                                // the above url has code for windows (really easy implementation)

    // helper function to download an image from a url
    bool DownloadImage1(std::string& webStringURL, int num);
    bool DownloadImage2(std::string& webStringURL, int num);


    // helper function for retrieving image data from a url
    static size_t CallBack(void *pointer, size_t size, size_t nMemb, void* userData);

    // helper function for Call Back to retrieve HTML code
    static size_t CallBackHTML(void *text, size_t size, size_t nMemb, void *userPtr);

    // retrieves URL of github user's profile image
    std::string RetrieveProfileURL(char* webURL);


    wxDECLARE_EVENT_TABLE(); // for events (i.e. clicking buttons)

public:

    // Graph Data and Functions
    void Graph(const std::string& fileName);
    void ReadInput(const std::string& fileName);
    void Follow(const unsigned int user, const unsigned int following);
    void Unfollow(const unsigned int user, const unsigned int following);
    unsigned int GetNumFollowing(const unsigned int user);
    unsigned int GetNumUsers();
    unsigned int BFSConnect(const unsigned int user, const unsigned int target);
    bool CheckConnected(const unsigned int user, const unsigned int target);
    std::vector<unsigned int> DijkstrasConnect(const unsigned int user, const unsigned int target);
    std::set<unsigned int> GetFriends(const unsigned int user);
    std::set<unsigned int> GetMutualFriends(const unsigned int user1, const unsigned int user2);
    unsigned int GetNumFriends(const unsigned int user);

    // constructor and destructor for the frame
    cFrame();
    ~cFrame();

    // objects for the frame
    wxButton * FPBtn = nullptr;         // "Find Path" button

    wxTextCtrl * FrUser = nullptr;      // input text/box for "From User"
    wxStaticText * FrUseLbl = nullptr;  // "From User" label
    wxTextCtrl * TrgUser = nullptr;     // input text/box for "Target User"
    wxStaticText * TrgUseLbl = nullptr; // "Target User" label
    wxStaticText * TotCon = nullptr;    // text to display of Max degree of Sep.

    wxStaticText * PthLbl = nullptr;    // "Path" label
    wxStaticText * TotConLbl = nullptr; // "Max Deg. of Separation" Label

    wxListBox * PthList = nullptr;      // "Path" List Box

    wxButton * StatsBtn = nullptr;         //Shows the friend stats
    wxStaticText * NumFrndLabel = nullptr;  // "Number of friends" label
    wxStaticText * Underline = nullptr; // "Number of friends" label underline
    wxStaticText * FrNumFrnd = nullptr; // Number of friends of the "From User"
    wxStaticText * TrgNumFrnd = nullptr;// Num of friends of the "Target User"
    wxStaticText * MutFrndLabel = nullptr;  //"List of mutual friends" label
    wxListBox * MutFrndList = nullptr;  // List of mutual friends
    wxStaticText * FromUserDisp = nullptr; // username of From user for display
    wxStaticText * TargetUserDisp = nullptr; // username of Target user for display

    // timing variables
    std::string DTime;
    std::string BTime;
    wxStaticText * DijkTime = nullptr;
    wxStaticText * BFSTime = nullptr;

    // for deletion of statistics button and numfriends info
    bool del = false;
    bool needDel = false;
};

#endif //UNTITLED1_CFRAME_H
