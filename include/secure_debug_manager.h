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

//! @brief Boolean type used in the SDM API.
//!
//! This type is used to avoid issues caused by differences in the size of the `bool` type between
//! C and C++, as well as any ABI differences.
typedef uint8_t SDMBool;

/*!
 * @brief API version number constants.
 *
 * These version numbers are used for #SDMOpenParameters::version fields. A change of the major version is
 * not backwards compatible with previous versions, whilst a minor version change is backwards compatible.
 * Minor version increments may include new features, as long as those features are not required for
 * proper functioning.
 */
enum SDMVersionEnum {
    SDMVersion_CurrentMajor = 1, /*!< Current API major version. */
    SDMVersion_CurrentMinor = 0, /*!< Current API minor version. */
};

/*!
 * @brief Return codes for SDM APIs and callbacks.
 */
enum SDMReturnCodeEnum {
    SDMReturnCode_Success = 0, /*!< Success, no error */
    SDMReturnCode_RequestFailed = 1, /*!< Generic failure, used when a more specific error is not available */
    SDMReturnCode_InvalidUserCredentials = 2, /*!< Invalid user credentials for the debugged platform */
    SDMReturnCode_InvalidArgument = 3, /*!< Invalid argument value */
    SDMReturnCode_UserCancelled = 4, /*!< User canceled the operation */
    SDMReturnCode_UnsupportedOperation = 5, /*!< Requested operation is not supported */
    SDMReturnCode_IOError = 6, /*!< Failed to communicate with the target */
    SDMReturnCode_TimeoutError = 7, /*!< No response, timeout */
    SDMReturnCode_UnsupportedTransferSize = 8, /*!< Device does not support the requested transfer size. */
    SDMReturnCode_TransferFault = 9, /*!< Memory or register access failed due to a transfer fault */
    SDMReturnCode_TransferError = 10, /*!< Memory or register access failed due to an error other than a fault */
    SDMReturnCode_InternalError = 11, /*!< An unspecified internal error occurred */
};

//! @brief Type for return codes.
typedef uint32_t SDMReturnCode;

//! @brief Opaque handle to an opened instance of the SDM.
typedef struct _SDMOpaqueHandle *SDMHandle;

/*!
 * @brief Possible debug architectures.
 */
enum SDMDebugArchitectureEnum {
    SDMDebugArchitecture_ArmADIv5 = 0,  //!< Arm ADIv5 debug architecture.
    SDMDebugArchitecture_ArmADIv6 = 1,  //!< Arm ADIv6 debug architecture.
    SDMDebugArchitecture_Nexus5001 = 2,  //!< Nexus 5001 (IEEE-ISTO 5001-2003) debug architecture.
};

//! @brief Type for debug architectures.
typedef uint32_t SDMDebugArchitecture;

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
    SDMResetType_Default = 0,

    //! @brief System reset via nSRST pin.
    //!
    //! If the target does not have hardware reset, this falls back to #SDMResetType_Default.
    SDMResetType_Hardware = 1,

    //! @brief System reset via software reset mechanism.
    SDMResetType_Software = 2,
};

//! @brief Type for reset type.
typedef uint32_t SDMResetType;

/*!
 * @brief Item details for #SDMForm_ItemSelect form element.
 *
 * The item info consists of a pair of strings. The first is a short name for the item. This will
 * appear in the list from which the user selects an item. When an item is selected, the long
 * description should be shown to provide the user more information.
 *
 * Both the short name and long description are UTF-8 encoded.
 */
typedef struct SDMFormItemInfo {
    const char *itemShortName;          //!< Item name that will appear in the list. Must not be NULL.
    const char *itemLongDescription;    //!< Optional descriptive text for this item. Can be NULL.
} SDMFormItemInfo;

/*!
 * @brief Control state values.
 */
enum SDMControlStateEnum {
    SDMControlState_Active = 0,     //!< The control is activated. For a checkbox, this means checked.
    SDMControlState_Inactive = 1,   //!< The control is inactivated. For a checkbox, this means unchecked.
    SDMControlState_Mixed = 2,      //!< The control has a mixed state. For a checkbox, this is the '-' state.
};

//! @brief Control state type.
typedef uint32_t SDMControlState;

/*!
 * @brief Types of user input form elements.
 */
enum SDMFormElementTypeEnum {
    SDMForm_StaticText = 0,     //!< Static text element.
    SDMForm_TextField = 1,      //!< Text input field.
    SDMForm_Checkbox = 2,       //!< Single checkbox.
    SDMForm_PathSelect = 3,     //!< File/folder path field/selector.
    SDMForm_ItemSelect = 4,     //!< One of many item select. For instance, a pop-up menu or scrolling list.
};

//! @brief Form element type.
typedef uint32_t SDMFormElementType;

/*!
 * @brief Flags for user input form elements.
 *
 * These enumerators are bit masks that are intended to be bitwise-or'd together to be used in the
 * SDMFormElement::flags field.
 *
 * Some flags apply to only specific form element types. The flag documentation indicates when this
 * is the case.
 */
enum SDMFormElementFlagsEnum {
    SDMForm_IsOptional = (1 << 0),  //!< The element does not have to be filled/set.
    SDMForm_IsDisabled = (1 << 1),  //!< The element is disabled.
    SDMForm_IsHidden = (1 << 2),    //!< The element should not be displayed.
    SDMForm_IsCacheable = (1 << 3), //!< The element's value may be cached for redisplay.
    SDMForm_IsPassword = (1 << 8),  //!< Text field only. The field contains a password; entered text should be masked.
    SDMForm_IsTristate = (1 << 12), //!< Checkbox only. Enable the mixed state value.
    SDMForm_IsFolder = (1 << 16),   //!< Path select only. Select folder instead of file.
};

/*!
 * @brief Item details for #SDMCallbacks::presentForm callback.
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
         * @brief Static text element descriptor (#SDMForm_StaticText).
         */
        struct {
            //! The static text string value. Must not be NULL.
            const char *text;
        } staticText;

        /*!
         * @brief Text field element descriptor (#SDMForm_TextField).
         */
        struct {
            //! Buffer for text field's contents.
            //!
            //! On input: The initial value for the text field.<br/>
            //! On output: Filled with the field's contents on form completion..
            //!
            //! The buffer's input and output values are null-terminated, UTF-8 encoded strings.
            //!
            //! Must not be NULL.
            char *textBuffer;

            //! Size in bytes of the buffer pointed to by `textBuffer`.
            //!
            //! The maximum entered text length will not be greater than this value - 1 (to accomodate the terminating
            //! null). Must be greater than 0.
            uint32_t textBufferLength;
        } textField;

        /*!
         * @brief Checkbox element descriptor (#SDMForm_Checkbox).
         */
        struct {
            //! On input: Initial checkbox state.<br/>
            //! On output: Output checkbox state.
            //!
            //! Must not be NULL.
            SDMControlState *state;
        } checkbox;

        /*!
         * @brief Path select element descriptor (#SDMForm_PathSelect).
         *
         * By default a file is selected. If a folder is required, the #SDMForm_IsFolder flag can be set.
         */
        struct {
            //! Array of filename extensions to allow. May be NULL, in which case any file
            //! can be selected. Not used if #SDMForm_IsFolder is set.
            const char **extensions;

            //! Number of entries in the filename extensions array. Not used if #SDMForm_IsFolder is set.
            uint32_t extensionsCount;

            //! Buffer for selected file path.
            //!
            //! On input: Initial UTF-8 encoded path value.<br/>
            //! On output: Filled with the selected null-terminated UTF-8 encoded path on success.
            //!
            //! Must not be NULL.
            char *pathBuffer;

            //! Size in bytes of the buffer pointed to by `pathBuffer`.
            //!
            //! The maximum entered text length will not be greater than this value - 1 (for the terminating null).
            //! Must be greater than 0.
            uint32_t pathBufferLength;
        } pathSelect;

        /*!
         * @brief Item select element descriptor (#SDMForm_ItemSelect).
         */
        struct {
            //! Pointer to array of item descriptors. Must not be NULL.
            const SDMFormItemInfo *items;

            //! Number of elements of the @a items array.
            uint32_t itemCount;

            //! On input: Initial selected item.<br/>
            //! On output: Set to the index of the selected item.
            //!
            //! The index is base 0. A value of -1 is used to indicate "no selection".
            //! Must not be NULL.
            int32_t *selectionIndex;
        } itemSelect;
    };
} SDMFormElement;

/*!
 * @brief Descriptor for a user input form.
 *
 * A form consists of an array of UI element descriptions.
 */
typedef struct SDMForm {
    const char *id;         //!< Unique form ID string used for automation-supplied input. Must be a valid C identifier.
    const char *title;      //!< Title of the form. Must not be NULL.
    const char *info;       //!< Optional additional description of the form. May be NULL.
    uint32_t flags;         //!< Mask for flags that apply to the entire form. Reserved for future use.
    const SDMFormElement **elements; //!< Pointer to the array of element pointers.
    uint32_t elementCount;  //!< Number of element pointers.
} SDMForm;

/*!
 * @brief Transfer sizes for memory transfer callbacks.
 *
 * These enums are used with the #SDMCallbacks::readMemory and #SDMCallbacks::writeMemory
 * callbacks.
 *
 * Note that not all MEM-APs support all transfer sizes. If a transfer with an unsupported size is
 * attempted, a SDM_Fail_Unsupported_Transfer_Size error will be returned.
 */
enum SDMTransferSizeEnum {
    SDMTransferSize_8 = 8,    //!< Perform an 8-bit memory transfer.
    SDMTransferSize_16 = 16,  //!< Perform a 16-bit memory transfer.
    SDMTransferSize_32 = 32,  //!< Perform a 32-bit memory transfer.
    SDMTransferSize_64 = 64,  //!< Perform a 64-bit memory transfer.
};

//! @brief Memory transfer size parameter type.
typedef uint32_t SDMTransferSize;

/*!
 * @brief Arm ADI architecture-specific memory transfer attributes.
 *
 * These enumerators define bit masks for the memory transfer attributes that can be specified
 * with a MEM-AP. Not all available memory attributes are provided.
 */
enum SDMArmADITransferAttributesEnum {
    //! @brief Abstracted nonsecure attribute.
    //!
    //! The debugger must translate this attribute to the appropriate HNONSEC (AHB) or AxPROT[1] (AXI).
    SDMTransferAttr_ArmADI_Nonsecure = 0x0001,

    //! @brief Abstracted non-privileged attribute.
    //!
    //! The debugger must translate this attribute to the appropriate HPROT[1] (AHB) or AxPROT[0] (AXI).
    SDMTransferAttr_ArmADI_NonPrivileded = 0x0002,

    //! @brief Flag indicating #SDMTransferAttr_ArmADI_DirectAttrMask should be used.
    SDMTransferAttr_ArmADI_DirectAttrEnable = 0x0080,

    //! @brief Mask for passing direct memory transfer attributes.
    //!
    //! If #SDMTransferAttr_ArmADI_DirectAttrEnable is set, any bits within this mask are passed directly as the
    //! MEM-AP CSW.PROT field.
    SDMTransferAttr_ArmADI_DirectAttrMask = 0x7F00,
};

/*!
 * @brief Supported types of device descriptors.
 */
enum SDMDeviceTypeEnum {
    SDMDeviceType_ArmADI_AP = 0,                    //!< Arm ADI Access Port device.
    SDMDeviceType_ArmADI_CoreSightComponent = 1,    //!< Arm ADI memory mapped CoreSight component accessible through a MEM-AP or the DP.
};

//! @brief Type for device type enum parameter.
typedef uint32_t SDMDeviceType;

/*!
 * @brief Descriptor for the target device in memory and AP transfer callbacks.
 */
typedef struct SDMDeviceDescriptor {
    SDMDeviceType deviceType; //!< The type of target device described by this descriptor.
    union {
        /*!
         * @brief Arm ADI Access Port device type (#SDMDeviceType_ArmADI_AP).
         */
        struct {
            //! Debug Port index. The first DP is index 0.
            uint8_t dpIndex;

            //! For v1 APs in ADIv5: 8-bit AP index. Only the low 8 bits are used.<br/>
            //! For v2 APs in ADIv6: AP base address, up to 64-bit.
            uint64_t address;
        } armAP;

        /*!
         * @brief Arm ADI CoreSight component device type (#SDMDeviceType_ArmADI_CoreSightComponent).
         */
        struct {
            //! Debug Port index. The first DP is index 0.
            uint8_t dpIndex;

            //! Pointer to the descriptor for the MEM-AP through which the component is accessed.
            //!
            //! For ADIv6, this may be NULL to indicate that the component is within the DP address space.
            const struct SDMDeviceDescriptor *memAp;

            //! Base address of the component with the specified MEM-APs address space.
            uint64_t baseAddress;
        } armCoreSightComponent;
    };
} SDMDeviceDescriptor;

/*!
 * @brief Register access operation enum.
 *
 * These enums are used to specify the operation type of an individual #SDMRegisterAccess.
 */
enum SDMRegisterAccessOpEnum {
    //! @brief Register read.
    SDMRegisterAccessOp_Read = 1,

    //! @brief Register write.
    SDMRegisterAccessOp_Write = 2,

    //! @brief Register poll.
    //!
    //! Repeatedly read register until an expected value or retry limit reached.
    SDMRegisterAccessOp_Poll = 3,
};

//! @brief Type for register access operation.
typedef uint32_t SDMRegisterAccessOp;

/*!
 * @brief Details for individual register access.
 */
typedef struct SDMRegisterAccess {
    //! @brief Register address.
    //!
    //! Interpretation depends on the device type. Typically either an address within a memory space
    //! or an offset from the device's base.
    uint64_t address;

    //! @brief Register access operation.
    SDMRegisterAccessOp op;

    //! @brief Register value.
    //!
    //! For #SDMRegisterAccessOp_Read, [out] read value.<br/>
    //! For #SDMRegisterAccessOp_Write, [in] write value.<br/>
    //! For #SDMRegisterAccessOp_Poll, [in] poll match value.
    //!
    //! Must not be NULL.
    uint32_t *value;

    //! @brief Poll mask to match regValue.
    //!
    //! Only valid for #SDMRegisterAccessOp_Poll.
    uint32_t pollMask;

    //! @brief Poll retry count.
    //!
    //! Only valid for #SDMRegisterAccessOp_Poll.
    //! Zero indicates retry forever, although host or probe may have an upper limit, or may interrupt.
    size_t retries;
} SDMRegisterAccess;

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
    //! @brief Debug architecture-specific callbacks. Reserved for future use.
    void *architectureCallbacks;

    //! @name Progress
    //@{
    /*!
     * @brief Inform the debugger of the current authentication progress.
     *
     * This callback should only be invoked during a call to the SDMAuthenticate() API. Otherwise
     * calls will be ignored.
     *
     * Host support for reporting progress is optional.
     *
     * @param[in] progressMessage
     * @param[in] percentComplete
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
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    void (*setErrorMessage)(const char *errorMessage, const char *errorDetails, void *refcon);
    //@}

    //! @name Target reset
    //@{
    /*!
     * @brief Reset assertion stage.
     * @param[in] resetType One of the #SDMResetTypeEnum enumerators.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     *
     * @retval SDMReturnCode_Success Reset stage completed.
     * @retval SDMReturnCode_RequestFailed
     * @retval SDMReturnCode_IOError
     * @retval SDMReturnCode_TimeoutError
     */
    SDMReturnCode (*resetStart)(SDMResetType resetType, void *refcon);

    /*!
     * @brief Reset deassertion stage.
     * @param[in] resetType One of the #SDMResetTypeEnum enumerators. Must be the same value passed to
     *  `resetStart()`.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     *
     * @retval SDMReturnCode_Success Reset stage completed.
     * @retval SDMReturnCode_RequestFailed
     * @retval SDMReturnCode_IOError
     * @retval SDMReturnCode_TimeoutError
     */
    SDMReturnCode (*resetFinish)(SDMResetType resetType, void *refcon);
    //@}

    //! @name Memory accesses
    //!
    //! The _device_ parameter must be a pointer to a device descriptor of valid type as defined by the
    //! debug architecture. The _address_ parameter is always the address to access within the memory space
    //! controlled by the specified device.
    //!
    //! Arm ADIv5 and ADIv6 debug architecture allowed device types:
    //! - #SDMDeviceType_ArmADI_AP
    //!     - If the specified AP is not a MEM-AP, the resulting behaviour is undefined.
    //!     - The _address_ parameter is the address within the memory space accessible through the MEM-AP.
    //! - #SDMDeviceType_ArmADI_CoreSightComponent
    //!     - The _address_ parameter is an offset relative to the base address of the CoreSight component's
    //!         4 kB memory region.
    //!     - This device type also has an associated MEM-AP set in the device descriptor.
    //!
    //! Addresses must be aligned to transfer size.
    //@{
    /*!
     * @brief Read target memory.
     *
     * @param[in] device Pointer to descriptor for device through which the read will be performed.
     * @param[in] address Memory address of the data to read.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[in] transferCount Number of memory elements of size _transferSize_ to read.
     * @param[in] attributes Debug-architecture-defined set of attributes that will apply to the transfer, such
     *  as Non-secure, Privileged, Cacheable, and so on. For Arm ADI, this is a value produced by OR'ing
     *  the enums defined in #SDMArmADITransferAttributesEnum.
     * @param[out] data Buffer where read data will be written. Must be at least _transferSize_ * _transferCount_
     *  bytes in length.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     * @retval SDMReturnCode_Success The read completed successfully and _value_ holds the data.
     * @retval SDMReturnCode_No_Response
     *
     * @retval SDMReturnCode_Success Transfer completed.
     * @retval SDMReturnCode_InvalidArgument
     * @retval SDMReturnCode_TransferFault
     * @retval SDMReturnCode_TransferError
     * @retval SDMReturnCode_UnsupportedTransferSize
     * @retval SDMReturnCode_TimeoutError
     */
    SDMReturnCode (*readMemory)(
        const SDMDeviceDescriptor *device,
        uint64_t address,
        SDMTransferSize transferSize,
        size_t transferCount,
        uint32_t attributes,
        void *data,
        void *refcon);

    /*!
     * @brief Write target memory.
     *
     * @param[in] device Pointer to descriptor for device through which the write will be performed.
     * @param[in] address Memory address of the data to write.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[in] transferCount Number of memory elements of size _transferSize_ to write.
     * @param[in] attributes Debug-architecture-defined set of attributes that will apply to the transfer, such
     *  as Non-secure, Privileged, Cacheable, and so on. For Arm ADI, this is a value produced by OR'ing
     *  the enums defined in #SDMArmADITransferAttributesEnum.
     * @param[in] data Buffer from where data to be written is read. Must be at least _transferSize_ * _transferCount_
     *  bytes in length.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     *
     * @retval SDMReturnCode_Success Transfer completed.
     * @retval SDMReturnCode_InvalidArgument
     * @retval SDMReturnCode_TransferFault
     * @retval SDMReturnCode_TransferError
     * @retval SDMReturnCode_UnsupportedTransferSize
     * @retval SDMReturnCode_TimeoutError
     */
    SDMReturnCode (*writeMemory)(
        const SDMDeviceDescriptor *device,
        uint64_t address,
        SDMTransferSize transferSize,
        size_t transferCount,
        uint32_t attributes,
        const void *value,
        void *refcon);
    //@}

    //! @name Register accesses
    //@{
    /*!
     * @brief Perform a series of device registers accesses.
     *
     * A sequence of zero or more read, write, or poll operations is performed in the order specified. Operations
     * may be mixed in any combination.
     *
     * For poll operations, the indicated register is repeatedly read as fast as the probe and interface allow.
     * Each read value is ANDed with SDMRegisterAccess::pollMask and the result compared with
     * *SDMRegisterAccess::value. If the comparison is a match, polling stops and the access sequence moves to
     * the next operation (or terminates).
     *
     * All register reads and writes are of the same size, specified by the _transferSize_ parameter. In v1.0 of
     * the SDM API, only 32-bit transfers (#SDMTransferSize_32) are allowed.
     *
     * The _device_ parameter must be a pointer to a device descriptor of valid type as defined by the
     * debug architecture.
     *
     * Arm ADIv5 and ADIv6 debug architecture allowed device types:
     * - #SDMDeviceType_ArmADI_AP
     *      - Can be any type of AP.
     *      - The _SDMRegisterAccess::address_ value is the address of one of the AP's registers.
     * - #SDMDeviceType_ArmADI_CoreSightComponent
     *      - The _SDMRegisterAccess::address_ value is an offset relative to the base address of the CoreSight
     *          component's 4 kB memory region.
     *      - This device type also has an associated MEM-AP set in the device descriptor.
     *
     * Depending on the SDM host and debug probe capabilities, the register accesses may be performed one at a
     * time or be sent as a group to the debug probe. The timing interval between separate accesses is not guaranteed.
     *
     * Example that waits for a FIFO to have room, then fills it:
     *
     * ```c
     * // Register value variables.
     * uint32_t statusWord = 0; // Match (reg & FIFO_STATUS_FULL_MASK) == 0.
     * uint32_t dataWord; // Set to value to write into FIFO.
     *
     * // Sequence of register accesses to perform.
     * SDMRegisterAccess accesses[] = {
     *         {
     *             .address = FIFO_STATUS_ADDR,
     *             .op = SDMRegisterAccessOp_Poll,
     *             .value = &statusWord,
     *             .pollMask = FIFO_STATUS_FULL_MASK,
     *             .retries = 0, // Infinite wait.
     *         },
     *         {
     *             .address = FIFO_DATA_ADDR,
     *             .op = SDMRegisterAccessOp_Write,
     *             .value = &dataWord,
     *         },
     *     };
     *
     * size_t accessesCompleted = 0;
     * SDMReturnCode result = params->callbacks->registerAccess(
     *                                                  &myDevice,
     *                                                  SDMTransferSize_32,
     *                                                  &accesses,
     *                                                  ARRAY_SIZE(accesses),
     *                                                  &accessesCompleted,
     *                                                  params->refcon
     *                                                  );
     * ```
     *
     * @param[in] device Pointer to descriptor for device through which the read will be performed.
     * @param[in] transferSize The size of all register accesses in this call. Only #SDMTransferSize_32 is supported in
     *  SDM API v1.0.
     * @param[in,out] accesses Array of SDMRegisterAccess register accesses descriptors.
     * @param[in] accessCount Number of register accesses. The _accesses_ parameter must point to an array containing
     *  at least this number of elements. A value of zero is allowed, and results in no operation.
     * @param[out] accessesCompleted Number of register accesses completed. On success this will equal accessCount.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     *
     * @retval SDMReturnCode_Success Transfer completed.
     * @retval SDMReturnCode_InvalidArgument
     * @retval SDMReturnCode_TransferFault
     * @retval SDMReturnCode_TransferError
     * @retval SDMReturnCode_UnsupportedTransferSize
     * @retval SDMReturnCode_TimeoutError
     */
    SDMReturnCode (*registerAccess)(
        const SDMDeviceDescriptor *device,
        SDMTransferSize transferSize,
        const SDMRegisterAccess *accesses,
        size_t accessCount,
        size_t *accessesCompleted,
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
     * @retval SDMReturnCode_Success User provided requested input.
     * @retval SDMReturnCode_InvalidArgument There was an issue with the form descriptors.
     * @retval SDMReturnCode_UserCancelled The user cancelled.
     */
    SDMReturnCode (*presentForm)(const SDMForm *form, void *refcon);
    //@}

} SDMCallbacks;

/*!
 * @brief Debugger connection modes.
 */
enum SDMConnectModeEnum {
    //! @brief Connect in order to load application into memory before running.
    //!
    //! When the SDM authenticates, the application that will be debugged has not yet been loaded.
    SDMConnectMode_Load = 0,

    //! @brief Connect and reset to run previously loaded application.
    //!
    //! When the SDM authenticates, the application that will be debugged has already been loaded.
    SDMConnectMode_Restart = 1,

    //! @brief Attach to running application.
    //!
    //! The SDM should not reset the target.
    SDMConnectMode_Attach = 2,
};

//! @brief Type for connect mode enum.
typedef uint32_t SDMConnectMode;

/*!
 * @brief Parameters passed to SDMOpen() by the debugger.
 */
typedef struct SDMOpenParameters {
    struct {
        uint16_t major; /*!< Client interface major version. */
        uint16_t minor; /*!< Client interface minor version. */
    } version; /*!< Client interface version. @see #SDMVersionEnum. */
    SDMDebugArchitecture debugArchitecture; /*!< Debug architecture for the target. */
    SDMCallbacks *callbacks; /*!< Callback collection */
    void *refcon; /*!< Debugger-supplied value passed to each of the callbacks. */
    const char *resourcesDirectoryPath; /*!< Absolute path to the directory containing the SDM plugin's resources. */
    const char *manifestFilePath; /*!< Absolute path to the manifest XML file. */
    uint32_t flags; /*!< Flags passed to the SDM from the debugger. Reserved for future use. */
    const char **locales; /*!< Pointer to a NULL-terminated array of IETF BCP 47 language tags, e.g. "en-US", "fr-FR", "sv", etc. The  tags are sorted in decreasing priority order. */
    SDMConnectMode connectMode; /*!< Debugger connect mode. */
} SDMOpenParameters;

/*!
 * @brief Parameters passed by the debugger to the SDMAuthenticate() API.
 */
typedef struct SDMAuthenticateParameters {
    SDMBool isLastAuthentication; //!< False if at least one subsequent call to SDMAuthenticate() is expected.
} SDMAuthenticateParameters;

/*!
 * @brief Target protection states.
 *
 * These enums represent potential states for the target security controls. The exact definition of each state
 * is target-specific and can depend on the configuration of the device.
 */
enum SDMTargetProtectionStateEnum {
    SDMTargetProtectionState_Unlocked = 0,  //!< Target is accessible.
    SDMTargetProtectionState_Locked = 1,    //!< Target requires authentication to enable debug access.
};

//! @brief Type for target protection state enum.
typedef uint32_t SDMTargetProtectionState;

#ifdef __cplusplus
extern "C" {
#endif

//! @name API
//@{
/*!
 * @brief This function is called by the debugger to start a secure debug session with the remote platform.
 *
 * @param[in] handle New handle to the SDM instance.
 * @param[in] params Connection details and callbacks. This pointer and all nested pointers will remain valid
 *  until SDMClose() is called, so the plugin can cache the value for later use without having to copy all
 *  the data.
 *
 * @retval SDMReturnCode_Success SDM plugin initialized successfully.
 * @retval SDMReturnCode_RequestFailed
 */
SDM_EXTERN SDMReturnCode SDMOpen(SDMHandle *handle, const SDMOpenParameters *params);

/*!
 * @brief Determine whether the target is currently locked.
 *
 * This API is primarily intended to be used for determining whether the user should be requested to
 * authenticate in order to debug the target. For most devices, "locked" means that the device is in the
 * production lifecycle state and has not been previously unlocked by authentication.
 *
 * Even if a device is locked, some access permissions may be enabled by default. For instance, the non-secure
 * world may be debuggable, while the secure world can only be debugged after authentication. In this case
 * #SDMTargetProtectionState_Locked would be returned because the target is in its default production
 * lifecycle debug access configuration. Similarly, an unlocked device does not necessary have all available,
 * access permissions enabled. A previously performed authentication may have only unlocked a subset
 * of permissions.
 *
 * @param[in] handle Handle to the SDM instance.
 * @param[out] state On successful return, the current debug access protection state for the target.
 *
 * @retval SDMReturnCode_Success Target state determined.
 * @retval SDMReturnCode_UnsupportedOperation The target does not support a method to determine its
 *  current protection state.
 * @retval SDMReturnCode_RequestFailed
 * @retval SDMReturnCode_IOError
 * @retval SDMReturnCode_TimeoutError
 */
SDM_EXTERN SDMReturnCode SDMGetTargetProtectionState(SDMHandle handle, SDMTargetProtectionState *state);

/*!
 * @brief Perform authentication to unlock debug access.
 *
 * More than one call to this API is allowed, if the "multiple-authentications" feature is enabled
 * in the SDM XML.
 *
 * @param[in] handle Handle to the SDM instance.
 * @param[in] params Parameters for the authentication. The pointer only needs to be valid during
 *      the call to this API.
 *
 * @retval SDMReturnCode_Success Authentication succeeded.
 * @retval SDMReturnCode_RequestFailed
 * @retval SDMReturnCode_InvalidUserCredentials
 * @retval SDMReturnCode_UserCancelled
 * @retval SDMReturnCode_IOError
 * @retval SDMReturnCode_TimeoutError
 */
SDM_EXTERN SDMReturnCode SDMAuthenticate(SDMHandle handle, const SDMAuthenticateParameters *params);

/*!
 * @brief Called by the debugger to resume the boot of the remote platform.
 *
 * This API is only a request from the host to the plugin. It may be implemented as a no-op. Note also
 * that the effect may be different depending on the runtime environment of the target, as well as the
 * SDMOpen::connectMode value.
 *
 * It is typically called after the debugger places its breakpoints at the booting debugged system.
 * It is only useful if the debugged system supports the introduction of debug certificate in the
 * early boot stages, otherwise if the debugged system processes the secure debug certificate at runtime,
 * it does not wait for the resume command.
 *
 * @param[in] handle Handle to the SDM instance.
 *
 * @retval SDMReturnCode_Success The target device has resumed its boot process.
 * @retval SDMReturnCode_RequestFailed
 * @retval SDMReturnCode_UnsupportedOperation
 */
SDM_EXTERN SDMReturnCode SDMResumeBoot(SDMHandle handle);

/*!
 * @brief Close the SDM session.
 *
 * This call ends a secure debug session with the remote platform. The SDM may free any
 * resources it allocated.
 *
 * @param[in] handle Handle to the SDM instance.
 *
 * @retval SDMReturnCode_Success SDM plugin was successfully uninitialized.
 */
SDM_EXTERN SDMReturnCode SDMClose(SDMHandle handle);
//@}

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _SECURE_DEBUG_MANAGER_H_ */
