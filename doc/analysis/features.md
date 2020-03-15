# Features:

## features and their status
| Feature                              | Implementation status | 
|--------------------------------------|:---------------------:|
| Trial license with expiry date       | :heavy_check_mark:    |
| Link software to "physical" hardware | :heavy_check_mark:    |
| Virtual machine detection            | In progress           |
| Easy license retrieval               | :heavy_check_mark:    | 
| Manage licenses                      | Planned               |
| Easy Customizable limits             | Planned               |
| Java/C# bindings                     | Planned               |
| Floating/network licenses            | Planned               |

### Issue a "demo" license with only expiry date.
Software editors often want to release a trial license that is not linked to a specified machine, but just limit the usage in time or to some other feature (number of cpu, process memory).
See [execution limits](Implemented/Planned execution limits) section.

### Licenses linked to "physical" hardware id
Link the software to a physical hardware (a pc). In this scenario the software editor wants his software to be executed on a specific piece of hardware. We suppose the operating system is installed "bare to metal" and we're able to calculate an identifier of that hardware. This isn't always true for virtualized environments for which alternative strategies must be planned [eg. floating licenses (not yet implemented)]. See the section on [hardware identifiers](hardware-identifiers) for a detailed discussion of the use cases.

Depending on the situation there are are different kinds of hardware id that can be used. See [execution limits](Implemented/Planned execution limits) section.

### Virtual machine detection
Detect if the software is running in a virtualized environment and (optionally) forbid the execution. This is useful if software editors want to prevent the execution in virtual machines (lxc/dockers...) and be able to compute a meaningful hardware id. 
Another use case is the software is to be used only in a specific class of virtualized environments. For instance it was initially packaged as a docker image, the software editor don't want it to be "extracted" and placed elsewhere.

### License retrieval
Open License Manager, when integrated into a licensed software will find its license file based on many different strategies.
It can handle multiple licenses at the same time. See the wiki page about [license retrieval](license-retrieval) for details.
 
### Licensed software features (partially implemented)
A licensed software may be composed by many features (functions) each one activable independently. Licensing system tells the licensed software which features are enabled, and which features are disabled.
 
### License Management (planned)
It is necessary to keep track of the licenses that have been issued, of the clients, and their hardware identifier. 
A web application is planned that will provide integration with payment systems to allow licensed software customers to buy licenses.

### Customizable execution limits (planned)
It should be easy to add and implement your own "execution limit": that is require/forbid some feature in the application execution environment.

### Java/C# bindings (planned)
The application should support bindings to java language. The library has to be compiled as a .dll for use with JNI or C# integration layer. This is especially challenging on the api side.
Since the api is public, and the library is packaged as a separate .dll/.so everybody can compile a version of it always returning LICENSE_OK easily defeating the license schema. 

### Floating licenses (planned)
Limit the execution of the licensed software to a certain number of instances per license. 
This can be done in multiple ways:
	* without a central server, using broadcast or scanning the local network to find peers.
	* with a central server installed in the client's network. 
	* with a central server on internet.

Status of this implementation is tracked in [issue #4](https://github.com/open-license-manager/open-license-manager/issues/4)

## Implemented/Planned execution limits
Open License Manager can limit execution of licensed software based on some criteria, called "execution limits".
Depending on the execution environment some criteria may not be applicable:

| | Windows BareToMetal | Linux BareToMetal | Linux container (lxc/docker) |Virtual Machine |
|----------------|:------------------:|:------------------:|:------------------:|:------------------:|
| date           | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| disk label     | :heavy_check_mark: | :heavy_check_mark: | :x:<sup>1</sup>    | :heavy_check_mark: |
| disk id        | :heavy_check_mark: | :heavy_check_mark: | :x:<sup>1</sup>    | :heavy_check_mark: |
| ip address     | :heavy_check_mark: | :heavy_check_mark: | :interrobang:      | :heavy_check_mark: |
| mac address    | :heavy_check_mark: | :heavy_check_mark: | :interrobang:      | :heavy_check_mark: |
| cpu number     | Planned            | Planned            | Planned            | Planned            |
| process memory | Planned            | Planned            | Planned            | Planned            |
| machine memory | Planned            | Planned            | Planned            | Planned            |
| virtualization type | Planned       | Planned            | Planned            | Planned            |
| concurrent execution | Planned | Planned | Planned | Planned |
| licensed sw version | Planned       | Planned            | Planned            | Planned            |

### Date
This is the most common limit set on a license, allow the software to expire after the defined date.

### Disk label/Disk id
It is possible to limit the execution to a computer that has a certain disk connected.
This kind of limitation doesn't make sense in linux containerized environments.

### Cpu Number/Machine memory
Allow to restrict the execution only to a machine with the specified amount of cpu or memory or less. This is useful in virtualized environments, to create a licensed software demo version with limited performaces or with limited scalability.

Status of this implementation is tracked in [issue #4](https://github.com/open-license-manager/open-license-manager/issues/4)

### Virtualization type
Allow the execution of the licensed software only on some type of hardware or virtualized environment eg. only bare to metal, only in virtual machine, only in docker.

### Licensed software version
A licensed software may declare a version (eg 1.0.0). A license may span multiple releases of the software e.g. allow version 1.1.1 but not 2.0.0.
