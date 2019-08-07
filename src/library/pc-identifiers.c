/*
 * pc-identifiers.c
 *
 *  Created on: Apr 16, 2014
 *
 */

#include "os/os.h"
#include "pc-identifiers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "base/base64.h"
#include "base/base.h"
#ifdef __linux__
#include <stdbool.h>
#include <valgrind/memcheck.h>
#else
#ifdef __MINGW32__
#include <windows.h>
#else
#include <Windows.h>
#endif
#endif

static FUNCTION_RETURN generate_disk_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, bool use_label);

static FUNCTION_RETURN generate_ethernet_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, int use_mac);

static FUNCTION_RETURN generate_default_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers) {
	size_t adapter_num, disk_num;
	FUNCTION_RETURN result_adapterInfos, result_diskinfos, function_return;
	unsigned int caller_identifiers, i, j, k, array_index;
	DiskInfo * diskInfoPtr;
	OsAdapterInfo *adapterInfoPtr;

	if (identifiers == NULL || *num_identifiers == 0) {
		result_adapterInfos = getAdapterInfos(NULL, &adapter_num);
		if ((result_adapterInfos != FUNC_RET_OK) || (adapter_num == 0)) {
			return generate_disk_pc_id(identifiers, num_identifiers, false);
		}
		result_diskinfos = getDiskInfos(NULL, &disk_num);
		if ((result_diskinfos != FUNC_RET_OK) || (disk_num == 0)) {
			return generate_ethernet_pc_id(identifiers, num_identifiers, true);
		}
		*num_identifiers = disk_num * adapter_num;
		function_return = FUNC_RET_OK;
	} else {
		adapterInfoPtr = (OsAdapterInfo*) malloc(
				(*num_identifiers) * sizeof(OsAdapterInfo));
		adapter_num = *num_identifiers;
		result_adapterInfos = getAdapterInfos(adapterInfoPtr, &adapter_num);
		if (result_adapterInfos != FUNC_RET_OK
				&& result_adapterInfos != FUNC_RET_BUFFER_TOO_SMALL) {
			free(adapterInfoPtr);
			return generate_disk_pc_id(identifiers, num_identifiers, false);
		}
		diskInfoPtr = (DiskInfo*) malloc((*num_identifiers) * sizeof(DiskInfo));
		disk_num = *num_identifiers;
		result_diskinfos = getDiskInfos(diskInfoPtr, &disk_num);
		if (result_diskinfos != FUNC_RET_OK
				&& result_diskinfos != FUNC_RET_BUFFER_TOO_SMALL) {
			free(diskInfoPtr);
			free(adapterInfoPtr);
			return generate_ethernet_pc_id(identifiers, num_identifiers, true);
		}
		function_return = FUNC_RET_OK;

		caller_identifiers = *num_identifiers;
		for (i = 0; i < disk_num; i++) {
			for (j = 0; j < adapter_num; j++) {
				array_index = i * adapter_num + j;
				if (array_index >= caller_identifiers) {
					function_return = FUNC_RET_BUFFER_TOO_SMALL;
					//sweet memories...
					goto end;
				}
				for (k = 0; k < 6; k++)
					identifiers[array_index][k] = diskInfoPtr[i].disk_sn[k + 2]
							^ adapterInfoPtr[j].mac_address[k + 2];
			}
		}
end:
#ifdef _MSC_VER
        *num_identifiers = min(*num_identifiers, adapter_num * disk_num);
#else
        *num_identifiers = cmin(*num_identifiers, adapter_num * disk_num);
#endif
		free(diskInfoPtr);
		free(adapterInfoPtr);
	}
	return function_return;
}

static FUNCTION_RETURN generate_ethernet_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, int use_mac) {
	FUNCTION_RETURN result_adapterInfos;
	unsigned int j, k;
	OsAdapterInfo *adapterInfos;
	size_t defined_adapters, adapters = 0;

	if (identifiers == NULL || *num_identifiers == 0) {
		result_adapterInfos = getAdapterInfos(NULL, &adapters);
		if (result_adapterInfos == FUNC_RET_OK
				|| result_adapterInfos == FUNC_RET_BUFFER_TOO_SMALL) {
			*num_identifiers = adapters;
			result_adapterInfos = FUNC_RET_OK;
		}
	} else {
		defined_adapters = adapters = *num_identifiers;
		adapterInfos = (OsAdapterInfo*) malloc(
				adapters * sizeof(OsAdapterInfo));
		result_adapterInfos = getAdapterInfos(adapterInfos, &adapters);
		if (result_adapterInfos == FUNC_RET_BUFFER_TOO_SMALL
				|| result_adapterInfos == FUNC_RET_OK) {
			for (j = 0; j < adapters; j++) {
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
			result_adapterInfos = (
					adapters > defined_adapters ?
							FUNC_RET_BUFFER_TOO_SMALL : FUNC_RET_OK);
		}
		free(adapterInfos);
	}
	return result_adapterInfos;
}

static FUNCTION_RETURN generate_disk_pc_id(PcIdentifier * identifiers,
		unsigned int * num_identifiers, bool use_label) {
	size_t disk_num, available_disk_info = 0;
	FUNCTION_RETURN result_diskinfos;
	unsigned int i, j;
	int defined_identifiers;
	char firstChar;
	DiskInfo * diskInfos;

	result_diskinfos = getDiskInfos(NULL, &disk_num);
	if (result_diskinfos != FUNC_RET_OK) {
		return result_diskinfos;
	}
	diskInfos = (DiskInfo*) malloc(disk_num * sizeof(DiskInfo));
	memset(diskInfos,0,disk_num * sizeof(DiskInfo));
	result_diskinfos = getDiskInfos(diskInfos, &disk_num);
	if (result_diskinfos != FUNC_RET_OK) {
		free(diskInfos);
		return result_diskinfos;
	}
	for (i = 0; i < disk_num; i++) {
		firstChar = use_label ? diskInfos[i].label[0] : diskInfos[i].disk_sn[0];
		available_disk_info += firstChar == 0 ? 0 : 1;
	}

	defined_identifiers = *num_identifiers;
	*num_identifiers = available_disk_info;
	if (identifiers == NULL) {
		free(diskInfos);
		return FUNC_RET_OK;
	} else if (available_disk_info > defined_identifiers) {
		free(diskInfos);
		return FUNC_RET_BUFFER_TOO_SMALL;
	}

	j = 0;
	for (i = 0; i < disk_num; i++) {
		if (use_label) {
			if (diskInfos[i].label[0] != 0) {
				memset(identifiers[j], 0, sizeof(PcIdentifier)); //!!!!!!!
				strncpy((char*)identifiers[j], diskInfos[i].label,
						sizeof(PcIdentifier));
				j++;
			}
		} else {
			if (diskInfos[i].disk_sn[0] != 0) {
				memcpy(identifiers[j], &diskInfos[i].disk_sn[2],
						sizeof(PcIdentifier));
				j++;
			}
		}
	}
	free(diskInfos);
	return FUNC_RET_OK;
}

/**
 *
 * Calculates all the possible identifiers for the current machine, for the
 * given calculation strategy requested. Pc identifiers are more than one,
 * for instance a machine with more than one disk and one network interface has
 * usually multiple identifiers.
 *
 * First 4 bit of each pc identifier are reserved 3 for the type of strategy
 * used in calculation and 1 for parity checks (not implemented here)
 *
 * @param identifiers
 * @param array_size
 * @param
 * @return
 */

FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers,
		unsigned int * array_size, IDENTIFICATION_STRATEGY strategy) {
	FUNCTION_RETURN result;
	unsigned int i, j;
	const unsigned int original_array_size = *array_size;
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
		return FUNC_RET_ERROR;
	}

	if (result == FUNC_RET_OK && identifiers != NULL) {
		strategy_num = strategy << 5;
		for (i = 0; i < *array_size; i++) {
			//encode strategy in the first three bits of the pc_identifier
			identifiers[i][0] = (identifiers[i][0] & 15) | strategy_num;
		}
		//fill array if larger
		for (i = *array_size; i < original_array_size; i++) {
			identifiers[i][0] = STRATEGY_UNKNOWN << 5;
			for (j = 1; j < sizeof(PcIdentifier); j++) {
				identifiers[i][j] = 42; //padding
			}
		}
	}
	return result;
}

char *MakeCRC(char *BitString) {
	static char Res[3];                                 // CRC Result
	char CRC[2];
	int i;
	char DoInvert;

	for (i = 0; i < 2; ++i)
		CRC[i] = 0;                    // Init before calculation

	for (i = 0; i < strlen(BitString); ++i) {
		DoInvert = ('1' == BitString[i]) ^ CRC[1];         // XOR required?

		CRC[1] = CRC[0];
		CRC[0] = DoInvert;
	}

	for (i = 0; i < 2; ++i)
		Res[1 - i] = CRC[i] ? '1' : '0'; // Convert binary to ASCII
	Res[2] = 0;                                         // Set string terminator

	return (Res);
}

FUNCTION_RETURN encode_pc_id(PcIdentifier identifier1, PcIdentifier identifier2,
		PcSignature pc_identifier_out) {
	//TODO base62 encoding, now uses base64
	PcIdentifier concat_identifiers[2];
	char* b64_data = NULL;
	int b64_size = 0;
	size_t concatIdentifiersSize = sizeof(PcIdentifier) * 2;
	//concat_identifiers = (PcIdentifier *) malloc(concatIdentifiersSize);
	memcpy(&concat_identifiers[0], identifier1, sizeof(PcIdentifier));
	memcpy(&concat_identifiers[1], identifier2, sizeof(PcIdentifier));
	b64_data = base64(concat_identifiers, concatIdentifiersSize, &b64_size);
	if (b64_size > sizeof(PcSignature)) {
		free(b64_data);
		return FUNC_RET_BUFFER_TOO_SMALL;
	}
	sprintf(pc_identifier_out, "%.4s-%.4s-%.4s-%.4s", &b64_data[0],
			&b64_data[4], &b64_data[8], &b64_data[12]);
	//free(concat_identifiers);
	free(b64_data);
	return FUNC_RET_OK;
}

FUNCTION_RETURN parity_check_id(PcSignature pc_identifier) {
	return FUNC_RET_OK;
}

FUNCTION_RETURN generate_user_pc_signature(PcSignature identifier_out,
		IDENTIFICATION_STRATEGY strategy) {
	FUNCTION_RETURN result;
	PcIdentifier* identifiers;
	unsigned int req_buffer_size = 0;
	result = generate_pc_id(NULL, &req_buffer_size, strategy);
	if (result != FUNC_RET_OK) {
		return result;
	}
	if (req_buffer_size == 0) {
		return FUNC_RET_ERROR;
	}
	req_buffer_size = req_buffer_size < 2 ? 2 : req_buffer_size;
	identifiers = (PcIdentifier *) malloc(
			sizeof(PcIdentifier) * req_buffer_size);
	memset(identifiers, 0, sizeof(PcIdentifier) * req_buffer_size);
	result = generate_pc_id(identifiers, &req_buffer_size, strategy);
	if (result != FUNC_RET_OK) {
		free(identifiers);
		return result;
	}
#ifdef __linux__
	VALGRIND_CHECK_VALUE_IS_DEFINED(identifiers[0]);
	VALGRIND_CHECK_VALUE_IS_DEFINED(identifiers[1]);
#endif
	result = encode_pc_id(identifiers[0], identifiers[1], identifier_out);
#ifdef __linux__
	VALGRIND_CHECK_VALUE_IS_DEFINED(identifier_out);
#endif
	free(identifiers);
	return result;
}

/**
 * Extract the two pc identifiers from the user provided code.
 * @param identifier1_out
 * @param identifier2_out
 * @param str_code: the code in the string format XXXX-XXXX-XXXX-XXXX
 * @return
 */
static FUNCTION_RETURN decode_pc_id(PcIdentifier identifier1_out,
		PcIdentifier identifier2_out, PcSignature pc_signature_in) {
	//TODO base62 encoding, now uses base64

	unsigned char * concat_identifiers = NULL;
	char base64ids[17];
	int identifiers_size;

	sscanf(pc_signature_in, "%4s-%4s-%4s-%4s", &base64ids[0], &base64ids[4],
			&base64ids[8], &base64ids[12]);
	concat_identifiers = unbase64(base64ids, 16, &identifiers_size);
	if (identifiers_size > sizeof(PcIdentifier) * 2) {
		free(concat_identifiers);
		return FUNC_RET_BUFFER_TOO_SMALL;
	}
	memcpy(identifier1_out, concat_identifiers, sizeof(PcIdentifier));
	memcpy(identifier2_out, concat_identifiers + sizeof(PcIdentifier),
			sizeof(PcIdentifier));
	free(concat_identifiers);
	return FUNC_RET_OK;
}

static IDENTIFICATION_STRATEGY strategy_from_pc_id(PcIdentifier identifier) {
	return (IDENTIFICATION_STRATEGY) identifier[0] >> 5;
}

EVENT_TYPE validate_pc_signature(PcSignature str_code) {
	PcIdentifier user_identifiers[2];
	FUNCTION_RETURN result;
	IDENTIFICATION_STRATEGY previous_strategy_id, current_strategy_id;
	PcIdentifier* calculated_identifiers = NULL;
	unsigned int calc_identifiers_size = 0;
	int i = 0, j = 0;
	//bool found;
#ifdef _DEBUG
	printf("Comparing pc identifiers: \n");
#endif
	result = decode_pc_id(user_identifiers[0], user_identifiers[1], str_code);
	if (result != FUNC_RET_OK) {
		return result;
	}
	previous_strategy_id = STRATEGY_UNKNOWN;
	//found = false;
	for (i = 0; i < 2; i++) {
		current_strategy_id = strategy_from_pc_id(user_identifiers[i]);
		if (current_strategy_id == STRATEGY_UNKNOWN) {
			free(calculated_identifiers);
			return LICENSE_MALFORMED;
		}
		if (current_strategy_id != previous_strategy_id) {
			if (calculated_identifiers != NULL) {
				free(calculated_identifiers);
			}
			previous_strategy_id = current_strategy_id;
			generate_pc_id(NULL, &calc_identifiers_size, current_strategy_id);
			calculated_identifiers = (PcIdentifier *) malloc(
					sizeof(PcIdentifier) * calc_identifiers_size);
			memset(calculated_identifiers, 0, sizeof(PcIdentifier) * calc_identifiers_size);
			generate_pc_id(calculated_identifiers, &calc_identifiers_size,
					current_strategy_id);
		}
		//maybe skip the byte 0
		for (j = 0; j < calc_identifiers_size; j++) {
#ifdef _DEBUG
			printf("generated id: %02x%02x%02x%02x%02x%02x index %d, user_supplied id %02x%02x%02x%02x%02x%02x idx: %d\n",
					calculated_identifiers[j][0], calculated_identifiers[j][1], calculated_identifiers[j][2],
					calculated_identifiers[j][3], calculated_identifiers[j][4], calculated_identifiers[j][5], j,
					user_identifiers[i][0], user_identifiers[i][1], user_identifiers[i][2], user_identifiers[i][3], user_identifiers[i][4], user_identifiers[i][5], i);

#endif
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
