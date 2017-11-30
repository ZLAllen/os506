#ifndef _DEFS_H
#define _DEFS_H

#define NULL ((void*)0)

/** push all registers */
#define PUSHREGS \
    "pushq %rax; "\
    "pushq %rcx; "\
    "pushq %rdx; "\
    "pushq %rbx; "\
    "pushq %rbp; "\
    "pushq %rsi; "\
    "pushq %rdi; "\
    "pushq %r8;  "\
    "pushq %r9;  "\
    "pushq %r10; "\
    "pushq %r11; "\
    "pushq %r12; "\
    "pushq %r13; "\
    "pushq %r14; "\
    "pushq %r15; "

/* pop all registers */
#define POPREGS \
    "popq %r15; "\
    "popq %r14; "\
    "popq %r13; "\
    "popq %r12; "\
    "popq %r11; "\
    "popq %r10; "\
    "popq %r9;  "\
    "popq %r8;  "\
    "popq %rdi; "\
    "popq %rsi; "\
    "popq %rbp; "\
    "popq %rbx; "\
    "popq %rdx; "\
    "popq %rcx; "\
    "popq %rax; "

typedef unsigned long  uint64_t;
typedef          long   int64_t;
typedef unsigned int   uint32_t;
typedef          int    int32_t;
typedef unsigned short uint16_t;
typedef          short  int16_t;
typedef unsigned char   uint8_t;
typedef          char    int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;

typedef unsigned short mode_t; // permission bits

typedef unsigned int ino_t; //struct stat
typedef long blkcnt_t; //struct stat

/*************** Custom Definitions***************/

typedef unsigned int uint;

typedef enum { false = 0, true = !false } bool;

// function pointer
typedef void (*function)();

#endif
