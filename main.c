#include  <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include <signal.h>
#include <time.h>

// flag to determine if the process is forground process (1) or background process(0)
int flag;

//to get the time at which the process was killed
char* currentTime()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  return  asctime (timeinfo);
}

//write in the file when the child process is killed
void handler(int sig)
{
	FILE *logFile = fopen("logfile.txt","a");
	fputs("Child process was terminated ",logFile);
	fputs(currentTime(),logFile);
	fclose(logFile);
}

//split the line to 2d array (argv)
void  parse(char *line, char **argv)
{
     while (*line != '\0') {
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';
          *argv++ = line;
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;
     }
     *argv = '\0';
//check if it is a background process
      flag = 1;   
      argv--;
      if (strcmp(*argv, "&") == 0)
      {
        flag = 0;
        *argv = '\0'; 
      }
}

void  execute(char **argv)
{
     pid_t  pid;
     int    status;
//check for forking error
     if ((pid = fork()) < 0) {
          printf("ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {
// executing child process
          if (execvp(*argv, argv) < 0) {
               printf("ERROR: wrong input\n");
               exit(1);
          }
     }
     else{
	    if(flag)
          	while (wait(&status) != pid);// if foreground process parent process waits
	    else
	    {
		signal(SIGCHLD, handler);// if background process parent process doesn't wait , and handler handles the sigchld interrupt
	    }
     	 }
}

void  main(void)
{
     char  line[1024];
     char  *argv[64];

     while (1) {
          printf("Shell >");
          gets(line);
          parse(line, argv);
          if (strcmp(argv[0], "exit") == 0)//if input equal exit close the shell else execute the command
               exit(0);
          execute(argv);
     }
}

