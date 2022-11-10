#ifndef SSDP_H
#define SSDP_H

#define SSDP_PORT  1900
#define SSDP_ADDR "239.255.255.250"

void ssdp_init(struct protocol_t *this);
size_t ssdp_buildRequest(char *request, size_t size);
void ssdp_decodeProtocol(const char *data, const size_t size, struct in_addr address, resultCallback_t callback);
void ssdp_decodeJson(const char *json, struct in_addr address, resultCallback_t callback);

#endif
