# Meta Licensor
#### A Multi-Platform C++ Software License Generator and Manager ([Original Project](https://github.com/open-license-manager/open-license-manager))

This framework allows the generation, verification and management of C++ software licenses distributed via unique textual keys to protect software from unauthorized copies, limit its usage period, limit its usage on specific machines, and prevent its usage in virtualized environments.

Of course, since this is an open source project, reverse engineering this solution to bypass it would be a fairly simple task to a dedicated software end-user. Developers should fork this project privately and extend the protection it provides by adding their own verifications keys/steps to each license.

This fork of the original 'Open License Manager' project offers a rehauled CMake structure, and may later also offer extended license protection settings. See [this page](https://github.com/open-license-manager/open-license-manager) for the (seemingly abandoned) original project. The framework here still primarily consists of a key generator application (located in 'apps/generator') and a static library with minimal external dependencies (located in 'libs/reader') to integrate in the distributed software.

This framework is distributed under its original BSD 3-clause license, making private usage totally possible; [see here for more information](./LICENSE).
