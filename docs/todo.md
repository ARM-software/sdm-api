Secure Debug Manager API Todo
-----------------------------

- Look at l10n: .po file or manifest xml
    - Document how l10n is applied.

- How best to report errors to the user beyond simple error codes.

- Finish describing requirements for the implementation to guarantee that aligned/sized transfers.

- userSelectedFilePath and selectItem needs thought
    - flexible enough?
    - can use a user-selected config file for more complex requirements?
        - does this just move the burden?

- selectItem partner feedback:
    Potentially the selectItem() api we understand that it allows to select one file.
    when in our case it would have to select
    the signature element:        Secure Element, Host, Local (non-secure).
    and the Debug type we expect (ConstDebug, ROTDebug, UnconstDebug)

- Finish SDMAuthenticateParameters
    - Interaction with user interface and credential providers

- Is SDM_ResumeBoot() even needed?
    - Closing the session probably automatically resumes boot for those execution contexts where it makes sense, e.g. boot ROM.

- Define manifest schema

- Document manifest, including set of capabilities

- SDM_Authenticate:
    - deal with:
        - reset type/exec context
        - multiple auths

- what params for auth API?

- why was resetType removed from SDM_Open (vs SDM_Init)? what implications does this have?

- Possibility to package SDM plugin as zip file
    - With .sdm extension
    - Contains manifest and all shared libraries for each supported host OS and architecture
    - Maybe .po files if that option is chosen for l10n
    - Example:
        plugin.sdm/
            manifest.xml
            arch/
                darwin-x86_64/
                    plugin.dylib
                    sdc600_mailbox.dylib
                darwin-aarch64/
                    plugin.dylib
                    sdc600_mailbox.dylib
                linux-i386/
                    plugin.so
                    sdc600_mailbox.so
                linux-x86_64/
                    plugin.so
                    sdc600_mailbox.so
                win-i386/
                    plugin.dll
                    sdc600_mailbox.dll
                win-x86_64/
                    plugin.dll
                    sdc600_mailbox.dll
