#include <stdio.h>
#include <tsh.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <bitset>

using namespace std;

void simple_shell::parse_command(char* cmd, CmdTokens** tokens, int* tokenCount) {
  // Tokenize the command string into arguments

  // Checks if cmd is valid
  size_t length = strlen(cmd);
  if (length > 0 && cmd[length - 1] == '\n') {
    cmd[length - 1] = '\0';
  }

  // Splits cmd via whitespace
  char *temp = strtok(cmd, " ");
  char **cmdTokens = (char**) malloc(sizeof(char));
  int i = 0;
  CmdTokens* curr = *tokens;
  while (temp != NULL) {
    // If it finds pipe
    if (strcmp(temp, "|") == 0) {
      // Finalizing data
      cmdTokens[i] = NULL;
      curr->cmd = cmdTokens;
      // Creating new struct & continueing to finalize data
      CmdTokens *t = (CmdTokens*) malloc(sizeof(CmdTokens*));
      curr->pipe = t;
      *tokenCount += 1;
      // resetting data
      curr = t;
      cmdTokens = (char**) malloc(sizeof(char));
      i = 0;
      temp = strtok(NULL, " ");
    } else {
      cmdTokens[i++] = temp;
      temp = strtok(NULL, " ");
    }
  }
  // Finalizing to final struct
  cmdTokens[i] = NULL;
  curr->cmd = cmdTokens;
  *tokenCount += 1;
}

void simple_shell::exec_command(CmdTokens* tokens, int tokenCount) {
  // My tests: 
  // ls
  // ls | sort
  // ls | sort | grep t
  // cat src/tsh.cpp | grep string | grep c | grep f

  CmdTokens* curr = tokens;
  int pipes[tokenCount-1][2];
  int pids[tokenCount];
  for (int i = 0; i < tokenCount; i++) {
    char** args = curr->cmd;
    // TODO: if final one, dont create
    if (i != tokenCount-1) {
      if (pipe(pipes[i]) == 1) {
        perror("Error: failed to create pipe\n.");
        exit(1);
      }
    }
    
    // creating the first child to handle argv1
    pids[i] = fork();
    if (pids[i] < 0) {
      cout << "Fork failed to execute\n.";
      exit(1);
    } else if (pids[i] == 0) {
      if (i != 0) {
        //piping inputs
        close(pipes[i-1][1]);
        dup2(pipes[i-1][0], STDIN_FILENO);
        close(pipes[i-1][0]);
      }

      if (i != tokenCount-1) {
        // Piping outputs
        close(pipes[i][0]);
        dup2(pipes[i][1], STDOUT_FILENO);
        close(pipes[i][1]);
      }

      // Try replace some parts of args with existing aliases.
      int length = sizeof(args)/sizeof(args[0]);
      for (int j = 0; j < length; j++) {
     	string currentElement = args[0];
        for (list<Pair>::iterator it = pairs.begin(); it != pairs.end(); it++) {
          string currentName = (it)->name;
          if (currentElement.compare(currentName) == 0) {
            args[0] = (it)->value;
            break;
          }
        }
      }

      execvp(args[0], args);
      perror("execvp error in child 1");
      exit(1);
    }

    // closing previous' pipes
    if (i != 0) {
      close(pipes[i-1][0]);
      close(pipes[i-1][1]);
    }

    curr = curr->pipe;
  }

  // Reap the children
  for (int i = 0; i < tokenCount; i++) {
    int waiting = waitpid(pids[i], NULL, 0);
  }
}

void simple_shell::printf_command(char** cmdTokens, ...) {
    // Check if there are arguments after "printf"
    if (cmdTokens[1] == nullptr) {
      std::cerr << "printf: missing arguments" << std::endl;
      return;
    }

    std::string formatString;
    va_list args;
    va_start(args, cmdTokens);

    // Collect arguments into a single format string
    //std::string formatString;
    for (int i = 1; cmdTokens[i] != nullptr; ++i) {
      formatString += cmdTokens[i];
      if (cmdTokens[i + 1] != nullptr) {
        formatString += " ";  // Add space between words
      }
    }

    // Process format extensions
    size_t pos = 0;
    int i =0;
    while ((pos = formatString.find("%", pos)) != std::string::npos) {
      if (formatString[pos + 1] == 'b') {
        // Handle %b - expand backslash escape sequences
        // Note: This assumes that cmdTokens[i + 1] is a string
        std::string expanded;
        binary_get(cmdTokens[i + 1], expanded);
        formatString.replace(pos, 16, expanded);
      } else if (formatString[pos + 1] == 't') {
        //cout << ctime(&timenow) <<  endl;
        std::string timestamp;
        get_current_timestamp(timestamp);
        formatString.replace(pos, 2, timestamp);
        //formatString += ctime(&timenow);
      }
      pos += 1;  // Move to the next position after the processed extension
      i++;
    }

    pos = 0;
    while ((pos = formatString.find("\\", pos)) != std::string::npos) {
      // Handle \n - replace with newline character
      if (formatString[pos + 1] == 'n') {
        formatString.replace(pos, 2, "\n");
      }
      pos += 1;  // Move to the next position after the processed extension
      i++;
    }

    // Call printf with the concatenated format string and variable arguments
    /*va_list args;
    va_start(args, cmdTokens[1]);
    vprintf(formatString.c_str(), args);
    va_end(args);
    */
    vprintf(formatString.c_str(), args);

    va_end(args);
}

void simple_shell::help_command() {
  // Provide information about available commands
  cout << "Welcome to the simple shell!" << endl;
  cout << "  printf to print" << endl;
  cout << "  help - Display this help message" << endl;
  cout << " alias - Make shortcut for a single command" << endl;
  cout << "  echo - Output provided arguments" << endl;
  cout << "  read - Read and store one line from standard input" << endl;
  cout << "  quit - Exit the shell" << endl;
}

// this function is for the permanent alias, not the temporary one
// Note: this function does not support options on alias
//       also, it only work for a single command without options as value
// EX: alias list='ls'       is a correct input
// EX: alias list='ls -a'    will cause error
// EX: alias -p              will cause error
void simple_shell::alias_command(char** cmdTokens) {
  // Set a shortcut command for another command
  // Syntax: alias [name]='[value]'

  // Print out all existing aliases
  if (cmdTokens[1] == nullptr) {
    //cout << "listing begin" << endl;
    for (list<Pair>::iterator it = pairs.begin(); it != pairs.end(); it++) {
      cout << "alias " << (it)->name << "='" << (it)->value << "'"  << endl;
      //cout << "name: " << (it)->name << endl;
      //cout << "value: " << (it)->value << endl;
    }
    //cout << "listing completed" << endl;
  } else {
    // Parse alias command
    char** tokens = (char**)malloc(sizeof(char*));
    parse_alias_command(cmdTokens[1], tokens);
    
    // Extracting name
    int length1 = strlen(tokens[0]);
    char* name = (char*) malloc(sizeof(char));
    strncpy(name, tokens[0], length1);
    name[length1] = '\0';
    //cout << "name: " << name << endl;
    
    // Extracting value
    int length2 = strlen(tokens[1]);
    char* value = (char*) malloc(sizeof(char));
    strncpy(value, tokens[1] + 1, length2 - 2);
    value[length2 - 1] = '\0';
    //cout << "value: " << value << endl;

    // Iterate through pairs to see if name already exists
    // If the name exists, erase that pair
    string nameStr = name;
    for (list<Pair>::iterator it = pairs.begin(); it != pairs.end(); it++) {
      string currentName = (it)->name;
      //cout << "currentName: " << currentName << endl;
      //cout << "nameStr: " << nameStr <<	endl;
      if (nameStr.compare(currentName) == 0) {
	      pairs.erase(it);
	      break;
      }
    }
    
    // Add the input command to pairs
    Pair newPair = {.name = name, .value = value};
    pairs.push_back(newPair);
        
    //cout << "new alias added" << endl;
    
    // Replace command received from parse with its aliases
    // Replace [name] with [value]
  }
}

void simple_shell::parse_alias_command(char* cmd, char** tokens) {
  // Tokenize the command of alias into arguments
  // Checks if cmd is valid   
  size_t length = strlen(cmd);
  if (length > 0 && cmd[length - 1] == '\n') {
    cmd[length - 1] = '\0';
  }

  // Splits cmd via the equal sign
  char* delimiter = "=";
  char* temp1 = strtok(cmd, delimiter);
  tokens[0] = temp1;
  char* temp2 = strtok(NULL, delimiter);
  tokens[1] = temp2;
}

bool simple_shell::isAlias(char* cmd){
  // Check for the command "alias", shell will reponse accordingly  
  string cmdStr = cmd;
  string aliasCommand = "alias";
  return (cmdStr.compare(aliasCommand)==0);
}

void simple_shell::read_command(char** cmdTokens, ...) {
  string formatString;
  for (int i = 1; cmdTokens[i] != nullptr; ++i) {
    formatString += cmdTokens[i];
    formatString += " ";
  }
  // Store the provided read variable in the second value of read_line pair in simple_shell class
  read_line.second = formatString;
  // Prompt user for standard input, stored in the first value of read_line pair in simple_shell class
  getline(cin, read_line.first);
}

void simple_shell::echo_command(char** cmdTokens, ...) {
  // Check if there are arguments after "echo"
  if (cmdTokens[1] == nullptr) {
    std::cerr << "echo: missing arguments" << std::endl;
    return;
  }

  // Concatenate arguments into a single format string
  string formatString;
  for (int i = 1; cmdTokens[i] != nullptr; ++i) {
    formatString += cmdTokens[i];
    formatString += " ";
  }
  // Check if the echo argument is a stored variable --> print stored variable,
  // "$REPLY" used if no read variable was provided. Else print out provided argument.
  if((formatString == "$" + read_line.second || formatString == "$REPLY ") && formatString[0] == '$'){
    cout << read_line.first << endl;
  } else {
    va_list args;
    va_start(args, formatString.c_str());
    vprintf(formatString.c_str(), args);
    va_end(args);
  }
}

bool simple_shell::isQuit(char* cmd) {
  // Check for the command "quit" that terminates the shell
  string cmdStr = cmd;
  string quitCommand = "quit";
  return (cmdStr.compare(quitCommand) == 0);
}

bool simple_shell::isHelp(char* cmd){
  // check for the command "help", shell will reponse accordingly
  string cmdStr = cmd;
  string helpCommand = "help";
  return (cmdStr.compare(helpCommand)==0);
}

bool simple_shell::isPrintf(char* cmd){
  // check for the command "printf", shell will reponse accordingly
  string cmdStr = cmd;
  string PrintfCommand = "printf";
  return (cmdStr.compare(PrintfCommand)==0);
}

bool simple_shell::isRead(char* cmd) {
  // check for "read" command
  string cmdStr = cmd;
  string readCommand = "read";
  return(cmdStr.compare(readCommand) == 0);
}

bool simple_shell::isEcho(char* cmd) {
  // check for "echo" command
  string cmdStr = cmd;
  string echoCommand = "echo";
  return(cmdStr.compare(echoCommand) == 0);
}

void simple_shell::binary_get(const std::string& input, std::string& output) {
  output.clear();  // Clear the output string before appending characters
  std::ostringstream oss;
  for (size_t i = 0; i < input.size(); ++i) {
    // Check for escape sequences
    std::bitset<8>binaryRepresentation(input[i]);
    oss << binaryRepresentation.to_string();
  }
  output = oss.str();
}

void simple_shell::get_current_timestamp(std::string& output) {
  // Get the current timestamp in seconds since the epoch
  time_t current_time = std::time(nullptr);

  // Format the current timestamp using strftime
  char buffer[256]; // Adjust the buffer size as needed
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&current_time));

  // Assign the formatted string to the output
  output = buffer;
  // return output;
}
