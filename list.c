#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "list.h"

/* static */ struct listEntry *listHead;
/* static */ struct listEntry *listTail;

int inet_cmp(uint8_t af, void *addr1, void *addr2)
{
	if (af == AF_INET)
	{
		return memcmp(&(((struct in_addr *)addr1)->s_addr),  &(((struct in_addr *)addr2)->s_addr), 4);
	}	

	if (af == AF_INET6)
	{
		return memcmp(&(((struct in6_addr *)addr1)->s6_addr), &(((struct in6_addr *)addr2)->s6_addr), 16);
	}

	return 0;
}

struct listEntry *_listSearch(struct listEntry *current, uint8_t protocol, uint8_t af, void *addr)
{
	char address[256];

	if (current == NULL)
	{
		return NULL;
	}

	if (protocol == current->protocol
		&& current->af == af
		&& inet_cmp(af, addr, &(current->addr)) == 0)
	{
		return current;
	}

	if (current->next == NULL)
	{
		return NULL;
	}
	return _listSearch(current->next, protocol, af, addr);
}

void listAdd(uint8_t protocol, uint8_t af, void *addr)
{
	struct listEntry *listNewItem;

	listNewItem = malloc(sizeof(struct listEntry));

	listNewItem->protocol = protocol;
	if (af == AF_INET)
	{
		listNewItem->af = AF_INET;
		listNewItem->addr.ipv4 = *((struct in_addr *)addr);
	}
	else if (af == AF_INET6)
	{
		listNewItem->af = AF_INET6;
		listNewItem->addr.ipv6 = *((struct in6_addr *)addr);
	}
	else
	{
		listNewItem->af = 0;
	}
	listNewItem->next = NULL;

	if (listHead == NULL || listTail == NULL)
	{
		listHead = listNewItem;
		listTail = listNewItem;
		return;
	}
	else
	{
		listTail->next = listNewItem;
		listTail = listNewItem;
	}
}

void _listList(struct listEntry *current)
{
	char address[256];

	if (current == NULL)
	{
		return;
	}
	printf("{\"@\":0x%08x,", current);
	printf("\"protocol\":%d", current->protocol);
	if (current->af == AF_INET)
	{
		inet_ntop(AF_INET, &(current->addr.ipv4), address, sizeof(address));
		printf(",\"address\":\"%s\"", address);
	}
	if (current->af == AF_INET6)
	{
		inet_ntop(AF_INET6, &(current->addr.ipv6), address, sizeof(address));
		printf(",\"address\":\"%s\"", address);
	}
	printf("}\n");
	if (current->next == NULL)
	{
		return;
	}
	_listList(current->next);
}

