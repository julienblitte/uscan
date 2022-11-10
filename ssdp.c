#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <regex.h>

#include "uscan.h"
#include "ssdp.h"

static struct protocol_t *context;

void ssdp_init(struct protocol_t *this)
{
	context = this;

	context->name = "SSDP";
	context->buildRequest = ssdp_buildRequest;
	context->decodeProtocol = ssdp_decodeProtocol;
	context->address = inet_addr(SSDP_ADDR);
	context->port = SSDP_PORT;
	context->multicastSocket = 0;
	context->broadcastSocket = 0;
	context->selftest = "SSDP";

	multicastListener(context, this->port, this->address);
}

size_t ssdp_buildRequest(char *request, size_t size)
{
	int request_size;

	request_size = snprintf(request, size, "M-SEARCH * HTTP/1.1\r\nHost: %s:%u\r\nST:upnp:rootdevice\r\nMan:\"ssdp:discover\"\r\nMX:2\r\n\r\n", SSDP_ADDR, SSDP_PORT);

	return request_size + 1; // including tailling zero
}

void printBuffer(const char *data, const size_t size)
{
	int i, j;
	char car[16];
	char hex[48];
	char digit[8];

	for(i=0; i <= size/16; i++)
	{
		car[0] = '\0';
		hex[0] = '\0';

		for(j=0; j < size-(16*i) ; j++)
		{
			if (car[j] >= 0x20)
			{
				car[j] = data[16*i+j];
			}
			else
			{
				car[j] = '.';
			}
			snprintf(digit, 8, "%02x ", data[16*i+j]);
			strcat(hex, digit);
		}
		car[j] = '\0';
		printf("%48s  %16s\n", hex, car);
	}
}

void ssdp_decodeProtocol(const char *data, const size_t size, struct in_addr from, resultCallback_t callback)
{
	char *type, *uuid;
	char *value;

	char *text;
	text = malloc(size+1);
	memcpy(text, data, size);
	text[size] = 0;

	if (strlen(text) < 6)
	{
		free(text);
		return;
	}

	//printBuffer(data, size);

	if (strncmp(text, "NOTIFY", 6) != 0)
	{
		free(text);
		return;
	}

	uuid = strdup("unknown");
	type = strdup("unknown");

	callback(context->id, context->name, AF_INET, &from, type, uuid);

	free(type);
	free(uuid);
}

