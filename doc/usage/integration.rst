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

Method B — ``find_package`` with ``Findlicensecc.cmake`` (alternative)
----------------------------------------------------------------------

Use this method when ``licensecc`` is built and installed externally (not as a
submodule). Copy the CMake module ``Findlicensecc.cmake`` into your project's
CMake modules directory, then add to your ``CMakeLists.txt``:

.. code-block:: cmake

   find_package(licensecc 2.0.0 REQUIRED)

   add_executable(my_app src/main.cpp)
   target_link_libraries(my_app PRIVATE licensecc::licensecc)

``Findlicensecc.cmake`` recognises the following input variables:

==================== =====================================================================
CMake variable         Description
==================== =====================================================================
LICENSECC_LOCATION     | Hint to locate the library when not checked out as a submodule.
                       | May point to the installation folder or the source folder.
LCC_PROJECT_NAME       | Name of the project (the software being licensed).
                       | Alternatively, pass it as a component in ``find_package``.
==================== =====================================================================

A complete working example using this method is available in the
`simple_pc_identifier <https://github.com/open-license-manager/examples/tree/develop/simple_pc_identifier>`_
project. Its ``CMakeLists.txt``:

.. literalinclude:: ../../extern/examples/simple_pc_identifier/CMakeLists.txt
   :language: cmake
   :linenos:

Step 6 — Call Licensecc from your code
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
the `simple_pc_identifier example <https://github.com/open-license-manager/examples/blob/develop/simple_pc_identifier/src/example.cpp>`_
on GitHub.

See the :ref:`public api <api/public_api:Public api>` reference for full
documentation of all types and functions.
