#ifndef DAHUA_H
#define DAHUA_H

#define DAHUA_PORT  37810
#define DAHUA_ADDR "239.255.255.251"
#define DAHUA_MAGIC 0x44484950   // 'DHIP', BIG ENDIAN

#define DAHUA_DISCOVERY_SIZE 256

struct Dahua2Header
{
	uint32_t headerSize;
	uint32_t headerMagic;
	uint32_t _reserved_08;
	uint32_t _reserved_0C;
	uint32_t packetSize1;
	uint32_t _reserved_14;
	uint32_t packetSize2;
	uint32_t _reserved_1C;
};

void dahua_init(struct protocol_t *this);
size_t dahua_buildRequest(char *request, size_t size);
void dahua_decodeProtocol(const char *data, const size_t size, struct in_addr address, resultCallback_t callback);
void dahua_decodeJson(const char *json, struct in_addr address, resultCallback_t callback);

#endif
