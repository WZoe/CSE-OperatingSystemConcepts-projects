/*
<CTRL><C> = history
<CTRL><D> = exit
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE 80
#define BUFFER_SIZE 50
#define buffer "\n\nCommand History:\n"

char history[10][BUFFER_SIZE];
int count = 0;
int flag = 0;

void DisplayHistory()
{
    int i;
    int j = 0;
    int history_counter = count;

    //print command history stored
    printf("Command History:\n");
    for (i = 0; i<10;i++)
    {
        printf("%d. ", history_counter);
        while (history[i][j] != '\n' && history[i][j] != '\0')
        {
            printf("%c", history[i][j]);
            j++;
        }
        printf("\n");
        j = 0;
        history_counter--;
        if (history_counter ==  0)
            break;
    }
    printf("\n");
    printf("COMMAND->");
}

/* the signal handler function */
void handle_SIGINT()
{
    write(STDOUT_FILENO, buffer, sizeof(buffer));
    DisplayHistory();
    flag = 1;
}

/** setup() reads in the next command line, separating it into
distinct tokens using whitespace as delimiters.
setup() modifies the args parameter so that it holds pointers
to the null-ternimated strings that are the tokens in the most
recent user command line as well as a NULL pointer, indicating
the end of the argument list, which comes after the string
pointers that have been assigned to args. */


void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */

    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
     if (flag == 1)
     {
      length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
      flag = 0;
     }

     start = -1;
        if (length == 0)
     exit(0);            /* ^d was entered, end of user command stream */
         if (length < 0)
     {
             perror("Error reading the command\n");
      exit(-1);           /* terminate with error code of -1 */
     }
    for (i=0;i<length;i++)
    { /* examine every character in the inputBuffer */
        switch (inputBuffer[i])
        {
            case ' ':
            case '\t' :               /* argument separators */
                if(start != -1)
                {
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                    ct++;
                }
                inputBuffer[i] = '\0'; /* add a null char; make a C string */
                start = -1;
                break;

            case '\n':                 /* should be the final char examined */
                if (start != -1)
                {
                    args[ct] = &inputBuffer[start];
                    ct++;
                }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
                break;

            default :             /* some other character */
                if (start == -1) start = i;
                if (inputBuffer[i] == '&')
                {
                    *background  = 1;
                    inputBuffer[i-1] = '\0';
                }
        } /* end of switch */
     }    /* end of for */
     args[ct] = NULL; /* just in case the input line was > 80 */

    for (i = 0; i <= ct; i++)//Show parser result
		printf("args %d = %s\n",i,args[i]);

     //examine if it calls command in history
    if(strcmp(args[0],"r")==0)
    {
        if (args[1]==NULL) //repeat the last command
            strcpy(inputBuffer,history[0]);
        else
        {
            char x = args[1][0]; // x
            int j =0;
            if(x>'z' || x<'a') //check whether x exists
            {
            printf("\nWrong command. You should enter a letter from a to z.\n");
            strcpy(inputBuffer,"Wrong command");
            }
            else //x is a formal letter
            {
                int found = 0;
                for(j=0;j<10;j++)
                {
                    if(history[j][0]==x)
                    {
                        strcpy(inputBuffer, history[j]);
                        found = 1;
                        break;
                    }
                }
                if (found==0)
                {
                    printf("\nNo such command in the last 10 commands.");
                    strcpy(inputBuffer,"Wrong command");
                }
            }
        }
	}

    for (i = 9;i>0; i--)
      strcpy(history[i], history[i-1]);

     strcpy(history[0],inputBuffer);
     count++;
} /* end of setup routine */


int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/+1];/* command line arguments */

    pid_t pid;
    int i;

    /* set up the signal handler */
    struct sigaction handler;
    handler.sa_handler = handle_SIGINT;
    sigaction(SIGINT, &handler, NULL);


    /* loop until we receive <Control><C> */
    while (1)
    {
        background = 0;
        printf("COMMAND->");
        fflush(stdout);
        /*setup() calls exit() when Control-D is entered */
        setup(inputBuffer, args, &background);

        /** the steps are:
        (1) fork a child process using fork()
        (2) the child process will invoke execvp()
        (3) if background == 0, the parent will wait,
        otherwise it will invoke the setup() function again. */
        // (1) fork a child process using fork()
        pid = fork();
        if (pid < 0)
        {
            printf("Error command. Fork failed.\n");
            exit (1);
        }
        // (2) the child process will invoke execvp()
        else if (pid == 0)
        {
            if (execvp (args[0], args) == -1) printf("Error command. Execution faild.\n");
        }

        // (3) if background == 0, the parent will wait,
        // otherwise it will invoke the setup() function again.
        else
        {
            if (background == 0)
            wait(NULL);
        }
    }
}
