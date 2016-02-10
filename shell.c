#include "shell.h"

/**
 * shell.c - the interactable shell portion of my shell. heavily
 * inspired by cornell university's cs414 operating systems course
 * assignment
 */

int main(int argc, char *argv[])
{
		 char workDir[150];
		 char *prompt;
		 char *origPrompt; // We need to rebuild the pwd bit each time.
		 // Simple defaults for prompt. Maybe consider using a struct to store the info?
		 strcpy(prompt, "[msh] ");
		 origPrompt = strdup(prompt);

		 struct cmdInfo *shellInfo = malloc(sizeof(struct cmdInfo));
		 init_info(shellInfo);
		 while(1) {
					
					strcpy(prompt, origPrompt);
					getcwd(workDir, sizeof(workDir));
					strcat(prompt, workDir);
					strcat(prompt, ": ");
					
					char *cmdLine = readline((const char *)prompt);
					if (strcmp(cmdLine, "") == 0) { // Skip empty lines
							 continue;
					}
					
					shellInfo = parse(cmdLine);
					free(cmdLine);
					
					if (isBuiltInCommand(shellInfo) == TRUE) {
							 executeBuiltIn(shellInfo);
					} else {
							 
							 pid_t childPid = fork();
							 pid_t wpid;
							 int status;
							 if (childPid == 0) {
										executeCommand(shellInfo);
										exit(0);
							 } else {
										if (0 == 1) { //line for background handling, currently empty
												 ;
										} else {
												 do {
															wpid = waitpid(childPid, &status, WUNTRACED);
												 } while (!WIFEXITED(status) && !WIFSIGNALED(status));
										}
							 }
					}
					//print_info(shellInfo); //for testing the parser only
		 }

		 free_info(shellInfo);
		 free(workDir);
		 free(origPrompt);
		 free(prompt);
}

bool isBuiltInCommand(struct cmdInfo *cmd)
{
		 char *program = cmd->commandTokens[0];
		 if (strcmp(program, "cd") == 0) {
					return TRUE;
		 } else if (strcmp(program, "exit") == 0) {
					return TRUE;
		 }
		 return FALSE;
}

int executeBuiltIn(struct cmdInfo *cmd)
{
		 if (strcmp(cmd->commandTokens[0], "cd") == 0) {
					if (chdir(cmd->commandTokens[1]) != -1) {
							 return 0;
					} else {
							 printError(errno);
					}					 
		 } else if (strcmp(cmd->commandTokens[0], "exit") == 0) {
					exit(0);
		 }
		 return 1;
}

int executeCommand(struct cmdInfo *cmd)
{
		 char **cmdArgs = malloc(cmd->numArgs+2*sizeof(char *));
		 for (int i=0; i<=cmd->numArgs; i++) {
					cmdArgs[i]=cmd->commandTokens[i];
		 }
		 cmdArgs[cmd->numArgs+1] = NULL;
		 if (execvp(cmdArgs[0], cmdArgs) == -1) {
					printError(errno);
		 }
		 free(cmdArgs);
		 return 0;
}

void printError(int err)
{
		 switch (err) {
		 case EACCES:
					printf("Insufficient privileges to access directory\n"); break;
		 case ENOENT:
					printf("Directory does not exist\n"); break;
		 case EFAULT:
					printf("EFAULT error.\n"); break;
		 default:
					printf("Error number: %d", errno);
		 }
}