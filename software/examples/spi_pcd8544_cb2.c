#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "PCD8544.h"

unsigned char version;
littleWire *lw = NULL;

int contrast = 50;
int _dc = PIN3;
int _rst = PIN4;

int main()
{
	lw = littleWire_connect();
	if(lw == NULL)
	{
		printf("Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}
	version = readFirmwareVersion(lw);
	printf("Little Wire firmware version: %d.%d\n", ((version & 0xF0)>>4),(version & 0x0F));
	if(version==0x10)
	{
		printf("Requires the new 1.1 version firmware. Please update soon.\n");
		return 0;
	}

	spi_updateDelay(lw, 0);

	LCDInit(lw, _dc, _rst, contrast);
	LCDclear(lw);
	LCDdisplay(lw);

	LCDshowLogo(lw);
	delay(2000);
	LCDclear();
	LCDdisplay(lw);

	int one = 0;

	for(;;)
	{
		LCDclear();
		struct sysinfo sys_info;
		if(sysinfo(&sys_info) != 0)
		{
			printf("sysinfo-Error\n");
		}

		char uptimeInfo[15];
		unsigned long uptime = sys_info.uptime / 60;
		sprintf(uptimeInfo, "Uptime %ld m", uptime);

		char cpuInfo[10];
		unsigned long avgCpuLoad = ((sys_info.loads[0] / 1000)+(sys_info.loads[1] / 1000)/2);
		sprintf(cpuInfo, "CPU %ld%%", avgCpuLoad);

		char ramInfo[10];
		unsigned long totalRam = sys_info.freeram / 1024 / 1024;
		sprintf(ramInfo, "RAM %ld MB", totalRam);

		char ipInfo[15];

		if(one == 0)
		{
			int fd;
			struct ifreq ifr;

			fd = socket(AF_INET, SOCK_DGRAM, 0);
			/* IPv4 IP Address */
			ifr.ifr_addr.sa_family = AF_INET;

			strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
			ioctl(fd, SIOCGIFADDR, &ifr);

			close(fd);

			sprintf(ipInfo, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
			one++;
		}

		LCDdrawstring(0,0, "Cubieboard 2:");
		LCDdrawline(0, 10, 83, 10, BLACK);
		LCDdrawstring(0,12, uptimeInfo);
		LCDdrawstring(0,20, cpuInfo);
		LCDdrawstring(0,28, ramInfo);
		LCDdrawstring(0,36, ipInfo);
		LCDdisplay(lw);

		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
		delay(10000);
	}
	return 0;
}
