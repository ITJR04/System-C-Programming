//Test whether a word is a abecedarian word
//No need to check whether each character is an alphabetic character 
int abecedarian(const char *word)
{
        for(int i = 0; i < strlen(word-1); i++) {
                //printf("Word[i] = %c\n", word[i]);
                //printf("Word[i+1] = %c\n", word[i+1]);
                if(word[i] < word[i+1]){
                printf("abecedarian");
                }

        }
}


//redirect standard input to the specified file
void redirectStdin(const char *filename)
{

        //int pd1[2];

        int fd = open("dict.txt", O_RDONLY);
        if(fd < 0)
        {
                perror("Error opening the file\n");
                exit(-1);
        
        }
	//TODO
        //replace file descriptor to standard output. STDOUT right now is 1. 
        //fill in the code below
        int child = fork();
        if(child == -1) {
                perror("Fork failed.");
                exit(1);

        }
        if(child == 0){
                printf("Works");
        }


 /*       else{
                if(pipe(pd1) == -1){
                die("pipe() 1");
                } 
                dup2()
                


        } */
        dup2(fd, 0);
        close(fd);
        
}

//redirect standad output to the specified file
void redirectStdout(const char *filename)
{
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if(fd < 0)
        {
                perror("Error opening the file\n");
                exit(-1);
        }
	//TODO
	//fill in the code below
        dup2(fd, 1);
        close(fd);
}
