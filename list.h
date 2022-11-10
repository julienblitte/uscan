#ifndef LIST_H
#define LIST_H

#include <stdint.h>
#include <netinet/in.h>

struct listEntry
{
	uint8_t protocol;
	uint8_t af;
	union {
		struct in_addr ipv4;
		struct in6_addr ipv6;
	} addr;
	struct listEntry *next;
};

extern struct listEntry *listHead;
extern struct listEntry *listTail;

struct listEntry *_listSearch(struct listEntry *current, uint8_t protocol, uint8_t af, void *addr);
#define listSearch(protocol,af,addr) _listSearch(listHead,protocol,af,addr)

void listAdd(uint8_t protocol, uint8_t af, void *addr);

void _listList(struct listEntry *current);
#define listList() _listList(listHead)

#endif
