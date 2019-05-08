# Terms4Teams - A Collaborative Terminal 

Seungin Lyu, Nathan Shuster, Benjamin Ziemann

### Project Summary

Terms4Teams is a CLI (Command Line Interface) that supports real-time collaboration with your team. By establishing a network connection among the team members, we let a team live-share a terminal and empower them to seamlessly work on cool projects! We built the terminal on top of the [ncurses](https://www.gnu.org/software/ncurses/ncurses.html) library which allows for real-time screen and buffer management.

## Project Goals
### MVP
- Build a terminal (on top of ncurses library) which updates the screen and the buffer on every keystroke 
- Establish a network connection between a host and a client using socket
- Enable the terminal to execute basic commands `(ls, cd, cat, mkdir, rm, git, ls, etc` as child processes and pipe the results back to the parent terminal.
    
### MAP (Most Awesome Project)
We do have some stretch goals.
- Enable multiple people (more than 3 people) to connect to a selected terminal and edit and run command line arguments in real time.
- Create a sidebar window that shows the status of and information about all connected users.
    
### Where We've Been

Using the ncurses library, we made a wrapper program that passes all input commands to the terminal and prints/returns their responses. This includes built in commands like `ls` and `cat` in addition to several implemented commands like `cd`, `help`, and `exit`.

Please take a look at our [demo](https://drive.google.com/open?id=1kTatByUHmqYgjs_GM8AlblINYWshqY2r) of the terminal!

On the networking side of the programming we've had more obstacles. We identified a work flow for which the data will be processed (see Figure 1). However while we have made a simple socket connection between a client and server program, we are only currently able to pass a single character from the client where it is consolidated by the server before being returned to the client's terminal. Anything more than that is not input. We believe this is currently caused on the server sides, specifically our process creation when accepting new characters and how and when it is being called to process data that we are getting hung up on.

![Figure 1: The plan for the program and how data is processed and in what order](https://github.com/zneb97/SoftSys---Terms4Teams/blob/master/reports/assets/workflow.png)


### Where We're Going

The next step in the remaining time will be to debug the current networking code to find and fix what is stopping us from having more than one input character. We have an idea that the stoppage has to do with process creation when a input is recieved by the server so we have a starting point. Once this clears it will be a matter of ensuring we integrated our terminal code with the network

While it is unlikely we will get to it, the nearest stretch goal after completing the above would be to add better syncing between multiple clients. That way if two people are typing at the same time, the result is not a mix of both inputs.

### Reflection
<b>Team:</b>

We believe we had set ourselves some challenging initial goals (build a terminal from scratch, establish connections between a server and some clients, synchronize the buffers) and we are happy that we were able to achieve the majority of the initial goals.

<b>Seungin</b> - I wanted to gain deeper insights into how a server and its clients work when string buffers need to synchronize quickly. I've learned some good lessons I wish I had known before jumping into this project. 

1. There is a limit on the size of your stack frame (read this Stackoverflow [article](https://stackoverflow.com/questions/216259/is-there-a-max-array-length-limit-in-c) if you did not know, and forgetting this fact will give you nightmares with segmentation faults. 
   ```
   ----WRONG----
   #define BUFSIZE 1000000000 // Giving Buffer array enough size 
   int main(){
       char buff[BUFSIZE];
   }
   ```
2. Incremental intergration is definetely easier than trying to merge in giant pull requests at once. Our team had a working terminal code, a working server code, and a wokring client code and merging these together did not make us happy (especially with C)
3. A debugger can save you a lot of time by telling you exactly on which line a segmentation fault occured

<b>Nathan</b> - I wanted to gain more experience developing applications in C and to expand on my knowledge of networking from the last project. I succeeded in building my skills and confidence but a concussion in the final week unfortunately made it difficult for me to help polish up and iron out points that the group was stuck on.

<b>Ben</b> - I wanted to get a better understanding of how real time collaboration works, namely the algorithms and intuitions used to make the work feel natural and provide a strong user experience.

### Resources

We have found plenty of quality resources that helped us along the course of this project.
- If you are working on a project that involves a terminal receving real-time input from the user, we recommend using the [ncurses](https://en.wikipedia.org/wiki/Ncurses) library as we did. There are [alternatives](https://alternativeto.net/software/ncurses/) to ncurses available, but we believe ncurses has the best [documentation and tutorial](https://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/) for beginners. 
- If you are building from scratch a terminal that executes commands, we recommend Stephen Brennan's [shell tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/) which helped us troubleshoot and improve our program significantly.
- If you are a building server and clients that use sockets, GeeksforGeeks has a nice [tutorial](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/) on the concepts. Head First C Chapter 11 (Sockets and Networking) has a lot more details on the concept and we highly checking out both resources.


### Links 
- [Team Github](https://github.com/zneb97/SoftSys---Terms4Teams)
- [Team Trello board](https://trello.com/b/MWdqdabQ/terms4teams)


