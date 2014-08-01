#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <valgrind/memcheck.h>
#include <paths.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "../os.h"
#include "../../base/public-key.h"

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <mntent.h>
#include <dirent.h>
#include <stdio.h>

static int ifname_position(char *ifnames, char * ifname, int ifnames_max) {
	int i, position;
	position = -1;
	for (i = 0; i < ifnames_max; i++) {
		if (strcmp(ifname, &ifnames[i * NI_MAXHOST]) == 0) {
			position = i;
			break;
		}
	}
	return position;

}

FUNCTION_RETURN getAdapterInfos(AdapterInfo * adapterInfos,
		size_t * adapter_info_size) {

	FUNCTION_RETURN f_return = OK;
	struct ifaddrs *ifaddr, *ifa;
	int family, i,  n, if_name_position;
	unsigned int if_num, if_max;
	//char host[NI_MAXHOST];
	char *ifnames;

	if (getifaddrs(&ifaddr) == -1) {
		perror("getifaddrs");
		return ERROR;
	}

	if (adapterInfos != NULL) {
		memset(adapterInfos, 0, (*adapter_info_size) * sizeof(AdapterInfo));
	}

	/* count the maximum number of interfaces */
	for (ifa = ifaddr, if_max = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		if_max++;
	}

	/* allocate space for names */
	ifnames = (char*) malloc(NI_MAXHOST * if_max);
	memset(ifnames, 0, NI_MAXHOST * if_max);
	/* Walk through linked list, maintaining head pointer so we
	 can free list later */
	for (ifa = ifaddr, n = 0, if_num = 0; ifa != NULL;
			ifa = ifa->ifa_next, n++) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		if_name_position = ifname_position(ifnames, ifa->ifa_name, if_num);
		//interface name not seen en advance
		if (if_name_position < 0) {
			strncpy(&ifnames[if_num * NI_MAXHOST], ifa->ifa_name, NI_MAXHOST);
			if (adapterInfos != NULL && if_num < *adapter_info_size) {
				strncpy(adapterInfos[if_num].description, ifa->ifa_name,
				NI_MAXHOST);
			}
			if_name_position = if_num;
			if_num++;
			if (adapterInfos == NULL) {
				continue;
			}
		}
		family = ifa->ifa_addr->sa_family;
		/* Display interface name and family (including symbolic
		 form of the latter for the common families) */
#ifdef _DEBUG
		printf("%-8s %s (%d)\n", ifa->ifa_name,
				(family == AF_PACKET) ? "AF_PACKET" :
				(family == AF_INET) ? "AF_INET" :
				(family == AF_INET6) ? "AF_INET6" : "???", family);
#endif
		/* For an AF_INET* interface address, display the address
		 * || family == AF_INET6*/
		if (family == AF_INET) {
			/*
			 s = getnameinfo(ifa->ifa_addr,
			 (family == AF_INET) ?
			 sizeof(struct sockaddr_in) :
			 sizeof(struct sockaddr_in6), host, NI_MAXHOST,
			 NULL, 0, NI_NUMERICHOST);
			 */
#ifdef _DEBUG
			s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host,
					NI_MAXHOST,
					NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
			}
			printf("\t\taddress: <%s>\n", host);
#endif

			if (adapterInfos != NULL && if_name_position < *adapter_info_size) {
				struct sockaddr_in *s1 = (struct sockaddr_in*) ifa->ifa_addr;
				in_addr_t iaddr = s1->sin_addr.s_addr;
				adapterInfos[if_name_position].ipv4_address[0] = (iaddr
						& 0x000000ff);
				adapterInfos[if_name_position].ipv4_address[1] = (iaddr
						& 0x0000ff00) >> 8;
				adapterInfos[if_name_position].ipv4_address[2] = (iaddr
						& 0x00ff0000) >> 16;
				adapterInfos[if_name_position].ipv4_address[3] = (iaddr
						& 0xff000000) >> 24;
			}
		} else if (family == AF_PACKET && ifa->ifa_data != NULL) {
			struct sockaddr_ll *s1 = (struct sockaddr_ll*) ifa->ifa_addr;
			if (adapterInfos != NULL && if_name_position < *adapter_info_size) {
				for (i = 0; i < 6; i++) {
					adapterInfos[if_name_position].mac_address[i] =
							s1->sll_addr[i];
#ifdef _DEBUG
					printf("%02x:", s1->sll_addr[i]);
#endif
				}
#ifdef _DEBUG
				printf("\t %s\n", ifa->ifa_name);
#endif

			}
		}
	}

	*adapter_info_size = if_num;
	if (adapterInfos == NULL) {
		f_return = OK;
	} else if (*adapter_info_size < if_num) {
		f_return = BUFFER_TOO_SMALL;
	}
	freeifaddrs(ifaddr);
	free(ifnames);
	return f_return;
}
/**
 *Usually uuid are hex number separated by "-". this method read up to 8 hex
 *numbers skipping - characters.
 *@param uuid uuid as read in /dev/disk/by-uuid
 *@param buffer_out: unsigned char buffer[8] output buffer for result
 */
static void parseUUID(const char *uuid, unsigned char* buffer_out,
		unsigned int out_size) {
	size_t len;
	unsigned int i, j;
	char * hexuuid;
	unsigned char cur_character;
	//remove characters not in hex set
	len = strlen(uuid);
	hexuuid = (char *) malloc(sizeof(char) * strlen(uuid));
	memset(buffer_out, 0, out_size);
	memset(hexuuid, 0, sizeof(char) * strlen(uuid));

	for (i = 0, j = 0; i < len; i++) {
		if (isxdigit(uuid[i])) {
			hexuuid[j] = uuid[i];
			j++;
		} else {
			//skip
			continue;
		}
	}
	if (j % 2 == 1) {
		hexuuid[j++] = '0';
	}
	hexuuid[j] = '\0';
	for (i = 0; i < j / 2; i++) {
		sscanf(&hexuuid[i * 2], "%2hhx", &cur_character);
		buffer_out[i % out_size] = buffer_out[i % out_size] ^ cur_character;
	}

	free(hexuuid);
}

#define MAX_UNITS 20
FUNCTION_RETURN getDiskInfos(DiskInfo * diskInfos, size_t * disk_info_size) {
	struct stat mount_stat, sym_stat;
	/*static char discard[1024];
	 char device[64], name[64], type[64];
	 */
	char cur_dir[MAX_PATH];
	struct mntent *ent;

	int maxDrives, currentDrive, i, drive_found;
	__ino64_t *statDrives;
	DiskInfo *tmpDrives;
	FILE *aFile;
	DIR *disk_by_uuid_dir, *disk_by_label;
	struct dirent *dir;
	FUNCTION_RETURN result;

	if (diskInfos != NULL) {
		maxDrives = *disk_info_size;
		tmpDrives = diskInfos;
	} else {
		maxDrives = MAX_UNITS;
		tmpDrives = (DiskInfo *) malloc(sizeof(DiskInfo) * maxDrives);
	}
	memset(tmpDrives, 0, sizeof(DiskInfo) * maxDrives);
	statDrives = (__ino64_t *) malloc(maxDrives * sizeof(__ino64_t ));
	memset(statDrives, 0, sizeof(__ino64_t ) * maxDrives);
	;

	aFile = setmntent("/proc/mounts", "r");
	if (aFile == NULL) {
		/*proc not mounted*/
		return ERROR;
	}

	currentDrive = 0;
	while (NULL != (ent = getmntent(aFile))) {
		if ((strncmp(ent->mnt_type, "ext", 3) == 0
				|| strncmp(ent->mnt_type, "vfat", 4) == 0
				|| strncmp(ent->mnt_type, "ntfs", 4) == 0)
				&& ent->mnt_fsname != NULL
				&& strncmp(ent->mnt_fsname, "/dev/", 5) == 0) {
			if (stat(ent->mnt_fsname, &mount_stat) == 0) {
				drive_found = -1;
				for (i = 0; i < currentDrive; i++) {
					if (statDrives[i] == mount_stat.st_ino) {
						drive_found = i;
					}
				}
				if (drive_found == -1) {
#ifdef _DEBUG
					printf("mntent: %s %s %d\n", ent->mnt_fsname, ent->mnt_dir,
							mount_stat.st_ino);
#endif
					strcpy(tmpDrives[currentDrive].device, ent->mnt_fsname);
					statDrives[currentDrive] = mount_stat.st_ino;
					drive_found = currentDrive;
					currentDrive++;
				}
				if (strcmp(ent->mnt_dir, "/") == 0) {
					strcpy(tmpDrives[drive_found].label, "root");
#ifdef _DEBUG
					printf("drive %s set to preferred\n", ent->mnt_fsname);
#endif
					tmpDrives[drive_found].preferred = true;
				}
			}
		}
	}
	endmntent(aFile);

	if (diskInfos == NULL) {
		*disk_info_size = currentDrive;
		free(tmpDrives);
		result = OK;
	} else if (*disk_info_size >= currentDrive) {
		disk_by_uuid_dir = opendir("/dev/disk/by-uuid");
		if (disk_by_uuid_dir == NULL) {
#ifdef _DEBUG
			printf("Open /dev/disk/by-uuid fail");
#endif
			free(statDrives);
			return ERROR;
		}
		result = OK;
		*disk_info_size = currentDrive;
		while ((dir = readdir(disk_by_uuid_dir)) != NULL) {
			strcpy(cur_dir, "/dev/disk/by-uuid/");
			strncat(cur_dir, dir->d_name, 200);
			if (stat(cur_dir, &sym_stat) == 0) {
				for (i = 0; i < currentDrive; i++) {
					if (sym_stat.st_ino == statDrives[i]) {
						parseUUID(dir->d_name, tmpDrives[i].disk_sn,
								sizeof(tmpDrives[i].disk_sn));
#ifdef _DEBUG
						VALGRIND_CHECK_VALUE_IS_DEFINED(tmpDrives[i].device);

						printf("uuid %d %s %02x%02x%02x%02x\n", i,
								tmpDrives[i].device,
								tmpDrives[i].disk_sn[0],
								tmpDrives[i].disk_sn[1],
								tmpDrives[i].disk_sn[2],
								tmpDrives[i].disk_sn[3]);
#endif
					}
				}
			}
		}
		closedir(disk_by_uuid_dir);

		disk_by_label = opendir("/dev/disk/by-label");
		if (disk_by_label != NULL) {
			while ((dir = readdir(disk_by_label)) != NULL) {
				strcpy(cur_dir, "/dev/disk/by-label/");
				strcat(cur_dir, dir->d_name);
				if (stat(cur_dir, &sym_stat) == 0) {
					for (i = 0; i < currentDrive; i++) {
						if (sym_stat.st_ino == statDrives[i]) {
							strncpy(tmpDrives[i].label, dir->d_name, 255);
							printf("label %d %s %s\n", i, tmpDrives[i].label,
									tmpDrives[i].device);
						}
					}
				}
			}
			closedir(disk_by_label);
		}
	} else {
		result = BUFFER_TOO_SMALL;
	}
	/*
	 FILE *mounts = fopen(_PATH_MOUNTED, "r");
	 if (mounts == NULL) {
	 return ERROR;
	 }

	 while (fscanf(mounts, "%64s %64s %64s %1024[^\n]", device, name, type,
	 discard) != EOF) {
	 if (stat(device, &mount_stat) != 0)
	 continue;
	 if (filename_stat.st_dev == mount_stat.st_rdev) {
	 fprintf(stderr, "device: %s; name: %s; type: %s\n", device, name,
	 type);
	 }
	 }
	 */
	free(statDrives);
	return result;
}

void os_initialize() {
	static int initialized = 0;
	if (initialized == 0) {
		initialized = 1;
		ERR_load_ERR_strings();
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
	}
}

static void _getCpuid(unsigned int* p, unsigned int ax) {
	__asm __volatile
	( "movl %%ebx, %%esi\n\t"
			"cpuid\n\t"
			"xchgl %%ebx, %%esi"
			: "=a" (p[0]), "=S" (p[1]),
			"=c" (p[2]), "=d" (p[3])
			: "0" (ax)
	);
}

FUNCTION_RETURN getCpuId(unsigned char identifier[6]) {
	unsigned int i;
	unsigned int cpuinfo[4] = { 0, 0, 0, 0 };
	_getCpuid(cpuinfo, 0);
	for (i = 0; i < 3; i++) {
		identifier[i * 2] = cpuinfo[i] & 0xFF;
		identifier[i * 2 + 1] = (cpuinfo[i] & 0xFF00) >> 8;
	}
	return OK;
}

VIRTUALIZATION getVirtualization() {
//http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
//
//bool rc = true;
	/*__asm__ (
	 "push   %edx\n"
	 "push   %ecx\n"
	 "push   %ebx\n"
	 "mov    %eax, 'VMXh'\n"
	 "mov    %ebx, 0\n" // any value but not the MAGIC VALUE
	 "mov    %ecx, 10\n"// get VMWare version
	 "mov    %edx, 'VX'\n"// port number
	 "in     %eax, dx\n"// read port on return EAX returns the VERSION
	 "cmp    %ebx, 'VMXh'\n"// is it a reply from VMWare?
	 "setz   [rc] \n"// set return value
	 "pop    %ebx \n"
	 "pop    %ecx \n"
	 "pop    %edx \n"
	 );*/

	return NONE;
}

