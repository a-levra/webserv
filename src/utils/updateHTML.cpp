#include "utils/utils.hpp"

#include <cstring>
#include <unistd.h>
#include <stdlib.h>


int updateHTML(){
	coloredLog("updating HTML", "", RED);
	coloredLog("updating HTML", "", BLUE);
	coloredLog("updating HTML", "", YELLOW);
	coloredLog("updating HTML", "", PURPLE);
	coloredLog("updating HTML", "", GREEN);

	int pid = fork();
	if (pid < 0)
		return EXIT_FAILURE;
	if (pid == 0){
		char *file = strdup("/app/updateHTML.sh");
		char *args[] = {file, NULL};
		execve(file, args, NULL);
		free(file);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}