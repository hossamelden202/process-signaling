#include <stdlib.h>
#include<stdio.h>
#include<signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
void printE();

char *PATHS[100];
int numP=0;
void handle_cd(char **args,int argc){
if(argc!=2)
{
    printE();
   
}
else{
    if(chdir(args[1])!=0)
    printE();
   
}

}
void setup_default_path() {
  PATHS[0] = strdup("/bin");
    numP = 1;
}
char *resolve_path(char *command) {
    for (int i = 0; i < numP; ++i) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", PATHS[i], command);
        if (access(full_path, X_OK) == 0) {
            return strdup(full_path);
        }
    }
    return NULL;
}
void execute_external(char **args, int argc, char *redirect_file) {
pid_t pid = fork();
if (pid == 0) {
if (redirect_file) {
int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd < 0 || dup2(fd, STDOUT_FILENO) < 0) {
printE();
 exit(1);
 }
 close(fd);
}
 char *full_path = resolve_path(args[0]);
 if (!full_path) {
   printE();
exit(1);
 }
 execv(full_path, args);
  printE();
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        printE();
    }
}

void handle_pwd(){
char cwd[1024];
if(getcwd(cwd,1024)!=NULL)
printf("%s\n",cwd);
else printE();

}
void freeP(){
for(int i=0;i<numP;i++)
free(PATHS[i]);
numP=0;

}
void handle_paths(char **args,int argc)
{
freeP();
for(int i=1;i<argc&&i<100;i++)
PATHS[i]=strdup(args[i]);
i++;
}




void printE(){
char *msg="An error has occured\n";
    write(STDOUT_FILENO,msg,strlen(msg));
}
void signal_handler()
{

write(STDOUT_FILENO,"cmpsh> ",8);
fflush(stdout);

}
void parse(char *line)
{
    char *redirectFILE=NULL;
if(strchr(line,'>'))
{
char *split=strtok(line,">");
line=split;
char *helper=strtok(NULL,">\n");
if (!helper || strtok(NULL, ">\n")) {
    printE();
    return;
}
redirectFILE=strtok(helper,"\t\n");
}
char *args[100];
int argc = 0;
char *token = strtok(line, " \t\n");
while (token && argc < 100 - 1) {
    args[argc++] = token;
    token = strtok(NULL, " \t\n");
}
args[argc] = NULL;
if (argc == 0) return;

if (strcmp(args[0], "exit") == 0) {
    if (argc != 1) printE();
    else exit(0);
} else if (strcmp(args[0], "cd") == 0) {
    handle_cd(args, argc);
} else if (strcmp(args[0], "pwd") == 0) {
    handle_pwd();
} else if (strcmp(args[0], "paths") == 0) {
    handle_paths(args, argc);
} else {
    execute_external(args, argc, redirectFILE);
}

}

void run(FILE *file)
{
    char line[1024];
    while(1){
if(file==stdin)
{
    signal_handler();
}    

    if(fgets(line,1024,file)==NULL)break;
parse(line);



    }exit(0);
}
int main(int argc,char *argv[])
{
signal(SIGINT,signal_handler);
signal(SIGTSTP,signal_handler);
setup_default_path();
if(argc==1)
{
    run(stdin);

}
else if(argc==2)
{
FILE*file=fopen(argv[1],"r");
if(!file)
{printE();
exit(1);
}
run(file);
fclose(file);

}else{

printE();
exit(1);

}
return 0;
}