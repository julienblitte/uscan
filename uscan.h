#ifndef USCAN_H
#define USCAN_H

#define BUFFER_SIZE 1500

typedef void (*resultCallback_t)(uint8_t protocol, const char *protocolName, uint8_t af, void *address, const char *type, const char *uuid);

struct protocol_t;

typedef void (*init_t)(struct protocol_t *this);
typedef size_t (*buildRequest_t)(char *request, size_t size);
typedef void (*decodeProtocol_t)(const char *data, const size_t size, struct in_addr address, resultCallback_t callback);

struct protocol_t
{
	uint32_t id;
	const char *name;
	init_t init;
	buildRequest_t buildRequest;
	decodeProtocol_t decodeProtocol;
	in_addr_t address;
	uint16_t port;
	int multicastSocket;
	int broadcastSocket;
	pthread_t multicastThread;
	pthread_t broadcastThread;
	const char *selftest;
};

void registerProtocol(uint32_t id, init_t init);
void usage();
void panic(const char* error);
void printResult(uint8_t protocol, const char *protocolName, uint8_t af, void *address, const char *type, const char *uuid);
void interface();

void multicastListener(struct protocol_t *protocol, uint16_t port, in_addr_t address);
void *listener(void *protocol);
void *sender(void *protocol);

void selftest(struct protocol_t protocol);

#endif
