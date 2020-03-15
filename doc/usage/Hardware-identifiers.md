# Hardware Identifiers

## Use cases
Linking the execution of a software to a physical hardware (a pc). With the recent coming of virtualized environments 
the installation of software directly on the machine has been less and less.

OpenLicenseManager guess the environment where the user is trying to launch the software, and reports it to the software publisher when he is generating a license in a way that he can decide how to limit the usage on per license basis.

### Executing in a physical hardware
This is straightforward case. If the software is executing on a physical hardware it is possible to identify the hardware by various parameters (cpu type/memory/disk label/mac address)

### Execution in a virtual machine
Generating a hardware identifier on a virtual machine doesn't make much sense, since the vm can be copied as a whole elsewhere 
and there are few ways to detect this without using an external license server.

*   Usually when the machine is copied the MAC address is changed. But sometimes it changes on its own. Software publishers may want to use this as a last resort to prevent the vm for being cloned. It has to be verified case by case.
*   Software editors may want to prevent people from extracting the software from the VM. They can use an hidden registry key or a specific file in a location outside the installation directory to verify the software is distributed in the original VM. Or they can link the execution to a specific kind of virtualization (this is supported by OpenLicenseManager).

### Executing in a container
Depending on how containers are used having hardware identifiers may make sense or no sense at all. For instance if containers are used
to avoid to pollute the external distribution it makes perfect sense to have an hardware identifier, if users are running dockers in a kubernetes cluster it makes no sense at all.

## Identifier Generation
Below the identifier generation workflow. 


If the licensed software specify the parameter
If the licensed software uses `STRATEGY_DEFAULT` and the strategy generates an unstable identifier it is possible to ask the user to 
set the environemnt variable.
