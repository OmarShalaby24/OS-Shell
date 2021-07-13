#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_WORD 10
#define MAX_CHAR 100

typedef void (*sighandler_t)(int);

//function to handle the signals like if the user entered ^C during a process it will ends
void sigint_handler(int signo) {
    printf("\nCaught SIGINT\n");
}
sighandler_t signal(int signum, sighandler_t handler);


//each command ends with enter(\n) so we need to replace it with with end of string(\0)
void remove_endOfLine(char line[])
{
    int i=0;
    while(line[i]!='\n')
        i++;
    line[i++] = '\0';
    line[i] = '\n';
}
/*
read input command, remove last char(\n) to avoid error in arguments of the command
after reading the command. if its ^D or exit, the program exits
*/
void read_line(char line[])
{
    char* ret = fgets(line,MAX_CHAR,stdin);

    if(ret == NULL) // ^D is a null input
        exit(0);

    remove_endOfLine(line);

    if(strcmp(line,"exit")==0)
        exit(0);
    if(strcmp(line,"")==0)
        printf("No command entered\n");
}

/*
divide the line(command) into arguments
argument[0] is the name of the command
argument[1 to n] are the arguments of the main command
*/
int process_line(char* args[], char line[])
{
    int i=0;
    args[i] = strtok(line," ");
    if(args[i] == NULL)
        return 1;
    while(args[i] != NULL){
        i++;
        args[i] = strtok(NULL," ");
    }
    return 1;
}

/*
read the line and process it to be arguments
*/
int read_parse_line(char* args[],char line[])
{
    read_line(line);
    process_line(args,line);
    return 1;
}
/*
check if the argument ends with & to put it in the background and accept other commands without waiting it
*/
int is_background(char line[])
{
    int i=0;
    while(line[i]!='\n'){
        if(line[i]=='&'){
            return 1;
        }
        i++;
    }
    return 0;
}

int main()
{
    char* args[MAX_WORD];
    char line[MAX_CHAR];
    char buf[1000];
    char  *gdir = getcwd(buf,sizeof(buf));
    char  *dir;
    char  *to;

    int b;
    signal(SIGINT, sigint_handler);
    while(1){
        printf("%s>>$ ",gdir);

        read_parse_line(args,line);

        //empty command entered
        if(strcmp(line,"")==0)
            continue;

        //Change directory command
        if (!strcmp(args[0], "cd")){
            gdir = getcwd(buf, sizeof(buf));
            dir = strcat(gdir, "/");
            to = strcat(dir, args[1]);
            chdir(to);
            continue;
        }

        //check if the command will be in background or in foreground
        b = is_background(line);

        pid_t child_pid = fork();
        if(child_pid == 0){
                if(execvp(args[0],args)<0){ //if the entered command does not exist, "Command not found" message appears
                    printf("Command not found\n");
                }
                exit(0);
        }
        else{
            if(!b) // if the command is not in background, parent process will wait it's child process to ends
                //waitpid(child_pid,NULL, 0);
                wait(NULL);
        }

        //chdir(args[1]);
        printf("\n");

    }
    return 0;
}
