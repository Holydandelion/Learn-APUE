#include	"open_server.h"
#include 	"server_unix_sock.h"
#include 	"handle_request.h"
#include	<sys/select.h>
 

void loop(void)
{
	int		i, n, max_fd, max_index, listen_fd, connect_fd, n_read;
	char	buf[MAXLINE] = {0};
	uid_t	uid;
	fd_set	read_set, origin_set;

	FD_ZERO(&origin_set);

	/* obtain fd to listen for client requests on */
	if ((listen_fd = server_listen(SERVER_PATH)) < 0)
	{
		printf("server_listen error, %d, %s\n", listen_fd, strerror(errno));
		exit(1);
	}	
	printf("fd:%d listening\n", listen_fd);
	FD_SET(listen_fd, &origin_set);
	max_fd = listen_fd;
	max_index = -1;

	for ( ; ; ) 
	{
		read_set = origin_set;	/* rset gets modified each time around */
		if ((n = select(max_fd + 1, &read_set, NULL, NULL, NULL)) < 0)
		{
			printf("select error, %d, %s\n", n, strerror(errno));
			exit(1);
		}

		if (FD_ISSET(listen_fd, &read_set)) 
		{
			/* accept new client request */
			if ((connect_fd = server_accept(listen_fd, &uid)) < 0)
			{
				printf("server_accept error, %d, %s\n", connect_fd, strerror(errno));
				exit(1);
			}			

			i = client_add(connect_fd, uid);
			FD_SET(connect_fd, &origin_set);
			if (connect_fd > max_fd)
				max_fd = connect_fd;	/* max fd for select() */
			if (i > max_index)
				max_index = i;	/* max index in client[] array */
			
			printf("new connection: uid %d, fd %d \n", uid, connect_fd);
			continue;
		}
		
		printf("%d connections is ready to read.\n", n);
		for (i = 0; n != 0 && i <= max_index; i++) 
		{	/* go through client[] array */
			if ((connect_fd = client_array[i].fd) < 0)
				continue;
				
			if (FD_ISSET(connect_fd, &read_set)) 
			{
				/* read argument buffer from client */
				if ((n_read = read(connect_fd, buf, MAXLINE)) < 0) 
				{
					printf("read error on fd %d %s\n", connect_fd, strerror(errno));
					exit(1);
				} 
				else if (n_read == 0) 
				{
					printf("closed: uid %d, fd %d \n", client_array[i].uid, connect_fd);
					client_del(connect_fd);	/* client has closed cxn */
					FD_CLR(connect_fd, &origin_set);
					close(connect_fd);
				} 
				else 
				{	/* process client's request */
					handle_request(buf, (size_t)n_read, connect_fd, client_array[i].uid);
				}
				/*test exit handler  */
				/* exit(0); */
				n--;
			}
		}
	}
}
