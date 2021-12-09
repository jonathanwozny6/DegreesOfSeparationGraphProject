# DegreesOfSeparationGraphProject

# Motivation
The motivation for this project is based off of the theory called six degrees of separation.
This theory states that any 2 people in the world are connected by at most 6 people.  
Using graph theory, this would state that any the shortest path between any two vertices (people)
is at most 6 edges. To do this, a Snap Stanford Github dataset was used which consisted of a list
of users (vertices) and connections (edges). The project resulted in a number higher than 6 on average,
with the largest value seen during testing being 11.  This could have resulted because the Github data
was not representative of the entire world population.

# GUI Functions and Features
The GUI for this project displays a 2 textboxes, one for the from user and the other for the target user.
Once any 2 users are entered and the "Find Path" button has been clicked, the GUI displays a list, starting 
with the from user and ending with the target user.  In between are the people that allow the shortest 
connection between the 2 users to be formed. Any of the people in the list can be clicked; doing so will
take the GUI user to the GitHub profile on the default web browser. The maximum degrees of separation between 
the from user and any given user in the graph is displayed above. The profile pictures of the from and target 
user are also displayed off to the side along with their usernames.  Additional user information can be 
displayed, including the number of friends of each of the users, as well as a list of their mutual connections. 
Dijkstra's algorithm and a BFS traversal are used to find the number of connections, and their run time is 
displayed at the bottom of the frame.  

wxWidgets was used to create the GUI

cApp was used to create the frame for the GUI, which is where all the information was displayed.

cFrame was used to create the different functions of the GUI, including the buttons, text boxes,
reading in information inputted by the GUI user, and displaying profile images and data. 

The cURL library was used to read information from HTML code online and retrieve the link to the
Github image.  It was also used to read and download image data.

