#include "cFrame.h"
#include <fstream>
#include <limits.h>
#include <queue>
#include <iostream>
#include <curl/curl.h>
#include <math.h>
#include <chrono>


// Event table for clicking stuff
wxBEGIN_EVENT_TABLE(cFrame, wxFrame)
    EVT_BUTTON(10001, cFrame::OnButtonClicked)
    EVT_LISTBOX_DCLICK(10002, cFrame::URLButtonClicked)
    EVT_BUTTON(10003, cFrame::OnStatButtonClicked)
wxEND_EVENT_TABLE()

cFrame::cFrame() : wxFrame(nullptr, wxID_ANY, "Degrees of Separation", wxPoint(30, 30), wxSize(500, 440))
{
    // create new image handler for image
    wxJPEGHandler* handlerJPEG = new wxJPEGHandler;
    wxImage::AddHandler(handlerJPEG);

    // Find Path Button
    FPBtn = new wxButton(this, 10001, "Find Path", wxPoint(50,110), wxSize(100,80));

    // Refer to cFrame.h to see meaning of each object
    FrUser = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10,30), wxSize(200,30));
    FrUseLbl = new wxStaticText(this, wxID_ANY, "Enter From User", wxPoint(10, 10), wxSize(200, 30));
    TrgUser = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10,90), wxSize(200,30));
    TrgUseLbl = new wxStaticText(this, wxID_ANY, "Enter Target User", wxPoint(10, 70), wxSize(200, 30));
    TotCon = new wxStaticText(this, wxID_ANY, "--", wxPoint(160, 170), wxSize(30,30));

    PthLbl = new wxStaticText(this, wxID_ANY, "Path: ", wxPoint(10, 190), wxSize(200, 30));
    TotConLbl = new wxStaticText(this, wxID_ANY, "Max Deg. of Separation: ", wxPoint(10, 170), wxSize(200, 30));

    PthList = new wxListBox(this, 10002, wxPoint(10,210), wxSize(230,150));

    oldFrUser = -1; // set to default -1 for first run

    CreateStatusBar(); // Status Bar --> Good to use for error messages on bottom of frame

    // create graph
    Graph("musae_git_edges.csv"); // Github graph
}

// Destructor --> not used
cFrame::~cFrame()
{

}

// Find Path button
void cFrame::OnButtonClicked(wxCommandEvent &evt)
{
    //Remove Stat Fields
    if (needDel == true){
        delete NumFrndLabel;
        delete Underline;
        delete FrNumFrnd;
        delete TrgNumFrnd;
        delete MutFrndLabel;
        delete MutFrndList;
        needDel = false;
    }

    // remove timing and username displays
    delete FromUserDisp;
    delete TargetUserDisp;
    delete DijkTime;
    delete BFSTime;

    // delete stats button if not deleted
    if (del == false)
    {
        delete StatsBtn;
        del = true;
    }


    // get connections to list in wxListBox
    int in, out, result;

    // convert user to integer
    in = wxAtoi(FrUser->GetValue());

    // 0 is the default value when nothing is input or string is input
    if ((in == 0 && FrUser->GetValue() != "0")) {
        SetStatusText("Must Input Integer Value"); // error message
        return;
    }
    else if (in > 37699)
    {
        SetStatusText("Maximum node size is 37699. Re-input lower value.");
        return;
    }
    else {
        SetStatusText("");
        out = wxAtoi(TrgUser->GetValue()); // convert target user to integer

        // find the path and display
        std::vector<unsigned int> path = DijkstrasConnect(in, out);
        DijkTime = new wxStaticText(this, wxID_ANY, "Dijkstra's Time: " + DTime + " µs", wxPoint(10,360));
        FromUserDisp = new wxStaticText(this, wxID_ANY, userMap[wxAtoi(FrUser->GetValue())], wxPoint(250,95));
        TargetUserDisp = new wxStaticText(this, wxID_ANY, userMap[wxAtoi(TrgUser->GetValue())], wxPoint(370, 95));

        // clear the list box
        PthList->Clear();

        // print out the path
        if (path.empty())
            PthList->AppendString("No Connection");
        else
        {
            std::string myTab;

            // print from user
            if (std::to_string(in).length() < 4)
                myTab = "\t\t";
            else myTab = "\t";
            PthList->AppendString(std::to_string(in) + myTab + userMap[in]);

            // if not max value (aka -1)
            if (path[0] != -1) {
                // print users in between
                for (int i = path.size() - 1; i > -1; i--)
                {
                    if (std::to_string(path[i]).length() < 4)
                        myTab = "\t\t";
                    else myTab = "\t";
                    PthList->AppendString(std::to_string(path[i]) + myTab + userMap[path[i]]);
                }

            }
            // print target user
            if (std::to_string(out).length() < 4)
                myTab = "\t\t";
            else myTab = "\t";
            PthList->AppendString(std::to_string(out) + myTab + userMap[out]);
        }

        if (in != oldFrUser)
        {
            // if the From User has been changed, re-calculate largest degree of separation
            result = BFSConnect(in, out);

            if (result == -1)
                TotCon->SetLabel("'From User' not Found"); // error message
            else
                TotCon->SetLabel(std::to_string(result)); // display the result
        }
        BFSTime = new wxStaticText(this, wxID_ANY, "BFS Time: " + BTime + " µs", wxPoint(10, 380));

        oldFrUser = in; // old user is now what was input previously
    }

    // get the url to the user profile
    std::string FRName = "https://github.com/";
    FRName = FRName + userMap[wxAtoi(FrUser->GetValue().ToStdString())];

    // char pointer to insert into function
    char* profileURL1 = &FRName[0];

    // retrieve URL for USER profile picture
    std::string webURL1 = RetrieveProfileURL(profileURL1);

    // call the function to download image; if the download is successful
    int num = 1;
    if (DownloadImage1(webURL1, num)) {

        // create image from retrieved data
        wxImage* img = new wxImage("profile1.jpeg");

        // rescale the image
        img->Rescale(80, 80, wxIMAGE_QUALITY_NORMAL);

        // place image in GUI
        wxStaticBitmap* image1 = new wxStaticBitmap( this, wxID_ANY, *img, wxPoint(250,10));
    }

    // get the url to the target profile
    std::string TrgName = "https://github.com/";
    TrgName = TrgName + userMap[wxAtoi(TrgUser->GetValue().ToStdString())];

    // char pointer to insert into function
    char* profileURL2 = &TrgName[0];

    // retrieve URL for TARGET profile picture
    std::string webURL2 = RetrieveProfileURL(profileURL2);

    // call the function to download image; if the download is successful
    num = 2;
    if (DownloadImage2(webURL2, num)) {


        // create image from retrieved data
        wxImage* img2 = new wxImage("profile2.jpeg");

        // rescale the image
        img2->Rescale(80, 80, wxIMAGE_QUALITY_NORMAL);

        // place image in GUI
        wxStaticBitmap* image2 = new wxStaticBitmap( this, wxID_ANY, *img2, wxPoint(370,10));
    }

    //Generate Statistics Button
    StatsBtn = new wxButton (this, 10003, "User Statistics", wxPoint( 250, 110), wxSize(200, 30));
    del = false; // stats button needs to be deleted now

    evt.Skip(); // tell computer that this event has been handled
}

// User Stats button
void cFrame::OnStatButtonClicked(wxCommandEvent &evt)
{
    //Get usernums
    unsigned int FrInt = wxAtoi(FrUser->GetValue());
    unsigned int TrgInt = wxAtoi(TrgUser->GetValue());

    //Remove Stats button
    delete StatsBtn;
    del = true; // stats button has been deleted already

    //Find number of friends each
    unsigned int FrCount = GetNumFriends(FrInt);
    unsigned int TrgCount = GetNumFriends(TrgInt);

    //Fill in data
    NumFrndLabel = new wxStaticText(this, wxID_ANY, "Number of Friends:", wxPoint(285, 120), wxSize(200, 30));
    Underline = new wxStaticText(this, wxID_ANY, "____________________", wxPoint(285, 121), wxSize(200, 30));
    FrNumFrnd = new wxStaticText(this, wxID_ANY, std::to_string(FrCount), wxPoint(289, 150), wxSize(200, 50));
    TrgNumFrnd = new wxStaticText(this, wxID_ANY, std::to_string(TrgCount), wxPoint(401, 150), wxSize(200, 50));
    MutFrndLabel = new wxStaticText(this, wxID_ANY, "Mutual Friends:", wxPoint(250, 190), wxSize(200, 30));
    MutFrndList = new wxListBox(this, 10002, wxPoint(250,210), wxSize(230,150));

    // find the path
    std::set<unsigned int> friends = GetMutualFriends(FrInt, TrgInt);

    // print out the list of mutual friends
        //Check for any mutuals
    if (friends.empty())
        MutFrndList->AppendString("No Mutual Friends");
    else
    {
        for (auto user : friends)
        {
            MutFrndList->AppendString(std::to_string(user));
        }
    }
    needDel = true; // the stat fields need to be deleted now
}

// Double Click in Path List button
void cFrame::URLButtonClicked(wxCommandEvent &evt)
{
    // convert wxstring object that was double-clicked to integer
    int userNum = wxAtoi(PthList->GetString(PthList->GetSelection()).ToStdString());

    // find name in map to use in url
    std::string name = userMap[userNum];

    // open the url for the selected person
    openURL("https://github.com/" + name);
    evt.Skip(); // tell the computer that this event has been handled
}

// opens a browser to persons profile
// NOTE: THIS FUNCTION IS FOR A MAC
// BORROWED FROM: https://stackoverflow.com/questions/4177744/c-os-x-open-default-browser
void cFrame::openURL(const std::string &webURL) {

    // create reference to CFURL object using specified url
    CFURLRef url = CFURLCreateWithBytes (NULL, (UInt8*)webURL.c_str(), webURL.length(),kCFStringEncodingASCII, NULL);

    // function to open a browser and travel to the specified website in the default browser (Safari)
    LSOpenCFURLRef(url,0);

    // releases the CFURL object
    CFRelease(url);
}

// Function to get retrieve image data
size_t cFrame::CallBack(void *pointer, size_t size, size_t nMemb, void* userData)
{
    // CITE: https://stackoverflow.com/questions/42336140/using-c-curl-how-to-get-image-by-url-that-needed-user-id-password

    // create stream
    FILE* strm = (FILE*)userData;

    // check for stream
    if (!strm)
    {
        printf("Stream was not created! \n");
        return 0;
    }

    // write stream of pixel data to return
    size_t pixelData = fwrite((FILE*)pointer, size, nMemb, strm);

    // return the stream of data
    return pixelData;
}


// Function to download a jpeg image
// CITE: https://stackoverflow.com/questions/42336140/using-c-curl-how-to-get-image-by-url-that-needed-user-id-password
bool cFrame::DownloadImage1(std::string& webStringURL, int num)
{

    // create file to store profile picture
    FILE* image1 = fopen("profile1.jpeg", "wb");

    // pointer to URL
    char* webURL = &webStringURL[0];

    // if the file could not be created, return false
    if (!image1)
    {
        std::cout << "Failed to open output file for JPEG download!" << std::endl;
        return false;
    }

    // pointer to CURL object
    CURL* cURL = curl_easy_init();

    // specifies the url to access
    curl_easy_setopt(cURL, CURLOPT_URL, webURL);

    // specifies where to save the image
    curl_easy_setopt(cURL, CURLOPT_WRITEDATA, image1);

    // specifies the callback function
    curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, CallBack);

    // tells the curl to access the specified url location
    curl_easy_setopt(cURL, CURLOPT_FOLLOWLOCATION, 1);

    // request to download image
    CURLcode request = curl_easy_perform(cURL);

    // check for result of request
    if (request) {
        std::cout << "Failed to download JPEG!" << std::endl;
        return false;
    }

    // initialize response code variable
    long response = 0;

    // receive the http response code
    curl_easy_getinfo(cURL, CURLINFO_RESPONSE_CODE, &response);

    // check to make sure the response code does not produce an error
    if (!((response == 200 || response == 201) && request != CURLE_ABORTED_BY_CALLBACK))
    {
        std::cout << "Response code failure: " << response << std::endl;
        return false;
    }

    // end curl session
    curl_easy_cleanup(cURL);

    // close output into image
    fclose(image1);

    // successful image download
    return true;
}


// Function to download a jpeg image
// CITE: https://stackoverflow.com/questions/42336140/using-c-curl-how-to-get-image-by-url-that-needed-user-id-password
bool cFrame::DownloadImage2(std::string& webStringURL, int num)
{

    // create file to store profile picture
    FILE* image2 = fopen("profile2.jpeg", "wb");

    // pointer to URL
    char* webURL = &webStringURL[0];

    // if the file could not be created, return false
    if (!image2)
    {
        std::cout << "Failed to open output file for JPEG download!" << std::endl;
        return false;
    }

    // pointer to CURL object
    CURL* cURL = curl_easy_init();

    // specifies the url to access
    curl_easy_setopt(cURL, CURLOPT_URL, webURL);

    // specifies where to save the image
    curl_easy_setopt(cURL, CURLOPT_WRITEDATA, image2);

    // specifies the callback function
    curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, CallBack);

    // tells the curl to access the specified url location
    curl_easy_setopt(cURL, CURLOPT_FOLLOWLOCATION, 1);

    // request to download image
    CURLcode request = curl_easy_perform(cURL);

    // check for result of request
    if (request) {
        std::cout << "Failed to download JPEG!" << std::endl;
        return false;
    }

    // initialize response code variable
    long response = 0;

    // receive the http response code
    curl_easy_getinfo(cURL, CURLINFO_RESPONSE_CODE, &response);

    // check to make sure the response code does not produce an error
    if (!((response == 200 || response == 201) && request != CURLE_ABORTED_BY_CALLBACK))
    {
        std::cout << "Response code failure: " << response << std::endl;
        return false;
    }

    // end curl session
    curl_easy_cleanup(cURL);

    // close output into image
    fclose(image2);

    // successful image download
    return true;
}

// CallBack function to retrieve HTML code
// CITE:  https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
size_t cFrame::CallBackHTML(void *text, size_t size, size_t nMemb, void *userPtr)
{
    // add text to string for storing
    ((std::string*)userPtr)->append((char*)text, size * nMemb);
    return size * nMemb;
}

// Function to retrieve HTML code from specified URL
// CITE:  https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
std::string cFrame::RetrieveProfileURL(char* webURL)
{
    // create pointer to cURL object
    CURL *cURL = curl_easy_init();

    // response code
    CURLcode response;

    // string to read the string in the HTML code
    std::string htmlData;

    // if the curl pointer exists
    if(cURL) {
        //*** START BORROWED CODE
        // specifies the URL to access
        curl_easy_setopt(cURL, CURLOPT_URL, webURL);

        // specifies the callback function
        curl_easy_setopt(cURL, CURLOPT_WRITEFUNCTION, CallBackHTML);

        // reads in the HTML string into the variable htmlData
        curl_easy_setopt(cURL, CURLOPT_WRITEDATA, &htmlData);

        // retrieve response code
        response = curl_easy_perform(cURL);

        // end reading of HTML code
        curl_easy_cleanup(cURL);

        //*** END BORROWED CODE

        // create index for traversing through string of HTML code
        unsigned long long index = 0;

        // string to stop at
        std::string stopSearch = "og:image";
        std::string currSearch = htmlData.substr(0, 8);
        int stopSize = stopSearch.size();

        // while we are not at the specified line
        while (currSearch != stopSearch)
        {
            // move to the next character
            index++;

            // move to the next substring
            currSearch = htmlData.substr(index, stopSize);
        }

        // variable to store the number of ' " ' counted
        int numQuotes = 0;

        // stop at the 2rd quotation mark
        while (numQuotes != 2)
        {
            // if a quotation mark is detected, increase the count of them
            if (htmlData[index] == '"')
                numQuotes++;

            // move to the next character
            index++;
        }

        // string to store the url, or "link", to return
        std::string link;

        // retrieve the URL
        while (htmlData[index] != '"')
        {
            // concatenate character onto the URL
            link += htmlData[index];

            // move to the next character
            index++;
        }

        // return the URL
        //return &link[0];
        return link;
    }

    std::string defaultLink = "https://avatars.githubusercontent.com/u/71037154?v=4";

    // if failure to read the HTML occurs, return default profile picture
    //return &defaultLink[0];
    return defaultLink;
}

void cFrame::Graph(const std::string& fileName)
{
    ReadInput(fileName);
}

void cFrame::ReadInput(const std::string& fileName)
{
    // create input stream
    std::ifstream input;

    // open input stream
    input.open(fileName);

    // check that input stream was opened properly
    bool opened = true;
    if (!input){
        opened = false;
        std::cout << "Unable to open input file 1" << std::endl;
    }

    if (opened)
    {
        // variables to store the user and person the user follows
        unsigned int user;
        unsigned int following;
        char comma;

        // while the end of the file is not yet reached
        while (!input.eof()) {
            input >> user;
            input >> comma;
            input >> following;
            if (user+1 > adjList.size())
                adjList.resize(user+1);
            adjList[user].insert(following);
            if (following + 1 > adjList.size())
                adjList.resize(following+1);
            adjList[following].insert(user); // make it undirected graph
        }
        input.close();
    }

    // create map between nodes and usernames
    input.open("musae_git_target.csv");
    opened = true;
    if (!input){
        opened = false;
        std::cout << "Unable to open input file 2" << std::endl;
    }
    if (opened)
    {
        unsigned int numUser;
        std::string userName;
        char comma;
        int junk; // extra binary digit at end

        // while the end of the file is not yet reached
            // there are no spaces between commas and strings so must use getline
        while (input >> numUser >> comma && getline(input, userName, ',') && input >> junk) {
            userMap[numUser] = userName;
        }

        input.close();
    }

}

void cFrame::Follow(const unsigned int user, const unsigned int following)
{
    adjList[user].insert(following);
}

void cFrame::Unfollow(const unsigned int user, const unsigned int following)
{
    adjList[user].erase(following);
}

unsigned int cFrame::GetNumFollowing(const unsigned int user)
{
    return adjList[user].size();
}

unsigned int cFrame::GetNumUsers()
{
    return adjList.size()-1;
}

unsigned int cFrame::BFSConnect(const unsigned int user, const unsigned int target)
{
    // start of measuring time
    auto tBegin = std::chrono::high_resolution_clock::now();

    // if not present in list do nothing
    if (adjList.size() < user)
        return UINT_MAX;

    // create queue for breadth first search and push the start vertex
    std::queue<int> q;
    q.push(user);

    // create set to keep track of visited vertices and insert first vertex
    std::set<unsigned int> visited;
    visited.insert(user);

    // variable to store number of connections (distance away from source vertex)
    unsigned int numConn = 0;

    // while the whole graph has been searched and that target has not been found
    while (!q.empty())
    {
        // for each in the next level
        unsigned int size = q.size();
        for (int i = 0; i < size; i++)
        {
            unsigned int u = q.front();
            q.pop();

//            if (u == target)
//                return numConn;

            // for each adjacent node to the current
            for (auto v : adjList[u])
            {
                // if the node has not been visited
                if (visited.find(v) == visited.end())
                {
                    // add it to the queue
                    q.push(v);

                    // mark as visited
                    visited.insert(v);
                }
            }
        }

        // move to the next level
        numConn++;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    unsigned long long int opTime;
    opTime = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tBegin).count();
    BTime = std::to_string(opTime);

    // farthest reach in breadth first search
    return numConn-1;
}

std::vector<unsigned int> cFrame::DijkstrasConnect(const unsigned int user, const unsigned int target)
{
    // path vector to return
    std::vector<unsigned int> path;

    if (!CheckConnected(user, target))
        return path;

    // distance vector (no negative distances)
    std::vector<unsigned int> d;

    // vector containing previous vertices
    std::vector<int> p;

    // set containing vertices that have not been visited
    std::set<unsigned int> unvisited;

    // weight of each edge
    unsigned int weight = 1;

    // start of measuring time
    auto tBegin = std::chrono::high_resolution_clock::now();

    // initialize vectors and sets
    for (int i = 0; i < adjList.size(); i++)
    {
        // insert all vertices into unvisited
        unvisited.insert(i);

        // set all distances to infinity
        d.push_back(UINT_MAX);

        // set previous nodes to -1
        p.push_back(-1);
    }

    // start at the source vertex
    d[user] = 0;
    int v = user;

    // keep track of whether the vertex has been found
    bool found = false;

    // while not every vertex has been visited
    while (!unvisited.empty() && !found)
    {
        // remove the visited vertex from unvisited
        unvisited.erase(v);

        // relax and update distances to directed vertices
        for (auto u : adjList[v])
        {
            // if the there is a shorter path to the connected vertex, record it
            if (d[u] > d[v] + weight)
            {
                // set the shorter distance
                d[u] = d[v] + weight;

                // record the previous node
                p[u] = v;
            }
        }

        // find the next vertex with the smallest distance that is unvisited
        // set min to the maximum size of an integer
        unsigned int min = UINT_MAX;

        // find the smallest distance of the unvisited vertex
        for (auto next : unvisited)
        {
            // find the minimum distance and the corresponding vertex
            if (min > d[next])
            {
                min = d[next];
                v = next;
            }
        }

        // the target value has been found
        if (v == target)
            found = true;
    }

    // backtrack using previous nodes and find the shortest route from target to user
    unsigned int numConn = 0;    // number of connections to return
    if (found) {
        // set prev to previous node
        unsigned int prev = p[target];


        // while the start node is not reached
        while (prev != user) {
            path.push_back(prev);
            prev = p[prev];     // set prev to the previous node
            numConn++;          // increase the number of edges in the path
        }
        // add the last edge
        numConn++;
    }
    // eng of measuring time
    auto tEnd = std::chrono::high_resolution_clock::now();

    // calculate total time
    unsigned long long int opTime;
    opTime = std::chrono::duration_cast<std::chrono::microseconds>(tEnd - tBegin).count();
    DTime = std::to_string(opTime);

    // if the node is found, return the number of connections
    if (found) {
        if (path.size() == 0)
            path.push_back(-1);
        return path;
    }
        // otherwise return "infinity"
    else return path;
}

// check to make sure that 2 nodes are connected and in the same graph
// uses a BFS
bool cFrame::CheckConnected(const unsigned int user, const unsigned int target)
{
    // check if present
    if (user > adjList.size())
        return false;

// use BFS
    // create queue and push the start
    std::queue<unsigned int> q;
    q.push(user);

    // create set to keep track of visited vertices
    std::set<unsigned int> visited;
    visited.insert(user);

    while(!q.empty())
    {
        unsigned int u = q.front();
        q.pop();

        for (unsigned int v : adjList[u])
        {
            // if the adjacent node has not been visited
            if (visited.find(v) == visited.end())
            {
                if (v == target)
                    return true;
                // add to the queue
                q.push(v);

                // mark as visited
                visited.insert(v);
            }
        }
    }


    if (numUsers == visited.size())
        return true;
    else return false;
}

// returns the number of mutual friends a user has
std::set<unsigned int> cFrame::GetMutualFriends(const unsigned int user1, const unsigned int user2)
{
    // create set
    std::set<unsigned int> mutual;

    //
    for (int x : adjList[user1])
    {
        if (adjList[user2].find(x) != adjList[user2].end())
        mutual.insert(x);
        }

    return mutual;
}

// returns the set of friends that a user has
std::set<unsigned int> cFrame::GetFriends(const unsigned int user)
{
    return adjList[user];
}

unsigned int cFrame::GetNumFriends(const unsigned int user)
{
    return adjList[user].size();
}
