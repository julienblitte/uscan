#ifndef WSDISCOVERY_H
#define WSDISCOVERY_H

#define WSDISCOVERY_PORT  3702
#define WSDISCOVERY_ADDR "239.255.255.250"

void wsdiscovery_init(struct protocol_t *this);
size_t wsdiscovery_buildRequest(char *request, size_t size);
void wsdiscovery_decodeProtocol(const char *data, const size_t size, struct in_addr address, resultCallback_t callback);
void wsdiscovery_decodeJson(const char *json, struct in_addr address, resultCallback_t callback);

#endif
