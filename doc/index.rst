##############
Licensecc
##############
*Copy protection, licensing library and license generator for Windows and Linux.*

|c11|_ |License|_ |LinuxBuild|_ |WindowsBuild|_ |CodacyBadge|_ |codecov|_ |issues|_ |forum|_ |maintenance|_ 

.. |c11| image:: https://img.shields.io/badge/c%2B%2B-11-blue.svg
.. _c11: https://en.wikipedia.org/wiki/C%2B%2B#Standardization
.. |License| image:: https://img.shields.io/badge/License-BSD%203--Clause-blue.svg
.. _License: https://opensource.org/licenses/BSD-3-Clause
.. |LinuxBuild| image:: https://github.com/open-license-manager/licensecc/actions/workflows/linux-standard.yml/badge.svg
.. _LinuxBuild: https://github.com/open-license-manager/licensecc/actions/workflows/linux-standard.yml
.. |WindowsBuild| image:: https://github.com/open-license-manager/licensecc/actions/workflows/windows-standard.yml/badge.svg
.. _WindowsBuild: https://github.com/open-license-manager/licensecc/actions/workflows/windows-standard.yml
.. |CodacyBadge| image:: https://app.codacy.com/project/badge/Grade/81a1f6bc15014618934fc5fab4d3c206
.. _CodacyBadge: https://www.codacy.com/gh/open-license-manager/licensecc/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=open-license-manager/licensecc&amp;utm_campaign=Badge_Grade
.. |codecov| image:: https://codecov.io/gh/open-license-manager/licensecc/branch/develop/graph/badge.svg?token=vdrBBzX6Rl
.. _codecov: https://codecov.io/gh/open-license-manager/licensecc
.. |issues| image:: https://img.shields.io/github/issues/open-license-manager/licensecc
.. _issues: http://github.com/open-license-manager/licensecc/issues
.. |forum| image:: https://img.shields.io/badge/forum-licensecc-blue.svg?style=flat
.. _forum: https://github.com/open-license-manager/licensecc/discussions
.. |maintenance| image:: https://img.shields.io/badge/maintenance-seeking--maintainers-yellow
.. _maintenance: https://open-license-manager.github.io/licensecc/licensecc/other/maintenance.html

Protect the software you develop from unauthorized copies, limit the usage in time, to a specific set of 
machines, or prevent the usage in  virtualized environments. `Licensecc` is an open source licensing library 
that helps to keep your software closed |:smirk:| . 

Among the other features the most notable one is: if it runs on 'metal' (not a container/vm)
it can generate a signature of that hardware and report if the signature doesn't match, because for instance 
the software has been copied to another place.

A comprehensive :ref:`list of features <analysis/features:Features>`, and their status is available in the project wiki. 

If you're experiencing problems, or you just need informations you can't find here in the documentation,
please contact us on the user `forum`_ , we'll be happy to help. 

License (BSD)
**************
The project is donated to the community. It comes with a very large freedom of use for everyone, and it will always be. 
It uses a `BSD 3 clauses`_ licensing schema, that allows modification and inclusion in GPL and commercial software.

.. _BSD 3 clauses: https://opensource.org/licenses/BSD-3-Clause  

Project Structure
*******************
The software is made by 4 main sub-components:

* ``licensecc``    : the C++ library with a C api (the part you have to integrate in your software) with minimal external dependencies. (github project `licensecc <https://github.com/open-license-manager/licensecc>`_ )
* ``lccinspector`` : a license debugger to be sent to the final customer to diagnose licensing problems or for calculating the hardware id before issuing the license.
* ``lccgen``       : a license generator (github project `lcc-license-generator`_ ) to initialize the library and generate the licenses.
* ``examples``     : usage samples (github project `examples <https://github.com/open-license-manager/examples>`_ ).

.. _lcc-license-generator: https://github.com/open-license-manager/lcc-license-generator

How to build
****************
For quick compilation instructions see the `README.md`_ file in the repository root. 
Detailed, platform-specific instructions (including how to run the tests) are available for 

* :ref:`Linux <development/Build-the-library:Build - Linux>` 
* :ref:`Windows <development/Build-the-library-windows:Build - Windows>`.

For a complete list of dependencies and supported environments see the :ref:`dependencies <development/Dependencies:Dependencies>` section.

.. _README.md: https://github.com/open-license-manager/licensecc#how-to-build

How to use
**************
The `examples`_ repository shows various ways to integrate `licensecc` into your project.

.. _examples: https://github.com/open-license-manager/examples 

Branches and status
*********************

* On branch ``master`` there is the 'stable' 2.1.0 version, that corresponds to this documentation.   
* On branch ``develop`` there is the upcoming 2.x.0 version that is under active development and has not been extensively used/tested. 

How to contribute
********************

.. TIP::
  The easiest way you can solve your problems or ask help is through the `forum`_ on github.

Otherwise you can open an issue in the `issue system`_. Have a look to the `contribution guidelines`_ before reporting.


.. _forum: https://github.com/open-license-manager/licensecc/discussions
.. _issue system: https://github.com/open-license-manager/licensecc/issues
.. _contribution guidelines: https://github.com/open-license-manager/licensecc/blob/develop/CONTRIBUTING.md

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

   
.. toctree::
   :glob:
   :maxdepth: 2
   :hidden:
   :caption: Build the library:
  
   development/*

.. toctree::
   :glob:
   :maxdepth: 2
   :hidden:
   :caption: Integrate and use:
   
   usage/*
   
.. toctree::
   :maxdepth: 2
   :hidden:
   :caption: API:
  
   api/public_api
   api/extend
   api/hardware_identifiers

.. toctree::
   :glob:
   :maxdepth: 2
   :hidden:
   :caption: Analysis:
  
   analysis/*

.. toctree::
   :glob:
   :maxdepth: 2
   :hidden:
   :caption: Miscellaneous:
  
   other/*

.. meta::
   :description: open source license library, copy protection library in C++. 
   :keywords: c++, open source, licensing software, copy protection, license manager, hardware identifcation
   
.. title::
   C++ copy protection library 
