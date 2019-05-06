/* 
* Server code to sync and push master copy of terminal buffer to clients
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

#define PORT 8000
#define BUF_SIZE 1024
#define MESSAGE_SIZE 1000000
#define CLIENT_NUM 30
#define TOKSIZE 64
#define TOKEN_DELIM " \t\r\n\a"
#define CHAT_NUM 10
#define BACKLOG 3

// Defining ctrl+c to get around ncurses raw() mode
#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

/*
  Function Declarations for builtin shell commands:
 */
int t4t_cd(char **args);
int t4t_help(char **args);
int t4t_exit(char **args);
char** parse_line(char* line);
int t4t_execute(char **args);


//Create processes to run the commands
char* run_line(char **args){
    
    int status = 0;
    int     fd[2], nbytes;
    char    readbuffer[80];
    
    pipe(fd);
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        // Child process
        close(fd[0]);    // close reading end in the child

        dup2(fd[1], 1);  // send stdout to the pipe
        dup2(fd[1], 2);  // send stderr to the pipe

        close(fd[1]);    // this descriptor is no longer needed
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "Command Not Found\n");
        }
        exit(1);
    } 
    else if (pid < 0) {
        // Error forking
        close(fd[0]);    // close reading end in the child

        dup2(fd[1], 1);  // send stdout to the pipe
        dup2(fd[1], 2);  // send stderr to the pipe
        close(fd[1]);
        fprintf(stderr, "Command Not Found\n");
        return "Command not found";
    } 
    else {
        // Parent process
        close(fd[1]);
        do { 
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        
        // shiitty hack to give the buffer enough size
        char buffer[MESSAGE_SIZE];
       if(read(fd[0], buffer, sizeof(buffer)) != 0)
        {
            // need to return the address to the buffer that contains command output
            return strdup(buffer);
        }
    }
    return "nonstandard";
}
/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &t4t_cd,
  &t4t_help,
  &t4t_exit
};

int t4t_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int t4t_cd(char **args)
{
  if (args[1] == NULL) {
    printf("t4t: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("t4t");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int t4t_help(char **args)
{
  int i;
  for (i = 0; i < t4t_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }
  printf("Sorry, we don't offer help \n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int t4t_exit(char **args)
{
  exit(1);
}

int t4t_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }
  for (i = 0; i < t4t_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  return 0;
}

/*
* Breaks the built up buffer into tokens. Called before running
* the line
*
* line - built up string buffer from user inputs
*/
char** parse_line(char* line){
    int bufsize = BUF_SIZE;
    int b_position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;

    if (!tokens) {
        printf("Error: memory allocation failed for tokens\n");
        exit(1);
    }
    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[b_position] = token;
        b_position++;

        if (b_position >= bufsize) {
        bufsize += TOKSIZE;
        tokens_backup = tokens;
        tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                free(tokens_backup);
                printf("Error: memory reallocation failed for tokens\n");
                exit(1);
            }
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[b_position] = NULL;
    return tokens;
}

int main(int argc, char *argv[]){
    // Termianl line management
    char client_char[2]; //The single char sent to the server from clients
    char command_buf_prev[255]; //The full buffer up to most recent recieve
    char command_buf_curr[255]; //The full buffer including the most recent recieve
    char* result_buffer; // Pointer to the strdupped result of command 
    int ch; // Stored character for analysis
    int b_pos = 0; // position in the buffer
    char **tokens;
    int status = 0; // status flag foor the terminal loop
    int location = 0; //Cursor location in the terminal
    int exec_flag = 0; //If a command was executed in the loop

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
    
    char *welcome_message = "Welcome to : \n";

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
            // if( send(new_socket, welcome_message, strlen(welcome_message), 0) != strlen(welcome_message) ){
            //     perror("send");
            // }
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
                if ((valread = read( sd , client_char, BUF_SIZE)) == 0){
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr),
                    ntohs(address.sin_port));
                    //Clean up
                    close(sd);
                    client_sockets[i] = 0;
                }

                //Handle new buffer
                else{

                    //RUN COMMAND
                    if(strcmp(client_char, "\n")==0){
                        //Make it a proper string
                        command_buf_curr[location] = '\0';
                        //Execute command
                        char *line = strndup(command_buf_curr,location);
                        printf("Command Received : %s\n",line);
                        char **tokens = parse_line(line);
                        char* out;
                        int status = t4t_execute(tokens);
                        if(status == 0){
                            result_buffer = run_line(tokens);                          
                        }
                        exec_flag = 1;
                        location = 0;
                    }

                    //ADD TO COMMAND BUFFER
                    else{
                        //Assume we are only getting one character per update
                        command_buf_curr[location] = client_char[0];
                        location++;
                    }

                    //Send to all clients
                    for (int j = 0; j < max_clients; j++) {
                        sd = client_sockets[j];
                        //Do not send if same as last
                        if (strcmp(command_buf_curr, command_buf_prev) != 0) {
                            //Command was run, send the output
                            printf("curr : %s | prev: %s\n", command_buf_curr, command_buf_prev);
                            if(exec_flag == 1){
                                printf("Sending client result : %s\n", result_buffer);
                                send(sd , result_buffer , strlen(command_buf_curr) , 0 );
                                exec_flag = 0;
                            }
                            //New character, update command buffer
                            else{
                                printf("Sending all clients command update : %s\n", command_buf_curr);
                                send(sd , command_buf_curr , strlen(command_buf_curr) , 0 );
                            }
                            strcpy(command_buf_prev, command_buf_curr);
                        }
                    }
                    
                }
            }
        }
    }
    return 0;
}
