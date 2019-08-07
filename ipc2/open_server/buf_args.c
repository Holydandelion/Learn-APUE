#include "buf_args.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define	WHITE_CHARS	" \t\n"	/* white space for tokenizing arguments */

/*
 * buf[] contains white-space-separated arguments.  We convert it to an
 * argv-style array of pointers, and call the user's function (optfunc)
 * to process the array.  We return -1 if there's a problem parsing buf,
 * else we return whatever optfunc() returns.  Note that user's buf[]
 * array is modified (nulls placed after each token).
 */
int buf_args(char *buf, int (*optfunc)(int argc, char **argv))
{
	char	*ptr = buf; 
	char 	*sub_str = buf;
	char 	**argv = NULL;
	int		argc = 0;
	int 	rval = 0;

	/*need to append NULL*/
	if ((argv = malloc(strlen(buf) + 1 + sizeof(NULL))) == NULL)
		return (-1);

	while ((sub_str = strsep(&ptr, WHITE_CHARS)) != NULL) 
		argv[argc++] = sub_str;
	
	argv[argc] = NULL;

	rval = (*optfunc)(argc, argv);	
	free(argv);
	return(rval);
}
