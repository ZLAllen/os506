#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char **environ;

int setenv(const char *name, const char *value, int overwrite){
    // old value in environ
    char *currval = getenv(name);
    printf("old value:%s\n", currval);
    char *nc = malloc(sizeof(value) + 1); // TODO used malloc but sont know what to do next
    if(overwrite == 1) {
        memset(currval, 0, sizeof(currval));
        strcpy(currval, value);
    }
    // now value should be the new one in environ
    printf("new value:%s\n", currval);
    return 0;
}

char *extract_from_environ(char *var){
    int c = 0;
    while(*environ[c] != NULL) {
    if(strncmp(environ[c], var, strlen(var)) == 0) {
    // key=value; get pointer to the value
    return environ[c]+strlen(var)+1;
    }
    c++;
    }
    return NULL;
}


char *getenv(const char *name){

    char *pa, *us, *hs, *ps, *gn;
    // env handling
    char* user="USER";
    char* home="HOME";
    char* path="PATH";
    char* ps1 ="PS1";

    pa = extract_from_environ(path);
    us = extract_from_environ(user);
    hs = extract_from_environ(home);
    ps = hs;

    if(strcmp(name, home) == 0){
        return hs;
    }else if(strcmp(name, path) == 0){
        return pa;
    }else if(strcmp(name, user) == 0){
        return us;
    }
    else if(strcmp(name, ps1) == 0){
        return ps;
    }
    else{
        printf("env setting for this variable not implemented\n");
    }
    return 0;
}

int get_value(const char* cmd){
    if(cmd==NULL){
        return -1;
    }
    int idx=0;
    while(cmd[idx]!='='){
        idx++;
        if(cmd[idx]=='\0')
            return -1;
    }
    return idx;
}

void export(char *cmd){

    int idx=get_value(cmd);
    if(idx<0){
        printf("error\n");
        return;
    }
    char* env_type=malloc(sizeof(char)*idx);
    strncpy(env_type, cmd, idx);

    char* var_val=cmd+idx+1;

    if(strcmp(env_type,"HOME") == 0){
        setenv(env_type, var_val, 1);
    }
    else if(strcmp(env_type,"USER") == 0){
        setenv(env_type, var_val, 1);
    }
    else if(strcmp(env_type,"PATH") == 0){
        setenv(env_type, var_val, 1);
    }else{
        printf("printf(\"env setting for this variable not implemented\n");
        return;
    }
}


int main(int argc, char *argv[], char*envp[])
{
    // print som continuos path variables
    printf("%s \n %s \n %s \n", environ[0], environ[1], environ[3]);
    argv[0] = "export";
    // set this to longer than your path
    char *cmd = "PATH=veryverylongpath/home/leena/bin:/home/leena/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/home/leena/Documents/apache-opennlp-1.7.2/bin:/snap/bin ";
    export(cmd);
    printf("%s \n %s \n %s \n", environ[0], environ[1], environ[3]);
}

