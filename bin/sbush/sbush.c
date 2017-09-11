#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syscalls.h>
#include <env.h>

#define MAX_ARGS 10

/* declarations */
int getcmd(char* buf, int max, int fd);
struct cmd* parsecmd(char* buf);
struct cmd* getpipe(char** src, char* dst);
struct cmd* getexec(char** src, char* dst);
void fetchtoken(char** ps, char* dst);
struct cmd* formEcmd();
struct cmd* formBcmd(struct cmd* cmd);
void runcmd(struct cmd* cmd);
void execute_cmd(char **argv, char **envp);

extern char** environ;

// for read: should cover it with gets

struct cmd 
{
    char type;    // need a general template to identify commands
};

struct ecmd
{
    char type;  // type of command, here we distinguish between piple and execut command
    char* argv[MAX_ARGS];       // command name 
};

struct pcmd
{
    char type;         // pipe command basically holds two executable commands, left and right
    struct cmd* left;
    struct cmd* right;
};

struct bcmd
{
    char type;
    struct cmd* cmd;
};

int getcmd(char* buf, int max, int fd)
{
    // roughly cover read() to terminate at newline
    int i, cc;
    char c;

    /* need to implement a prompt message */
    if(fd == 0)
        printf("%s > ", getenv("PS1"));
    
    memset(buf, 0, max);

    for(i=0;i<max-1;++i){
        cc = read(fd, &c, 1);

        if(cc < 0) {
            // error
            return -1;
        }

        //end of line condition
        if(c == '\r' || c == '\n' || cc == 0){
            buf[i] = '\0';
            if(cc == 0){
                return 0;
            }
            break;
        }else{
            buf[i] = c;
        }
    }

    //return something positive
    return 1;

}

// form an exectuable command, but return it as a general command
struct cmd* formEcmd(){
    struct ecmd* cmd;
    cmd = malloc(sizeof(struct ecmd));
    // printf("the allocated space is %ld\n", sizeof(struct ecmd));

    cmd->type = 'e';

    return (struct cmd*)cmd;
}

struct cmd* formPcmd(struct cmd* left, struct cmd* right){
    struct pcmd* cmd;
    cmd = malloc(sizeof(struct pcmd));

    cmd->type = 'p';
    cmd->left = left;
    cmd->right = right;

    return (struct cmd*)cmd;
}

struct cmd* formBcmd(struct cmd* cmd){
    struct bcmd* subcmd;
    subcmd = malloc(sizeof(struct bcmd));

    subcmd->type = 'b';
    subcmd->cmd = cmd;

    return (struct cmd*)subcmd;

}
void fetchtoken(char** ps, char* dst){
    char* q;

    q = *ps;
    //always clean up whitespace 
    while(*q == ' ') {
        q++;
    }

    while((*q != '|') && (*q != '&') && (*q != ' ') && (*q != '\0')) {
        q++;
    }

    //if this is a whitespace, we will null terminated it for the argument.
    if(*q == ' '){
        *q = '\0';
    }

    *ps = ++q;

}


struct cmd* getexec(char** src, char* dst){
    char* ps, *sq;
    int argc;
    struct ecmd* cmd;

    ps = *src;
    argc = 0;

    cmd = (struct ecmd*)formEcmd();
    // take the first argument as command, rest as arguments
    while((*ps != '|') && (*ps != '&') && (*ps != '\0')) {
        sq = ps;
        // printf("ps is %s\n", sq);
        fetchtoken(&ps,dst);
        cmd->type = 'e';
        cmd->argv[argc] = sq;
        argc++;
        *src = ps;
    }

    return (struct cmd*)cmd;


}

struct cmd* getpipe(char** src, char* dst){
    char* ps;
    struct cmd* cmd;

    ps = *src;

    //always clean up whitespace 
    while(*ps == ' ') {
        ++ps;
    }

    *src = ps;

    // take the first command, and form an executable command
    cmd = getexec(src, dst);
    if(*(*src) == '|'){
        //null terminate this for previous argument
        *(*src) = '\0';
        ++(*src);
        cmd = formPcmd(cmd, getpipe(src, dst));
    }

    return cmd;

}

//to parse command, 
struct cmd* parsecmd(char* buf){
    //first we should check if a command contains pipe
    char* src, *dst;
    struct cmd* command;

    //get the head and tail of the line
    src = buf;
    dst = src + strlen(buf);

    //create a pipe command
    command = getpipe(&src, dst);


    if(src == dst){ 
        // printf("reach the end\n");
    }

    //now we check if this command should be running in background
    if(*src == '&'){
        // null terminated the last argument
        *src = '\0';
        command = formBcmd(command);
    }

    if(src != dst){ 
        // printf("reach the end\n");
        //error, something left unprocessed
    }
    return command;



}

void runcmd(struct cmd* cmd){
    pid_t pid;
    int p[2];
    struct ecmd* esub;
    struct bcmd* bsub;
    struct pcmd* psub;
    int status;
    int ret;

    if(cmd == 0)
        exit(0);

    //printf("command type %c\n", cmd->type);
    switch (cmd->type){
        case 'e':
            esub = (struct ecmd*) cmd; 
            // printf("command %s running\n", esub->argv[0]);
            // printf("with arguements:\n");
            // for(i = 0; esub->argv[i]; i++)
            if ((ret = execvpe(esub->argv[0], esub->argv, environ)) < 0) {  
                printf("error code: %d\n", ret);
                printf("failed to execute %s\n", esub->argv[0]);
            }
            //  printf("%s\n", esub->argv[i]);
            break;
        case 'b': 
            bsub = (struct bcmd*) cmd;
           // printf("background command running\n");
           //
            pid = fork();
            if(pid == 0){
                runcmd(bsub->cmd);
            }else if(pid < 0){
                exit(1);
            }
            break;

        case 'p':
            psub = (struct pcmd*) cmd;
            if(pipe(p) < 0){
                printf("ERROR: piping failed\n");
                exit(1);
            }

            pid = fork();
            if(pid < 0) {
                printf("ERROR: forking failed\n");
                exit(1);
            }

            if(pid == 0){
                //close stdout
                close(1);
                //assign write channel to stdout number
                dup(p[1]);
                //shut down both fds since no need
                close(p[0]);
                close(p[1]);
                // now we allow left side command to write to channel buffer
                runcmd(psub->left); 
            }

            pid = fork();
            
            // shoud make this a function call, too much code T.T
            if(pid <0) {
                //fork() error
                exit(1);
            }

            if(pid == 0){
                //close stdin
                close(0);
                //assign read channel to stdin number
                dup(p[0]);
                //shut down both fds since no need
                close(p[0]);
                close(p[1]);
                // now we allow right side command to write to channel buffer
                runcmd(psub->right); 
            }

            //in parents, we close these pipe channels now
            close(p[0]);
            close(p[1]);

            // now wait for child process to return
            wait(&status);
            wait(&status);
    
            break;
 /*       case 'p':
            psub = (struct pcmd*) cmd;
            int fd[2]; // pipe fd
            int pid; // forked process


            if (pipe(fd) < 0) {
                exit("ERROR: pipe failed\n");
                break;
            }

            pid = fork();
            if (pid == 0) { // child executes right side
                // receive input (stdin) from input part of pipe
                dup2(fd[0], 0);

                // run right command
                runcmd(psub->right);
            }
            else if (pid < 0) { // fork fail
                printf("ERROR: forking failed\n");
                exit(EXIT_FAILURE);
            }
            else { // parent executes left side

                // redirect output (stdout) with pipe output
                dup2(fd[1], 1);

                // run left command
                runcmd(psub->left);

            } 

            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));

            break;
            */
        default:
              printf("don't know what to run\n");
            break;
    }
    exit(0);
}
#ifdef disable
/*This function gets a command line argument list with the*/
/*first one being a file name followed by all the arguments.*/
/*It forks a child process to execute the command using*/
/*system call execvpe().                                             */
void  execute_cmd(char **argv, char **envp)
{

    pid_t  pid;
    int status;

    pid = fork();
    if (pid == 0) {          /*child process executes the command*/ 
        if (execvpe(*argv, argv, envp) == -1) {    
            printf("ERROR: exec failed\n");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0) {     /*fork a child process*/
        printf("ERROR: forking failed\n");
        exit(EXIT_FAILURE);
    }
    else {              /*parent waits on the child for completion*/
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}
#endif

void welcome_message(){
    printf("\n\t***********************************************\n");
    printf("\t               SBU Shell\n");
    printf("\t***********************************************\n");
    printf("\n\n");
}

int main(int argc, char *argv[], char *envp[]) {

    welcome_message();

    char buf[100];
    char* ptr;
    static char pwd[100];
    struct cmd* command;
    char* spam;
    int status;
    int fd = 0;
    extern char* dPath;
    char* pw, *rs;
    int ret;
    char* script = 0, *nsc = 0;

    pid_t  pid;


    //try to get scripts
    if(argc > 1){
        if((fd = open(argv[1], 0)) < 0){
            printf("failed to run script\n");
            exit(1);
        }
    }


    if(argc < 2){
        pw = getenv("PWD");
        rs = argv[0];
        dPath = malloc(strlen(pw)+strlen(rs));

        if (*rs == '/') { // absolute path
            strncpy(dPath, rs, strlen(rs) - 5);
        } else if (*rs != '/' && *rs != '.') { // relative path, no ./
            strncpy(dPath, pw, strlen(pw));
            strncpy(dPath + strlen(pw), "/", 1);
            strncpy(dPath + 1 + strlen(pw), rs, strlen(rs) - 5);
        } else { // relative path
            strncpy(dPath, pw, strlen(pw));
            strncpy(dPath+strlen(pw), rs+1, strlen(rs)-6);
        }
    }else{
        dPath = malloc(strlen(argv[0]));
        strncpy(dPath, argv[0], strlen(argv[0])-5);
    }

    while((ret = getcmd(buf, sizeof(buf), fd)) >= 0) {
        //  fprintf(stdout,"command is %s\n", buf);

        //clean up whitespaces
        //
        ptr = buf;
        while(*ptr == ' ') {
            ++ptr;
        }

        if(ptr[0] == 'c' && ptr[1] == 'd' && ptr[2] == ' '){

            if(chdir(buf+3) < 0){
                /*error message*/
                printf("cannot cd %s\n", ptr+3);
            }

            if(!(spam = getcwd(pwd, sizeof(pwd)))) {
                printf("getcwd failed\n");
            }
            // fprintf(stdout, "%s\n", pwd);
            printf("%s\n", pwd);
            continue;
        }
        else if(ptr[0] == '.' && ptr[1] == '/'){
            nsc = malloc(strlen(ptr) + 6);
            strncpy(nsc, "sbush ", 6);
            strncpy(nsc+6, ptr+2, strlen(ptr-2));
        }else if(ptr[0] == '#' && ptr[1] == '!'){
            script = malloc(strlen(ptr));
            strncpy(script, ptr+2, strlen(ptr-2));
            strncpy(script, " ", 1);
            continue;
        }


        // exit operation 
        char *exit_cmd = "exit";
        if(strncmp(ptr, exit_cmd, 4) == 0){
                        printf("exiting the shell\n");
                        exit(0);
        }

        pid = fork();


        if (pid == 0) {          /*child process executes the command*/
            if(script){
                ptr = malloc(strlen(script)+strlen(buf));
                strncpy(ptr, script, strlen(script));
                strncpy(ptr+strlen(script), buf, strlen(buf));
            }
            command = parsecmd(buf);
            runcmd(command);
        }
        else if (pid < 0) {     /*fork a child process*/
            printf("ERROR: forking failed\n");
            exit(1);
        }
        else {              /*parent waits on the child for completion*/
            wait(&status);

            if(!ret){
                break;
            }
        } 


    }


    /* changes end here */
    //puts("sbush> ");
    return 0;
}

