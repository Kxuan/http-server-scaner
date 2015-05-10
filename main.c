/*
 * main.c - scaner's main file. Output controller
 *
 * Author: kXuan <xuanmgr@gmail.com>
 * License: MIT
 */
#include<stdio.h>
#include "ip.h"
#include "scaner.h"

char buffer[4096];
void scan_subnet(uint32_t ipstart)
{
	int ret;
	uint32_t ip;
	for (ip = ipstart + 1; ip <= ipstart + 255; ++ip)
	{
		ret = get_server(ipstart, buffer, sizeof(buffer));
		if (ret > 0) {
			print_ip((uint8_t *)&ip);
			write(1, "\n", 1);
			write(1, buffer, ret);
			write(1, "\n", 1);
		}
	}
}

int main(int argc, char **argv)
{
	iplist_run(scan_subnet);
	return 0;
}
