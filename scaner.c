/*
 * scaner.c - get http server name
 *
 * Author: kXuan <xuanmgr@gmail.com>
 * License: MIT
 */
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#include<stdio.h>

#define HTTP_REQUEST "GET / HTTP/1.0\r\nConnection: close\r\nUser-Agent: ServerAppCollector/0.1\r\n\r\n"

static const char *find_server_field(const char *rep, size_t length) {
	const char *p = rep;
	while (p < rep + length - 7)
	{
		if (*p == '\n') break;
		++p;
	}

	while (p < rep + length - 7)
	{
		if ((p[0] == 's' || p[0] == 'S') &&
			(p[1] == 'e' || p[1] == 'E') &&
			(p[2] == 'r' || p[2] == 'R') &&
			(p[3] == 'v' || p[3] == 'V') &&
			(p[4] == 'e' || p[4] == 'E') &&
			(p[5] == 'r' || p[5] == 'R') &&
			p[6] == ':') {
			p += 7;
			
			while(*p == ' ' && p < rep + length)
				++p;
			
			break; //return
		}
		++p;
	}
	
	if (p >= rep + length) 
		return NULL;
	else
		return p;
}

/* get_server - try to connect to host and get the http server name (and version)
 * @host   - remote ipv4 address
 * @buffer - response buffer, the server name will be saved to here
 * @maxlen - buffer length
 *
 * return
 *   On successful, this function returns the length of http server name.
 *   On network fail, -1 will be returned and errno will be set.
 *   Otherwise, return 0
 */
int get_server(uint32_t host, char *buffer, int maxlen) {
	struct timeval timeout;
	fd_set sockset = {0};
	int ret = 0;
	int fd;
	const char *server_field;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in host_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(80),
		.sin_addr = {
			.s_addr = htonl(host)
		}
	};
	fcntl(fd, F_SETFL, O_NONBLOCK);
	ret = connect(fd, (struct sockaddr *)&host_addr, sizeof(host_addr));
	if (ret && EINPROGRESS != errno)
		goto out;

	FD_ZERO(&sockset);
	FD_SET(fd, &sockset);
	timeout = (struct timeval){1, 0};
	if (select(fd + 1, NULL, &sockset, NULL, &timeout) <= 0)
		goto out;

	int errlen = sizeof(errno);
	ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &errno, &errlen);
	if (ret || errno) {
		ret = -1;
		goto out;
	} 
	
	ret = send(fd, HTTP_REQUEST, sizeof(HTTP_REQUEST) - 1, MSG_NOSIGNAL);
	if (ret == -1)
		goto shut_out;
	
	FD_ZERO(&sockset);
	FD_SET(fd, &sockset);
	timeout = (struct timeval){1, 0};
	if (select(fd + 1, &sockset, NULL, NULL, &timeout) <= 0)
		goto shut_out;

	ret = read(fd, buffer, maxlen); //ret now equals to the full response body
	if (ret <= 0) {
		goto shut_out;
	}
	server_field = find_server_field(buffer, ret);
	
	if(server_field) {
		const char *eof = server_field;
		while(*eof != '\n' && eof < buffer + ret)
			++eof;
		ret = eof - server_field; //ret now equals to the value length of http server field
		memmove(buffer, server_field, ret);
	} else {
		ret = 0;
	}
shut_out:
	shutdown(fd, SHUT_RDWR);
out:
	close(fd);
	return ret;
}
