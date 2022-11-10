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
#include "wsdiscovery.h"
#include "xml.h"

static struct protocol_t *context;

void wsdiscovery_init(struct protocol_t *this)
{
	context = this;

	context->name = "WSDiscovery";
	context->buildRequest = wsdiscovery_buildRequest;
	context->decodeProtocol = wsdiscovery_decodeProtocol;
	context->address = inet_addr(WSDISCOVERY_ADDR);
	context->port = WSDISCOVERY_PORT;
	context->multicastSocket = 0;
	context->broadcastSocket = 0;
	context->selftest = "WSDiscovery";

	multicastListener(context, this->port, this->address);
}

size_t wsdiscovery_buildRequest(char *request, size_t size)
{
	int request_size;

	const char *announce = "<s:Envelope"
		" xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\""
		" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\""
		" xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\""
		" xmlns:w=\"http://schemas.xmlsoap.org/ws/2006/02/devprof\""
		" xmlns:o=\"http://www.onvif.org/ver10/device/wsdl\""
		">"
		"<s:Header>"
		"<a:Action s:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>"
		"<a:MessageID>urn:uuid:f686768c-3e60-4f9c-a344-0769929d665c</a:MessageID>"
		"<a:To s:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>"
		"<a:ReplyTo>"
		"<a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address>"
		"</a:ReplyTo>"
		"</s:Header>"
		"<s:Body>"
		"<d:Probe>"
		"<d:Types>w:Device o:Device</d:Types>"
		"</d:Probe>"
		"</s:Body>"
		"</s:Envelope>";

	const char *verbatim = "<s:Envelope"
		" xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\""
		" xmlns:a=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\""
		">"
		"<s:Header>"
		"<a:Action s:mustUnderstand=\"1\">http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</a:Action>"
		"<a:MessageID>uuid:f686768c-3e60-4f9c-a344-0769929d665c</a:MessageID>"
		"<a:ReplyTo><a:Address>http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous</a:Address></a:ReplyTo>"
		"<a:To s:mustUnderstand=\"1\">urn:schemas-xmlsoap-org:ws:2005:04:discovery</a:To>"
		"</s:Header><s:Body><Probe xmlns=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\">"
		"<d:Types xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" xmlns:dp0=\"http://www.onvif.org/ver10/network/wsdl\">dp0:NetworkVideoTransmitter</d:Types>"
		"</Probe>"
		"</s:Body>"
		"</s:Envelope>";

	request_size = strlen(announce);
	if (request_size > size)
	{
		panic("wsdiscovery_buildRequest: buffer too small!");
		return 0;
	}

	strncpy(request, announce, size);

	return request_size;
}

void wsdiscovery_decodeProtocol(const char *data, const size_t size, struct in_addr from, resultCallback_t callback)
{
	char *type, *uuid;
	char *value;

	//printf("[%s]\n", data);
	value = xmlExtract("Address", data, true);
	if (value == NULL)
	{
		uuid = strdup("unknown");
	}
	else
	{
		uuid = value;
	}

	value = xmlExtract("Types", data, true);
	if (value == NULL)
	{
		type = strdup("unknown");
	}
	else
	{
		type = value;
	}

	callback(context->id, context->name, AF_INET, &from, type, uuid);

	free(type);
	free(uuid);
}

