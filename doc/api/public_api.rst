#######################################
Public api
#######################################

The public C api of the library can be found in ``include/licensecc/licensecc.h`` that is the only file you should 
include when you want to use the library from C. 

C++ applications can use the C++ api declared in ``include/licensecc/Licensecc.hpp`` (see the
`C++ api`_ section below).

Print hardware identifier
**************************

.. doxygenfunction:: identify_pc

.. doxygenenum:: LCC_API_HW_IDENTIFICATION_STRATEGY

.. doxygengroup:: strategies  
   :content-only:


Verify a license
***********************

.. doxygenfunction:: acquire_license

.. doxygenstruct:: CallerInformations
	:members:

.. doxygenstruct:: LicenseLocation
	:members:
	
.. doxygenstruct:: LicenseInfo
	:members:

C++ api
*******

The library provides a C++ facade class that wraps the same functionality in an
object oriented interface while remaining backward compatible with the C api.

.. doxygenclass:: license::Licensecc
	:members: