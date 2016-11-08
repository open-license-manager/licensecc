
#ifndef PC_IDENTIFIERS_H_
#define PC_IDENTIFIERS_H_
#include "metalicensor/api/datatypes.h"
#include "metalicensor/base/base.h"

typedef unsigned char PcIdentifier[6];
typedef char PcSignature[21];

FUNCTION_RETURN generate_pc_id(PcIdentifier * identifiers, unsigned int * array_size, IDENTIFICATION_STRATEGY strategy);

EVENT_TYPE validate_pc_signature(PcSignature str_code);

/**
 * Generates an UserPcIdentifier.
 *
 * @param identifier_out
 * @param strategy
 * @return
 */
FUNCTION_RETURN generate_user_pc_signature(PcSignature identifier_out, IDENTIFICATION_STRATEGY strategy);

#endif /* PC_IDENTIFIERS_H_ */
