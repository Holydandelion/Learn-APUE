#include	"open_server.h"

#define	NALLOC	10		/* # client structs to alloc/realloc for */

static void client_alloc(void)		/* alloc more entries in the client[] array */
{
	int		i;

	if (client_array == NULL)
		client_array = malloc(NALLOC * sizeof(Client));
	else
		client_array = realloc(client_array, (client_size + NALLOC) * sizeof(Client));
		
	if (client_array == NULL)
	{
		printf("realloc error, %s\n", strerror(errno));
		exit(1);
	}			

	/* initialize the new entries */
	for (i = client_size; i < client_size + NALLOC; i++)
		client_array[i].fd = -1;	/* fd of -1 means entry available */

	client_size += NALLOC;
}

/*
 * Called by loop() when connection request from a new client arrives.
 */
int client_add(int fd, uid_t uid)
{
	int		i;

	if (client_array == NULL)		/* first time we're called */
		client_alloc();
again:
	for (i = 0; i < client_size; i++) {
		if (client_array[i].fd == -1) {	/* find an available entry */
			client_array[i].fd = fd;
			client_array[i].uid = uid;
			return(i);	/* return index in client[] array */
		}
	}

	/* client array full, time to realloc for more */
	client_alloc();
	goto again;		/* and search again (will work this time) */
}

/*
 * Called by loop() when we're done with a client.
 */
void client_del(int fd)
{
	int		i;

	for (i = 0; i < client_size; i++) {
		if (client_array[i].fd == fd) {
			client_array[i].fd = -1;
			return;
		}
	}
	printf("can't find client entry for fd %d \n", fd);
	exit(1);
}
