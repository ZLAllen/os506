#include <stdio.h>
#include <string.h>

extern char **environ;



void setEnv(char* env_var,char* newval){
    // copy the new value
    strcpy(env_var, newval);
}

char* getEnvValue(char* environ[],char* var){
    int c = 0;
    while(*environ[c] != NULL) {
        if(strncmp(environ[c], var, strlen(var)) == 0) {
            // key=value; get the value
            return environ[c]+strlen(var)+1;
        }
        c++;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    char **p = environ;
    /*while (*p != NULL)
    {
        printf("%s\n", *p);
        *p++;
    }*/

    char *path= "PATH";
    char *home = "HOME";
    char *user = "USER";


    char *PATH = getEnvValue(environ, path);
    char *USER = getEnvValue(environ, user);
    char *HOME = getEnvValue(environ, home);

    printf("old user:%s\n", USER);

    char *env = "USER";
    char *val = "leenashekharsingh";

    if(strcmp(env,"HOME") == 0){
        setEnv(HOME, val);
    }else if(strcmp(env,"PATH") == 0){
        setEnv(PATH, val);
    }else if (strcmp(env,"USER") == 0){
        setEnv(USER, val);
    }else{
        // how to handle PS1
    }

    printf("new user:%s\n", USER);
    return 0;
}

