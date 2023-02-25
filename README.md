# My-Shell
<br />
&emsp;&emsp;In this project I have implemented a mini shell (my own shell) that supports the classical commands found in the bash command line. <br /><br />

## Functionalities <br /><br />
&emsp;&emsp;This shell can handle pipes (|), accepts logical expressions (&&, ||), can suspend a process (CTRL^C) and solve classical commands (like ls, cd, mkdir, rmdir, pwd etc.)<br />
&emsp;&emsp;Commands that I have implemented: <br />
&emsp;&emsp;&emsp;-> **clear**	-> clear the terminal <br />
&emsp;&emsp;&emsp;-> **cd** -> change directory <br />
&emsp;&emsp;&emsp;-> **pwd**	-> show the path of the current working directory <br />
&emsp;&emsp;&emsp;-> **echo**	-> display line of text/string that are passed as an argument <br /> <br />
&emsp;&emsp;&emsp;-> **mkdir**	-> create a new directory <br />
&emsp;&emsp;&emsp;&emsp;-> mkdir options: <br />
&emsp;&emsp;&emsp;&emsp;&emsp;-> **-m**   -> used to set the file modes(permissions) <br />
&emsp;&emsp;&emsp;&emsp;&emsp;-> **-p**   -> flag that enables the command to create parent directories as necessary <br /> <br />
&emsp;&emsp;&emsp;-> **rmdir**	-> remove the chosen directory <br />
&emsp;&emsp;&emsp;-> **ls**	-> list files or directories from current directory <br />
&emsp;&emsp;&emsp;&emsp;-> ls options (you can combine these principal options): <br />
&emsp;&emsp;&emsp;&emsp;&emsp;-> **-l**   -> list the files in long format <br />
&emsp;&emsp;&emsp;&emsp;&emsp;-> **-a**   -> list all files including hidden ones <br />
&emsp;&emsp;&emsp;&emsp;&emsp;-> **-r**   -> list the files in the reverse sortin order <br />
&emsp;&emsp;&emsp;-> **history** -> list all used commands in that session <br />
&emsp;&emsp;&emsp;-> **help**	-> show and explain supported commands <br />
<br />

## How to use the new shell <br /><br />
&emsp;&emsp;Add this command line in terminal before compiling the project: **sudo apt-get install libreadline-dev** <br />
&emsp;&emsp;Add file **a.out** so you can write the output <br />
&emsp;&emsp;Use this flag to compile the code: <br />
&emsp;&emsp;&emsp;&emsp; **-lreadline**       ----->       **gcc myshell.c -o a.out -lreadline** <br />
&emsp;&emsp;&emsp;&emsp; executing 	------> 	**./a.out** <br />
