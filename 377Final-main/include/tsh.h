#ifndef _SIMPLE_SHELL_H
#define _SIMPLE_SHELL_H
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <list>

typedef struct CmdTokens CmdTokens;
struct CmdTokens {
  char** cmd;
  CmdTokens* pipe;
};

typedef struct Pair Pair;
struct Pair {
  char* name;
  char* value;
};

class simple_shell {
 public:
  void parse_command(char* cmd, CmdTokens** tokens, int* tokenCount);
  void exec_command(CmdTokens* tokens, int tokenCount);
  void printf_command(char **cmdTokens, ...);
  void help_command();
  void read_command(char** cmdTokens, ...);
  void echo_command(char **cmdTokens, ...);
  bool isQuit(char* cmd);
  bool isHelp(char* cmd);
  bool isPrintf(char* cmd);
  bool isEcho(char* cmd);
  bool isRead(char* cmd);
  void binary_get(const std::string& input, std::string& output);
  void get_current_timestamp(std::string& output);
  // storing the last line that was entered into the "read" command in a pair with it's variable
  // "$REPLY" if no variable provided
  std::pair<std::string, std::string> read_line;

  std::list<Pair> pairs;
  void alias_command(char** cmdTokens);
  void parse_alias_command(char* cmd, char** tokens);
  bool isAlias(char* cmd);
};

#endif
