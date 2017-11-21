#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255

struct dirent {
 char d_name[NAME_MAX+1];
};


#endif
