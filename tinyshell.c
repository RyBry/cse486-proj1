/* The skeleton code for Tiny Shell */

/**
 * You have to add your code to the places where it shows
 * <<YOUR CODE GOES HERE>> or 
 * any comment prefixed by "YCGH: ".
 * 
 * You do not need to modify/edit/remove the existing code.
 *
 * The source file contains additional comments to explain the code 
 * or provide hint that you can leverage while completing the code.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define LINESZ 1024 // Maximum number of characters in an input string

//========== MARCO for Printing error messages ================
// Simply prints an error message
#define PRINT_ERROR                    \
  do                                   \
  {                                    \
    printf("An error has occurred\n"); \
  } while (0)

// To be used to handle failures of system calls
// such as fork, execvp, waitpid, getcwd, etc.
#define PRINT_ERROR_SYSCALL(x) \
  do                           \
  {                            \
    perror(x);                 \
    PRINT_ERROR;               \
  } while (0)

// To be used to simply free up any dynamically allocated memory in the
// command data structure
#define FREE_ALLOCATED_MEMORY(x) \
  do                             \
  {                              \
    free_mem(&x);                \
  } while (0)
//---------------------------------------------


// Command data structure: 
// After parsing a given input string, this data structure
// is used to hold/store the parsed data
typedef struct tcommand
{
  // The actual command to be exectued 
  char cmd[LINESZ + 2];
  
  // The number of arguments including the command itself
  int arg_count;

  // The arguments along with the command (at args[0])
  char *args[LINESZ + 2];

  // A flag to denote whether command has output redirection
  // 1 = present 0 = absent
  unsigned char redirection;

  // In case of output redirection, the number of the output file ...
  char ofile[LINESZ + 2];

  // A flag to denote whether the command ends with "&" 
  // (to be executed in the background)
  // 1 = present , 0 = absent
  unsigned char detach;

} cmd_string;


// Initialize a given command structure
void initialize(cmd_string *cstring)
{
  // making argument count 0
  cstring->arg_count = 0;

  // ensuring redirection flag to be absent
  cstring->redirection = (unsigned char)0;

  // ensuring detach flag to be absent
  cstring->detach = (unsigned char)0;

  // Filling up the "ofile" buffer with 0
  memset(cstring->ofile, 0, LINESZ + 2);

  // Filling up the "cmd" buffer with 0
  memset(cstring->cmd, 0, LINESZ + 2);
}


/* 
 * This function frees up dynamically allocated
 * memory that are currently being pointed to by 
 * cstring->args[i]
 */
void free_mem(cmd_string *cstring)
{
  for (int i = 0; i < cstring->arg_count; i++)
  {
    if (cstring->args[i])
    {
      free(cstring->args[i]);
#ifdef DEBUG
      printf("DEBUG: freed memory\n");
#endif
      cstring->args[i] = NULL;
    }
  }
}



/* The function that populates/loads cmd_string after parsing a line
 * 
 * @param:
 *    line    - the input string read from stdin
 *    cstring - the command structure to be loaded with the parsed values from line
 */
void parse_cmd_string(char *line, cmd_string *cstring)
{
  initialize(cstring);
  char *p = strtok(line, " \t\n");
  while (p)
  {
    if (cstring->arg_count == 0)
    {
      strcpy(cstring->cmd, p);
    }
    cstring->args[cstring->arg_count] = strdup(p);
    ++cstring->arg_count;
    p = strtok(NULL, " \t\n");
  }
#ifdef DEBUG
  printf("DEBUG: arg_count = %d\n", cstring->arg_count);
#endif

  if (cstring->arg_count == 0)
    return; //empty line
  if (strcmp(cstring->args[cstring->arg_count - 1], "&") == 0)
  {
    free(cstring->args[cstring->arg_count - 1]); // free
    cstring->args[cstring->arg_count - 1] = NULL;
    cstring->detach = 1;
    --cstring->arg_count;
  }
  if (cstring->arg_count >= 3)
  {
    if (strcmp(cstring->args[cstring->arg_count - 2], ">") == 0)
    {
#ifdef DEBUG
      printf("DEBUG: Processing redirection\n");
#endif

      strcpy(cstring->ofile, cstring->args[cstring->arg_count - 1]);

      free(cstring->args[cstring->arg_count - 1]); // free
      free(cstring->args[cstring->arg_count - 2]); // free
      cstring->arg_count -= 2;
      cstring->args[cstring->arg_count] = NULL;
      cstring->redirection = (unsigned char)1;
      return;
    }
    else
    {
      cstring->args[cstring->arg_count] = NULL;
      return;
    }
  }
  cstring->args[cstring->arg_count] = NULL;
  return;
}

/*===============================================================*/
/*===============================================================*/
/*================DO NOT MODIFY THE CODE ABOVE ==================*/
/*===============================================================*/
/*===============================================================*/

/** 
 * Function
 * @name: process_builtin_commands
 *
 * @description:
 * Check if the given input is one of the supported built-in commands.
 * If so, process it here. 
 *
 * @param: C -- a pointer to the cmd_string struct
 *
 * @return: 
 *    1 if the input is a built-in command
 *    0 otherwise
 **/
unsigned char process_builtin_commands(cmd_string *C)
{
  if (strcmp(C->cmd, "cd") == 0)
  {
    /* << YOUR CODE GOES HERE >> */
    // YCGH: Write code for processing the command cd
    // One arg --> "cd". Else try to change directory, knowing any syntax errors are handled.
    int execStatus = 0;
    if (C->arg_count == 1){ 
      execStatus = chdir(getenv("HOME")); 
    }
    else { 
      execStatus = chdir(C->args[1]); 
    }
    
    if (execStatus == -1) { PRINT_ERROR_SYSCALL("chdir"); }
    return (unsigned char)1;
  }
  else if (strcmp(C->cmd, "pwd") == 0)
  {
    /* << YOUR CODE GOES HERE >> */
    // YCGH: Write code for processing the command pwd
    // Arbitrarily deciding the max filepath size will be same size as max terminal input size.
    char currDirectory[LINESZ];
    char * execStatus = getcwd(currDirectory, LINESZ);
    if (execStatus == NULL) { PRINT_ERROR_SYSCALL("pwd"); }
    else if (printf("%s\n", currDirectory) < 0) { // this both performs the print and checks status
      PRINT_ERROR_SYSCALL("print/write");
    }
    return (unsigned char)1;
  }
  else if (strcmp(C->cmd, "echo") == 0)
  {
    /* << YOUR CODE GOES HERE >> */
    // YCGH: Write code for processing the command echo
    if (C->redirection){
      // Print to the requested file redirect all args after "echo"
      int terminalFD = dup(STDOUT_FILENO); // Now terminal File Description is on both STDOUT(1) and some other fd(terminalFD)
      int fileFD = open(C->ofile, O_RDWR | O_CREAT | O_TRUNC, 00700); // manpage says 00700 is RWX
      dup2(fileFD, STDOUT_FILENO); // close the current FD on STDOUT (one of the terminal's references) and replace it with the new file FD (fileFD)
      for(int argI = 1; argI < C->arg_count; ++argI){  // print one argument per loop
        if (printf("%s", C->args[argI]) < 0) { // this both performs the print and checks status
          PRINT_ERROR_SYSCALL("print to stdout");
        }
        if (argI != C->arg_count -1) {printf(" ");} // add a space after this argument string if it is not the last. 
      }
      printf("\n"); // the buffer must be flushed before switching STDOUT!
      close(fileFD); // done writing so we close the new file back up
      dup2(terminalFD, STDOUT_FILENO); // restore terminal as the STDOUT file for printf()
      close(terminalFD); // once again, terminal now occupies STDOUT(1) and some other fd(terminalFD). Close the duplicate.
    } else {
      // Print directly to terminal all the args after "echo"
      for (int argI = 1; argI < C->arg_count; argI++){ // print one argument each loop
        if (printf("%s", C->args[argI]) < 0) { // this both performs the print and checks status
          PRINT_ERROR_SYSCALL("print to stdout");
        }
        if (argI != C->arg_count -1) {printf(" ");} // add a space after this argument string if it is not the last. 
      }
      printf("\n"); // flush the buffer!
    }
    return (unsigned char)1;
  }
  else
    return 0;
}


/* The main function starts here */

int main(int argc, char *argv[])
{
  char line[LINESZ + 2];
  cmd_string C;
  initialize(&C);

  // Loop forever
  while (1)
  {
    // Free up allocated memory, if any
    FREE_ALLOCATED_MEMORY(C);

    // Let's print the shell prompt
    printf("tinyshell> ");

    // read a line (i.e., input string) from stdin
    if (fgets(line, LINESZ, stdin) == NULL)
    {
      return 0; // reached EOF (end-of-file), so we can safely terminate
    }

#ifdef DEBUG
    printf("DEBUG: Input line: %s\n", line);
#endif

    // Parsing the input string and storing info in C
    parse_cmd_string(line, &C);

    if (C.arg_count == 0)
    {
#ifdef DEBUG
      printf("DEBUG: Ignoring empty line ...\n");
#endif
      continue; //Ignore empty line ....
    }

    /* Handling "exit" command. It's already implemented for you. */
    if (strcmp(C.cmd, "exit") == 0)
    {
      /* check for arguments to `exit` commands.
       * Any argument to `exit` command is an error.
       */
      if (C.arg_count > 1)
      {
        PRINT_ERROR; // Print an error message
        continue;
      }
      else
      {
        return 0; // we can safely terminate now
      }
    }

    /* Check if the input command is a built-in command */
    unsigned char r = process_builtin_commands(&C);
    if (r == 1) // 1 = built in. 0 = not built in.
      continue; // continue means STOP --- DON'T fall through to the next line; mark the current loop as done & cycle back to the top, continuing.

    // If we are here, that means the input is not a 
    // built-in command. So we must handle it using fork and execvp

    /* Command execution: use fork() to create a new child process */ 
    int fc = fork();

    if (fc < 0)
    {
      // Shows how to use PRINT_ERROR_SYSCALL() 
      PRINT_ERROR_SYSCALL("fork");
      return 1;
    }
    else if (fc == 0)
    {
      //child's code


      if (C.redirection == 1)
      {
#ifdef DEBUG
        printf("DEBUG: Redirection enabled\n");
#endif
        /* << YOUR CODE GOES HERE >> */

        /* YCGH: Implement output redirection
         * Open a file using the name specified in C.ofile 
         * and replace STDOUT with this file.
         *
         * Also handle error using PRINT_ERROR_SYSCALL().
         */

        // initalzing
        int fd = open(C.ofile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) // check for errors opening file
        {
          PRINT_ERROR_SYSCALL("open");
          exit(1);
        }

        // redirect standard output to the file
        // okay to never restore since we are in the child process
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
          PRINT_ERROR_SYSCALL("dup2");
          close(fd);
          exit(1);
        }

        // executes command
        if (execvp(C.args[0], C.args) == -1)
        {
          PRINT_ERROR_SYSCALL("execvp");
          exit(1);
        }
        close(fd);

      }



      /*  Implement the execution of the given command using "execvp()".
       * Also handle errors using PRINT_ERROR_SYSCALL("execvp"),
       * if execvp() fails
       */
      if (execvp(C.args[0], C.args) == -1) {
          PRINT_ERROR_SYSCALL("execvp");
          exit(1); // Exit child process if execvp fails
      }
      

      // if execvp() succeeds, child never executes the following
      FREE_ALLOCATED_MEMORY(C);
      return 1; 
    }
    else
    {
      // Parent's code
      //
      // Let's check if the parent needs to wait for the child. 
      // Otherwise, parent has nothing to do and will simply move 
      // to the next iteration.

      if (C.detach == 0)
      {
        // Implement foreground job
        // Make the parent wait using waitpid().
          int status;
          if (waitpid(fc, &status, 0) < 0) {
              PRINT_ERROR_SYSCALL("waitpid");
          }

      }
      // Implement background job: Parent does not wait
      else if (C.detach == 1) {
      
      #ifdef DEBUG
          printf("DEBUG: Background job started with PID %d\n", fc);
      #endif

      }
    }
  }

  return 0; // done
}
