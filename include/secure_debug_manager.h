/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*!
 * @addtogroup secure_debug_manager Secure Debug Manager
 * @brief Top-level API for secure debug library.
 *
 * The Secure Debug Manager API (SDM API) defines the interface used by debug clients, such as
 * debuggers, IDEs, and other tools, for performing protocol-independant secure debug
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

/*!
 * @brief API version number constants.
 */
enum _SDMVersion {
    SDM_CurrentVersion = 1, /*!< Use for SDMOopenParams::version */
};

/*!
 * @brief Reset method to use.
 */
typedef enum SDMResetType {
    SDM_none = 0, /*!< No reset*/
//     SDM_COMPortReset = 1, /*!< SDC-600 External COM port remote reboot */
    SDM_HardwareReset = 2, /*!< Full system reset via nSRST pin */
    SDM_SoftwareReset
} SDMResetType;

/*!
 * @brief Return codes for SDM APIs and callbacks.
 */
typedef enum SDMReturnCode {
    SDM_Success = 0, /*!< Success, no error */
    SDM_Fail_No_Response = 1, /*!< No response, timeout */
    SDM_Fail_Unsupported_Transfer_Size = 2, /*!< MEM-AP does not support the requested transfer size. */
    SDM_Fail_User_Credentials = 3, /*!< Invalid user credentials for the debugged platform */
    SDM_Fail_IO = 4, /*!< Failed to transmit/receive data to/from the device */
    SDM_Fail_Internal = 5, /*!< An unspecified internal error occurred */
} SDMReturnCode;

enum _SDMFlags {
    SDM_IsADIv6
};

/*!
 * @brief Transfer sizes for memory transfer callbacks.
 *
 * These enums are used with the SDMCallbacks::readMemory() and SDMCallbacks::writeMemory()
 * callbacks.
 *
 * Note that not all MEM-APs support all transfer sizes. If a transfer with an unsupported size is
 * attempted, a SDM_Fail_Unsupported_Transfer_Size error will be returned.
 */
typedef enum SDMMemorySize {
    SDM_Memory8,    //!< Perform an 8-bit memory transfer.
    SDM_Memory16,   //!< Perform a 16-bit memory transfer.
    SDM_Memory32,   //!< Perform a 32-bit memory transfer.
    SDM_Memory64,   //!< Perform a 64-bit memory transfer.
} SDMMemorySize;

typedef struct _SDMOpaqueHandle * SDMHandle;

/*!
 * @brief Item details for SDMCallbacks::selectItem() callback.
 *
 * The item info consists of a pair of strings. The first is a short name for the item. This will
 * appear in the list from which the user selects an item. When an item is selected, the long
 * description should be shown to provide the user more information.
 */
typedef struct _SDMItemInfo {
    const char *itemShortName; /*!< Item name that will appear in the list. Must not be NULL. */
    const char *itemLongDescription; /*!< Optional descriptive text for this item. Can be NULL. */
} SDMItemInfo;

enum {
    //! @brief Value indicating the default AP should be used.
    //!
    //! Passed for the _device_ parameter of AP and memmory access callbacks.
    SDM_DefaultDevice = -1,
};

/*!
 * \brief Collection of callback functions provided by the debugger.
 *
 * This interface is not designed for performance but for simplicity.
 *
 * Only bulk 8-bit r/w callbacks are provided to keep the set of callbacks simple. The implementation
 * must guarantee that aligned/sized transfers. [TODO]
 *
 * All callbacks accept a _refcon_ parameter. The SDM _must_ pass the value of the
 * SDMOpenParameters::refcon field that was provided to SDMOpen().
 */
typedef struct SDMCallbacks {
    //! @name User interaction
    //@{
    /*!
     * @brief Inform the debugger of the current authentication progress.
     *
     * This callback should only be invoked during a call to the SDMAuthenticate() API. Otherwise
     * calls will be ignored.
     *
     * @param progressMessage
     * @param percentComplete
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    void (*updateProgress)(const char *progressMessage, uint8_t percentComplete, void *refcon);

    /*!
     * @brief Ask the user to choose an item from the list.
     *
     * The intended use is to allow the user to select a credential or other configuration item
     * from the provided list.
     *
     * @param[in] title
     * @param[in] count
     * @param[in] items
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     * @return The index of the item the user selected, or -1 if the user cancelled.
     */
    int32_t (*selectItem)(const char *title, uint32_t count, const SDMItemInfo *items, void *refcon);
    //@}

    //! @name Target reset
    //@{
    /*!
     * @brief Reset stage 1.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*resetStart)(void *refcon);
    /*!
     * @brief Reset stage 2.
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*resetFinish)(void *refcon);
    //@}

// are these callbacks relative to the comm channel? -> YES
//  - makes more sense
//  - don't encode the AP address

// also support absolute addresses

    //! @name AP accesses
    //!
    //! The _device_ parameter indicates the address of the AP to access. It can also be set to
    //! @ref SDM_DefaultDevice, and the debugger will use a default AP. For ADIv5 systems, the
    //! AP address is an APSEL value in the range 0-255. For ADIv6 systems, the AP address is an
    //! APB address whose width depends on the target implementation.
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
    SDMReturnCode (*apWrite)(uint64_t device, uint64_t registerAddress, const uint32_t *value, void *refcon);
    //@}

    //! @name Memory accesses
    //!
    //! The _device_ parameter indicates the address of the MEM-AP to use. It can also be set to
    //! #SDM_DefaultDevice, and the debugger will use a default MEM-AP. The _address_ parameter
    //! is always the address to access within the memory space controlled by the selected MEM-AP.
    //!
    //! Addresses must be aligned to transfer size.
    //@{
    /*!
     * @brief Read one unit of memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address Memory address of the data to read.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[out] value
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     * @retval SDM_Success The read completed successfully and _value_ holds the data.
     * @retval SDM_Fail_No_Response
     */
    SDMReturnCode (*readMemory)(uint64_t device, uint64_t address, SDMMemorySize transferSize, void *value, void *refcon);
    /*!
     * @brief Write one unit of memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address Memory address of the data to write.
     * @param[in] transferSize Enum indicating the requested size of the transfer unit.
     * @param[in] value
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*writeMemory)(uint64_t device, uint64_t address, SDMMemorySize transferSize, const void *value, void *refcon);
    /*!
     * @brief Read a block of memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address
     * @param[in] byteCount
     * @param[out] data
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*readMemoryBulk)(uint64_t device, uint64_t address, uint32_t byteCount, uint8_t* data, void *refcon);
    /*!
     * @brief Write a block of memory.
     *
     * @param[in] device Address of the MEM-AP or #SDM_DefaultDevice.
     * @param[in] address
     * @param[in] byteCount
     * @param[in] data
     * @param[in] refcon Must be set to the reference value provided by the debugger through
     *  SDMOpenParameters::refcon.
     */
    SDMReturnCode (*writeMemoryBulk)(uint64_t device, uint64_t address, uint32_t byteCount, const uint8_t *data, void *refcon);
    //@}
} SDMCallbacks;

/*!
 * \brief Parameters passed to SDM_Open() by the debugger.
 */
typedef struct SDMOpenParameters {
    uint32_t version; /*!< Client interface version, should be set to #SDM_CurrentVersion. */
    SDMCallbacks *callbacks; /*!< Callback collection */
    void *refcon; /*!< Debugger-supplied value passed to each of the callbacks. */
    uint8_t adiVersion; /*!< ADI version of the target, either 5 or 6. */
    uint32_t flags; /*!< Flags passed to the SDM from the debugger. */
    const char *userSelectedFilePath; /*!< Path to file chosen by the user in connection config. Not valid if NULL. */
//    uint64_t commChannelBaseAddress; /*!< Base address of the communications channel. This can be an (APSEL<<24), AP address, or memory window base address. The comm link shared library must be selected to match the comm channel type represented by this address */
} SDMOpenParameters;

typedef struct SDMAuthenticateParams {
    SDMExecutionContext expectedExecutionContext;
    bool isLastAuthentication;
} SDMAuthenticateParams;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * This function is called by the debugger to start a secure debug session with the remote platform
 *
 * The caller is expected to set the resetType to a value other than link SDM_none when
 * it knows that the debugged platform implements the secure debug certificate processing in its ROM.
 * In the case of SDM_nSRSTReset, if the External COM port driver implementation does not have
 * nSRST capability the caller can provide callbacks pDebugIf->callbacks->f_nSRSTStage1 and
 * pDebugIf->callbacks->f_nSRSTStage2.
 *
 * @param[out] handle New handle to the SDM instance.
 * @param[in] params Connection details and callbacks.
 */
SDM_EXTERN SDMReturnCode SDM_Open(SDMHandle *handle, SDMOpenParameters *params);
// -> why was resetType removed? what implications does this have?

// SDM_EXTERN SDMReturnCode SDM_PreAuthentication(SDMHandle handle);

/*!
 * @brief Perform authentication to unlock debug access.
 *
 * More than one call to this API is allowed, if the "multiple-authentications" feature is enabled
 * in the SDM XML.
 *
 * @param[in] handle Handle to the SDM instance.
 * @param[in] params P
 */
SDM_EXTERN SDMReturnCode SDM_Authenticate(SDMHandle handle, const SDMAuthenticateParams *params);
// deal with
// - reset type/exec context
// - multiple auths

/*!
 * @brief Called by the debugger to resume the boot of the remote platform.
 *
 * It is typically called after the debugger places its breakpoints at the booting debugged system.
 * It is only useful if the debugged system supports the introduction of debug certificate in the
 * early boot stages, otherwise if the debugged system processes the secure debug certificate at runtime,
 * it does not wait for the resume command.
 *
 * The debugger can know this fact at the response of introduction of the secure debug certificate, which is
 * also the return value of {@link SDM_Open}. In case it happened at early boot then {@link SDM_Open} will
 * return SDM_SUCCESS_WAIT_RESUME. Otherwise if introduced at run time then {@link SDM_Open} will
 * return SDM_SUCCESS.
 *
 * In the 1st case, after the return of {@link SDM_Open} the user set breakpoints while the debugged system
 * is still waiting. After breakpoints are set, the debugger can call {@link SDM_ResumeBoot}
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
