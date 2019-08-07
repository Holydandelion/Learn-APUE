
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main (void)
{  
     char str[] = "0";
     char *sep_str = NULL;
     char *rest_str = str;
     char *delimiters = "|;";
     
    sep_str = strsep(&rest_str, delimiters);
    while (sep_str  != NULL)
	{
		printf("sep_str: %s \n", sep_str);
		printf("origin string is changed, str: %s \n", str);  /*will alway be 123, delimiter is repalced with \0 */  
		
		printf("rest of the string: %s \n", rest_str);		
		sep_str = strsep(&rest_str, delimiters);  
	}
	exit(0);
}