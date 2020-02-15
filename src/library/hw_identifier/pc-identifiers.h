/*
 * pc-identifiers.h
 *
 *  Created on: Apr 16, 2014
 *
 */

#ifndef PC_IDENTIFIERS_H_
#define PC_IDENTIFIERS_H_

#include <licensecc/datatypes.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "../base/base.h"

typedef char PcSignature[LCC_API_PC_IDENTIFIER_SIZE + 1];

FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers, unsigned int * array_size,
		LCC_IDENTIFICATION_STRATEGY strategy);

LCC_EVENT_TYPE validate_pc_signature(PcSignature str_code);

/**
 * Generates an UserPcIdentifier.
 *
 * @param identifier_out
 * @param strategy
 * @return
 */
FUNCTION_RETURN generate_user_pc_signature(PcSignature identifier_out, LCC_IDENTIFICATION_STRATEGY strategy);

#ifdef __cplusplus
}
#endif
#endif /* PC_IDENTIFIERS_H_ */
