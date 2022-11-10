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
#include "dahua.h"
#include "json.h"

static struct protocol_t *context;

void dahua_init(struct protocol_t *this)
{
	context = this;

	context->name = "Dahua";
	context->buildRequest = dahua_buildRequest;
	context->decodeProtocol = dahua_decodeProtocol;
	context->address = inet_addr(DAHUA_ADDR);
	context->port = DAHUA_PORT;
	context->multicastSocket = 0;
	context->broadcastSocket = 0;
	context->selftest = "Dahua2";

	multicastListener(context, this->port, this->address);
}

size_t dahua_buildRequest(char *request, size_t size)
{
	int sent;
	struct Dahua2Header header;

	char message[DAHUA_DISCOVERY_SIZE];
	int message_len;
	int request_size;

	// build message
	memset(&header, 0, sizeof(header));
	header.headerSize = htole32(sizeof(header));
	header.headerMagic = htobe32(DAHUA_MAGIC);

	strncpy(message, "{\"method\":\"DHDiscover.search\",\"params\":{\"mac\":\"\",\"uni\":0}}\n", DAHUA_DISCOVERY_SIZE);

	message_len = strlen(message);
	header.packetSize1 = htole32(message_len);
	header.packetSize2 = htole32(message_len);

	request_size = sizeof(header) + message_len;
	if (size < request_size)
	{
		panic("dahua.buildRequest: provided buffer is too small!");
	}

	memcpy(request, &header, sizeof(header));
	memcpy(request+sizeof(header), message, message_len);

	return request_size;
}

void dahua_decodeProtocol(const char *data, const size_t size, struct in_addr from, resultCallback_t callback)
{
	if (size >= sizeof(struct Dahua2Header))
	{
		dahua_decodeJson(data + sizeof(struct Dahua2Header), from, callback);
	}
}

void dahua_decodeJson(const char *json, struct in_addr from, resultCallback_t callback)
{
	char *type, *uuid;
	int ret;
	char *value, *value2, *value3;

	bool ipv6_en;

	struct in_addr ipv4;
	struct in6_addr ipv6;

	
	value = jsonExtract("method", json, false);
	if (value == NULL)
	{
		return;
	}

	if (strcmp(value, "client.notifyDevInfo") != 0)
	{
		free(value);
		return;
	}
	free(value);

	value = jsonExtract("IPv4Address", json, true);
	if (value != NULL)
	{
		value2 = jsonExtract("IPAddress", value, false);
		if (value2 != NULL)
		{
			if (inet_aton(value2, &ipv4) == 0)
			{
				ipv4 = from; //parse failure
			}
			free(value2);
		}
		else
		{
			ipv4 = from;
		}
		free(value);
	}


	ipv6_en = false;
	value = jsonExtract("IPv6Address", json, true);
	if (value != NULL)
	{
		value2 = jsonExtract("IPAddress", value, false);
		if (value2 != NULL)
		{
			value3 = strrchr(value2, '\\');
			if (value3 != NULL)
			{
				*value3 = '\0';
			}
			value3 = strrchr(value2, '/');
			if (value3 != NULL)
			{
				*value3 = '\0';
			}
			if (inet_pton(AF_INET6, value2, &ipv6) == 1)
			{
				ipv6_en = true;
			}
			free(value2);
		}
		free(value);
	}

	value = jsonExtract("DeviceType", json, false);
	if (value != NULL)
	{
		type = value;
	}
	else
	{
		type = strdup("");
	}

	value = jsonExtract("SerialNo", json, false);
	if (value != NULL)
	{
		uuid = value;
	}
	else
	{
		uuid = strdup("");
	}

	callback(context->id, context->name, AF_INET, &ipv4, type, uuid);
	if (ipv6_en)
	{
		callback(context->id, context->name, AF_INET6, &ipv6, type, uuid);
	}

	free(type);
	free(uuid);
}

