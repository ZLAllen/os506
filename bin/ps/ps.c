#include <stdio.h>
#include <syscall.h>

int main(int argc, char* argv[], char* envp[]) {
	char buf[1024];
	ps(buf);
	printf("%s\n", buf);
	exit();
}

