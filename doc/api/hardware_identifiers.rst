###############################################
Customize hardware signature generators 
###############################################

Hardware identifier encoding
****************************

Each hardware identifier is encoded as a 9-byte binary payload:

- byte 0 contains the identification strategy used to generate the id.
- bytes 1-8 hold the strategy-specific identification data.

The payload is serialised as a base64 string for storage in license files
and display. The identifier can be serialized as a string similar to 'AABm-73pY-0R4q'
and it is usually passed between the final user of the software and the software vendor
to generate an hardware linked license.

For the full byte-level layout see :doxygenclass:`license::hw_identifier::HwIdentifier`.

Change the hardware identification strategy
*************************************************

Included with the library there are three hardware identification strategies: `IP_ADDRESS`, `STRATEGY_ETHERNET` (mac address) and 
`STRATEGY_DISK` (partition serial number). If you want to change the strategy that is used to generate the default identifier:
  
  - locate the file `licensecc_properties.h`` (usually in ``projects/<$project_name>/include/licensecc/<$project_name>``
  - you can change the order of the strategies in the following code block (the strategies will be tried in sequence until the first one succeeds):

.. code-block:: c
#define LCC_BARE_TO_METAL_STRATEGIES { STRATEGY_DISK, STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_VM_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_LXC_STRATEGIES { STRATEGY_ETHERNET, STRATEGY_NONE }
#define LCC_DOCKER_STRATEGIES { STRATEGY_NONE }
#define LCC_CLOUD_STRATEGIES { STRATEGY_NONE }

Implement your own hardware signature generator 
*************************************************

.. doxygenclass:: license::hw_identifier::IdentificationStrategy
