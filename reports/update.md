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
    
### Learning Goals
<b>Team:</b>
- Learn different types of networking protocols and choose the appropriate protocol for this project
- Learn how to implement network connections in C
- Learn how to design a CLI and learn how processes and pipes work.
- Learn how to work with a string buffer and a screen that need to be updated on every key stroke instead of waiting for a `\n` character.

<b>Seungin</b> - I want to gain deeper insights into how processes work with real-time network I/O and learn how provide a solid user experience when there are string buffer conflicts to be resolved. 

<b>Nathan</b> - I want to gain more experience developing applications in C and to expand on my knowledge of networking from the last project. Real-time editing is also super cool.

<b>Ben</b> - I want to get a better understanding of how real time collaboration works, namely the algorithms and intuitions used to make the work feel natural and provide a strong user experience.

### Where We've Been

Using the ncurses library, we made a wrapper program that passes all input commands to the terminal and prints/returns their responses. We have found plenty of quality resources that helped us get to this point, for example this excellent ncurses [tutorial](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/) and Stephen Brennan's [shell tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/) which helped us troubleshoot and improve our program significantly. Given our current progress, we are confident we can successfully complete the MVP and are optimistic about integrating at least one aspect of the MAP.

### Where We're Going

Moving forward, since we have a functioning terminal, we now want to have it so multiple users can interface with it at once. The largest obstacle in this seems to be getting them to sync correctly. One plan we had for this was pulling from each user's local version after a keystroke, then creating a master copy, the pushing that back out. The difficulty here is the algorithm for updating when the line cursor moves. We've seen a few options online for solving this problem, including operational transforms, or just having faster update times. Three tasks for these to occur will be:

- Integrate our practice with TCP and sockets into our current terminal code.
- Set up a 'pull' function and trigger for it to pull from each client's version.
- Decide on a conflict resolution approach when trying to make a master copy of the terminal.

### Resources
- [Team Trello board](https://trello.com/b/MWdqdabQ/terms4teams)
- [Team Github](https://github.com/zneb97/SoftSys---Terms4Teams)


