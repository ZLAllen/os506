#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
    printf("the allocated space is %ld\n", sizeof(struct ecmd));

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

    *ps = q;

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
        printf("ps is %s\n", sq);
        fetchtoken(&ps,dst);
        cmd->type = 'e';
        cmd->args[argc] = sq;
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
       printf("reach the end\n");
   }

   //now we check if this command should be running in background
   if(*src == '&'){
       command = formBcmd(command);
   }

   return command;


   
}

void runcmd(struct cmd* cmd){
    int i;
    struct ecmd* esub;
    struct bcmd* bsub;
    struct pcmd* psub;

    printf("command type %c", cmd->type);
    switch (cmd->type){
        case 'e':
            esub = (struct ecmd*) cmd; 
            printf("command %s running\n", esub->argv);
            printf("with arguements:\n");
            for(i = 0; esub->args[i]; i++)
                *esub->args[i] = 0;
                printf("%s\n", esub->args[i]);
            break;
        case 'b': 
            bsub = (struct bcmd*) cmd;
            printf("background command running\n");
            runcmd(bsub->cmd);
            break;
        case 'p':
            psub = (struct pcmd*) cmd;
            printf("p left runs\n");
            runcmd(psub->left);
            printf("p right runs\n");
            runcmd(psub->right);
            break;
        default:
            printf("don't know what to run\n");
            break;
    }

}

int main(int argc, char* argv[]) {

    char buf[100];
    char* ptr;
    static char pwd[100];
    struct cmd* command;
    char* spam;

    while(getcmd(buf, sizeof(buf)) >= 0) {
        fprintf(stdout,"command is %s\n", buf);

        //clean up whitespaces
        ptr = buf;
        while(*ptr == ' ') {
            ++ptr;
        }
        
        if(ptr[0] == 'c' && ptr[1] == 'd' && ptr[2] == ' '){

            if(chdir(buf+3) < 0){
                /*error message*/
                fprintf(stderr, "cannot cd %s", ptr+3);
            }

            if((spam = getcwd(pwd, sizeof(pwd))) < 0) {
                fprintf(stderr, "wrong\n");
            }
            fprintf(stdout, "%s\n", pwd);
            continue;
        }


        command = parsecmd(buf);
        runcmd(command);
    }


    /* changes end here */
    //puts("sbush> ");
    return 0;
}
