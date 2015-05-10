/*
 * ip.c - ipv4 iterator
 *
 * Author: kXuan <xuanmgr@gmail.com>
 * License: MIT
 */
#include<stdio.h>
#include<stdint.h>
#include "ip.h"
typedef struct {
	uint8_t addr[4];
	uint8_t mask;
}addr_range;

static addr_range reserved_addr[] ={
{{0,0,0,0},8},
{{10,0,0,0},8},
{{100,64,0,0},10},
{{127,0,0,0},8},
{{169,254,0,0},16},
{{172,16,0,0},12},
{{192,0,0,0},24},
{{192,0,2,0},24},
{{192,88,99,0},24},
{{192,168,0,0},16},
{{198,18,0,0},15},
{{198,51,100,0},24},
{{203,0,113,0},24},
{{224,0,0,0},4},
{{240,0,0,0},4},
{{255,0,0,0},8},
{}
};

void inline print_ip(uint8_t *ipv)
{
	printf("%u.%u.%u.%u", (unsigned)ipv[3], (unsigned)ipv[2], (unsigned)ipv[1], (unsigned)ipv[0]);
}
void iplist_run(void(*op)(uint32_t)) {
	uint32_t rin, ipv[4]; //
	uint32_t ip = 0;
	uint8_t *ip_hi = ((uint8_t *)&ip) + 3;
	addr_range *rsv_ip = reserved_addr;
	
	rin = vton(rsv_ip->addr);
	do {
		if (ip == rin) {
			ip = (rin & ~((1 << (32 - rsv_ip->mask + 1)) - 1)) + (1 << (32 - rsv_ip->mask));
			++rsv_ip;
			rin = vton(rsv_ip->addr);
		} else {
			op(ip);
			ip += 0x100;
		}
	} while(*ip_hi != 224);
}
