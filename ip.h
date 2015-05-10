#pragma once
#include <stdint.h>
static inline uint32_t vton(uint8_t *addr)
{
	return 	(addr[0] << (3 * 8)) |
			(addr[1] << (2 * 8)) |
			(addr[2] << (1 * 8)) |
			(addr[3] << (0 * 8));
}
static inline void ntov(uint32_t ip, uint8_t *vector)
{
	vector[0] = (ip >> (3 * 8)) & 0xff;
	vector[1] = (ip >> (2 * 8)) & 0xff;
	vector[2] = (ip >> (1 * 8)) & 0xff;
	vector[3] = (ip >> (0 * 8)) & 0xff;
}

extern void inline print_ip(uint8_t *ipv);
extern void iplist_run(void(*op)(uint32_t));
