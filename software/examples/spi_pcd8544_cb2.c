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

static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

void get_ip_addr(int iface, char *buffer)
{
	int fd;
	struct ifreq ifr;

        fd = socket(AF_INET, SOCK_DGRAM, 0);
        /* IPv4 IP Address */
        ifr.ifr_addr.sa_family = AF_INET;
	if(iface == 0)
        	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	else
		strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
        ioctl(fd, SIOCGIFADDR, &ifr);

        close(fd);

        sprintf(buffer, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

double getCurrentValue(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow,
        &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
        totalSys < lastTotalSys || totalIdle < lastTotalIdle){
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else{
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
            (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

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
	
	FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &lastTotalUser, &lastTotalUserLow,
        &lastTotalSys, &lastTotalIdle);
    fclose(file);

	for(;;)
	{
		LCDclear();
		struct sysinfo sys_info;
		if(sysinfo(&sys_info) != 0)
		{
			printf("sysinfo-Error\n");
		}
		
		char hostname[1024];
        gethostname(hostname, 1024);

		char uptimeInfo[15];
		unsigned long uptime = sys_info.uptime / 60;
		sprintf(uptimeInfo, "Uptime %ld m", uptime);
        
        int coreCount = sysconf(_SC_NPROCESSORS_ONLN);
		char cpuInfo[10], cpuCount[2];
		unsigned long avgCpuLoad = getCurrentValue();
		sprintf(cpuInfo, "CPU %ld%%", avgCpuLoad);
        sprintf(cpuCount, "CPU cores %d", coreCount);
		char ramInfo[10];
		unsigned long freeRam = sys_info.freeram / 1024 / 1024;
		sprintf(ramInfo, "RAM %ld MB", freeRam);

		char ipInfoWlan[15];
		char ipInfoEth[15];

		if(one == 0)
		{
			get_ip_addr(0, ipInfoEth);
			get_ip_addr(1, ipInfoWlan);
			one++;
		}

		LCDdrawstring(0,0, hostname);
		LCDdrawline(0, 10, 83, 10, BLACK);
		LCDdrawstring(0,12, uptimeInfo);
		LCDdrawstring(0,20, cpuCount);
		LCDdrawstring(0,28, cpuInfo);
		LCDdrawstring(0,36, ramInfo);
		LCDdisplay(lw);
		delay(1000);
		/*
		LCDclear();
		LCDdrawstring(0,0, "WLAN0 :");
		LCDdrawstring(0,8, ipInfoWlan);
		LCDdrawstring(0,16, "ETH0 :");
                LCDdrawstring(0,24, ipInfoEth);
		LCDdisplay(lw);
        */
		if(lwStatus<0)
		{
			printf("> lwStatus: %d\n",lwStatus);
			printf("> Connection error!\n");
			return 0;
		}
		//delay(10000);
	}
	return 0;
}
