#ifndef _ENV_H
#define _ENV_H

// not sure if correct method signature
char *getenv(const char *name);
int setenv(const char *name, const char *value, int overwrite);

#endif
