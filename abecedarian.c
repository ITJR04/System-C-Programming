#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h> 
#include <fcntl.h>
#include <string.h> 
#include <sys/stat.h>
#include "abe.c"

#define MAX 60	
int main()
{
	redirectStdin("dict.txt");
        redirectStdout("result.txt");

	char word[MAX];
	while(scanf("%s\n", word)!=EOF)
		if(abecedarian(word)) printf("%s\n", word);
	return 0;
}
