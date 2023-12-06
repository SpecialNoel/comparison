# 377Final

Install 

Use cd to go to folder of this program and type "make" to install the program (NOTE: current Makefile is made for a UMass Edlab machine). Once it is installed on your computer, you can type ./tsh_app to get into the shell and use it.

Commands

Printf: 
          Printf prints strings and with 2 more options, %b and %t, it can prints binrary number of the character and the local time. To use the printf, you can go the shell, and call the command with typein printf with                string you like to print out. 
          For example, if you like to print out "hello world", you can type in printf hello world. No "" are nesscery. 
          To use the options, you will need to type in '%' after printf command and then, type the option charater after the '%'. 
          For exmaple, you can type in printf %bd to get the binrary format of the charater d. 


test:   
testing %b
```
tsh> printf %bd
001001010110001001100100
```
testing %t
```
tsh> printf %t
2023-12-05 17:08:28
```
testing \n
```
tsh> printf d\nc\nw
d
c
w
```
testing print string
```
tsh> printf hello world
hello world
```

          
Help:
          Help is a command you can use as a user guide of this simple shell. 
          To use the help command,  you can go to shell and type help. Shell will print out the infromation to help you to use the shell.

```         
tsh> help
Welcome to the simple shell!
printf - Write the formatted arguments to the standard output under the control of the format. 
help - Display helpful information about builtin commands. If pattern is specified, help gives detailed help on all commands matching pattern, otherwise a list of the builtins is printed.
Echo - Print out the line to screen
Read - One line is read from the standard input, or from the file descriptor fd supplied as an argument
quit - Exit the shell
```


Alias: 
          Alias is a command you can use to replace a single command with another name.
          To use this command, you can go to shell and type command in this syntax: alias [name]='[value]'. For example: 
```
alias list='ls'
```
will replace "list" with the ls command when it is used.
          The current version of this alias command does not support the additional options which the alias command in linux system has, and it can only take one command inside the [value] part. Wrong syntax will cause errors.

Tests:
test1: add one alias and show it
```
tsh> alias list='ls'
tsh> alias
alias list='ls'
```
test2: then, replace the first alias, and add one more alias, finally show them
```
tsh> alias list='cd'
tsh> alias
alias list='cd'
tsh> alias ok='cat'
tsh> alias
alias list='cd'
alias ok='cat'
```


Features:

Infinite pipeline:
          The user can pipe as many commands as they would like. Examples: `tsh> ls | sort | grep t`

Tests:

2 pipes:
```
tsh> ls | sort
include
Makefile
obj
README.md
src
submission.zip
test
tsh_app
tsh_test
```

3 pipes:
```
tsh> ls | sort | grep t
test
tsh_app
tsh_test
```

4 pipes:
```
tsh> cat src/tsh.cpp | grep string | grep c | grep f
  // cat src/tsh.cpp | grep string | grep c | grep f
    // Concatenate arguments into a single format string
    // Call printf with the concatenated format string
```
