#########################################
Integrate Licensecc in your application
#########################################

This guide shows how to integrate ``licensecc`` into a C++ application,
expanding the content of :ref:`quickstart <usage/quickstart:Quickstart>`.

Working examples are provided in the `examples <https://github.com/open-license-manager/examples>`_ project.

Step 1 — Locate and link the library (CMake)
*********************************************

Two integration methods are supported.

Method A — ``add_subdirectory`` (recommended)
----------------------------------------------

Add ``licensecc`` as a git submodule and include it directly in your build.
This is the simplest approach — no ``find_package`` or extra CMake modules needed.

.. code-block:: console

   cd /path/to/your/project
   git submodule add https://github.com/open-license-manager/licensecc.git submodules/licensecc
   git submodule update --init --recursive

In your ``CMakeLists.txt``:

.. code-block:: cmake

   add_subdirectory(submodules/licensecc)
   add_executable(my_app src/main.cpp)
   target_link_libraries(my_app PRIVATE licensecc::licensecc)


This is the method used in the basic_features `examples <https://github.com/open-license-manager/examples/tree/develop/basic_features>`_

Method B — ``find_package`` 
----------------------------------------------------------------------

Use this method when ``licensecc`` is built and installed externally (not as a
submodule). This usually makes your build a bit faster. 

.. code-block:: cmake

   find_package(licensecc 2.1.0 REQUIRED)
   add_executable(my_app src/main.cpp)
   target_link_libraries(my_app PRIVATE licensecc::licensecc)


Parameters to pass to cmake in the configuration

==================== =====================================================================
CMake variable         Description
==================== =====================================================================
CMAKE_PREFIX_PATH      | Installation prefix of ``licensecc`` (the folder passed to
                       | ``-DCMAKE_INSTALL_PREFIX`` when you built the library). This
                       | lets ``find_package`` locate the package.
licensecc_DIR          | Direct path to the directory containing ``licensecc-config.cmake``,
                       | i.e. ``$LCC_INSTALLATION_DIR/lib/cmake/licensecc``. Use this if
                       | ``find_package`` still cannot find the library.
LCC_PROJECT_NAME       | Name of the project (the software being licensed). It must match
                       | the project name you used when you compiled ``licensecc``. If you
                       | didn't specify one, leave it unset and the default ``DEFAULT`` is
                       | used.
==================== =====================================================================

The directory structure created by ``make install`` looks like this::

.. code-block:: 

   $CMAKE_INSTALL_PATH
      ├── bin
      │   ├── <<PROJECT_NAME>>                        <--- "DEFAULT"
      │   │   └── lccinspector
      │   ├── lccgen -> lccgen-2.1.0
      │   └── lccgen-2.1.0
      ├── include
      │   └── licensecc
      │       ├── datatypes.h
      │       ├── <<PROJECT_NAME>>
      │       │   ├── licensecc_properties.h
      │       │   └── public_key.h
      │       └── licensecc.h
      └── lib
            ├── cmake
            │   └── licensecc                           <--- Point your licensecc_DIR here
            │       ├── licensecc-config.cmake
            │       └── licensecc-config-version.cmake
            └── licensecc
               └── <<PROJECT_NAME>>
                   ├── cmake
                   │   ├── licensecc.cmake
                   │   └── licensecc-debug.cmake
                   └── liblicensecc.a


Debugging ``find_package``
--------------------------

If ``find_package(licensecc ...)`` fails, use CMake's built-in debugging output
to see exactly where it searches:

.. code-block:: console

   cmake .. -DCMAKE_FIND_DEBUG_MODE=ON


A complete working example using this method is available in the
`simple_pc_identifier <https://github.com/open-license-manager/examples/tree/develop/simple_pc_identifier>`_
project. Its ``CMakeLists.txt``:

.. literalinclude:: ../../extern/examples/simple_pc_identifier/CMakeLists.txt
   :language: cmake
   :linenos:


Call Licensecc from your code
***************************************

The public API is declared in ``include/licensecc/licensecc.h``. The two main
entry points are:

- ``identify_pc()`` — compute a hardware identifier for the current machine.
- ``acquire_license()`` — locate, parse, and verify a license file.

A typical usage pattern:

.. code-block:: cpp

   #include <licensecc/licensecc.h>

   LicenseInfo licenseInfo;
   LCC_EVENT_TYPE result = acquire_license(nullptr, nullptr, &licenseInfo);

   if (result == LICENSE_OK) {
       // License is valid -- proceed
   } else if (result == LICENSE_FILE_NOT_FOUND) {
       // No license found -- print hardware identifier so the user can request one
       char pc_identifier[LCC_API_PC_IDENTIFIER_SIZE + 1];
       size_t pc_id_sz = sizeof(pc_identifier);
       if (identify_pc(STRATEGY_DEFAULT, pc_identifier, &pc_id_sz, nullptr)) {
           std::cout << "Hardware ID: " << pc_identifier << std::endl;
       }
       exit(1);
   }

For a complete working example with error handling and event-type mapping, see
the `basic features examples <https://github.com/open-license-manager/examples/tree/develop/basic_features>`_
on GitHub.

See the :ref:`public api <api/public_api:Public api>` reference for full
documentation of all types and functions.
