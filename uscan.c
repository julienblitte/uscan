#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <regex.h>

#include "list.h"
#include "uscan.h"
#include "dahua.h"
#include "wsdiscovery.h"
#include "ssdp.h"

#define MAX_PROTOCOLS 256
#define MAX_PATH 512

static struct protocol_t protocols[MAX_PROTOCOLS];
static int protocolCount;

#define SCAN_PERIOD	2

int main(int argc, char **argv)
{
	pthread_t senderThread;
	int i;

	if (argc > 1)
	{
		usage();
		exit(EXIT_SUCCESS);
	}

	registerProtocol(1, wsdiscovery_init);
	registerProtocol(2, ssdp_init);
	registerProtocol(4, dahua_init);

	for(i=0; i < protocolCount; i++)
	{
		protocols[i].init(&(protocols[i]));
	}
	pthread_create(&senderThread, NULL, sender, NULL);

	interface();

	pthread_cancel(senderThread);
	for(i=0; i < protocolCount; i++)
	{
		if (protocols[i].multicastSocket)
		{
			pthread_cancel(protocols[i].multicastThread);
		}
		if (protocols[i].broadcastSocket)
		{
			pthread_cancel(protocols[i].broadcastThread);
		}
	}

	return EXIT_SUCCESS;
}

void usage()
{
	perror("Usage: uscan [options]\n\n");
}

void panic(const char* error)
{
	if (error)
	{
		fprintf(stderr, "%s\n\n", error);
	}
	fprintf(stderr, "Error code is %d. %s\n", errno, strerror(errno));
	exit(EXIT_FAILURE);
}

void registerProtocol(uint32_t id, init_t init)
{
	protocols[protocolCount].id = id;
	protocols[protocolCount].init = init;

	protocolCount++;
}

void multicastListener(struct protocol_t *protocol, uint16_t port, in_addr_t address)
{
	struct ip_mreq mreq;
	static struct sockaddr_in addr;

	// set up socket
	protocol->multicastSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (protocol->multicastSocket < 0)
	{
		panic("Error creating the endpoint socket sender.");
	}
	memset((char *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	// bind to port
	if (bind(protocol->multicastSocket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		panic("Error binding the address for listening.");
	}

	// multicast register
	mreq.imr_multiaddr.s_addr = address;
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(protocol->multicastSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		panic("Error setting socket option of the listener.");
	}

	pthread_create(&(protocol->multicastThread), NULL, listener, protocol);
}

void *listener(void *vargp)
{
	struct protocol_t *protocol;

	struct sockaddr_in from;
	int from_len;
	int recieved;

	char buffer[BUFFER_SIZE];

	protocol = (struct protocol_t*)vargp;

	from_len = sizeof(from);
	while (1)
	{
		recieved = recvfrom(protocol->multicastSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &from, &from_len);
		if (recieved < 0)
		{
			panic("Error recieving data.");
		}
		else if (recieved == 0)
		{
			break;
		}
		else
		{
			protocol->decodeProtocol(buffer, recieved, from.sin_addr, &printResult);
		}
	}
}

void *sender(void *vargp)
{
	int sent;
	int i;

	char request[1500];
	int request_size;

	static struct sockaddr_in addr[MAX_PROTOCOLS];
	static int addrlen;

	for(i=0; i < protocolCount; i++)
	{
		memset((char *)&(addr[i]), 0, sizeof(addr));
		addr[i].sin_family = AF_INET;
		addr[i].sin_addr.s_addr = protocols[i].address;
		addr[i].sin_port = htons(protocols[i].port);
	}

	while(1)
	{
		for(i=0; i < protocolCount; i++)
		{
			request_size = protocols[i].buildRequest(request, sizeof(request));
			addrlen = sizeof(addr[i]);

			sent = sendto(protocols[i].multicastSocket, request, request_size, 0, (struct sockaddr *)&(addr[i]), addrlen);
			if (sent < 0)
			{
				panic("Error sending data.");
			}
		}
		sleep(SCAN_PERIOD);
	}
}

#define ESC 0x1B
void interface()
{
	static struct termios oldt, newt;
	int k;
	int i;

	printf("Press q to exit.\n");

	for(int i=0; i < protocolCount; i++)
	{
		selftest(protocols[i]);
	}

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON|ECHO);          
	newt.c_cc[VMIN] = 1;
	newt.c_cc[VTIME] = 0;

	k = 0;
	while (1)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		k = getchar();
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
		switch((char)k)
		{
			case 'd':
			case 'D':
				listList();
				break;
			case 'q':
			case 'Q':
			case ESC:
				return;
			case 's':
			case 'S':
				// do scan
				break;
		}
	}
}

void printResult(uint8_t protocol, const char *protocolName, uint8_t af, void *address, const char *type, const char *uuid)
{
	char addr_str[256];

	if (listSearch(protocol, af, address) == NULL)
	{
		inet_ntop(af, address, addr_str, sizeof(addr_str));

		printf("%12s %40s %30s %30s\n", protocolName, addr_str, type, uuid);
		listAdd(protocol, af, address);
	}
}

void selftest(struct protocol_t protocol)
{
	char path[MAX_PATH];
	char buffer[BUFFER_SIZE];
	FILE *f;
	size_t read;
	struct in_addr addr;

	if (protocol.selftest == NULL)
	{
		return;
	}

	snprintf(path, sizeof(path), "selftest/%s.selftest", protocol.selftest);
	f = fopen(path, "rb");

	if (f == NULL)
	{
		return;
	}
	read = fread(buffer, 1, sizeof(buffer), f);
	if (read != 0)
	{
		addr.s_addr = htonl(0xF0000000 | protocol.id << 8);
		if (protocol.decodeProtocol != NULL)
		{
			protocol.decodeProtocol(buffer, read, addr, printResult);
		}
	}
	fclose(f);
}
