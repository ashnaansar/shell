#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" //Whitespace is taken as the delimiter

#define MAX_COMMAND_SIZE 255 // Maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports 10 arguments

//This handle_signal function determines which signal was caught and performs the appropriate task
//As a parameter it takes int signal value.
static void handle_signal(int sig)
{
	switch(sig)
	{
		case SIGINT:
			printf("\n");
		break;

		case SIGTSTP:
		printf("\n");

                break;

		
	}
}

int main()
{
	char history[50][255]; 
        int history_index=0;
        char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);
        pid_t listpid[15];
	int listpid_index=1;
	int k,g;
        

	while(1)
	{	

        // Print out the msh prompt
        printf("msh> ");
        // Read the command from the commandline.  The
        // maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait here until the user
        // inputs something since fgets returns NULL when there
        // is no input
	// fgets helps in reading a string with spaces 
        while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));
	//All the commands entered in the shell are stored here to keep a record of history
        strcpy(history[history_index++],cmd_str);
	//History command will print 50 commands entered by the user when the index reaches 50 it is reset back to 1
	if(history_index > 50)
	{
	history_index = 1;
	}

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];

        int token_count = 0;
        // Pointer to point to the token
        // parsed by strsep
        char *arg_ptr;

        char *working_str = strdup(cmd_str);
	
        // Moving the working_str pointer to
        // keep track of its original value
        char *working_root = working_str;

        // Parsing with whitespace used as the delimiter
        while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
               (token_count < MAX_NUM_ARGUMENTS))
        {
       		token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
                if (strlen(token[token_count]) == 0)
                {
                        token[token_count] = NULL;
                }
                token_count++;

	} 
	// (Requirement 6) When pressed enter token[0] is empty , Continue will proceed in the program and give a new prompt line 
	if (token[0]==0) continue;
	//Setting up the signal handling 
       struct sigaction act;
       //Zero out the sigaction struct
       //Set the handler to use the handle_signal
       memset(&act,'\0',sizeof(act));
       act.sa_handler = &handle_signal;
       // (Requirement 15) From the record saved previously the history of last 50 commands is printed on detecting the command "history"
             if(!strcmp(token[0],"history"))
	     {  for(k=0; k<history_index; k++)
	     	     { printf("%d: %s\n",k,history[k]);
		     
	             }continue;
	     }
	// (Requirement 12 & 8) Install the handler for SIGINT & SIGTSTP check the return value
	// The systemcall sigaction is used to change the action taken by a process on receipt of a specific signal
	if(sigaction(SIGINT,&act,NULL)<0)
	{
		return 1;
	}
        if (sigaction(SIGTSTP,&act,NULL)<0)
        {
	return 1;
        }
        if(!strcmp(token[0],"bg"))
	{
	 sigaction(SIGCONT,&act,NULL);
	continue;
        }
	//( Requirement 5) On receiving the command "exit" & "quit" exit function is called
	else  if (!strcmp(token[0], "exit"))
        {
                exit(0);
        }
	else if (!strcmp(token[0], "quit"))
	{
                exit(0);
        }
	//(Requirement 13) Implementing cd command in the shell, chdir opens the directory specified by the user in the token [1]
        else if (!strcmp(token[0], "cd"))
        {
                chdir(token[1]);
                 continue;
        }
	//(Requirement 14) The command listpid will print the process ids executed in the shell from the previously stored listpid array
	else if(!strcmp(token[0],"listpids"))                                                                          
	{
	       	for(g=0;g<listpid_index;g++)                     
		{
		      	printf("%d: %d\n",g,listpid[g]);
               	}     
		continue;     
	}
        //(Requirement 11) Fork is called and systemcall execvp is made
	pid_t pid=fork();
        if (pid == 0)
        { 
                execvp(token[0], token);
        //(Requirement 2) When an invalid command is given the system returns -1 which gives the prompt command not found 
	while(-1)
	{
		printf("%s:command not found\n",cmd_str);
		return 0;
	}
	}
        else{ 
            
            int status;
            waitpid(pid,&status,0);
        //Storing the Process Ids for the last 15 commands executed
	    listpid[listpid_index++]=pid;
	    if (listpid_index>15)
            {
		    listpid_index=1;
	     }
	}

                /*int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ )
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );
    }*/
      
        free(working_root);//deallocating the memory
		
}return 0;
}
