#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char * argv[])
{
	// variable declaration
	DIR 			*dp;
	struct dirent 	*dirp;
	
	// using message
	if (argc != 2) {
		printf("using message: a.out <direntroy_name>\n");
		exit(1);
	}
	
	// opendir
	if ((dp = opendir(argv[1])) == NULL) {
		printf("can't open dir %s \n", argv[1]);
		exit(1);
	}
	
	// readdir
	while ((dirp = readdir(dp)) != NULL) {
		printf("%s\n", dirp->d_name);
	}
	
	// closedir
	closedir(dp);
	
	return 0;
}