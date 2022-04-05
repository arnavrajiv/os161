/* main.c
 * ----------------------------------------------------------
 *  CS350
 *  Midterm Programming Assignment
 *
 *  Purpose:  - Use Linux programming environment.
 *            - Review process creation and management
 * ----------------------------------------------------------
 */
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/wait.h>
 #include <fcntl.h>

char error_message[30] = "An error has occurred\n";

int runCommand(char ** args)
{
  char * commandList[5] = { "cd", "pwd", "wait", "exit", "help"};
  int commandNumber = -1;
  char * userCommand = args[0];

  for (int i = 0; i < 5; i++) {
    if (strcmp(commandList[i], userCommand) == 0) {
      commandNumber = i;
      break;
    }
  }
  //printf("%d\n", commandNumber);
  switch(commandNumber) {
    // command cd
    char cwd[512];
    char * helpString;
    char n;

    case 0:
      if(args[1] == NULL) {
        char * homeDirectory;
        homeDirectory = getenv("HOME");
        chdir(homeDirectory);
        int status = chdir(homeDirectory);
        if (status == -1) {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
      else {
        int status = chdir(args[1]);
        if (status == -1) {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
      return 1;
      break;

    // command pwd
    case 1:
      getcwd(cwd, sizeof(cwd));
      char *cwdNew = malloc(strlen(cwd) + 2);
      n = '\n';
      strcpy(cwdNew, cwd);
      cwdNew[strlen(cwd)] = n;
      cwdNew[strlen(cwd) + 1] = '\0';
      write(STDOUT_FILENO, cwdNew, strlen(cwdNew));
      free(cwdNew);
      return 1;
      break;

    // command wait
    case 2:
      while(wait(NULL) != -1) {}
      return 1;
      break;

    // command exit
    case 3:
      exit(0);
      break;

    // command help
    case 4:
      helpString = malloc(sizeof(char)* 512);
      helpString = "cd \npwd \nwait \nexit \nhelp";
      char * helpNew = malloc(strlen(helpString) + 2);
      n = '\n';
      strcpy(helpNew, helpString);
      helpNew[strlen(helpString)] = n;
      helpNew[strlen(helpString) + 1] = '\0';
      write(STDOUT_FILENO, helpNew, strlen(helpNew));
      free(helpNew);
      return 1;
      break;
      // printf("cd \npwd \nwait \nexit \nhelp\n");
      // return 1;
      // break;

    default:
      return 0;
      break;
  }


  return 1;
}

void leggo(char ** args)
{
    pid_t p_pid = fork();
    if (p_pid == 0) {
    int status = execvp(args[0], args);
    if (status == -1) {
      write(STDERR_FILENO, error_message, strlen(error_message));
      }
    }
    else if (p_pid == -1) {
      write(STDERR_FILENO, error_message, strlen(error_message));
      return;
    }
    else {
      wait(NULL);
      return;
    }
}

int main( int argc, char ** argv )
{

  char *command; // to take the input command

  do {
      // printing initial shell prompt
      printf("> ");

      // command input
      command = (char *)malloc(sizeof(char) * 512); // assigning the pointer to a block of memory of max size 512
      int i = 0; // index for command array
      char input = getchar(); // taking in input character by character to check for command termination
      while(input != '\n') {
        command[i++] = input; // storing each character in the command anc incrementing it
        input = getchar();
      }

      if( strcmp(command, "") == 0) {
        continue; // if it is an empty string then go to the next iteration of do ... while loop
      }

      // processing the command
      char ** splitCommand = malloc(sizeof(char *) * 64);
      i = 0;
      char limit[2] = " ";
      char * splitWord = strtok(command, limit);
      while (splitWord != NULL){
        splitCommand[i++] = splitWord;
        splitWord = strtok(NULL, limit);
      }
      splitCommand[i] = NULL;

      // executing the command
      if (splitCommand[0] == NULL) {
        continue;
      }

      if(runCommand(splitCommand)) {
        continue;
      }
      else {
        leggo(splitCommand); // function to start process and leggo
      }



  } while(1);
    return 0;
}
