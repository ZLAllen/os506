#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <env.h>

char **environ;

int setenv(const char *name, const char *value, int overwrite);
char *extract_from_environ(char *var);
char *getenv(const char *name);
int get_value(const char* cmd);

int setenv(const char *name, const char *value, int overwrite){
    // old value in environ
    char *currval = getenv(name);
    printf("old value:%s\n", currval);

    // name + '=' + value + 1
    char *nc = malloc(sizeof(name) + 1 + sizeof(value) + 1);

    if(overwrite == 1) {
        int c = 0;

        // should be a more efficient way than search again
        // search and replace pointer
        while(*environ[c] != 0) {
            if(strncmp(environ[c], name, strlen(name)) == 0) {
                environ[c] = nc;

                strcpy(nc, name);
                strcat(nc, "=");
                strcat(nc, value);

                break;
            }
            c++;
        }

        char *newVal = getenv(name);
        printf("New value %s\n", newVal);
    }

    return 0;
}

char *extract_from_environ(char *var){
    int c = 0;
    while(*environ[c] != 0) {
    if(strncmp(environ[c], var, strlen(var)) == 0) {
    // key=value; get pointer to the value
    return environ[c]+strlen(var)+1;
    }
    c++;
    }
    return NULL;
}


char *getenv(const char *name){

    char *pa, *us, *hs, *ps;
    // env handling
    char* user="USER";
    char* home="HOME";
    char* path="PATH";
    char* ps1 ="PS1";
	char *extra = "@sbush$";

    pa = extract_from_environ(path);
    us = extract_from_environ(user);
    hs = extract_from_environ(home);
	// ps1 is user@sbush$
    ps = strcat(us, extra);


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
    }
    else if(strcmp(env_type,"PS1") == 0){
        setenv(env_type, var_val, 1);
    }else{
        printf("printf(\"env setting for this variable not implemented\n");
        return;
    }

    free(env_type);
}

/*
int main(int argc, char *argv[], char*envp[])
{
    // print som continuos path variables
    printf("0: %s \n 3: %s \n 4: %s \n\n\n", environ[0], environ[3], environ[4]);
    argv[0] = "export";
    // set this to longer than your path
        char *cmd = "PATH=veryverylongpath/home/leena/bin:/home/leena/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/home/leena/Documents/apache-opennlp-1.7.2/bin:/snap/bin ";

    export(cmd);
    printf("\n\n0: %s \n 3: %s \n 4: %s \n", environ[0], environ[3], environ[4]);
}
*/
