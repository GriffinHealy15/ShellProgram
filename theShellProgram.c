
// The inlcude files are listed here
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>

// Declaration of Functions. These functions each call eachother. For instance, parseInput -> executeParseFunction 
// -> builtInCommands. Main calls thePrompt and changeDirectory.
int parseInput(char []);
int changeDirectory(char*);
int executeParseFunction(int, char**, char**, char*);
int inputOutputRedirection(char*, int);
int builtInCommands(char**, char[], int);


// Main Function. This function is where thePrompt and changeDirectory are used, also user input and the batchfile
// are determined here. Accepts the argv which could be the batchfile aswell as the argc strings.
int main(int argc, char* argv[])

{
    
    printf("\nWelcome, This is a shell program: ");
    printf("For instruction on commands of the shell: Enter | help\n\n");
    /* If argv[1] (the batchfile) after ./a.out or make is equal to batchfile input, then the batchfile 
    is loaded and ran instead of user input */
    if((argv[1] != NULL) && (strcmp(argv[1], "batchfile") == 0)) {
        FILE *batchFile = fopen("batchfile", "r"); //Open file for reading
		
    
        // The fileString is loaded with the batchfile accepting 100 chars.
    char fileString[100];
    while(fgets(fileString, 100, batchFile)){
        /* The prompt is created here. An array of 1024 char is accepted and we call the internal function of getcwd
         for this because this will get your current directory and put it into maindirectory.*/
       char maindirectory[1024];
       getcwd(maindirectory, sizeof(maindirectory));
       // Printed the directory with prefix of >>
       printf("%s>> ", maindirectory);

        //printf("%s", fileString);
        // This determines if a newline occurs, if so, then end fileString
        int batchEnd = strlen(fileString)-1;
        if(fileString[batchEnd] == '\n') 
            fileString[batchEnd]= '\0';
             // To quit the program just tpye quit or Quit
        if((strcmp(fileString, "quit") == 0)| (strcmp(fileString, "Quit") == 0) ){ 
            exit(0);
        }

        // Change Directories
        char tempCD1[100];
        int pos1 = 0;
        int cdVar1 = 0;
        /* Fill tempCD array with first three entries of userInput Fill up ann array with first two index
        values of command from the batchfle */
        for(pos1 = 0; pos1 < 3; pos1++)
        {
            tempCD1[cdVar1] = fileString[pos1];
            cdVar1++;
        }
            tempCD1[cdVar1] = '\0';

           //printf("%s\n", tempCD1);
        // If the first thee indexes equal cd then(space)
        if(strcmp(tempCD1, "cd ") == 0){
            // Call the changeDirectory function on fileString from batchfile
        changeDirectory(fileString);
        continue;
       }
       /*To run command in the background we test to see if last char is equal to &. If so, then we can
       create a fork and call parse function on the fileString from batchfile
        */
       else if(fileString[strlen(fileString) - 1] == '&'){ //Run the command in the background
        fileString[strlen(fileString) - 1] = '\0';
        int pid = fork();

        if(pid == 0){ //In child process
            // Call the parseInput function
            if(parseInput(fileString) == -1){
                exit(0);
            }
        } else{
            /* Contiune allows this command to run in background rather than waiting, which will be done for 
             commands that we execute
             */
            continue;
        }
    }

    else {
        // Create fork for noraml execution (not to run in background)
        int pid = fork();
        if(pid == 0){
        if(parseInput(fileString) == -1){
            exit(0);
        }
    } else{
        // This stops the program from allowing any other commands to be entered until completed command output.
        waitpid(pid, NULL, 0);
    }
}
    }
}
    else {
    // While not exited
    while(1){

        /* The prompt is created here. An array of 1024 char is accepted and we call the internal function of getcwd
         for this because this will get your current directory and put it into maindirectory.*/
        char maindirectory[1024];
        getcwd(maindirectory, sizeof(maindirectory));
        printf("%s>> ", maindirectory);

        // Get Users Input
        char userInput[1024]; //String will collect user's input
        fgets(userInput, 4096, stdin); //Get the user's input
        // Below is checking for newline, if so, then end the userinput array at that point.
        int end;
        end = strlen(userInput)-1;
        if( userInput[end] == '\n') 
            userInput[end]= '\0';
      
        // To quit the program, type: quit or Quit
        if((strcmp(userInput, "quit") == 0)| (strcmp(userInput, "Quit") == 0) ){ 
        exit(0);
    }
        // Change Directories
        char tempCD[100];
        int pos = 0;
        int cdVar = 0;
        /* Fill tempCD array with first three entries of userInput Fill up ann array with first two index
        values of command from the userInput */
        for(pos = 0; pos < 3; pos++)
        {
            // tempCD takes first two indexes hoping it is cd.
            tempCD[cdVar] = userInput[pos];
            cdVar++;
        }
            tempCD[cdVar] = '\0';

           printf("%s\n", tempCD);
        // if temp array is equal to cd with space, then call changeDirectory function.
        if(strcmp(tempCD, "cd ") == 0){
        changeDirectory(userInput);
        continue;
       }

       else if(userInput[strlen(userInput) - 1] == '&'){ //Run the command in the background
        userInput[strlen(userInput) - 1] = '\0';
        int pid = fork();
        // If pid is zero, then enter the child process
        if(pid == 0){ //In child process
            // Call the parseInput function
            if(parseInput(userInput) == -1){
                exit(0);
            }
        } else{
            /* Contiune allows this command to run in background rather than waiting, which will be done for 
             commands that we execute
             */
            continue;
        }
    }

    else {
        // Create fork for foreground
        int pid = fork();
        // if pid is zero, then parseInput function is called.
        if(pid == 0){
        if(parseInput(userInput) == -1){
            exit(0);
        }
    } else{
        // This stops the program from allowing any other commands to be entered until completed command output.
        waitpid(pid, NULL, 0);
    }
}
}
}
}
// The main function is now over, the function parseInput calls the other functions it needs to.

    // Parse Input Function
    int parseInput(char userInput[]){
        // Set a few arrays, the commandArray, the filetoExec, the pipeArray.
        // Set a few variables which are global to parseInput
        // Use strtok to gather sections of string, and place them in appropriate array or *file pointer
        char *commandArray[1024];
        char *pipeArray[1024];
        char *inputArrayPointer = strtok(userInput, " ");
        char *fileToExec;
        int nextIndex = 0;
        int nextPipeIndex = 0;
        int result_for_execute = 0;

       // Checks if userInput or batchfile is still there and has reached newline which is '\0'
        while(inputArrayPointer){
            // Allocate some space for the command array which will be the size of the strtok (set to inputArrayPointer)
            commandArray[nextIndex] = malloc(sizeof(char*) * (strlen(inputArrayPointer) + 1));
            // The following if statement sections check for the type of input from user or batchfile
            /* It could be regualar command, then add to commandArray, <, >, >>, |. Then it will place rest
             of text in different array */
            if(strcmp(inputArrayPointer, "<") == 0){ //Input redirection with <
                // Places text string after < into a file which will later be executed.
                // strtok skips spaces. Then resumes after space (delimiter)
                inputArrayPointer = strtok(NULL, " ");
                // Set the result to number for each type of redirection
                result_for_execute = 1;
                // Allocates space for file
                fileToExec = malloc(sizeof(char*) * (strlen(inputArrayPointer) + 1));
                // Sets file to pointer value
                fileToExec = inputArrayPointer;
                break;
            }
            
            if(strcmp(inputArrayPointer, ">>") == 0){ //Output redirection with >>
                // Places text string after >> into a file which will later be executed.
                // strtok skips spaces. Then resumes after space (delimiter)
                inputArrayPointer = strtok(NULL, " ");
                // Set the result to number for each type of redirection
                result_for_execute = 2;
                // Allocates space for file
                fileToExec = malloc(sizeof(char*) * (strlen(inputArrayPointer) + 1));
                // Sets file to pointer value
                fileToExec = inputArrayPointer;
                break;
            }
            
           
            if(strcmp(inputArrayPointer, ">") == 0){ //Output redirection with >
                // Places text string after > into a file which will later be executed.
                // strtok skips spaces. Then resumes after space (delimiter)
                inputArrayPointer = strtok(NULL, " ");
                // Set the result to number for each type of redirection
                result_for_execute = 3;
                 // Allocates space for file
                fileToExec = malloc(sizeof(char*) * (strlen(inputArrayPointer) + 1));
                // Sets file to pointer value
                fileToExec = inputArrayPointer;
                break;
            }
            
            if(strcmp(inputArrayPointer, "|") == 0){
                // Set the result to number for each type of redirection
                result_for_execute = 4;
                 // Places text string after > into a file which will later be executed.
                // strtok skips spaces. Then resumes after space (delimiter)
                inputArrayPointer = strtok(NULL, " ");
                while(inputArrayPointer){	
                    // Allocates space for pipeArray
                    pipeArray[nextPipeIndex] = malloc(sizeof(char*) * (strlen(inputArrayPointer) + 1));
                    // Fill pipeArray
                    pipeArray[nextPipeIndex] = inputArrayPointer;
                    // Checks for more pipes
                    inputArrayPointer = strtok(NULL, " ");
                    nextPipeIndex++;
                }
                pipeArray[nextPipeIndex] = NULL;
                break;
            }
      
            // CommandArray fills up from first strtok section. Example.) ls or clr, echo.
            // ls would go into commandArray[0].
            commandArray[nextIndex] = inputArrayPointer;
            inputArrayPointer = strtok(NULL, " ");
            // Increment to commandArray[1]
            nextIndex++;
           
    }
        commandArray[nextIndex] = NULL;

        if(executeParseFunction(result_for_execute, commandArray, pipeArray, fileToExec) == -1){
            return -1;
        }

        return 1;
    }

    // The parse function executes commands that have been parsed, hence the name
    int executeParseFunction(int result_for_execute, char *commandArray[], char *pipeArray[], char *fileToExec){
            // Switch statments used to check for type of redirection will need to use.
               switch(result_for_execute) {
                   // No redirection. Command could be ls or ls -a or clr 
                case 0: // Execution with single command
                // If not built in command then keep going
                if(builtInCommands(commandArray, NULL, 0) == -1) {
                    // Execute the commandArray. Ex. could be ls
                    if(execvp(commandArray[0], commandArray) != 0){
                    return -1;
                }
            }
               // Input redirection is used <
                case 1:
                 // If not built in command then keep going
                if(builtInCommands(commandArray, NULL, 0) == -1) {
                // Call redirection function and set file to stdin at 0
                inputOutputRedirection(fileToExec, 1);
                // Execute the command array with input redirection
                if(execvp(commandArray[0], commandArray) != 0){
                    return -1;
                }
            }
                // Output redirection is used >> (Appends to file if created already)
                case 2:
                 // If not built in command then keep going
                if(builtInCommands(commandArray, NULL, 0) == -1) {
                // Call redirection function and set file to stdout at 1
                inputOutputRedirection(fileToExec, 2);
                // Execute the command array with output redirection
                if(execvp(commandArray[0], commandArray) != 0){
                    return -1;
                }
            }
               
                // Output redirection is used >
                case 3:
                 // If not built in command then keep going
                if(builtInCommands(commandArray, NULL, 0) == -1) {
                 // Call redirection function and set file to stdout at 1
                inputOutputRedirection(fileToExec, 3);
                // Execute the command array with output redirection
                if(execvp(commandArray[0], commandArray) != 0){
                    return -1;
                }
            }
                // Piping
                case 4:
                {
                    // Create file descriptors array
                int pfds[2];
                // Pipe the array
                pipe(pfds);
                // Create child process
                int pid = fork();
                if (pid == 0) {
                    // Set whatever is outputed to write into pipe array (not using stdout)
                    // Use dup2 to change stdout file desciptor to pipe write
                    dup2(pfds[1], 1);
                    close(pfds[0]);
                    // Check for builtin command
                    if(builtInCommands(commandArray, NULL, 0) == -1) {
                        // Execute command. Could be ls
                    if(execvp(commandArray[0], commandArray) != 0){
                        return -1;
                    }
                }
            }
                else {
                    // Read from file instead of using stdin
                    // Use dup2 to change stdin file desciptor to pipe read
                    dup2(pfds[0], 0);
                    close(pfds[1]);
                    // Check for builtin command
                    if(builtInCommands(commandArray, NULL, 0) == -1) {
                        // Execute command. Could be wc
                    if(execvp(pipeArray[0], pipeArray) != 0){
                        return -1;
                    }

                }
            }
            }
            }        
            return -1;
            }

            // Bulit in command function, this checks input from user or batch file as equal to one of array below
            int builtInCommands(char *commandArray[], char fileToExec[], int typeOfRedirect){	
                // Array of builtins
                char *arrayofBuiltIns[] = {"clr", "dir", "environ", "echo", "help", "pause"};
                int indexOfBuilt = -1;
                // If array equals builtin then set the index of that. environ would be index 2.
                for(int index = 0; index < 6; index++){
                    if(strcmp(commandArray[0], arrayofBuiltIns[index]) == 0){
                        indexOfBuilt = index;
                    }
                }
                // Switch goes to index of builtin. And operates on it. Clr would go to case 0, call System(clear)
                switch(indexOfBuilt) {
               
                case 0:{ //Clear the screen using the system("clear") command
                system("clear");
                return indexOfBuilt;
            }
                // Open a directory using dirent
                case 1: {
                    // Using opendir internal function on dir command[1]
                    DIR *direct;        
                    struct dirent *dirent1;
                    // Open the directory
                    direct = opendir(commandArray[1]);
                    if(direct != NULL){
                        while((dirent1 = readdir(direct))){
                            printf("%s	", dirent1 -> d_name);
                        }
                        (void) closedir(direct);
                    } 
                    return indexOfBuilt;
                    
                }
                // Use system to print the environment
                case 2:{ //Print the environment variables
                system("printenv");
                return indexOfBuilt;
            }
                // Echo
                case 3: {

                    // Makes array and appends with commandArray
                    char echoS[1024];
                    int i = 0;
                    while(commandArray[i] != NULL){
                        // Adds echo (empty with commandArray[i])
                        strcat(echoS, commandArray[i]); 
                        strcat(echoS, " ");
                        i++;
                    }
                    // Print the echo through system.
                    system(echoS);
                    strcpy(echoS, ""); 
                    return indexOfBuilt;
                }
                // Load help file and print to the screen
                case 4: {
                FILE *helpFile;
                helpFile = fopen("help", "r");
                char helpFileL[300];
        
                while(fgets(helpFileL, 100, helpFile)){ //Read each line
                    printf("%s", helpFileL);
                }	
                return indexOfBuilt;
            }
                // Keep getting characters until user types newline
                case 5: {
                char pauseC;
		        while((pauseC = getchar()) != '\n'){
                continue;
                if (pauseC == '\n')
                    break;
            }
		        return indexOfBuilt;
            }
            }
        
            return -1;
            }
    

    // Using chdir, Change Directories
    int changeDirectory(char userInput[]){
        char changingdirectory[1024];
        int directoryStart = 0;
        // Starts from 3 because cd space takes of first 0,1,2 indexes
        for(int filePath = 3; filePath < strlen(userInput); filePath++){
            // Sets a directory array to string after cd space.
            changingdirectory[directoryStart] = userInput[filePath];
            directoryStart++;
        }
        changingdirectory[directoryStart] = '\0';
        // Using internal chdir on directory array we just set
        chdir(changingdirectory);
        return 1;
    }

// Determines the input redirection and a case is called depending on executeparsefunction
int inputOutputRedirection(char fileToExec[], int typeOfRedirect){
        // Switch used for type of redirect
            switch(typeOfRedirect) {
                
                // This sets the file to stdin so it reads from file
            case 1: //Input with <
               {
                // Opens file and uses input instead of stdin
                int inputType = open(fileToExec, O_RDONLY);
                // dup2 replaces file desriptor 0 stdin 
                dup2(inputType, 0);
                close(inputType);
                break;
               }
                // This sets the file to stdout so it writes to the file (appending too)
            case 2: // Output with >>
            {
                // Opens file and prints output to file not to stdout
                int outputType = open(fileToExec, O_WRONLY | O_APPEND);
                // dup2 replaces file desriptor 1 stdout
                dup2(outputType, 1);
                close(outputType); 
                break;
            }
            // This sets the file to stdout so it writes to the file
            case 3: //Output with >
            {
                // Opens file and prints output to file not to stout
                int outputType = open(fileToExec, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                // dup2 replaces file desriptor 1 stdout
                dup2(outputType, 1);
                close(outputType); 
                break;
            }
            }
    
              return 0;
        }
