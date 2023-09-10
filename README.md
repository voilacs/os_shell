# os_shell
Git repository with all files required for assignment 2 of os course CSE231
https://github.com/voilacs/os_shell
A.)
Another example could be a command that uses advanced shell scripting constructs like arrays, associative arrays, or specific syntax that is unique to certain shells. For instance:
    array=("nikks", "viboots","cha")
    echo "${array[1]}"

Array parsing and interpreting has not been implemented and thus the shell cannot assign variables even when special symbols like $ are used.
B) This shell is able to implement basic functionalities of listing, making new files and all the commands as mentioned in the doc. We are even able to implement pipelining.
But the shell naturally lacks more complex functionalities of bash scripting like looping assinging variable names and such.
var_x=5
this command means nothing to our shell and it cannot interpret that this is actually a assigning attempt rather than the name of a command.
C) 
Special symbols like "", $ which are use to invoke the declaration of variable names and such are not recognised because the shell only uses the execvp command and nothing else.

CONTRIBUTIONS:-
1) Anmol Kumar( 2022081): I reviewed the lecture slides understood how the infinite loop of shell is implemented and implemented the basic framework of the shell by using execvp command and other related helper functions.
2) Dhawal Garg: Much work had to be done after implementing basic functionalites: Error implementation, making fib.c and helloworld.c files and extensive testing. Dhawal was instrumental in these tasks.

Really thankful to have a partner indeed takes out a lot of stress!

https://github.com/voilacs/os_shell