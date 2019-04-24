/* 
* Server code to sync and push master copy of terminal to clients
*
* Adapted from Head First C and Allen Downey's Software Systems class
*
*/

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 9000
#define BUF_SIZE 1024
#define CLIENT_NUM 30
#define CHAT_NUM 10
#define BACKLOG 3

int main(int argc, char *argv[])
{
    //Termianl line management
    char buf[255];
    char buf_old[255];
    int ch; // Stored character for analysis
    int b_pos = 0; // position in the buffer
    char **tokens;
    int status = 0; // status flag foor the terminal loop
    int row, col;
    int y, x; // cursor position

    //Client management
    int opt = 1;
    int client_count = 0;
    int master_socket , addrlen , new_socket , client_sockets[CLIENT_NUM] ,
          max_clients = CLIENT_NUM , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    char usernames[CLIENT_NUM][BUF_SIZE];
    char chat[CHAT_NUM][BUF_SIZE]; //name of chat the user opts into
    int flag[CLIENT_NUM] = {0}; //0 when no client; 1 when username set but not chat room not set; 2 when username and chat room set
    
    fd_set readfds;
    
    char *welcome_message = "Welcome to to: \n";

    //initialise all client_sockets[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_sockets[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //Set master socket to allow multiple connections
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //Type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //Bind the socket to localhost port 9000
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //Maximum of queued connections
    if (listen(master_socket, BACKLOG) < 0)
    {
        perror("Listenr");
        exit(EXIT_FAILURE);
    }

    //Accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");


    while(1){
        //Add master socket to cleared set
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //Add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            sd = client_sockets[i];
            if(sd > 0){
                FD_SET( sd , &readfds);
            }
            if(sd > max_sd){
                max_sd = sd;
            }
        }

        //Timeouts
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR)){
            printf("select error");
        }

        //New Connections
        if (FD_ISSET(master_socket, &readfds)){
            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0){
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            //send new connection greeting welcome_message
            if( send(new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) ){
                perror("send");
            }

            puts("Welcome welcome_message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++){
                if( client_sockets[i] == 0 ){
                    client_sockets[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
        }

        //Deal with new inputs
        for (i = 0; i < max_clients; i++){
            sd = client_sockets[i];

            if (FD_ISSET( sd , &readfds)){

                //User disconnect
                if ((valread = read( sd , buf, BUF_SIZE)) == 0){
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr),
                    ntohs(address.sin_port));
                    //Clean up
                    close(sd);
                    client_sockets[i] = 0;
                }

                //Handle new buffer
                else{
                  buf[valread] = '\0';
                  //Update local string
                  //
                   
                      
            
            }
            //Send out to all other clients
            for (int j = 0; j < max_clients; j++) {
                sd = client_sockets[j];
                if (j != i && (strcmp(chat[i], chat[j])==0)) {
                        send(sd , buf , strlen(buf) , 0 );
                      }
            }
        }
    }
    return 0;
}
