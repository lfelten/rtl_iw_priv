// SPDX-License-Identifier: GPL-3.0-only
/* Copyright(c) 2021 lothar.felten@gmail.com
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <linux/if.h>
#include <linux/sockios.h>

int do_ioctl(
		int sock,
		char *ifname,
		void *pBuffer,
		unsigned int length)
{
	int err = 0;
	struct ifreq rq;
	struct iw_point iwp;
	
	memset(&rq, 0, sizeof(struct ifreq));	
	strncpy(rq.ifr_ifrn.ifrn_name, ifname, strlen(ifname));
	rq.ifr_ifru.ifru_data = &iwp;

	iwp.pointer = pBuffer;
	iwp.length = (unsigned short)length;
	iwp.flags = 0;
	
	err = ioctl(sock, SIOCDEVPRIVATE, &rq);

	return err;
}

void usage()
{
	printf("\nusage:\n");
	printf("\nrtl_iw_priv interface command [parmeter] \n");
	printf("	interface: wireless interface to use, must be up\n");
	printf("	command: command to send (string)\n");
	printf("	[paramter]: optinal command parameter\n");
	printf("\nexamples:\n");
	printf("rtl_iw_priv wlan0 mp_start\n");
	printf("rtl_iw_priv wlan0 mp_txpower patha=44,pathb=44\n");
	printf("rtl_iw_priv wlan0 mp_stop\n");
	printf("\nfor an exhaustive list of supported commands check the rtlxxxx driver source code.\n");
}

int main(int argc, char **argv)
{
	char *ifname;
	char buf[400];
	int fd_sock;
	int err = 0;


	if (argc < 3){
		usage();
		return -1;
	}
	
	ifname = argv[1];
	printf(" interface: %s\n",ifname);
	int len = sprintf(buf, "%s", argv[2]);
	printf(" command: %s\n",buf);
	if(argc>3){
		printf(" parameter: %s\n",argv[3]);
		sprintf(buf+len, " %s", argv[3]);
	}

	fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd_sock < 0){
		fprintf(stderr, "error opening socket (AF_INET)\n");
		return -1;
	}

	err = do_ioctl(fd_sock, ifname, buf, strlen(buf)+1);

	close(fd_sock);

	if (err < 0) {
		fprintf(stderr, "interface %s iocontrol error %i (%s)\n",ifname,err,strerror(errno));
	} else {
		if(strlen(buf)>0)printf("%s %s: %s\n", ifname, argv[2], buf);
	}

	return err;
}

