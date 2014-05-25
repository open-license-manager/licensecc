/*
 * pc-identifiers.c
 *
 *  Created on: Apr 16, 2014
 *      Author: devel
 */

#include "os/os.h"
#include "pc-identifiers.h"
#include <stdlib.h>
#include <stdbool.h>
#include "base/base64.h"

static FUNCTION_RETURN generate_default_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers) {
	size_t adapter_num, disk_num;
	FUNCTION_RETURN result_adapterInfos, result_diskinfos;
	unsigned int required_id_size, i, j, k;
	DiskInfo * diskInfos;
	AdapterInfo *adapterInfos;

	result_adapterInfos = getAdapterInfos(NULL, &adapter_num);
	if (result_adapterInfos != OK) {
		//call generate_disk_pc_id;
		return result_adapterInfos;
	}
	result_diskinfos = getDiskInfos(NULL, &disk_num);
	if (result_diskinfos == OK) {
		required_id_size = disk_num * adapter_num;
	} else {
		required_id_size = disk_num;
	}
	if (identifiers == NULL) {
		*num_identifiers = required_id_size;
		return OK;
	} else if (*num_identifiers < required_id_size) {
		*num_identifiers = required_id_size;
		return BUFFER_TOO_SMALL;
	}
	diskInfos = (DiskInfo*) malloc(disk_num * sizeof(DiskInfo));
	result_diskinfos = getDiskInfos(diskInfos, &disk_num);
	adapterInfos = (AdapterInfo*) malloc(adapter_num * sizeof(AdapterInfo));
	result_adapterInfos = getAdapterInfos(adapterInfos, &adapter_num);
	for (i = 0; i < disk_num; i++) {
		for (j = 0; j < adapter_num; i++) {
			for (k = 0; k < 6; k++)
				identifiers[i * adapter_num + j][k] =
						diskInfos[i].disk_sn[k + 2]
								^ adapterInfos[j].mac_address[k + 2];
		}
	}

	free(diskInfos);
	free(adapterInfos);
	return OK;
}

static FUNCTION_RETURN generate_ethernet_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, bool use_mac) {
	size_t adapters;
	FUNCTION_RETURN result_adapterInfos;
	unsigned int i, j, k;
	AdapterInfo *adapterInfos;

	result_adapterInfos = getAdapterInfos(NULL, &adapters);
	if (result_adapterInfos != OK) {
		return result_adapterInfos;
	}
	if (identifiers == NULL) {
		*num_identifiers = adapters;
		return OK;
	} else if (*num_identifiers < adapters) {
		*num_identifiers = adapters;
		return BUFFER_TOO_SMALL;
	}
	adapterInfos = (AdapterInfo*) malloc(adapters * sizeof(AdapterInfo));
	result_adapterInfos = getAdapterInfos(adapterInfos, &adapters);
	for (j = 0; j < adapters; i++) {
		for (k = 0; k < 6; k++)
			if (use_mac) {
				identifiers[j][k] = adapterInfos[j].mac_address[k + 2];
			} else {
				//use ip
				if (k < 4) {
					identifiers[j][k] = adapterInfos[j].ipv4_address[k];
				} else {
					//padding
					identifiers[j][k] = 42;
				}
			}
	}
	free(adapterInfos);
	return OK;
}

static FUNCTION_RETURN generate_disk_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, bool use_label) {
	size_t disk_num;
	FUNCTION_RETURN result_diskinfos;
	unsigned int required_id_size, i, k;
	DiskInfo * diskInfos;

	result_diskinfos = getDiskInfos(NULL, &disk_num);
	if (result_diskinfos != OK) {
		return result_diskinfos;
	}
	if (identifiers == NULL) {
		*num_identifiers = disk_num;
		return OK;
	} else if (*num_identifiers < disk_num) {
		*num_identifiers = disk_num;
		return BUFFER_TOO_SMALL;
	}
	diskInfos = (DiskInfo*) malloc(disk_num * sizeof(DiskInfo));
	result_diskinfos = getDiskInfos(diskInfos, &disk_num);

	for (i = 0; i < disk_num; i++) {
		for (k = 0; k < 6; k++) {
			if (use_label) {
				identifiers[i][k] = diskInfos[i].label[k];
			} else {
				identifiers[i][k] = diskInfos[i].disk_sn[k + 2];
			}
		}
	}
	free(diskInfos);
	return OK;
}

/**
 *
 * Calculates all the possible identifiers for the current machine, for the
 * given calculation strategy requested. Pc identifiers are more than one,
 * for instance a machine with more than one disk and one network interface has
 * usually multiple identifiers.
 *
 * @param identifiers
 * @param array_size
 * @param
 * @return
 */
FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers,
		unsigned int * array_size, IDENTIFICATION_STRATEGY strategy) {
	FUNCTION_RETURN result;
	unsigned int i;
	unsigned char strategy_num;
	switch (strategy) {
	case DEFAULT:
		result = generate_default_pc_id(identifiers, array_size);
		break;
	case ETHERNET:
		result = generate_ethernet_pc_id(identifiers, array_size, true);
		break;
	case IP_ADDRESS:
		result = generate_ethernet_pc_id(identifiers, array_size, false);
		break;
	case DISK_NUM:
		result = generate_disk_pc_id(identifiers, array_size, false);
		break;
	case DISK_LABEL:
		result = generate_disk_pc_id(identifiers, array_size, true);
		break;
	default:
		return ERROR;
	}

	if (result == OK && identifiers != NULL) {
		strategy_num = strategy << 5;
		for (i = 0; i < *array_size; i++) {
			//encode strategy in the first three bits of the pc_identifier
			identifiers[i][0] = (identifiers[i][0] & 31) | strategy_num;
		}
	}
	return result;
}

FUNCTION_RETURN encode_pc_id(PcIdentifier identifier1, PcIdentifier identifier2,
		UserPcIdentifier* pc_identifier_out) {

}

FUNCTION_RETURN parity_check_id(UserPcIdentifier pc_identifier) {

}

/**
 * Extract the two pc identifiers from the user provided code.
 * @param identifier1_out
 * @param identifier2_out
 * @param str_code: the code in the string format XXXX-XXXX-XXXX-XXXX
 * @return
 */
static FUNCTION_RETURN decode_pc_id(PcIdentifier* identifier1_out,
		PcIdentifier* identifier2_out, UserPcIdentifier str_code) {

}

static IDENTIFICATION_STRATEGY strategy_from_pc_id(PcIdentifier identifier) {
	return (IDENTIFICATION_STRATEGY) identifier[0] >> 5;
}

EVENT_TYPE validate_user_pc_identifier(UserPcIdentifier str_code) {
	PcIdentifier user_identifiers[2];
	FUNCTION_RETURN result;
	IDENTIFICATION_STRATEGY previous_strategy_id, current_strategy_id;
	PcIdentifier* calculated_identifiers;
	size_t calc_identifiers_size;
	int i, j;
	bool found;

	result = decode_pc_id(&user_identifiers[0], &user_identifiers[1], str_code);
	if (result != OK) {
		return result;
	}
	previous_strategy_id = STRATEGY_UNKNOWN;
	found = false;
	for (i = 0; i < 2; i++) {
		current_strategy_id = strategy_from_pc_id(user_identifiers[i]);
		if (current_strategy_id == STRATEGY_UNKNOWN) {
			return LICENSE_MALFORMED;
		}
		if (current_strategy_id != previous_strategy_id) {
			if (calculated_identifiers != NULL) {
				free(calculated_identifiers);
			}
			current_strategy_id = previous_strategy_id;
			generate_pc_id(NULL, &calc_identifiers_size, current_strategy_id);
			calculated_identifiers = (PcIdentifier *) malloc(
					sizeof(PcIdentifier) * calc_identifiers_size);
			generate_pc_id(calculated_identifiers, &calc_identifiers_size,
					current_strategy_id);
		}
		//maybe skip the byte 0
		for (j = 0; j < calc_identifiers_size; j++) {
			if (!memcmp(user_identifiers[i], calculated_identifiers[j],
					sizeof(PcIdentifier))) {
				free(calculated_identifiers);
				return LICENSE_OK;
			}
		}
	}
	free(calculated_identifiers);
	return IDENTIFIERS_MISMATCH;
}
