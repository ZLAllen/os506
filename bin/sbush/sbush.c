#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

// for read: should cover it with gets

int getcmd(char* buf, int max)
{
    // roughly cover read() to terminate at newline
    int i, cc;
    char c;

    /* need to implement a prompt message */
    puts("sbush> ");
    
    for(i=0;i<max-1;++i){
        cc = read(0, &c, 1);

        if(cc < 0) {
            // error
            return -1;
        }

        //end of line condition
        if(c == '\r' || c == '\n' || cc == 0){
            buf[i] = 0;
            break;
        }else{
            buf[i] = c;
        }
    }

    return 0;

}

int main(int argc, char *argv[], char *envp[]) {

    char buf[100];
    char* ptr;
    static char pwd[100];
    char* spam;

    while(getcmd(buf, sizeof(buf)) >= 0) {
        fprintf(stdout,"command is %s\n", buf);
        // since cd is not executable, we resolve it here
        // might be messy, but want to remove any spaces before the command
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

        // need fork??? 
    }


    /* changes end here */
    //puts("sbush> ");
    return 0;
}

