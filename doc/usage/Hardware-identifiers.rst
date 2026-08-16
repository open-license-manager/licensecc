#######################
Hardware Identifiers
#######################

When the library was designed 15 years ago hardware identifiers were used to link the execution of the software to a physical hardware.
With the coming of virtualization `hardware identifiers` are a way to limit the usage of the licensed software.

The software executing on client's infrastructure (or physical hardware), when it finds the license is missing, calls the api 
:ref:`identify_pc <api/public_api:Public api>` and generates an hardware identifier. 
The client has to communicate the hardware id back to the software publisher (you) that 
will issue a license for him (for details see: :ref:`issue licenses <usage/issue-licenses:Issue Licenses>`).   

.. NOTE::

  Licensecc will guess the infrastructure type where the user is trying to launch the software (eg. a Virtual Machine), 
  embed the information into the hardware identifier and report it to the software publisher before he is issuing the license.
  **Planned 2.5.0(?)**

Hardware identifier encoding
****************************

The identifier is a 15 character string similar to 'AABm-73pY-0R4q'.

Each hardware identifier is a 9-byte binary payload:

- byte 0 contains the identification strategy used to generate the id, and the environment where it was generated (VM, docker...)
- bytes 1-8 hold the strategy-specific identification data.

The payload is serialised as a base64 string for storage in license files and display,  
and it is usually passed from the final user to the software vendor to generate an hardware linked license.

For the full byte-level layout see :doxygenclass:`license::hw_identifier::HwIdentifier`.

*****************
Available identification methods
*****************

The software vendor should understand how the end user will run the software to choose the
right identification strategy. For example, provisioning a short-lived virtual machine in a
continuous integration system has very different hardware stability than a permanent on-premises 
virtual server, but to `licensecc` they both appear as 'VM'. The chosen strategy should reflect that. 
Though `licensecc` is trying to use sensible defaults, the software vendor may have to customize the behavior.

The library offers the following strategies, each identified by a constant in
:cpp:enum:`LCC_API_HW_IDENTIFICATION_STRATEGY`:

.. _ETHERNET:

``STRATEGY_ETHERNET`` (0)
=========================

Uses the **MAC address** of the system's "primary" network adapter to generate the hardware
identifier. The implementation enumerates network adapters via the operating system, scores
them to prefer physical and connected adapters over virtual/VPN/disconnected ones, and picks the first non-zero MAC
address it finds.

- **Bare to metal**: Stable — the MAC address of a physical NIC does not change. Survives across reinstallation of the SO.
- **Virtual machine**: Works, if the machine is stable, but does not prevent the machine from being cloned with the same mac address. If you are using this inside a CI/CD and the VM is taken from a pool re-configured each time most probably won't work. 
- **Container (Docker/LXC)**: Usually doesn't work, each container does not have a stable mac address.

.. _IP_ADDRESS:

``STRATEGY_IP_ADDRESS`` (1)
===========================

Uses the **IPv4 address** of the "primary" network adapter. The implementation is the same.
as ``STRATEGY_ETHERNET`` but reads the IP address instead of the MAC address.

- **Bare to metal**: ✅ Works, but IP addresses are typically assigned via DHCP and can change on reboot or network reconfiguration.
- **Virtual machine**: ✅ Works, but subject to the same DHCP volatility.
- **Container (Docker/LXC)**: ❓ Usually doesn't work: in orchestrated environments the IP may change on every deployment.

.. _DISK:

``STRATEGY_DISK`` (2)
=====================

Uses the **disk serial number** and **disk label** of the system's storage devices. The
implementation queries the operating system for disk information, preferring the system
drive (on Windows) or the root filesystem device (on Linux). It produces multiple candidate
identifiers: one per disk, first by serial number then by label.

- **Bare to metal**: ✅ Stable — disk serial numbers are set at manufacturing time and
  rarely change.
- **Virtual machine**: ✅ Works, but be aware that some hypervisors generate random disk
  serials for each VM clone.
- **Container (Docker/LXC)**: ❌ Not available — containers typically do not have direct access to the host's block devices.

.. _CPU_MODEL:

``STRATEGY_CPU_MODEL`` (3)
==========================

Uses the **OS-specific machine identifier**:

- On **Intel** (Linux/Windows): use ``__cpuid`` to get the cpu model. 
- On **ARM Windows**: we can get cpu model information from registry.
- On **ARM Linux**: cpu model are taken from `/proc/cpuinfo`.

- **Bare to metal**: ✅ Stable — Does survive an OS reinstallation.
- **Virtual machine**: ✅ Not very useful — the cpu identifier is modified by the Hypervisor (KVM..).
- **Container (Docker/LXC)**: Stable - This is a good choice to lock the docker to the machine where it is executed.

.. _SYSTEM_ID:

``STRATEGY_SYSTEM_ID`` (4)
==========================

Uses the **OS-specific machine identifier**:

- On **Linux**: reads ``/etc/machine-id`` (the systemd machine ID).
- On **Windows**: uses ``GetSystemIdForPublisher`` if available, falling back to the
  ``ProductId`` registry key under ``HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion``.

- **Bare to metal**: ✅ Stable — the OS machine ID is set at installation time and
  persists across reboots. Does not survive an OS reinstallation.
- **Virtual machine**: ✅ Works — the identifier survives VM restarts, but may change
  if the VM is re-deployed from a template. So it may not be suitable for short lived VM [TO BE Tested]
- **Container (Docker/LXC)**: ❓ To be tested, to understand the applicability.


``STRATEGY_DEFAULT`` (-1)
==========================

This is the strategy you should use in 99% of cases. It selects the best method based on the environment it detects.

When the licensed software calls :ref:`identify_pc <api/public_api:Public api>` with :cpp:enumerator:`LCC_API_HW_IDENTIFICATION_STRATEGY::STRATEGY_DEFAULT` 
the identifier generation will follow these steps:

 - It will first look to the environment variable ``IDENTIFICATION_STRATEGY``. If set it will use the identification strategy in that variable.
 - It will try to determine which virtual environment the licensed software is running in. 
    * If no virtual environment found it will use the strategies in :c:macro:`LCC_BARE_TO_METAL_STRATEGIES`, it will try them one by one until the first one succeeds.
    * If it detects it's running in a Virtual Machine it will try the strategies in :c:macro:`LCC_VM_STRATEGIES`, it will try them one by one until the first one succeeds.

if you're interested in implementing your own hardware identification strategy you can have a look to the library
 :ref:`extension points <api/extend:Tweak hardware signature generator>`.

.. TIP:

    If `licensecc` is generating a bad hardware identifier (eg. 'AAAA-AAAA-AAAA') software licensor can ask the user 
    to set the environment variable ``IDENTIFICATION_STRATEGY`` and try again. Or he can send the user the `lccinspector`
    to generate all the possible identifiers for that machine.


.. NOTE::
    
    `licensecc` will try to validate the identifier using the same strategy that was used to generate it, regardless  
    of what is the default method now in use. eg: disk identifiers will always be validated by ``DiskStrategy``.

.. tip::

    The default strategies used in each environment are defined by the macros
    :c:macro:`LCC_BARE_TO_METAL_STRATEGIES`, :c:macro:`LCC_VM_STRATEGIES`,
    :c:macro:`LCC_DOCKER_STRATEGIES`, and :c:macro:`LCC_CLOUD_STRATEGIES`.
    These can be customized per-project in ``licensecc_properties.h``.

.. tip::

    Per-project customization of the strategy priorities requires editing
    ``licensecc_properties.h`` and recompiling. A more flexible (dependency injection)
    based approach is planned for **v2.5.0**.

*****************
Summary
*****************

 - Execution in a physical hardware: Use the (physical) disk SN. This survives a reinstallation of the pc, as a second choice use installation ID.
 - Execution in a virtual machine: Use the mac address. This provide a tiny protection on cloning, as a second choice use the installation ID. If the machine is ephemeral (eg. CI/CD) there is not much you can do to limit. **change when cpuid strategy available**
 - Execution in a docker: Use the installation ID strategy. This prevents the executable to be taken out of the docker and used elsewhere.

This is what `STRATEGY_DEFAULT` does for you, but you may want to customize it, as described above.

.. tip::
  For details on customizing the identifier generation, see the
  :doc:`hardware identifiers reference <api/hardware_identifiers>`.






