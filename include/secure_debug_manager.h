/*
 * Copyright (c) 2020-2022 Arm Ltd
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its contributors may
 *   be used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @addtogroup secure_debug_manager Secure Debug Manager
 * @brief Top-level API for secure debug library.
 *
 * The Secure Debug Manager API (SDM API) defines the interface used by debug clients, such as
 * debuggers, IDEs, and other tools, for performing protocol-independent secure debug
 * authentication.
 *
 * @{
 */

 /*!
 * @file
 *
 * @brief This header file is the entry point to Secure Debug Manager functionality and includes all
 *  necessary definitions.
 */

#ifndef _SECURE_DEBUG_MANAGER_H_
#define _SECURE_DEBUG_MANAGER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef SKIP_FOR_DOXYGEN
#ifdef _WIN32
#ifdef SDM_EXPORT_SYMBOLS
#define SDM_EXTERN __declspec(dllexport)
#else
#define SDM_EXTERN __declspec(dllimport)
#endif
#else
#define SDM_EXTERN
#endif
#endif // SKIP_FOR_DOXYGEN

// Note about API design:
// In cases where a value in the API is an enum, a typedef'd uint32_t is used instead of an enum type,
// with the enumerations serving only as named constants. The sizes of enums are variable and depend on
// compiler settings, so using a uint32_t type ensures ABI compatibility regardless of the compiler and
// its configuration.

/*!
 * @brief API version number constants.
 *
 * These version numbers are used for #SDMOpenParameters::version fields. A change of the major version is
 * not backwards compatible with previous versions, whilst a minor version change is backwards compatible.
 * Minor version increments may include new features, as long as those features are not required for
 * proper functioning.
 */
enum SDMVersionEnum {
    SDM_CurrentMajorVersion = 0, /*!< Current API major version. */
    SDM_CurrentMinorVersion = 1, /*!< Current API minor version. */
};

/*!
 * @brief Return codes for SDM APIs and callbacks.
 */
enum SDMReturnCodeEnum {
    SDM_Success = 0, /*!< Success, no error */
    SDM_Fail_No_Response = 1, /*!< No response, timeout */
    SDM_Fail_Unsupported_Transfer_Size = 2, /*!< MEM-AP does not support the requested transfer size. */
    SDM_Fail_User_Credentials = 3, /*!< Invalid user credentials for the debugged platform */
    SDM_Fail_IO = 4, /*!< Failed to transmit/receive data to/from the device */
    SDM_Fail_Internal = 5, /*!< An unspecified internal error occurred */
    SDM_Fail_Parameter = 6, /*!< Invalid parameter */
    SDM_Fail_UserCancelled = 7, /*!< User canceled the operation */
};

//! @brief Type for return codes.
typedef uint32_t SDMReturnCode;

//! @brief Opaque handle to an opened instance of the SDM.
typedef struct _SDMOpaqueHandle * SDMHandle;

/*!
 * @brief Possible debug architectures.
 */
enum SDMDebugArchitectureEnum {
    SDM_Arm_ADIv5 = 0,  //!< Arm ADIv5 debug architecture. Uses #SDMArmADICallbacks.
    SDM_Arm_ADIv6 = 1,  //!< Arm ADIv6 debug architecture. Uses #SDMArmADICallbacks.
    SDM_Nexus5001 = 2,  //!< Nexus 5001 (IEEE-ISTO 5001-2003) debug architecture. Uses #SDMNexus5001Callbacks.
};

//! @brief Type for debug architectures.
typedef uint32_t SDMDebugArchitecture;

/*!
 * @brief Flags for SDMOpenParameters.
 */
enum SDMFlags {
    SDM_IsDefaultDeviceInfoValid = (1 << 0),
};

/*!
 * @brief Supported reset types.
 *
 * The reset type is passed to the host in the `resetStart()` and `resetFinish()` callbacks.
 */
enum SDMResetTypeEnum {
    //! @brief System reset using the SDM host's default reset method.
    //!
    //! Note that this may include a target-specific reset type that is not directly selectable with one of these
    //! enumerators.
    SDM_DefaultReset = 0,

    //! @brief System reset via nSRST pin.
    //!
    //! If the target does not have hardware reset, this falls back to #SDM_DefaultReset.
    SDM_HardwareReset = 1,

    //! @brief System reset via software reset mechanism.
    SDM_SoftwareReset = 2,
};

//! @brief Type for reset type.
typedef uint32_t SDMResetType;

/*!
 * @brief Transfer sizes for memory transfer callbacks.
 *
 * These enums are used with the #SDMCallbacks::readMemory and #SDMCallbacks::writeMemory
 * callbacks.
 *
 * Note that not all MEM-APs support all transfer sizes. If a transfer with an unsupported size is
 * attempted, a SDM_Fail_Unsupported_Transfer_Size error will be returned.
 */
enum SDMMemorySizeEnum {
    SDM_Memory8 = 8,    //!< Perform an 8-bit memory transfer.
    SDM_Memory16 = 16,  //!< Perform a 16-bit memory transfer.
    SDM_Memory32 = 32,  //!< Perform a 32-bit memory transfer.
    SDM_Memory64 = 64,  //!< Perform a 64-bit memory transfer.
};

//! @brief Memory size parameter type.
typedef uint32_t SDMMemorySize;

/*!
 * @brief Item details for #SDM_ItemSelect form element.
 *
 * The item info consists of a pair of strings. The first is a short name for the item. This will
 * appear in the list from which the user selects an item. When an item is selected, the long
 * description should be shown to provide the user more information.
 *
 * Both the short name and long description are UTF-8 encoded.
 */
typedef struct SDMItemInfo {
    const char *itemShortName;          //!< Item name that will appear in the list. Must not be NULL.
    const char *itemLongDescription;    //!< Optional descriptive text for this item. Can be NULL.
} SDMItemInfo;

/*!
 * @brief Control state values.
 */
enum SDMControlStateEnum {
    SDM_ControlActiveState = 0,     //!< The control is activated. For a checkbox, this means checked.
    SDM_ControlInactiveState = 1,   //!< The control is inactivated. For a checkbox, this means unchecked.
    SDM_ControlMixedState = 2       //!< The control has a mixed state. For a checkbox, this is the '-' state.
};

//! @brief Control state type.
typedef uint32_t SDMControlState;

/*!
 * @brief Types of user input form elements.
 */
enum SDMFormElementTypeEnum {
    SDM_StaticText = 0,     //!< Static text element.
    SDM_TextField = 1,      //!< Text input field.
    SDM_Checkbox = 2,       //!< Single checkbox.
    SDM_FileSelect = 3,     //!< File path field/selector.
    SDM_ItemSelect = 4,     //!< One of many item select. For instance, a pop-up menu or scrolling list.
};

//! @brief Form element type.
typedef uint32_t SDMFormElementType;

/*!
 * @brief Flags for user input form elements.
 *
 * Not all flags apply to every form element type.
 */
enum SDMFormElementFlagsEnum {
    SDM_ElementIsOptional = (1 << 0),   //!< The element does not have to be filled/set.
    SDM_ElementIsDisabled = (1 << 1),   //!< The element is disabled.
    SDM_ElementIsHidden = (1 << 2),     //!< The element should not be displayed.
    SDM_ElementIsCacheable = (1 << 3),  //!< The element's value may be cached for redisplay.
    SDM_TextFieldIsPassword = (1 << 8), //!< Text field is for entering a password; entered text should be bulleted.
    SDM_CheckboxIsTristate = (1 << 12), //!< Checkbox must support the mixed state value.
    SDM_PathSelectIsFolder = (1 << 16), //!< File select will require user to select folder instead of file.
};

/*!
 * @brief Item details for #SDMCallbacks::enterText callback.
 *
 * All strings must be UTF-8 encoded and null-terminated.
 */
typedef struct SDMFormElement {
    const char *id;     //!< Unique element ID string used for automation-supplied input. Must be a valid C identifier.
    const char *title;  //!< Element's title. Must not be NULL.
    const char *help;   //!< Additional help for the element. May be presented, for example, as a tool tip. May be NULL.
    SDMFormElementType fieldType;   //!< The type of this element.
    uint32_t flags;     //!< Mask composed of #SDMFormElementFlagsEnum enums.
    union {
        /*!
         * @brief Static text element descriptor.
         */
        struct {
            //! The static text string value. Must not be NULL.
            const char *text;
        } staticText;

        /*!
         * @brief Text field element descriptor.
         */
        struct SDMTextFieldInfo {
            //! On input: containts the initial value for the text field.</br>
            //! On output: Buffer into which the entered text will be stored as a null-terminated UTF-8
            //! encoded string.
            //!
            //! Must not be NULL.
            char *textBuffer;

            //! Size in bytes of the buffer pointed to by `textBuffer`. The maximum entered text length will not be
            //! greater than this value - 1 (to accomodate the terminating null). Must be greater than 0.
            uint32_t textBufferLength;
        } textField;

        /*!
         * @brief Checkbox element descriptor.
         */
        struct SDMCheckboxInfo {
            //! On input: Initial checkbox state.</br>
            //! On output: Output checkbox state.
            //!
            //! Must not be NULL.
            SDMControlState *state;
        } checkbox;

        /*!
         * @brief Path select element descriptor.
         *
         * By default a file is selected. If a folder is required, the #SDM_PathSelectIsFolder flag can be set.
         */
        struct SDMPathSelectInfo {
            //! Array of filename extensions to allow. May be NULL, in which case any file
            //! can be selected. Not used if #SDM_PathSelectIsFolder is set.
            const char **extensions;

            //! Number of entries in the filename extensions array. Not used if #SDM_PathSelectIsFolder is set.
            uint32_t extensionsCount;

            //! Buffer for selected file path.
            //!
            //! On input: Initial UTF-8 encoded path value.</br>
            //! On output: Filled with the selected null-terminated UTF-8 encoded path on success.
            //!
            //! Must not be NULL.
            char *pathBuffer;

            //! Size in bytes of the buffer pointed to by `pathBuffer`. The maximum entered text length will not be
            //! greater than this value - 1 (for the terminating null). Must be greater than 0.
            uint32_t pathBufferLength;
        } pathSelect;

        /*!
         * @brief Item select element descriptor.
         *
         * The `selectionIndex` field is base 0. A value of -1 is used to indicate "no selection"".
         */
        struct SDMItemSelectInfo {
            //! Pointer to array of item descriptors. Must not be NULL.
            const SDMItemInfo *items;

            //! Number of elements of the @a items array.
            uint32_t itemCount;

            //! On input: Initial selected item.</br>
            //! On output: Set to the index of the selected item.
            //! Must not be NULL.
            int32_t *selectionIndex;
        } itemSelect;
    };
} SDMFormElement;

/*!
 * @brief Descriptor for a form.
 *
 * A form consists of
 */
typedef struct SDMForm {
    const char *id;         //!< Unique form ID string used for automation-supplied input. Must be a valid C identifier.
    const char *title;      //!< Title of the form. Must not be NULL.
    const char *info;       //!< Optional additional description of the form. May be NULL.
    uint32_t flags;         //!< Mask for flags that apply to the entire form. Reserved for future use.
    const SDMFormElement **elements; //!< Pointer to the array of element pointers.
    uint32_t elementCount;  //!< Number of element pointers.
} SDMForm;


enum SDMDefaultDeviceEnum {
    //! @brief Value indicating the default AP should be used.
    //!
    //! Passed for the _device_ parameter of AP and memory access callbacks.
    SDM_DefaultDevice = -1LL,
};

/*!
 * @brief Arm ADI architecture-specific memory transfer attributes.
 *
 * These enumerators define bit masks for the memory transfer attributes that can be specified
 * with a MEM-AP. Not all available memory attributes are provided.
 */
enum SDMArmADITransferAttributes {
    //! @brief Abstracted nonsecure attribute.
    //!
    //! The debugger must translate this attribute to the appropriate HNONSEC (AHB) or AxPROT[1] (AXI).
    SDM_ArmADI_Nonsecure_Attr = 0x0001,

    //! @brief Abstracted non-privileged attribute.
    //!
    //! The debugger must translate this attribute to the appropriate HPROT[1] (AHB) or AxPROT[0] (AXI).
    SDM_ArmADI_NonPrivileded_Attr = 0x0002,

    //! @brief Flag indicating #SDM_ADI_Direct_Attr_Mask should be used.
    SDM_ArmADI_Direct_Attr_Enable = 0x0080,

    //! @brief Mask for passing direct memory transfer attributes.
    //!
    //! If #SDM_ADI_Direct_Attr_Enable is set, any bits within this mask are passed directly as the
    //! MEM-AP CSW.PROT field.
    SDM_ArmADI_Direct_Attr_Mask = 0x7F00,
};

// Forward declare.
struct SDMNexus5001Callbacks;

/*!
 * @brief Arm ADI architecture-specific callbacks.
 *
 * For minor version API increments to remain backwards compatible, new callbacks must be added to the
 * end of this struct.
 */
typedef struct SDMArmADICallbacks {
    //! @name AP accesses
    //!
    //! The _device_ parameter indicates the address of the AP to access. It can also be set to
    //! @ref SDM_DefaultDevice, and the debugger will use a default AP. For ADIv5 systems, the
    //! AP address is an APSEL value in the range 0-255. For ADIv6 systems, the AP address is an
    //! APB address whose width depends on the target implementation. Nested ADIv6 APs are not
    //! supported directly.
    //@{
    /*!
     * @brief Read an AP register.
     *
     * @param[in] device Address of the AP or #SDM_DefaultDevice.
     * @param[in] registerAddress
     * @param[out] value
     * @param[in] refcon
     */
    SDMReturnCode (*apRead)(uint64_t device, uint64_t registerAddress, uint32_t *value, void *refcon);
    /*!
     * @brief Write an AP register.
     *
     * @param[in] device Address of the AP or #SDM_DefaultDevice.
     * @param[in] registerAddress
     * @param[in] value
     * @param[in] refcon
     */
    SDMReturnCode (*apWrite)(uint64_t device, uint64_t registerAddress, uint32_t value, void *refcon);
    //@}
} SDMArmADICallbacks;

/*!
 * @brief Union to map callback structure for the specific debug architecture.
 */
typedef union SDMArchitectureCallbacks {
    struct SDMArmADICallbacks *armADICallbacks;    //!< Callbacks for the Arm ADIv5 and ADIv6 debug architectures.
    struct SDMNexus5001Callbacks *nexus5001Callbacks;  //!< Callbacks for the Nexus 5001 debug architecture.
} SDMArchitectureCallbacks;

/*!
 * @brief Collection of common callback functions provided by the debugger.
 *
 * Debug architecture-specific callbacks are accessed through the architectureCallbacks member.
 *
 * This interface is not designed for performance but for simplicity.
 *
 * All callbacks accept a _refcon_ parameter. The SDM _must_ pass the value of the
 * SDMOpenParameters::refcon field that was provided to SDMOpen().
 *
 * For minor version API increments to remain backwards compatible, new callbacks must be added to the
 * end of this struct.
 */
typedef struct SDMCallbacks {
    //! @brief Debug architecture-specific callbacks.
    SDMArchitectureCallbacks architectureCallbacks;

    //! @name Progress
    //@{
    /*!
     * @brief Inform the debugger of the current authentication progress.
     *
     * This callback should only be invoked during a call to the SDM_Authenticate() API. Otherwise
     * calls will be ignored.
     *
     * Host support for reporting progress is optional.
     *
     * @param progressMessage
     * @param percentComplete
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    void (*updateProgress)(const char *progressMessage, uint8_t percentComplete, void *refcon);

    /*!
     * @brief Provide an error message for a failure.
     *
     * This callback is intended to be called just prior to returning an error status code from one of the
     * SDM plugin's entry points. Exactly when, how, and even if the error message is presented to the user
     * is the responsibility of the host. Host support for the @a errorDetails string is optional.
     *
     * @param[in] errorMessage The text of the error message. Must not be NULL.
     * @param[in] errorDetails Detailed description of the error. May be NULL.
     */
    void (*setErrorMessage)(const char *errorMessage, const char *errorDetails);
    //@}

    //! @name Target reset
    //@{
    /*!
     * @brief Reset assertion stage.
     * @param[in] resetType One of the #SDMResetTypeEnum enumerators.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*resetStart)(SDMResetType resetType, void *refcon);

    /*!
     * @brief Reset deassertion stage.
     * @param[in] resetType One of the #SDMResetTypeEnum enumerators. Must be the same value passed to
     *  `resetStart()`.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*resetFinish)(SDMResetType resetType, void *refcon);
    //@}

    //! @name Memory accesses
    //!
    //! The _device_ parameter indicates the a debug-architecture-defined address for the interface
    //! to the memory system. For the Arm ADI architecture, this is the address of the MEM-AP to use.
    //! It can also be set to #SDM_DefaultDevice, and the debugger will use the default memory interface
    //! device identified in SDMOpenParmeters. If no default memory interface device is indicated, then
    //! use of #SDM_DefaultDevice will return an error.
    //!
    //! The _address_ parameter is always the address to access within the memory space controlled
    //! by the selected device. For MEM-APs, this is obvious. When #SDM_DefaultDevice is used and the
    //! default device type is #SDM_ArmADI_CoreSight_Component, then the _address_ parameter becomes an
    //! offset relative to the base address of the CoreSight component's 4 kB memory region.
    //!
    //! Addresses must be aligned to transfer size.
    //@{
    /*!
     * @brief Read target memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address Memory address of the data to read.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[in] transferCount Number of memory elements of size _transferSize_ to read.
     * @param[in] attributes Debug-architecture-defined set of attributes that will apply to the transfer, such
     *  as Non-secure, Privileged, Cacheable, and so on. For Arm ADI, this is a value produced by OR'ing
     *  the enums defined in #SDMArmADITransferAttributes.
     * @param[out] data Buffer where read data will be written. Must be at least _transferSize_ * _transferCount_
     *  bytes in length.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     * @retval SDM_Success The read completed successfully and _value_ holds the data.
     * @retval SDM_Fail_No_Response
     */
    SDMReturnCode (*readMemory)(
        uint64_t device,
        uint64_t address,
        SDMMemorySize transferSize,
        size_t transferCount,
        uint32_t attributes,
        void *data,
        void *refcon);

    /*!
     * @brief Write target memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address Memory address of the data to write.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[in] transferCount Number of memory elements of size _transferSize_ to write.
     * @param[in] attributes Debug-architecture-defined set of attributes that will apply to the transfer, such
     *  as Non-secure, Privileged, Cacheable, and so on. For Arm ADI, this is a value produced by OR'ing
     *  the enums defined in #SDMArmADITransferAttributes.
     * @param[in] data Buffer from where data to be written is read. Must be at least _transferSize_ * _transferCount_
     *  bytes in length.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*writeMemory)(
        uint64_t device,
        uint64_t address,
        SDMMemorySize transferSize,
        size_t transferCount,
        uint32_t attributes,
        const void *value,
        void *refcon);
    //@}

    //! @name User interaction
    //@{
    /*!
     * @brief Present a form to receive input from the user.
     *
     * The intended use cases include selecting a credential or other configuration item, enter username
     * and/or password, select files, set requested permissions, and so on.
     *
     * @param[in] form Pointer to the form descriptor struct.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     *
     * @retval SDM_Success User provided requested input.
     * @retval SDM_Fail_Parameter There was an issue with the form descriptors.
     * @retval SDM_Fail_UserCancelled The user cancelled.
     */
    SDMReturnCode (*presentForm)(const SDMForm *form, void *refcon);
    //@}

} SDMCallbacks;

/*!
 * @brief Supported types of default devices.
 */
enum SDMDefaultDeviceTypeEnum {
    SDM_NoDefaultDevice = 0,
    SDM_ArmADI_AP = 1,
    SDM_ArmADI_MEM_AP = 2,
    SDM_ArmADI_CoreSight_Component = 3,
};

//! @brief Type for default device type enum parameter.
typedef uint32_t SDMDefaultDeviceType;

/*!
 * @brief Information about the default device.
 */
typedef struct SDMDefaultDeviceInfo {
    SDMDefaultDeviceType deviceType; //!< Type of the matched default device.
    uint64_t address;   //!< Base address of the device.
} SDMDefaultDeviceInfo;

/*!
 * @brief Parameters passed to SDM_Open() by the debugger.
 */
typedef struct SDMOpenParameters {
    struct {
        uint16_t major; /*!< Client interface major version. */
        uint16_t minor; /*!< Client interface minor version. */
    } version; /*!< Client interface version. @see #SDMVersion. */
    SDMDebugArchitecture debugArchitecture; /*!< Debug architecture for the target. */
    SDMCallbacks *callbacks; /*!< Callback collection */
    void *refcon; /*!< Debugger-supplied value passed to each of the callbacks. */
    uint32_t flags; /*!< Flags passed to the SDM from the debugger. */
    const char **locales; /*!< Pointer to a NULL-terminated array of IETF BCP 47 language tags, e.g. "en-US", "fr-FR", "sv", etc. The  tags are sorted in decreasing priority order. */
    SDMDefaultDeviceInfo defaultDeviceInfo; /*!< Information about the default device. */
} SDMOpenParameters;

/*!
 * @brief Possible execution contexts.
 */
enum SDMExecutionContextEnum {
    SDM_Boot_ROM = 0,
    SDM_Boot_Loader = 1,
    SDM_Runtime = 2,
};

//! @brief Type for execution context enum parameter.
typedef uint32_t SDMExecutionContext;

/*!
 * @brief Parameters passed by the debugger to the SDM_Authenticate() API.
 */
typedef struct SDMAuthenticateParameters {
    SDMExecutionContext expectedExecutionContext;
    bool isLastAuthentication;
} SDMAuthenticateParameters;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief This function is called by the debugger to start a secure debug session with the remote platform.
 *
 * @param[out] handle New handle to the SDM instance.
 * @param[in] params Connection details and callbacks. This pointer and all pointers within the
 *  structure will remain valid until SDM_Close() is called, so the plugin can cache the value for later
 *  use.
 */
SDM_EXTERN SDMReturnCode SDM_Open(SDMHandle *handle, const SDMOpenParameters *params);

/*!
 * @brief Perform authentication to unlock debug access.
 *
 * More than one call to this API is allowed, if the "multiple-authentications" feature is enabled
 * in the SDM XML.
 *
 * @param[in] handle Handle to the SDM instance.
 * @param[in] params Parameters for the authentication. The pointer only needs to be valid during
 *      the call to this API.
 */
SDM_EXTERN SDMReturnCode SDM_Authenticate(SDMHandle handle, const SDMAuthenticateParameters *params);

/*!
 * @brief Called by the debugger to resume the boot of the remote platform.
 *
 * This API is only a request from the host to the plugin. It may be implemented as a no-op.
 *
 * It is typically called after the debugger places its breakpoints at the booting debugged system.
 * It is only useful if the debugged system supports the introduction of debug certificate in the
 * early boot stages, otherwise if the debugged system processes the secure debug certificate at runtime,
 * it does not wait for the resume command.
 *
 * @param[in] handle Handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_ResumeBoot(SDMHandle handle);

/*!
 * @brief Close the SDM session.
 *
 * This call ends a secure debug session with the remote platform. The SDM may free any
 * resources it allocated.
 *
 * @param[in] handle Handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_Close(SDMHandle handle);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _SECURE_DEBUG_MANAGER_H_ */
