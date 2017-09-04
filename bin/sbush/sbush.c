#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ARGS 10

/* declarations */
int getcmd(char* buf, int max);
struct cmd* parsecmd(char* buf);
struct cmd* getpipe(char** src, char* dst);
struct cmd* getexec(char** src, char* dst);
void fetchtoken(char** ps, char* dst);
struct cmd* formEcmd();
struct cmd* formBcmd(struct cmd* cmd);
void runcmd(struct cmd* cmd);
void execute_cmd(char **argv, char **envp);

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

int getcmd(char* buf, int max)
{
    // roughly cover read() to terminate at newline
    int i, cc;
    char c;

    /* need to implement a prompt message */
    puts("sbush> ");
    memset(buf, 0, max);
    
    for(i=0;i<max-1;++i){
        cc = read(0, &c, 1);

        if(cc < 0) {
            // error
            return -1;
        }

        //end of line condition
        if(c == '\r' || c == '\n' || cc == 0){
            buf[i] = '\0';
            break;
        }else{
            buf[i] = c;
        }
    }

    return 0;

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
    int i;
    struct ecmd* esub;
    struct bcmd* bsub;
    struct pcmd* psub;

    //printf("command type %c\n", cmd->type);
    switch (cmd->type){
        case 'e':
            esub = (struct ecmd*) cmd; 
           // printf("command %s running\n", esub->argv[0]);
           // printf("with arguements:\n");
           // for(i = 0; esub->argv[i]; i++)
          if (execvp(argv[0], argv) == -1) {   
              perror("ERROR: exec failed\n");
          }
          exit(EXIT_FAILURE);
              //  printf("%s\n", esub->argv[i]);
            break;
        case 'b': 
            bsub = (struct bcmd*) cmd;
           // printf("background command running\n");
            runcmd(bsub->cmd);
            break;
        case 'p':
            psub = (struct pcmd*) cmd;
          //  printf("p left runs\n");
            runcmd(psub->left);
         //   printf("p right runs\n");
            runcmd(psub->right);
            break;
        default:
          //  printf("don't know what to run\n");
            break;
    }

}
#ifdef disable
/*This function gets a commend line argument list with the*/
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
               perror("ERROR: exec failed\n");
          }
          exit(EXIT_FAILURE);
     }
     else if (pid < 0) {     /*fork a child process*/
          perror("ERROR: forking failed\n");
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

int main(int argc, char *argv[], char *envp[]) {

    char buf[100];
    char* ptr;
    static char pwd[100];
    struct cmd* command;
    char* spam;

    pid_t  pid;
    int status;

    while(getcmd(buf, sizeof(buf)) >= 0) {
      //  fprintf(stdout,"command is %s\n", buf);

        //clean up whitespaces
        ptr = buf;
        while(*ptr == ' ') {
            ++ptr;
        }
        
        if(ptr[0] == 'c' && ptr[1] == 'd' && ptr[2] == ' '){

            if(chdir(buf+3) < 0){
                /*error message*/
              //  fprintf(stderr, "cannot cd %s", ptr+3);
            }

            if((spam = getcwd(pwd, sizeof(pwd))) < 0) {
              //  fprintf(stderr, "wrong\n");
            }
           // fprintf(stdout, "%s\n", pwd);
            continue;
        }

     pid = fork();


     if (pid == 0) {          /*child process executes the command*/

          command = parsecmd(buf);
          runcmd(command);
     }
     else if (pid < 0) {     /*fork a child process*/
          perror("ERROR: forking failed\n");
          exit(EXIT_FAILURE);
     }
     else {              /*parent waits on the child for completion*/
         do {
             waitpid(pid, &status, WUNTRACED);
         } while (!WIFEXITED(status) && !WIFSIGNALED(status));
     } 


    }


    /* changes end here */
    //puts("sbush> ");
    return 0;
}

