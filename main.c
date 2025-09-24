#include<stdlib.h>
#include<stdio.h>
#include<libgen.h>
#include<string.h>
#include <sys/wait.h> 
#include <unistd.h>

#define CRASH_RL_BUFSIZE 1024
#define CRASH_TOK_BUFSIZE 64
#define CRASH_TOK_DELIM " \t\r\n\a"
#define ANSI_TEXT_CLEAR "\033[0m"
#define TRUECOLOR_BG_1 "\x1b[48;2;167;139;250m"
#define TRUECOLOR_FG_1 "\x1b[38;2;167;139;250m"
#define TRUECOLOR_BG_2 "\x1b[48;2;106;90;205m"
#define TRUECOLOR_FG_2 "\x1b[38;2;106;90;205m"


// void colorTest(){
//   printf(TRUECOLOR_BG_1 "Hello" TRUECOLOR_FG_1 TRUECOLOR_BG_2"" "Hello" ANSI_TEXT_CLEAR ANSI_BG_CLEAR);
//       //printf(ANSI_TEXT_MAGENTA "\n[%s@%s]%s:"ANSI_TEXT_CLEAR, username,hostname, local_dir);

// }
// Forward declarations for builtin functions
int crash_cd(char **args);
int crash_help(char **args);
int crash_exit(char **args);

// Forward declarations for other functions
int crash_launch(char **args);
int crash_num_builtins(void);

// Builtin command arrays
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &crash_cd,
  &crash_help,
  &crash_exit
};

int crash_execute(char **args){
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < crash_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return crash_launch(args);
}

int crash_cd(char **args);
int crash_help(char **args);
int crash_exit(char **args);

int crash_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int crash_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "crash: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("crash");
    }
  }
  return 1;
}

int crash_help(char **args)
{
  int i;
  printf("Adrian Tennies's CraSH(based on Stephen Brennan's lsh)\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < crash_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int crash_exit(char **args)
{
  return 0;
}

int crash_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("crash");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("crash");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

char **crash_split_line(char *line)
{
  int bufsize = CRASH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "CraSH: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, CRASH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += CRASH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "CraSH: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, CRASH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
char *crash_read_line(void)
{
  int bufsize = CRASH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "CraSH: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += CRASH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "CraSH: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void crash_loop(void){
  char *line;
   char **args;
   int status;

  do {
    char* username = getenv("USER");
    char* directory = getenv("PWD");
    char* hostname = getenv("HOSTNAME");
    char *local_dir = basename(directory);
    printf(TRUECOLOR_BG_1 "\n%s@%s "TRUECOLOR_FG_1 TRUECOLOR_BG_2"%s" ANSI_TEXT_CLEAR TRUECOLOR_FG_2"" ANSI_TEXT_CLEAR ":" , username,hostname, local_dir);
    line = crash_read_line();
    args = crash_split_line(line);
    status = crash_execute(args);

    free(line);
    free(args);
  } while (status);
}

void init_shell(){
    printf("\x1b[38;2;255;0;0m Hewwo");

    printf("\n\033[95m╔═════════════════════════════════════╗");
    printf("\n║                                     ║");
    printf("\n║          Welcome to CraSH!          ║");
    printf("\n║  This could only go well... Right?  ║");
    printf("\n║                                     ║");
    printf("\n╚═════════════════════════════════════╝" ANSI_TEXT_CLEAR);
    printf("hello");
}

int main(int argc, char **argv){
  // Load config files, if any.

    // return 0;


  // Run command loop.
  // colorTest();
  init_shell();
  crash_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
