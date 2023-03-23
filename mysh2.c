/*Justin Moore*/
/*CS-390 Linux*/
/*3-22-23*/
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_ARGS 3
#define MAX_LINE 256
#define DELIMITERS " \t\n"



void do_echo(int argc, char* args[]) {
    int i;
    if (argc == 1) {                                                                        /*executing echo command(print nothing)*/
        printf("\n");
    }
    if (strcmp(args[1], "-n") == 0) {                                                       /*executing echo command(print argument, no carriage return)*/
        i = 2;
        while(i < argc) {
            printf("%s ", args[i]);
            i++;
        }
    } else {                                                                                /*executing echo command(print argument, with carriage return)*/
        i = 1;
        while(i < argc) {
            printf("%s ", args[i]);
            i++;
        }
        printf("\n");
    }
}


void do_psone(int argc, char* args[], char* user_prompt) {

    if (argc != 2) {                                                                        /*executing PS1 command(not enought args, displays error)*/
        printf("Usage: PS1 new_prompt\n");
        return;
    }
    char str[] = " ";
    strncat(args[1], str, 1);
    strcpy(user_prompt, args[1]);

}

void do_cat(int argc, char* args[]) {

    if (argc != 2) {                                                                        /*executing cat command(not enought args, displays error)*/
        printf("Usage: cat file\n");
        return;
    }
    char* filename = args[1];
    FILE* file = fopen(filename, "r");
    if (file == NULL) {                                                                     /*executing cat command(can not find file, displays error)*/
        printf("Error: Unable to open file %s\n", filename);
        return;
    }
    char c = fgetc(file);
    while (c != EOF) {                                                                      /*executing cat command(reads chars from file and displays to console until end of file)*/
        printf("%c", c);
        c = fgetc(file);
    }
    fclose(file);

}

void do_cp(int argc, char* args[]) {

    if (argc != 3) {                                                                        /*executing cp command(if anything other than three args, displays error)*/
        printf("Usage: cp source_file destination_file\n");
        return;
    }
    char* source_filename = args[1];
    char* destination_filename = args[2];
    FILE* source_file = fopen(source_filename, "r");
    if (source_file == NULL) {                                                              /*executing cp command(no source filename, displays error)*/
        printf("Error: Unable to open file %s\n", source_filename);
        return;
    }
    FILE* destination_file = fopen(destination_filename, "w");
    if (destination_file == NULL) {                                                         /*executing cp command(no destination filename, displays error)*/
        printf("Error: Unable to open file %s\n", destination_filename);
        fclose(source_file);
        return;
    }
    char c = fgetc(source_file);                                                            /*reading every char from source file and writing to destination file*/
    while (c != EOF) {
        fputc(c, destination_file);
        c = fgetc(source_file);
    }
    fclose(source_file);
    fclose(destination_file);

}

void do_rm(int argc, char* args[]) {

    if (argc != 2) {                                                                        /*executing cp command(if anything other than two args, displays error)*/
        printf("Usage: rm file\n");
        return;
    }
    char* filename = args[1];
    if (unlink(filename) != 0) {                                                            /*executing cp command(if unable to delete file, displays error; otherwise file is deleted)*/
        printf("Error: Unable to delete file %s\n", filename);
        return;
    }

}

void do_mkdir(int argc, char* args[]) {

    if (argc != 2) {                                                                         /*executing mkdir command(if number of args is not two, display error)*/
        printf("Usage: mkdir directory\n");
        return;
    }
    char* dirname = args[1];
    if (mkdir(dirname, 0777) != 0) {                                                        /*executing mkdir command(if dir is able then we create dir with read, write, and execute permissions for users, groups, others; d = directory when viewing with ls -l*/
            printf("Error: Unable to create directory %s\n", dirname);
        return;
    }

}


void do_rmdir(int argc, char* args[]) {

    if (argc != 2) {                                                                        /*executing rmdir command(if number of args is not two, display error)*/
        printf("Usage: rmdir directory\n");
        return;
    }
    char* dirname = args[1];
    if (rmdir(dirname) != 0) {                                                              /*executing rmdir command(if dir exists then remove, otherwise display error)*/
        printf("Error: Unable to delete directory %s\n", dirname);
        return;
    }

}

void do_exit() {                                                                            /*exiting program*/

    exit(0);

}

int do_exec(int argc, char* args[]) {                                                       /*executing all other commands found in PATH environment variable*/


    char path_from_getenv [256];
    char progpath[256];
    char* p;
    int found;

    strcpy(path_from_getenv ,getenv("PATH"));                                                /*getting the PATH environment variable and copying to char array*/

    p = strtok(path_from_getenv,":");                                                        /*tokenizing PATH with colon delimeter*/

    while(p)
    {
        strcpy(progpath,p);                                                                  /*checking if program exists*/
        strcat(progpath,"/");
        strcat(progpath, args[0]);

        if (access(progpath,X_OK) == 0)                                                      /*X_OK means it exists and has execute permission*/
        {
            found = 1;
            break;
        }

        p = strtok(0,":");

    }


    char* path[] = {progpath, NULL};                                                        /*adding NULL for the execv() call*/
    char* the_args[256] = {args[0]};

    int i = 1;
    while(i < argc) {
        the_args[i] = args[i];
        i++;
    }



    if (found == 0) printf("COULD NOT FIND PROGRAM\n");
    else {

            pid_t child_pid;                                                                 /* the child process that the execution will soon run inside of.*/
            child_pid = fork();                                                              /*creating child process with fork()*/
            int return_status;

            if(child_pid == 0) {                                                             /*if fork() was successfull run the command with the arguments*/
               execv(path[0], the_args);
            }

            waitpid(child_pid, &return_status, 0);

            return return_status;
        }

}




int main() {

    char prompt[3] = "$ ";                                                          /*prompt symbol*/
	char line[MAX_LINE];                                                            /*line for user input*/
	char* args[MAX_ARGS];                                                           /*inputs copied from the command lines argv*/

	while (1) {                                                                     /*infinite loop*/

        int argc = 0;                                                               /*declaring number of arguments here since we didn't need it from the actual cmd line*/
		printf("%s", prompt);
		fgets(line, MAX_LINE, stdin);                                               /*reading input line*/

		char* arg = strtok(line, DELIMITERS);                                       /*tokenizing the input line*/


		while (arg != NULL && argc < MAX_ARGS) {                                    /*loop while there is a valid string and num of args don't exceed max*/
			args[argc] = arg;                                                       /*storing each argument as an index in a array for use by execute_command routine*/
			argc++;
			arg = strtok(NULL, DELIMITERS);                                         /*use previous string with same delimiters*/
		}

        if (argc == 0) printf("\r");                                                 /*if no arguments then do nothing*/
        else if (strcmp(args[0], "echo") == 0) do_echo(argc, args);                  /*executing echo command(print inputs to the screen)*/
        else if (strcmp(args[0], "PS1") == 0) do_psone(argc, args, prompt);          /*executing PS1 command(changes the symbol in command prompt at beginning of input line)*/
        else if (strcmp(args[0], "cat") == 0) do_cat(argc, args);                    /*executing cat command(displays contents of a file)*/
        else if (strcmp(args[0], "cp") == 0) do_cp(argc, args);                      /*executing cp command(used for copying file; source and destination files as arguments)*/
        else if (strcmp(args[0], "rm") == 0) do_rm(argc, args);                      /*executing rm command(used for removing file; source and destination files as arguments)*/
        else if (strcmp(args[0], "mkdir") == 0) do_mkdir(argc, args);                /*executing mkdir command(command is used to create a new folder that will contain files)*/
        else if (strcmp(args[0], "rmdir") == 0) do_rmdir(argc, args);                /*executing rmdir command(command is used to remove an existing folder that will contain no files; if you want to delete folder with files use rmdir -r, r = recursion)*/
        else if (strcmp(args[0], "exit") == 0) do_exit();                            /*executing exit command(command is used to exit my shell)*/
        else do_exec(argc, args);                                                    /*executing all other commands found in the $PATH environment variable*/

	}


	return 0;

}
