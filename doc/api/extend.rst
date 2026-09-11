#######################################
Extension points
#######################################

Version 2.1 of the library comes with predefined customization points.

 * For "no code customization"   : change default behaviors, enable/disable features customize pre-defined behaviors 
 * Implement new features        : if you need a feature that is not supported licensecc provides 4 extension points.
   - locators  : find and read a license file 
   - parser    : modify the license format 
   - verifier  : change how the license is validated (signature/limits/hardware identifiers)
   - merger    : if multiple licenses or errors are found change the strategy the error is reported.

***************************************************************
API parameters, constants, defaults: licensecc_properties.h
***************************************************************

The easiest configuration point is the file `licensecc_properties.h` in your project directory. 
This file is generated per project, when the project is first built near your public and private key. 
If you placed your projects outside `licensecc` source tree it may be committed in your private git.

The file is never overwritten once it's generated. 

It contains the default buffer sizes used in the API, environment variable names, default 
strategies behavior. If there is some arbitrary name in the library, you may find it here.

.. code-block:: c

   //names of the environment variables to find 
   #define LCC_LICENSE_LOCATION_ENV_VAR "LICENSE_LOCATION"
   #define LCC_LICENSE_DATA_ENV_VAR "LICENSE_DATA"

   //Maximum size of a license file or base64 data (we don't want somebody to crash our software)
   #define LCC_API_MAX_LICENSE_DATA_LENGTH 1024 * 8

   // Miscellaneous api data structure sizes
   #define LCC_API_PC_IDENTIFIER_SIZE 15
   #define LCC_API_PROPRIETARY_DATA_SIZE 64
   #define LCC_API_AUDIT_EVENT_NUM 5
   #define LCC_API_AUDIT_EVENT_PARAM2 255
   #define LCC_API_VERSION_LENGTH 15
   #define LCC_API_FEATURE_NAME_SIZE 15
   #define LCC_API_EXPIRY_DATE_SIZE 10
   #define LCC_API_ERROR_BUFFER_SIZE 256

Tweak default hardware signature generator
=============================================

If the provided hardware signatures don't behave well for your customers, or you want to change the default
way the library generates the pc identifier you can have a look at the following section.

First of all be sure to read about the standard behavior of :c:func:`identify_pc` here:

.. toctree::

   hardware_identifiers

Then you can change the way the default strategy works in `licensecc` by changing 
the underlying strategies in this section of `licensecc_properties.h`:

.. code-block:: c

   /*
    * The following group of settings change the behavior of the underlying strategy selection
    * when LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DEFAULT is choosen.
    *--------
    * List the strategies used when no virtual envrionment is detected
    */
   #define LCC_BARE_TO_METAL_STRATEGIES {STRATEGY_DISK, STRATEGY_SYSTEM_ID, STRATEGY_CPU_MODEL, STRATEGY_NONE}
   /**
    * List the strategies used when the software is executing in a virtual machine
    */
   #define LCC_VM_STRATEGIES {STRATEGY_ETHERNET, STRATEGY_SYSTEM_ID, STRATEGY_NONE}
   /**
    * List the strategies used when it's detected a VM executed execution in the cloud.
    * CPU_MODEL is not very restrictive. Need to debug with Inspector.
    */
   #define LCC_CLOUD_STRATEGIES {STRATEGY_CPU_MODEL, STRATEGY_NONE}
   #define LCC_LXC_STRATEGIES {STRATEGY_ETHERNET, STRATEGY_SYSTEM_ID, STRATEGY_CPU_MODEL, STRATEGY_NONE}
   #define LCC_DOCKER_STRATEGIES {STRATEGY_CPU_MODEL, STRATEGY_NONE}

**************************************************************
Finding the licenses in new places: custom license locators 
**************************************************************
Your software struggle to find the license file? you can implement your own way to find it. For instance if you want 
to download the license from a remote server, you could implement the logic here. 
 
The extension points are declared in the public C++ header
``include/licensecc/LocatorStrategy.hpp`` (``license::locate::LocatorStrategy``).


.. TODO::
   
   this section need to be completed

**************************************************************
Adding new limits: 
**************************************************************

Verifiers use ``include/licensecc/datatypes_cpp.hpp``
(``license::LimitVerifierFn``).

.. TODO::
   
   this section need to be completed