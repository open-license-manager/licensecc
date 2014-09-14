/*
 * pc-identifiers.h
 *
 *  Created on: Apr 16, 2014
 *      
 */

#ifndef PC_IDENTIFIERS_H_
#define PC_IDENTIFIERS_H_
#include "api/datatypes.h"
#include "base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char PcIdentifier[6];
typedef char PcSignature[21];

FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers, unsigned int * array_size,
		IDENTIFICATION_STRATEGY strategy);

EVENT_TYPE validate_pc_signature(PcSignature str_code);

/**
 * Generates an UserPcIdentifier.
 *
 * @param identifier_out
 * @param strategy
 * @return
 */
FUNCTION_RETURN generate_user_pc_signature(PcSignature identifier_out,
		IDENTIFICATION_STRATEGY strategy);

#ifdef __cplusplus
}
#endif
#endif /* PC_IDENTIFIERS_H_ */
