#include <tsh.h>

using namespace std;

int main() {
  char cmd[81];
  simple_shell *shell = new simple_shell();
  
  cout << "tsh> ";
  while (fgets(cmd, sizeof(cmd), stdin)) {
    CmdTokens *tokens = new CmdTokens();
    int tokenCount = 0;
    if (cmd[0] != '\n') {
      shell->parse_command(cmd, &tokens, &tokenCount);

      if (shell->isQuit(tokens->cmd[0])) {                                                 
        exit(0);
      } else if (shell->isPrintf(tokens->cmd[0])) {
        shell->printf_command(tokens->cmd);
        cout << endl;
      } else if (shell->isHelp(tokens->cmd[0])) {
        shell->help_command();
      } else if (shell->isRead(tokens->cmd[0])) {
        shell->read_command(tokens->cmd);
      } else if (shell->isEcho(tokens->cmd[0])) {
        shell->echo_command(tokens->cmd);
        cout << endl;
      } else if (shell->isAlias(tokens->cmd[0])) {
      	shell->alias_command(tokens->cmd);
      } else else {
        // only this command supports pipe commands
        shell->exec_command(tokens, tokenCount);
      }
      
    }
    cout << "tsh> ";
  }
  cout << endl;
  exit(0);

}
