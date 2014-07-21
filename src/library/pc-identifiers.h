/*
 * pc-identifiers.h
 *
 *  Created on: Apr 16, 2014
 *      Author: devel
 */

#ifndef PC_IDENTIFIERS_H_
#define PC_IDENTIFIERS_H_
#include "api/datatypes.h"
#include "base/base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char PcIdentifier[6];
typedef char UserPcIdentifier[21];


FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers, unsigned int * array_size,
		IDENTIFICATION_STRATEGY strategy);

EVENT_TYPE validate_user_pc_identifier(UserPcIdentifier str_code);

#ifdef __cplusplus
}
#endif
#endif /* PC_IDENTIFIERS_H_ */
