#######################
Hardware Identifiers
#######################

Hardware identifiers are used to link the execution of the software to a physical hardware (a pc). 
The software executing on client's hardware, when it finds the license is missing, calls the api 
:ref:`identify_pc <api/public_api:Public api>`
and generates an hardware identifier. The client has to communicate the hardware id back to the software publisher that 
will issue a license for him (for details see: :ref:`issue licenses <usage/issue-licenses:Issue Licenses>`).   

.. NOTE::

  Licensecc will guess the environment where the user is trying to launch the software (eg. a Virtual Machine), 
  embed the information into the hardware id and report it to the software publisher before he is issuing the license.

*****************
Usage scenarios
*****************
With the recent coming of virtualized environments the installation of software directly on the machine has been less and less.


Execution in a physical hardware
=================================
If the client executes the software on a physical hardware it is possible to identify the 
hardware by various parameters (cpu type/memory/disk label/mac address) see :ref:`features <analysis/features:Features>`
for detail of supported identification strageties.

Execution in a virtual machine
==============================
If you're allowing software users to generate pc identifiers in a virtual machine
you should ask yourself what's the real use of it, since the vm can be copied as a whole elsewhere 
and there are few ways to detect this (without using an external license server that's not yet supported by this library).

* Usually when the machine is copied the MAC address is changed. But sometimes it changes on its own. Software publishers may want to use this as a last resort to prevent the vm for being cloned. It has to be verified case by case.
* Software editors may want to prevent people from extracting the software from the VM. They can use an hidden registry key or a specific file in a location outside the installation directory to verify the software is distributed in the original VM. Or they can link the execution to a specific kind of virtualization (this is supported by OpenLicenseManager).

.. TIP::

    In this case issuing a `demo` license with just a date limitation is advised.

Execution in a container
========================
Depending on how containers are used having hardware identifiers may make sense or no sense at all. 
For instance if containers are used to avoid to pollute the external distribution it makes perfect sense to have an 
hardware identifier, if users are running dockers in a kubernetes cluster in the cloud it makes no sense at all.

************************
Identifier Generation
************************
`Licensecc` is able to identify which virtual environment the user is running in and select the appropriate generation
strategy. Below the identifier generation workflow used by the :ref:`identify_pc <api/public_api:Public api>` method. 

.. figure:: ../_static/pc-id-selection.png


First of all it takes in account the application specified parameter 
If the licensed software uses `STRATEGY_DEFAULT` and the strategy generates an unstable identifier it is possible to ask the user to 
set the environemnt variable.

