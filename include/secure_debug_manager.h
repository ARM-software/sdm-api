/*
 * Copyright (c) 2020 Arm Ltd
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
#include <stdint.h>

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
 *
 * These version numbers are used for #SDMOpenParameters::version.
 */
enum SDMVersion {
    SDM_CurrentVersion = 1, /*!< Current API version. */
};

/*!
 * @brief Return codes for SDM APIs and callbacks.
 */
enum SDMReturnCode {
    SDM_Success = 0, /*!< Success, no error */
    SDM_Fail_No_Response = 1, /*!< No response, timeout */
    SDM_Fail_Unsupported_Transfer_Size = 2, /*!< MEM-AP does not support the requested transfer size. */
    SDM_Fail_User_Credentials = 3, /*!< Invalid user credentials for the debugged platform */
    SDM_Fail_IO = 4, /*!< Failed to transmit/receive data to/from the device */
    SDM_Fail_Internal = 5, /*!< An unspecified internal error occurred */
    SDM_Fail_Parameter = 6, /*!< Invalid parameter */
};

//! @brief Type for return codes.
typedef uint32_t SDMReturnCode;

//! @brief Opaque handle to an opened instance of the SDM.
typedef struct _SDMOpaqueHandle * SDMHandle;

/*!
 * @brief Possible debug architectures.
 */
typedef enum SDMDebugArchitecture {
    SDM_Arm_ADIv5 = 0,  //!< Arm ADIv5 debug architecture. Uses #SDMArmADICallbacks.
    SDM_Arm_ADIv6 = 1,  //!< Arm ADIv6 debug architecture. Uses #SDMArmADICallbacks.
    SDM_Nexus5001 = 2,  //!< Nexus 5001 (IEEE-ISTO 5001-2003) debug architecture. Uses #SDMNexus5001Callbacks.
} SDMDebugArchitecture;

/*!
 * @brief Flags for SDMOpenParameters.
 */
enum SDMFlags {
    SDM_IsDefaultDeviceInfoValid = (1 << 0),
};

/*!
 * @brief Transfer sizes for memory transfer callbacks.
 *
 * These enums are used with the #SDMCallbacks::readMemory and #SDMCallbacks::writeMemory
 * callbacks.
 *
 * Note that not all MEM-APs support all transfer sizes. If a transfer with an unsupported size is
 * attempted, a SDM_Fail_Unsupported_Transfer_Size error will be returned.
 */
enum SDMMemorySize {
    SDM_Memory8 = 8,    //!< Perform an 8-bit memory transfer.
    SDM_Memory16 = 16,  //!< Perform a 16-bit memory transfer.
    SDM_Memory32 = 32,  //!< Perform a 32-bit memory transfer.
    SDM_Memory64 = 64,  //!< Perform a 64-bit memory transfer.
};

//! @brief Memory size parameter type.
typedef uint32_t SDMMemorySize;

/*!
 * @brief Item details for #SDMCallbacks::selectItem callback.
 *
 * The item info consists of a pair of strings. The first is a short name for the item. This will
 * appear in the list from which the user selects an item. When an item is selected, the long
 * description should be shown to provide the user more information.
 */
typedef struct SDMItemInfo {
    const char *itemShortName; /*!< Item name that will appear in the list. Must not be NULL. */
    const char *itemLongDescription; /*!< Optional descriptive text for this item. Can be NULL. */
} SDMItemInfo;

enum {
    //! @brief Value indicating the default AP should be used.
    //!
    //! Passed for the _device_ parameter of AP and memmory access callbacks.
    SDM_DefaultDevice = -1LL,
};

// Forward declare.
struct SDMNexus5001Callbacks;

/*!
 * @brief Arm ADI architecture-specific callbacks.
 */
typedef struct SDMArmADICallbacks {
    //! @name Debug sequences
    //@{
    /*!
     * @brief Invoke a CMSIS debug sequence by name.
     */
    SDMReturnCode (*invokeDebugSequence)(const char *name, const char *pname);
    //@}

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
 * Debug archicture-specific callbacks are accessed through the architectureCallbacks member.
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
    //! @brief Debug archicture-specific callbacks.
    SDMArchitectureCallbacks architectureCallbacks;

    //! @name User interaction
    //@{
    /*!
     * @brief Inform the debugger of the current authentication progress.
     *
     * This callback should only be invoked during a call to the SDM_Authenticate() API. Otherwise
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

    //! @name Memory accesses
    //!
    //! The _device_ parameter indicates the a debug-architecture-defined address fior the interface
    //! to the memory system. It can also be set to #SDM_DefaultDevice, and the debugger will use a
    //! default memory interface device. For the Arm ADI architecture, this is the address of the
    //! MEM-AP to use.
    //!
    //! The _address_ parameter is always the address to access within the memory space controlled
    //! by the selected MEM-AP.
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
 * @brief Supported types of default devices.
 */
enum SDMDefaultDeviceType {
    SDM_ArmADI_AP = 0,
    SDM_ArmADI_MEM_AP = 1,
    SDM_ArmADI_CoreSight_Component = 2,
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
    uint32_t version; /*!< Client interface version, should be set to #SDM_CurrentVersion. */
    SDMDebugArchitecture debugArchitecture; /*!< Debug architecture for the target. */
    SDMCallbacks callbacks; /*!< Callback collection */
    void *refcon; /*!< Debugger-supplied value passed to each of the callbacks. */
    uint32_t flags; /*!< Flags passed to the SDM from the debugger. */
    const char *userSelectedFilePath; /*!< Path to file chosen by the user in connection config. Not valid if NULL. */
    SDMDefaultDeviceInfo defaultDeviceInfo; /*!< Information about the default device. */
} SDMOpenParameters;

/*!
 * @brief Possible execution contexts.
 */
enum SDMExecutionContext {
    SDM_Boot_ROM = 0,
    SDM_Boot_Loader = 1,
    SDM_Runtime = 2,
};

//! @brief Type for exection context enum parameter.
typedef uint32_t SDMExecutionContext;

/*!
 * @brief Parameters passed by the debugger to the SDM_Authenticate() API.
 */
typedef struct SDMAuthenticateParameters {
    SDMExecutionContext expectedExecutionContext;
    uint8_t isLastAuthentication;
} SDMAuthenticateParameters;

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief This function is called by the debugger to start a secure debug session with the remote platform
 *
 * The caller is expected to set the resetType to a value other than link SDM_none when
 * it knows that the debugged platform implements the secure debug certificate processing in its ROM.
 * In the case of SDM_nSRSTReset, if the External COM port driver implementation does not have
 * nSRST capability the caller can provide callbacks pDebugIf->callbacks->f_nSRSTStage1 and
 * pDebugIf->callbacks->f_nSRSTStage2.
 *
 * @param[out] handle New handle to the SDM instance.
 * @param[in] params Connection details and callbacks. This pointer and all pointers within the
 *     structure must remain valid until SDM_Close() is called.
 */
SDM_EXTERN SDMReturnCode SDM_Open(SDMHandle *handle, const SDMOpenParameters *params);

// SDM_EXTERN SDMReturnCode SDM_PreAuthentication(SDMHandle handle);

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
