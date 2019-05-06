#include <paths.h>
#include <sys/stat.h>
#include "os.h"
#include "../base/logger.h"

//#include <openssl/evp.h>
//#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <mntent.h>
#include <dirent.h>

#include <sys/utsname.h>
#ifdef USE_DBUS
#include <dbus-1.0/dbus/dbus.h>
#endif

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
	__ino64_t *statDrives = NULL;
	DiskInfo *tmpDrives = NULL;
	FILE *aFile = NULL;
	DIR *disk_by_uuid_dir = NULL, *disk_by_label = NULL;
	struct dirent *dir = NULL;
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
		free(tmpDrives);
		free(statDrives);
		return FUNC_RET_ERROR;
	}

	currentDrive = 0;
	while (NULL != (ent = getmntent(aFile))) {
		if ((strncmp(ent->mnt_type, "ext", 3) == 0
				|| strncmp(ent->mnt_type, "xfs", 3) == 0
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
					LOG_DEBUG("mntent: %s %s %d\n", ent->mnt_fsname, ent->mnt_dir,
							(unsigned long int)mount_stat.st_ino);
					strncpy(tmpDrives[currentDrive].device, ent->mnt_fsname, 255-1);
					statDrives[currentDrive] = mount_stat.st_ino;
					drive_found = currentDrive;
					currentDrive++;
				}
				if (strcmp(ent->mnt_dir, "/") == 0) {
					strcpy(tmpDrives[drive_found].label, "root");
					LOG_DEBUG("drive %s set to preferred\n", ent->mnt_fsname);
					tmpDrives[drive_found].preferred = true;
				}
			}
		}
	}
	endmntent(aFile);

	if (diskInfos == NULL) {
		*disk_info_size = currentDrive;
		free(tmpDrives);
		result = FUNC_RET_OK;
	} else if (*disk_info_size >= currentDrive) {
		disk_by_uuid_dir = opendir("/dev/disk/by-uuid");
		if (disk_by_uuid_dir == NULL) {
			LOG_WARN("Open /dev/disk/by-uuid fail");
			free(statDrives);
			return FUNC_RET_ERROR;
		}
		result = FUNC_RET_OK;
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

						LOG_DEBUG("uuid %d %s %02x%02x%02x%02x\n", i,
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
							strncpy(tmpDrives[i].label, dir->d_name, 255-1);
							printf("label %d %s %s\n", i, tmpDrives[i].label,
									tmpDrives[i].device);
						}
					}
				}
			}
			closedir(disk_by_label);
		}
	} else {
		result = FUNC_RET_BUFFER_TOO_SMALL;
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
	return FUNC_RET_OK;
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

FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	static struct utsname u;

	if (uname(&u) < 0) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, u.nodename, 6);
	return FUNC_RET_OK;
}

FUNCTION_RETURN getOsSpecificIdentifier(unsigned char identifier[6]) {
#if USE_DBUS
	char* dbus_id = dbus_get_local_machine_id();
	if (dbus_id == NULL) {
		return FUNC_RET_ERROR;
	}
	memcpy(identifier, dbus_id, 6);
	dbus_free(dbus_id);
	return FUNC_RET_OK;
#else
	return FUNC_RET_NOT_AVAIL;
#endif
}

FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
	FUNCTION_RETURN result;
	char path[MAX_PATH] = { 0 };
	char proc_path[MAX_PATH], pidStr[64];
	pid_t pid = getpid();
	sprintf(pidStr, "%d", pid);
	strcpy(proc_path, "/proc/");
	strcat(proc_path, pidStr);
	strcat(proc_path, "/exe");

	int ch = readlink(proc_path, path, MAX_PATH-1);
	if (ch != -1) {
		path[ch] = '\0';
		strncpy(buffer, path, ch);
		result = FUNC_RET_OK;
	} else {
		result = FUNC_RET_ERROR;
	}
	return result;
}
