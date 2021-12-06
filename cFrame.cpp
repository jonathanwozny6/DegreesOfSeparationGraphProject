#include "cFrame.h"
#include <fstream>
#include <limits.h>
#include <queue>
#include <set>
#include <iostream>

wxBEGIN_EVENT_TABLE(cFrame, wxFrame)
    EVT_BUTTON(10001, cFrame::OnButtonClicked)
wxEND_EVENT_TABLE()

cFrame::cFrame() : wxFrame(nullptr, wxID_ANY, "Degrees of Separation", wxPoint(30, 30), wxSize(500, 400))
{
    FPBtn = new wxButton(this, 10001, "Find Path", wxPoint(50,110), wxSize(100,80));

    FrUser = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10,30), wxSize(200,30));
    FrUseLbl = new wxStaticText(this, wxID_ANY, "Enter From User", wxPoint(10, 10), wxSize(200, 30));
    TrgUser = new wxTextCtrl(this, wxID_ANY, "", wxPoint(10,90), wxSize(200,30));
    TrgUseLbl = new wxStaticText(this, wxID_ANY, "Enter Target User", wxPoint(10, 70), wxSize(200, 30));
    TotCon = new wxStaticText(this, wxID_ANY, "", wxPoint(355, 80), wxSize(30,30));

    PthLbl = new wxStaticText(this, wxID_ANY, "Path: ", wxPoint(10, 170), wxSize(200, 30));
    TotConLbl = new wxStaticText(this, wxID_ANY, "Max Deg. of Separation: ", wxPoint(310, 40), wxSize(200, 30));

    PthList = new wxListBox(this, wxID_ANY, wxPoint(10,190), wxSize(460,150));

    oldFrUser = -1;

    // create graph
    //Graph("twitter-2010.txt");
    Graph("musae_git_edges.csv");
    //Graph("nodes.txt");
    //Graph("newNodes.txt");
}

cFrame::~cFrame()
{

}

void cFrame::OnButtonClicked(wxCommandEvent &evt)
{
    // get connections to list in wxListBox
    int in, out, result;
    in = wxAtoi(FrUser->GetValue());
    if (in == 0 && FrUser->GetValue() != "0") {
        std::cout << "Must Input Integer Value" << std::endl;
        return;
    }
    else {
        out = wxAtoi(TrgUser->GetValue());
        std::vector<int> path = DijkstrasConnect(in, out);

        // clear the list box
        PthList->Clear();

        // print out the path
        if (path.empty())
            PthList->AppendString("No Connection");
        else
        {
            // print from user
            PthList->AppendString(std::to_string(in));

            if (path[0] != -1) {
                // print users in between
                for (int i = path.size() - 1; i > -1; i--)
                    PthList->AppendString(std::to_string(path[i]));
            }
            // print target user
            PthList->AppendString(std::to_string(out));
        }

        if (in != oldFrUser)
        {
            result = BFSConnect(in, out);
            if (result == -1)
                TotCon->SetLabel("'From User' not Found");
            else
               TotCon->SetLabel(std::to_string(result));
        }

        oldFrUser = in;
    }
    evt.Skip(); // tell computer that this event has been handled
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
        std::cout << "Unable to open input file" << std::endl;
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
            //input >> comma;
            input >> following;
            if (user+1 > adjList.size())
                adjList.resize(user+1);
            adjList[user].insert(following);
            if (following + 1 > adjList.size())
                adjList.resize(following+1);
            adjList[following].insert(user);
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

    return numConn-1;
    return UINT_MAX;
}

std::vector<int> cFrame::DijkstrasConnect(const unsigned int user, const unsigned int target)
{
    // your code
    // path vector to return
    std::vector<int> path;

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