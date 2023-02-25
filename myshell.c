//add this command line in terminal before compiling the project
//	sudo apt-get install libreadline-dev
//add file a.out so you can write the output
//use this flag to compile the code
//	-lreadline       ----->       gcc myshell.c -o a.out -lreadline
//	executing 	------> 	./a.out

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>
#include <readline/readline.h>
//this library provides much more than a simple history command
#include <readline/history.h>
#include <stdbool.h>

#define BUFFSIZE 1024
#define BLUE "\e[0;34m"
#define GREEN "\033[0;32m"
#define WHITE "\033[0;37m"
#define YELLOW "\033[0;33m"
//function that clears the shell by deleting everithing that was written in it before this command
#define my_clear() printf("\033[H\033[J")


//implemented functions
void my_startUp();
void my_printDirectory();
void my_exit();
char** my_split_line(char *line);
void my_cd(char **args);
void my_pwd();
void my_echo(char **args);
void my_mkdir(char **args);
void my_rmdir(char **args);
void my_details_printing(struct stat details);
void my_ls_l(char **args);
void my_ls(char **args);
void my_history(char **args);
void my_help(char **args);
int my_search_buildins(char **args);
int my_decide(char **args);
void my_logical_exp_and(char *command);
void my_logical_exp_or(char *command);
void my_handling_pipes(char *line);

bool flag_apel = 1;
char *cmd_list[] = {"exit", "clear", "cd", "pwd", "echo", "mkdir", "rmdir", "ls", "history", "help"};

//function that startup the shell when we open it
void my_startUp()
{
	my_clear();
	printf("\n\n*****************************************************************************\n");
	printf("\nAuthor rights:\n     Rincu Stefania\n     Ghergu Laura-Ioana\n     Pintenaru-Dumitrescu Nicole-Melissa\n\n");
	printf("\n		********************************\n\n%s			****MY SHELL****", YELLOW);
	printf("\n\n		*******OS PROJECT - 2022********\n\n%s		********************************\n\n", WHITE);
	printf("\n\n******************************************************************************\n");
	sleep(5);
	my_clear();
}

//function that gets the current directory and prints it
void my_printDirectory()
{
	char cwd[1024];
	//getcwd() -> function that determines the path name of the working directory and stores it in buffer
	getcwd(cwd, sizeof(cwd));
	printf("\nDir: %s", cwd);
}

//function that shows and explains supported commands by our shell
void my_help(char **args)
{
	if(args[1] != NULL)
	{
		printf(stderr, "Help command not properly entered!\n");
		exit(EXIT_FAILURE); 
	}
	printf("\n\n%s		MyShell supported Commands:\n", YELLOW);
	printf("****************************************************************************\n\n");
	printf("%s", WHITE);
	printf("exit	-> exit the terminal\n");
	printf("CTRL+C	-> exit the terminal\n");
	printf("clear	-> clear the terminal\n");
	printf("cd	-> change directory\n");
	printf("pwd	-> show the path of the current working directory\n");
	printf("echo	-> display line of text/string that are passed as an argument\n");
	printf("mkdir	-> create a new directory\n");
	printf("   mkdir options:\n");
	printf("     	-m   -> used to set the file modes(permissions)\n");
	printf("     	-p   -> flag that enables the command to create parent directories as necessary\n");
	printf("rmdir	-> remove the chosen directory\n");
	printf("ls	-> list files or directories from current directory\n");
	printf("   ls options (you can combine these principal options):\n");
	printf("     	-l   -> list the files in long format\n");
	printf("     	-a   -> list all files including hidden ones\n");
	printf("     	-r   -> list the files in the reverse sortin order\n");
	printf("history -> list all used commands in that session\n");
	printf("help	-> show and explain supported commands\n");
	printf("%s****************************************************************************\n\n", YELLOW);
	printf("%s", WHITE);
}

//function that prints all used commands (invoked when we use "history" command) 
void my_history(char **args)
{
	//history command has only one argument, so if we introduced more we raise an error
	if(args[1] != NULL)
	{
		printf(stderr, "History command not properly entered!\n");
		exit(EXIT_FAILURE); 
	}
	/*
		A structure used to pass around the current state of the history.
 		typedef struct _hist_state {
  			HIST_ENTRY **entries;       Pointer to the entries themselves
  			int offset;                 The location pointer within this array
  			int length; 		     Number of elements within this array
  			int size;                   Number of slots allocated to this array
  			int flags;
		} HISTORY_STATE;

	*/
	
	//we create a list of commands that we find encapsulated into the history library
	register HIST_ENTRY **cmd_history;
	register int hist_count;
	
	cmd_history = history_list();
	
	//we iterate through the command that we hind inside the historic and print them
	//the format is -> number of command inside the history, followed by the command itself
	for(hist_count = 0; cmd_history[hist_count]; hist_count++)
		printf("%d %s\n", hist_count + history_base, cmd_history[hist_count]->line);
}

//function that exit the shell
void my_exit()
{
	printf("\nGoodbye!\n\n");
	exit(0);
}

//function that changes the directory (invoked when we use "cd" command) 
void my_cd(char **args)
{
	//if we use the "cd" command without giving the name of the directory that we want to move into, the terminal will change the directory by going out of the current directory
	if(args[1] == NULL)
	{
		///chdir() -> changes the current directory of the calling process to the directory specified in path
		///getenv() -> searches the environment list to find the environment variable name and return a pointer if he finds it or not
		///so if we do not gave any name for the new directory the terminal verify if we can go back (home)
		if(chdir(getenv("HOME")) != 0)
		{
			perror("Change Directory failed!\n");
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		//if we specify the new directory name we try to change it using "chdir" function
		if(chdir(args[1]) != 0)
		{
			perror("Change Directory failed!\n");
			exit(EXIT_FAILURE);
		}
	}
}

//function that print the current directory (invoked when we use "pwd" command) 
void my_pwd()
{
	char cwd[1024];
	//we try to get the current directory and print it if we find it
	if(getcwd(cwd, sizeof(cwd)) == NULL)
		perror("Error at getting the directory!\n");
	else 
	{
		printf("Current directory: %s", cwd);
		printf("\n");
	}
	
}

//function that display line of text/string that are passed as an argument
void my_echo(char **args)
{
	int arg_count;
	for(arg_count = 1; args[arg_count]; arg_count++)
		printf("%s ", args[arg_count]);
	printf("\n\n");
}

//function that makes(creates) a new directory, giving it the name passed by us as an argument (invoked when we use "mkdir" command, followed by the name that we want to give to the new directory) 
void my_mkdir(char **args)
{
	int arg_count = 0, op;
	//0777 mode -> means that the new directory will have all permissions (read-write-execute) for owner, roup and other
	mode_t mode = 0777;
	char cwd[1024];
	
	getcwd(cwd, sizeof(cwd));
	while(args[arg_count])
		arg_count++;
		
	//getopt()  ---->  returns -1 when no more options are present
	//m: --> (--mode):   specify octal permissions of directories created by mkdir
	//p: --> (--parents):   parents or path will also create all directories leaing up to the given directory that do not exist already
	while((op = getopt(arg_count, args, "m:p:")) != -1)
	{
		switch(op)
		{	
			//strtol(const char *str, char **endptr, int base) ---->  converts the initial part of the str to a long int value according to given base
			//optarg ---> indicates an optional parameter to a command line option
			case 'm': 
			{	
				//initialy we set the directory mode to 0777 (all permissions given), but if we introduced "-m" option we reassign the variable "mode" with the new given mode
				mode = (mode_t)strtol(optarg, NULL, 8);
				break;
			}
			//optarg ---> indicates an optional parameter to a command line option
			case 'p':
			{
				//inserting each one of the new parent directory
				strcpy(cwd, optarg);
				break;
			}
			default: 
			{
				//error if we introduce more than one argument for "-m" or "-p" options
				printf(stderr, "Handling only -m or -p options only for --  %s  --\n", args[0]);
				exit(EXIT_FAILURE);
			}
		}
	}
	//optind -> index of the next element to be processed in args
	//knowing that we can have "-m" and "-p" options followed by another argument we will add optind to args so we will process the first argument not already processed
	args += optind;
	//if there is a name given to the new directory we proceed to create it
	if(*args)
	{
		//we try to make the new directory and add it to the current existing path
		strcat(cwd, "/");
		strcat(cwd, *args);
		if(mkdir(cwd, mode) == -1)
		{
			perror("mkdir failed!\n\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Directory created!\n\n");
		}
	}
	//otherwise we can not create the new directory because we do not have a name for it
	else 
	{
		printf(stderr, "Directory name not given ---> couldn't execute mkdir!\n\n");
		exit(EXIT_FAILURE);
	}
}

//function that removes(deletes) a given directory (invoked when we use "rmdir" command, followed by the name of the directory that we want to remove) 
void my_rmdir(char **args)
{
	//rmdir -> function that tries to remove a directory
	if(rmdir(args[1]) == -1)
	{
		perror("Unable to remove directory!\n\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("Directory removed!\n\n");
	}	
}

//function that handles "ls" command that is followed by an option that contains "l" in it, meaning that we want to print the long list format of the files and directories
void my_ls_l(char **args)
{
	//we iterate through the list of arguments to see if we have more than two parameters, which makes it an invalid command for "ls" type
	int arg_count = 0, i;
	while(args[arg_count])
		arg_count++;
	if(arg_count > 2)
	{
		printf("Command not properly entered!\n\n");
		exit(EXIT_FAILURE);
	}
		
	//struct dirent  ---> predefined structure that refers to directory entry
	struct dirent **dir_list;
	//struct stat  ----> predefined structure that provides detailed information about a file
	struct stat details;
	//scandir(const char *restrict, struct dirent ***restrict, int (*filter)(const struct *dirent), int (*compar)(const struct dirent **, const struct dirent **))  ----> function that scans the directory calling filter() on each directory entry, if filter is NULL all entries are selected
	//alphasort() ----> used as compar() to sort all the directory entries into alphabetical order
	//"." --> scanning parent directory
	int count = scandir(".", &dir_list, 0, alphasort);
	
	if(count == -1)
	{
		perror("Error scanning the directory!\n\n");
		exit(EXIT_FAILURE);
	}
	else if(count == 0)
	{
		printf("Empty directory!\n\n");
		return;
	}
	else
	{
		printf("Total %d\n", count);
		//"l + a + r" permutation ---> we want to print the long, including hidden files, reversed list
		if(strcmp(args[1], "-lar") == 0 || strcmp(args[1], "-lra") == 0 || strcmp(args[1], "-alr") == 0 || strcmp(args[1], "-arl") == 0 || strcmp(args[1], "-ral") == 0 || strcmp(args[1], "-rla") == 0) 
		{
			for(i = count - 1; i >= 0; i--)
			{
				//we verify if we found an unknown files which means we have an error
				if(stat(dir_list[i]->d_name, &details))
					continue;
				//printing the long version of the details about directory and files
				my_details_printing(details);
				//directory
				if(dir_list[i]->d_type == DT_DIR)
					printf("%s%s\n", BLUE, dir_list[i]->d_name);
				//executable file
				//access() -> function that checks to see if a file or directory exists
				//X_OK -> execute permission
				else if(access(dir_list[i]->d_name, X_OK) != -1)
					printf("%s%s\n", GREEN, dir_list[i]->d_name);
				//regular file
				else
					printf("%s\n", dir_list[i]->d_name);
				//we want the output to be white again
				printf("\033[0m");
			}
		}
		//"l + a" permutation ---> we want to print the long, including hidden files list
		else if((strcmp(args[1], "-la") == 0 || strcmp(args[1], "-al") == 0 ))
		{
			for(i = 0; i < count; i++)
			{
				//we verify if we found an unknown files which means we have an error
				if(stat(dir_list[i]->d_name, &details))
					continue;
				//printing the long version of the details about directory and files
				my_details_printing(details);
				//directory
				if(dir_list[i]->d_type == DT_DIR)
					printf("%s%s\n", BLUE, dir_list[i]->d_name);
				//executable file
				//access() -> function that checks to see if a file or directory exists
				//X_OK -> execute permission
				else if(access(dir_list[i]->d_name, X_OK) != -1)
					printf("%s%s\n", GREEN, dir_list[i]->d_name);
				//regular file
				else
					printf("%s\n", dir_list[i]->d_name);
				//we want the output to be white again
				printf("\033[0m");
			}
		}
		//"l + r" permutation ---> we want to print the long, reversed list
		else if(strcmp(args[1], "-lr") == 0 || strcmp(args[1], "-rl") == 0)
		{
			for(i = count - 1; i >= 0; i--)
			{
				if(strcmp(dir_list[i]->d_name, ".") == 0 || strcmp(dir_list[i]->d_name, "..") == 0)
					continue; 
				else
				{
					//we verify if we found an unknown files which means we have an error
					if(stat(dir_list[i]->d_name, &details))
						continue;
					//printing the long version of the details about directory and files
					my_details_printing(details);
					//directory
					if(dir_list[i]->d_type == DT_DIR)
						printf("%s%s\n", BLUE, dir_list[i]->d_name);
					//executable file
					//access() -> function that checks to see if a file or directory exists
					//X_OK -> execute permission
					else if(access(dir_list[i]->d_name, X_OK) != -1)
						printf("%s%s\n", GREEN, dir_list[i]->d_name);
					//regular file
					else
						printf("%s\n", dir_list[i]->d_name);
					//we want the output to be white again
					printf("\033[0m");
				}
			}
		}
		//"l" ---> we want to print the long list
		else if(strcmp(args[1], "-l") == 0)
		{
			for(i = 0; i < count; i++)
			{
				if(strcmp(dir_list[i]->d_name, ".") == 0 || strcmp(dir_list[i]->d_name, "..") == 0)
					continue; 
				else
				{
					//we verify if we found an unknown files which means we have an error
					if(stat(dir_list[i]->d_name, &details))
						continue;
					//printing the long version of the details about directory and files
					my_details_printing(details);
					//directory
					if(dir_list[i]->d_type == DT_DIR)
						printf("%s%s\n", BLUE, dir_list[i]->d_name);
					//executable file
					//access() -> function that checks to see if a file or directory exists
					//X_OK -> execute permission
					else if(access(dir_list[i]->d_name, X_OK) != -1)
						printf("%s%s\n", GREEN, dir_list[i]->d_name);
					//regular file
					else
						printf("%s\n", dir_list[i]->d_name);
					//we want the output to be white again
					printf("\033[0m");
				}
			}
		}
		else
		{
			printf("Command not properly entered!\n\n");
			exit(EXIT_FAILURE);
		}
	}
}

//function that handles "ls" command that is followed by an option that does not contain "l" in it, meaning that we do not want to print the long list format of the files and directories
void my_ls(char **args)
{
	//we iterate through the list of arguments to see if we have more than two parameters, which makes it an invalid command for "ls" type
	int arg_count = 0, i;
	while(args[arg_count])
		arg_count++;
	if(arg_count > 2)
	{
		printf("Command not properly entered!\n\n");
		exit(EXIT_FAILURE);
	}	
	//struct dirent  ---> predefined structure that refers to directory entry
	struct dirent **dir_list;
	//scandir(const char *restrict, struct dirent ***restrict, int (*filter)(const struct *dirent), int (*compar)(const struct dirent **, const struct dirent **))  ----> function that scans the directory calling filter() on each directory entry, if filter is NULL all entries are selected
	//alphasort() ----> used as compar() to sort all the directory entries into alphabetical order
	//"." scanning parent directory
	int count = scandir(".", &dir_list, 0, alphasort);
	
	if(count == -1)
	{
		perror("Error scanning the directory!\n\n");
		exit(EXIT_FAILURE);
	}
	else if(count == 0)
	{
		printf("Empty directory!\n\n");
		return;
	}
	else
	{
		//ls simple command, without any other option
		if(arg_count == 1)
		{
			for(i = 0; i < count; i++)
			{
				if(strcmp(dir_list[i]->d_name, ".") == 0 || strcmp(dir_list[i]->d_name, "..") == 0)
					continue; 
				else
				{
					//directory
					if(dir_list[i]->d_type == DT_DIR)
						printf("%s%s\n", BLUE, dir_list[i]->d_name);
					//executable file
					//access() -> function that checks to see if a file or directory exists
					//X_OK -> execute permission
					else if(access(dir_list[i]->d_name, X_OK) != -1)
						printf("%s%s\n", GREEN, dir_list[i]->d_name);
					//regular file
					else
						printf("%s\n", dir_list[i]->d_name);
					//we want the output to be white again
					printf("\033[0m");
				}
			}
		}
		//"a + r" permutation ---> we want to print the reversed list, including hidden files
		else if(strcmp(args[1], "-ar") == 0 || strcmp(args[1], "-ra") == 0) 
		{
			for(i = count - 1; i >= 0; i--)
			{
				//directory
				if(dir_list[i]->d_type == DT_DIR)
					printf("%s%s\n", BLUE, dir_list[i]->d_name);
				//executable file
				//access() -> function that checks to see if a file or directory exists
				//X_OK -> execute permission
				else if(access(dir_list[i]->d_name, X_OK) != -1)
					printf("%s%s\n", GREEN, dir_list[i]->d_name);
				//regular file
				else
					printf("%s\n", dir_list[i]->d_name);
				//we want the output to be white again
				printf("\033[0m");
			}
		}
		//"r" ---> we want to print the reversed list
		else if(strcmp(args[1], "-r") == 0)
		{
			for(i = count - 1; i >= 0; i--)
			{
				if(strcmp(dir_list[i]->d_name, ".") == 0 || strcmp(dir_list[i]->d_name, "..") == 0)
					continue; 
				else
				{
					//directory
					if(dir_list[i]->d_type == DT_DIR)
						printf("%s%s\n", BLUE, dir_list[i]->d_name);
					//executable file
					//access() -> function that checks to see if a file or directory exists
					//X_OK -> execute permission
					else if(access(dir_list[i]->d_name, X_OK) != -1)
						printf("%s%s\n", GREEN, dir_list[i]->d_name);
					//regular file
					else
						printf("%s\n", dir_list[i]->d_name);
					//we want the output to be white again
					printf("\033[0m");
				}
			}
		}
		//"a" ---> we want to print the list, including hidden files
		else if(strcmp(args[1], "-a") == 0)
		{
			for(i = 0; i < count; i++)
			{
				//directory
				if(dir_list[i]->d_type == DT_DIR)
					printf("%s%s\n", BLUE, dir_list[i]->d_name);
				//executable file
				//access() -> function that checks to see if a file or directory exists
				//X_OK -> execute permission
				else if(access(dir_list[i]->d_name, X_OK) != -1)
					printf("%s%s\n", GREEN, dir_list[i]->d_name);
				//regular file
				else
					printf("%s\n", dir_list[i]->d_name);
				//we want the output to be white again
				printf("\033[0m");
			}
		}
		else
		{
			printf("Command not properly entered!\n\n");
			exit(EXIT_FAILURE);
		}
	}
}

//function that search for buildin commands and try to execute them
//this function is invoked if we can not match the given command with one of the commands that we defined and handled inside our shell
int my_search_buildins(char **args)
{
	int status;
	pid_t pid = fork();
	//error -> fork() failed
	if(pid < 0)
	{
		perror("fork() failed!\n");
		exit(EXIT_FAILURE);
	}
	//child process
	else if(pid == 0)
	{
		if(execvp(args[0], args) < 0)
		{
			perror("Execution failed!\n");
			exit(EXIT_FAILURE);
		}
	}
	//parent process
	else
	{
		//WIFEXITED --> returns true if the child terminated normally
		//WIFSIGNALED --> returns true if the child proces was terminated by a signal
		do
		{
			//waitpid(pid_t pid, int *status, int options)
			//WUNTRACED --> also returns if a child has stopped
			waitpid(pid, &status, WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

//function that we use for "ls" command to print the details about the files and directories
void my_details_printing(struct stat details)
{
	/*
		possible permissions ---> r (read),  w (write),  x (execute)
		output format ---> (file type)(permissions owner (*3 permissions))(permissions Group(*3 permissions))(permissions Other(*3 permissions)) (link count) (owner) (group) (file size) (last modify date) (file name)
		!!!!each () has one space left for printing ---> if the file type or one of the permissions is NULL then the space saved for that will be completed with the symbol "-"
		
		EXAMPLE 
			drwx---r-- (missing permissions for group and other)
			-rwxrwxrwx (the file is not a directory)
			--wx---r-- (the file is not a directory and there are missing permissions)
			
		stat library predicates used
			IS_DIR -> tests if a given file is a directory
			S_IRUSR -> tests if the owner has the permission to read from the given file
			S_IWUSR -> tests if the owner has the permission to write into the given file
			S_IXUSR -> tests if the owner has the permission to execute the given file
			S_IRGRP -> tests if Group have the permission to read from the given file
			S_IWGRP -> tests if Group have the permission to write into the given file
			S_IXGRP -> tests if Group have the permission to execute the given file
			S_IROTH -> tests if Other have the permission to read from the given file
			S_IWOTH -> tests if Other have the permission to write into the given file
			S_IXOTH -> tests if Other have the permission to execute the given file
	
		struct stat fields used
			st_mode --> file type and it's permissions
			st_nlink --> number of hard links
			st_uid --> user ID of owner
			st_gid --> group ID of owner
			st_size --> total size in bytes
			st_mtime --> last modify time
	*/		
	
	//we want to know if the given file is a directory for the first character of the outputed message
	printf("%1s", (S_ISDIR(details.st_mode)) ? "d" : "-");
	
	//Permissions owner
	printf("%1s", (details.st_mode & S_IRUSR) ? "r" : "-");
	printf("%1s", (details.st_mode & S_IWUSR) ? "w" : "-");
	printf("%1s", (details.st_mode & S_IXUSR) ? "x" : "-");
	
	//Permissions Group
	printf("%1s", (details.st_mode & S_IRGRP) ? "r" : "-");
	printf("%1s", (details.st_mode & S_IWGRP) ? "w" : "-");
	printf("%1s", (details.st_mode & S_IXGRP) ? "x" : "-");
	
	//Permissions Other
	printf("%1s", (details.st_mode & S_IROTH) ? "r" : "-");
	printf("%1s", (details.st_mode & S_IWOTH) ? "w" : "-");
	printf("%1s", (details.st_mode & S_IXOTH) ? "x" : "-");
	
	//Link count --> hardlink count of the file, when referring to a file, or the number of contained directory entries, when referring to a directory
	printf("%2ld ", (unsigned long)(details.st_nlink));
	
	//Owner
	//using pwd library
	//getpwuid() --> search the user database for an entry with a matching 'uid' (user id) 
	//pw_name --> user's login name -->  struct passwd field	
	printf("%s ", (getpwuid(details.st_uid))->pw_name);
	
	//Group
	//using grp library
	//getgrgid() --> search the group database for an entry with a matching 'gid' (group id) 
	//gr_name --> group's name -->  struct group field	
	printf("%s ", (getgrgid(details.st_gid))->gr_name);
	
	//File size
	printf("%5lld ", (unsigned long long)(details.st_size));
	
	//Last modify date
	char date[13];
	//%b --> abbreviation month name
	//%d --> day of the month
	//%Y --> year
	strftime(date, 13, "%b %d %Y", localtime(&details.st_mtime));
	printf("%s ", date);
}

//function that splits the line(command) into a list of arguments
char** my_split_line(char *line)
{
	int buffsize = BUFFSIZE, pos = 0;
	
	//we memorize the given arguments
	char **buffer = malloc(buffsize * sizeof(char*));
	char *word;
	
	if(!buffer)
	{
		//we specify the file name and the line that raised the error
		printf(stderr, "Allocation error in file %s at line # %d\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	
	word = strtok(line, " \n\t\r\a");
	while(word)
	{
		//'*' ----> used in shell meaning 'current directory' or as a wild card by matching every filename in a given directory
		if(word[0] == '*' || word[strlen(word) - 1] == '*')
		{
			//data type that represents a directory stream
			DIR *directory;
			//struct dirent  ---> predefined structure that refers to directory entry
			struct dirent *dir;
			
			//we are trying to open the current directory
			directory = opendir(".");
			if(directory == NULL)
			{
				perror("Unable to read directory!\n");
				exit(1);
			}
			else
			{
				//trying to read from the directory
				dir = readdir(directory);
				while(dir != NULL)
				{
					if(dir->d_name != ".")
					{
						if(strlen(word) == 1 || (word[0] == '*' && (!strncmp(word + 1, (dir->d_name + strlen(dir->d_name) - strlen(word) + 1), strlen(word) - 1))) || (word[strlen(word) - 1] == '*' && (!strncmp(word, dir->d_name, strlen(word) - 1))))
						{
							buffer[pos] = dir->d_name;
							pos++;
						}
					}
				}
				//we should close the directory before exiting the loop
				closedir(directory);
			}
		}
		else
		{
			buffer[pos] = word;
			pos++;
		}
		
		if(pos >= buffsize)
		{
			buffsize += BUFFSIZE;
			buffer = realloc(buffer, buffsize * sizeof(char*));
			if(!buffer)
			{
				//we specify the file name and the line that raised the error
				printf(stderr, "Allocation error in file %s at line nr. %d\n", __FILE__, __LINE__);
				exit(EXIT_FAILURE);
			}
		}
		word = strtok(NULL, " \n\t\r\a");
	}
	buffer[pos] = NULL;	
	return buffer;
}

//function that gets the split command and try to match the arguments with the ones declared by us
int my_decide(char **args)
{
	int i;
	if(args[0] == NULL)
	{
		return 1;
	}
	
	//looking for commands that we previously defined
	for(i = 0; i < (sizeof(cmd_list) / sizeof(char*)); i++)
	{
		if(strcmp(args[0], cmd_list[i]) == 0)
		{
			switch(i)
			{
				case 0: {my_exit(); return 0;}
				case 1: {my_clear(); return 0;}
				case 2: {my_cd(args); return 0;}
				case 3: {my_pwd(); return 0;}
				case 4: {my_echo(args); return 0;}
				case 5: {my_mkdir(args); return 0;}
				case 6: {my_rmdir(args); return 0;}
				case 7: 
				{
					if(args[1] != NULL && strchr(args[1], 'l'))
						my_ls_l(args);
					else
						my_ls(args); 
					return 0;
				}
				case 8: {my_history(args); return 0;}
				case 9: {my_help(args); return 0;}
			}
		}
	}
	my_search_buildins(args);
	return 0;
}

//Logical expression AND
void my_logical_exp_and(char *command)
{
	long i = 0;
	int f_cmd1 = 0;
	char cmd1[5000], cmd2[5000], **args;
	memset(cmd1, 0, strlen(cmd1));
	//searching for the pair of "&&" so we can keep the first half inside an array and the second part inside another
	for(i = 0; i < strlen(command); i++)
		if(command[i] == '&' && command[i + 1] == '&')
		{
			i += 3;
			break;
		}
		else
			cmd1[strlen(cmd1)] = command[i];
	strcpy(cmd2, command + i);
	//if both command are true we move forward
	if(strcmp("true", cmd1) == 0 && strcmp("true", cmd2) == 0);
	//if the first command is true we have to handle only the second one to see if the expression is true or false
	else if(strcmp("true", cmd1) == 0)
	{
		flag_apel = 0;
		args = my_split_line(cmd2);
		my_decide(args);
	}
	//if the second command is true we have to handle only the first one to see if the expression is true or false
	else if(strcmp("true", cmd2) == 0)
	{
		flag_apel = 0;
		args = my_split_line(cmd1);
		my_decide(args);
	}
	//we have to handle both of the commands to see if the expression is true or false
	else
	{
		args = my_split_line(cmd1);
		if(my_decide(args) == 0)
		{
			flag_apel = !flag_apel;
			args = my_split_line(cmd2);
			my_decide(args);
		}	
	}
}

//Logical expression OR
void my_logical_exp_or(char *command)
{
	long i = 0;
	int f_cmd1 = 0;
	char cmd1[5000], cmd2[5000], **args;
	memset(cmd1, 0, strlen(cmd1));
	//searching for the pair of "||" so we can keep the first half inside an array and the second part inside another
	for(i = 0; i < strlen(command); i++)
		if(command[i] == '|' && command[i + 1] == '|')
		{
			i += 3;
			break;
		}
		else
			cmd1[strlen(cmd1)] = command[i];
	strcpy(cmd2, command + i);
	//if both command are true we move forward
	if(strcmp(cmd1, "true") == 0 || strcmp(cmd2, "true") == 0);
	//if the first command is true we have to handle only the second one to see if the expression is true or false
	else if(strcmp(cmd1, "false") == 0)
	{
		args = my_split_line(cmd2);
		my_decide(args);
	}
	//if the second command is true we have to handle only the first one to see if the expression is true or false
	else if(strcmp(cmd2, "false") == 0)
	{
		args = my_split_line(cmd1);
		my_decide(args);
	}
	//we have to handle both of the commands to see if the expression is true or false
	else
	{
		//if the first command is true we do not have to handle the second one, because in OR command is enough if only one expression is true
		args = my_split_line(cmd1);
		if(my_decide(args) == 0);
		else
		{
			args = my_split_line(cmd2);
			my_decide(args);
		}	
	}
}

//function that handles pipes, knowing that we can have multiple pipes in the inserted command 
void my_handling_pipes(char *line)
{
	pid_t pid;
	int i, pipe_count = 0, cmd_count = 0;
	char **args;
	
	for(i = 0; i < strlen(line) - 1; i++)
	{
		if(i > 0)
			if(line[i] == '|' && line[i-1] != '|' && line[i+1] != '|')
				pipe_count++;
	}
	//pipefds - pointer where we memorize the pipes that we find in the inserted command 
	//pipefds - has dimension of the number of pipes multiplied by 2 because each pipe has 2 components ----> the reading (first pipe from the pair) and the writing (second pipe from the pair) ends
	int *pipefds = (int*)malloc((2*pipe_count)*sizeof(int));
	char *token = (char*)malloc((1024)*sizeof(char));
	//reentrant version of strtok()
	token = strtok_r(line, "|", &line);
	//loop where we try to create the pipes
	//we have to create the pipes before fork() so we can connect two processes
	//pipe() ----> creates a pipe and puts the file descriptors for the reading and writing 
	for(i = 0; i < pipe_count; i++)
	{
		if(pipe(pipefds + i * 2) < 0)
		{
			perror("Pipe creation failed!\n");
			exit(EXIT_FAILURE);
		}
	} 
	do
	{
		pid = fork();
		//error -> fork() failed
		if(pid < 0)
		{
			perror("fork() failed!\n");
			exit(EXIT_FAILURE);
		}
		//child process ---> handling pipes
		else if(pid == 0)
		{	//dup2() ----> function that create a copy of the existing file descriptor
		
			//we verify if there is a previous pipe
			if(cmd_count != 0)
				//0 ---> stdin file descriptor
				//here we want to create a copy of what we read from the first pipe from the current pair of pipes 
				if(dup2(pipefds[(cmd_count - 1) * 2], 0) < 0)
				{
					perror("Couldn't get input!\n");
					exit(EXIT_FAILURE);
				}
			//we verify if there is a next pipe
			if(cmd_count != pipe_count)
				//1 ---> stdout file descriptor
				//here we want to create a copy of what we want to write inside the second pipe from the current pair of pipes 
				if(dup2(pipefds[cmd_count * 2 + 1], 1) < 0)
				{
					perror("Couldn't output!\n");
					exit(EXIT_FAILURE);
				}
			//loop where we use close() calls to close unnecessary duplicate file descriptors
			for(i = 0; i < 2 * pipe_count; i++)
				close(pipefds[i]);
			args = my_split_line(token);
			if(execvp(args[0], args) < 0)
			{
				perror("Execution failed!\n");
				exit(EXIT_FAILURE);
			}
		}
		cmd_count++;
		token = strtok_r(NULL, "|", &line);
	}while(cmd_count <= pipe_count && token != NULL);
	for(i = 0; i < 2 * pipe_count; i++)
		close(pipefds[i]);
	free(pipefds);
	for(i = 0; i <= pipe_count; i++)
		wait(NULL);
	printf("\n\n\n");
}

void signal_ctrlc()
{
	rl_backward_kill_line(1, 0);
	printf("\n");
	rl_redraw_prompt_last_line();
}

int main()
{
	//My Shell
	
	my_startUp();
	
	long k, i, j;
	int  flag_and = 0, flag_or = 0, flag;
	char *line, **args;
	char end_line[2] = {"\n"};
	char aux_line[50000], command[50000], cpy_line[50000];
	
	signal(SIGINT, signal_ctrlc);	
	
	while(1)
	{
		my_printDirectory();
		
		//reading a line(command)
		line = readline("\n>>> ");
		
		if(strlen(line))
		{	
			if(strcmp(line, end_line)==0)
			{
				continue;
			}
							
			
			//adding the line(command) to the history
			add_history(line);
			
			if(strcmp(line, "exit")==0)
			{
				free(line);
				my_exit();
			}
		
			//logic operators
			if(strstr(line, "&&") || strstr(line, "||"))
			{
				//we want to make the proggram handle more than one logical expression, meaning we can have more expressions, each expression inside brackets
				//here we search for expressions inside brackets, so we can handle them first
				
				strcpy(aux_line, line);
				//this flag tels us if we still have brackets inside the command or not
				flag = 1;
				while(flag == 1)
				{
					//flag_and -> tells us if we have to handle an "AND" expression
					//flag_or -> tells us if we have to handle an "OR" expression
					flag = 0; flag_and = 0; flag_or = 0;
					//we erase everything from cpy_line, because we do not know if we used it previously
					memset(cpy_line, 0, strlen(cpy_line));
					
					for(i = 0; i < strlen(aux_line); i++)
					{
						//if we find an opening bracket we iterate through the array looking for the closing one, memorizing the logical expression inside them
						if(aux_line[i] == '(')
						{
							flag = 1;
							//we erase everything from command
							memset(command, 0, strlen(command));
							for(j = i + 1; j < strlen(aux_line); j++)
							{
								if(aux_line[j] != ')')
								{	
									//we can not handle brackets inside brackets
									if(aux_line[j] == '(')
									{
										perror("Can not handle brackets inside other brackets!\n\n");
										exit(EXIT_FAILURE);
									}
									//we determine if we have an "AND" expression
									//we can not have both expressions inside one bracket
									if(j < strlen(aux_line) - 1 && aux_line[j] == '&' && aux_line[j + 1] == '&')
									{
										if(flag_or == 1)
										{
											perror("Can not handle '&&' and '||' without '()' in the same command!\n\n");
											exit(EXIT_FAILURE);
										}
										flag_and = 1;
									}
									//we determine if we have an "OR" expression
									//we can not have both expressions inside one bracket
									else if (j < strlen(aux_line) - 1 && aux_line[j] == '|' && aux_line[j + 1] == '|')
									{
										if(flag_and == 1)
										{
											perror("Can not handle '&&' and '||' without '()' in the same command!\n\n");
											exit(EXIT_FAILURE);
										}
										flag_or = 1;
									}
									command[strlen(command)] = aux_line[j];
								}
							}
							//"AND"
							if(flag_and)
								my_logical_exp_and(command);
							//"OR"
							else if(flag_or)
								my_logical_exp_or(command);
							memset(command, 0, strlen(command));
							strcpy(command, "true");
							strcat(cpy_line, command);	
							i = j;
							//reseting
							flag_and = 0;
							flag_or = 0;
						}
						else
						{
							cpy_line[strlen(cpy_line)] = aux_line[i];
						}
					}
					memset(aux_line, 0, strlen(aux_line));
					strcpy(aux_line, cpy_line);
				}
				//if we still have logical expressions that are not inside btrackets we simply prelucrate them one by one (from left to right)
				if(strcmp("true", aux_line) != 0)
				{
					//flag_and -> tells us if we have to handle an "AND" expression
					//flag_or -> tells us if we have to handle an "OR" expression
					flag_or =  0; flag_and = 0;
					for(i = 0; i < strlen(aux_line); i++)
					{
						//we determine if we have an "AND" expression
						//we can not have both expressions inside if we do not have brackets
						if(i < strlen(aux_line) - 1 && aux_line[i] == '&' && aux_line[i + 1] == '&')
						{
							if(flag_or > 0)
							{
								perror("Can not handle '&&' and '||' without '()' in the same command!\n\n");
								exit(EXIT_FAILURE);
							}
							flag_and++;
						 }
						 //we determine if we have an "OR" expression
						//we can not have both expressions inside if we do not have brackets
						 else if (i < strlen(aux_line) - 1 && aux_line[i] == '|' && aux_line[i + 1] == '|')
						 {
							if(flag_and > 0)
							{
								perror("Can not handle '&&' and '||' without '()' in the same command!\n\n");
								exit(EXIT_FAILURE);
							}
							flag_or++;
						}
					}
					//we can not handle more than one logical expression
					if(flag_and > 1 || flag_or > 1)
					{
						perror("Can not handle more logical expressions in the same command! Use brackets!\n\n");
						exit(EXIT_FAILURE);
					}
					else
					{
						//"AND"
						if(flag_and == 1)
							my_logical_exp_and(aux_line);
						//"OR"
						else if(flag_or == 1)
							my_logical_exp_or(aux_line);
					}
				}
				printf("\n\nLogical expressions executed!\n\n");
			}
			//PIPES
			else if(strchr(line, '|'))
			{
				my_handling_pipes(line);
				printf("\nPipes handeled!\n\n");
			}
			//executing commands
			else
			{
				args = my_split_line(line);
				my_decide(args);
				free(args);
			}
		free(line);
		}
	}
	return 0;
}
