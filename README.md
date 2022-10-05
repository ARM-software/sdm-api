# Secure Debug Manager API

This repository contains the API definition for the Secure Debug Manager (SDM).

The SDM is the host-side component that implements a secure debug protocol. An abstract system architecture for secure debug is shown below. 

![](docs/generic_system_diagram.png)

The Debug Client is the process that requests debug access a locked target. Usually this would be an IDE or standalone debugger used by a developer or technician, but it can be any sort of tool that needs debug access to the target. The SDM is used by the Debug Client to securely unlock debug access.

The requirements for the SDM API are as follows:

- Support any secure debug protocol that uses the debug link for communications.
- Usable for any debug client.
- Is independent of the debug link.
- Debug architecture independent.
- OS independent (though the shared library for an implementation will be OS dependent).

The SDM API enables tools to support a variety of secure debug implementations using a single API. Tool vendors are encouraged to use this API to ease the support burden on silicon and software providers, as well as themselves, and to provide a simple and easy to use integration of secure debug for their users.

The goal is for silicon or software vendors to provide an implementation of the API as a shared library, that is included with tools as part of device support. The SDM library will also be included in CMSIS Device Family Packs.

The SDM API is designed to be debug architecture agnostic, so it can be used on debug architectures other than Arm ADI. However, a given implementation of the API will only support the debug architectures for which it was written.

The SDM API supports any secure debug protocol. A reference implementation is provided for the PSA Authenticated Debug Access Control (ADAC) protocol. As part of this reference implementation, additional lower-level APIs are planned. These will define layers such as a Credential provider and Debug mailbox interface.

### Structure

The main [`secure_debug_manager.h`](include/secure_debug_manager.h) header file is located in the [`include/`](./include/) directory.

A doxygen configuration file is available to generate documentation for the API.

An XML manifest file will be included with the SDM shared library. The included [`xml/example-manifest.xml`](xml/example-manifest.xml) file is an example manifest for experimentation purposes.

The manifest serves several purposes:

- It allows one SDM implementation to be configured for different targets.
- It gives the debug client information about the library.
- It describes user interface options.

Eventually a schema will be created for the manifest.

### Status

The overall structure for the API is well defined, and some details are in progress. All feedback is appreciated.

PSA ADAC related lower-level APIs are not yet defined.

### License

The files are covered by the BSD 3-Clause license.

Copyright © 2020-2022 Arm Ltd.
