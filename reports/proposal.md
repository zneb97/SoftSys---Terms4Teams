# Terms4Teams - Collaborate on Terminal
## Benjamin Ziemann, Seungin Lyu, Nathan Shuster

## Project Description

A CLI(Command Line Interface) that supports real-time collaboration with your team. By establishing a network connection among the team members, we let the team live-share a terminal and empower them to seamlessly work on cool projects!

## Project Goals
  ### MVP
    - Create a CLI tool that allows two people to both edit a single terminal together in real time over a network.
      - Implement Network connection (Using Socket)
      - Implement redirection of commands to a default native CLI tool (tentative)
    - The CLI will allow for basic command uses (Git commands, ls, cd, etc)
    
  ### MAP(Most Awesome Project)
    - Create a CLI that allows multiple people to connect to a selected terminal and edit it in real time.
    - Create a sidebar view that shows connected user information and user status.
    
## Learning Goals
    - Learn different types of networking protocols and choose the appropriate protocol for this project
    - Learn how to implement network connections in C
    - Learn how to design a CLI

## Plans Going Forward
As we start, we plan to research more about network communications and learn which protocol will best help us achieve our goal. We will also look into existing similar solutions, like VS Code's collaborative document sharing. For the creation of the CLI we will For our first steps we will
    - Identify and select a communications protocol to use for the project that has support in C.
    - Establish a network connection between our two devices using said protocol.
    - Create a shell with no commands (yet)

Networks and communications seemed to be a popular topic during the project ideation session in class and several previous projects also seem to utilize it. Because of this, sharing resources or reaching out to the NINJAs or previous students of the class seems like it would work well for human resources. As far as online resources, there seems to be a host of create your own shell blogs. Stack overflow will of course be helpful for more specific questions, especially in helping us parse the needed information for communication protocol implementations and usages.
