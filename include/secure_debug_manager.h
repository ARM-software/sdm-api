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

 /**
 * \file
 *
 * \brief This header file is the entry point to Secure Debug Manager
 * functionality and includes all necessary definitions.
 */

#ifndef _SECURE_DEBUG_MANAGER_H_
#define _SECURE_DEBUG_MANAGER_H_

#include <stddef.h>

#ifdef _WIN32
#ifdef SDM_EXPORT_SYMBOLS
#define SDM_EXTERN __declspec(dllexport)
#else
#define SDM_EXTERN __declspec(dllimport)
#endif
#else
#define SDM_EXTERN
#endif

enum _SDMVersion {
    SDM_CurrentVersion = 1, /*!< Use for SDMDebugIf::version */
};

/**
 * \brief Reset method to perform during {@link SDM_Init} and {@link SDM_End}
 */
typedef enum SDMResetType {
    SDM_none = 0, /*!< No reset*/
//     SDM_COMPortReset = 1, /*!< SDC-600 External COM port remote reboot */
    SDM_HardwareReset = 2, /*!< Full system reset via nSRST pin */
    SDM_SoftwareReset
} SDMResetType;

/**
 * \brief SDM_Init() progress steps reported via f_progressIndicationCallbackFunc callback
 */
// TODO: revisit init steps
typedef enum SDMProgressStep {
    SDM_Reserved = 0, /*!< Unused */
    SDM_COM_Port_Init_Done = 1, /*!< SDC-600 COM port driver and COM port link initialized */
    SDM_Received_Expected_IDA_Response = 2, /*!< IDA response from debugged system has been verified */
    SDM_Sent_Get_SOC_ID = 3, /*!< 'Get SoC Id' command sent */
    SDM_Received_SOC_ID = 4, /*!< 'Get SoC Id' response received */
    SDM_Creating_Secure_Debug_Certificate = 5, /*!< Secure debug certificate requested from Authentication Token Provider */
    SDM_Received_Secure_Debug_Certificate = 6, /*!< Secure debug certificate received from Authentication Token Provider*/
    SDM_Sent_Secure_Debug_Certificate = 7, /*!< 'Introduce Debug Certificate' command sent */
    SDM_Complete = 8 /*!< 'Introduce Debug Certificate' response received */
} SDMProgressStep;

/**
 * \brief Return codes from SDM_* functionality.
 */
typedef enum SDMReturnCode {
    SDM_Success = 0, /*!< Success, no error */
    SDM_Success_Wait_Resume = 1, /*!< Success, debugged system waits for 'Resume Boot' command. {@link SDM_ResumeBoot} should follow */
    SDM_Fail_No_Response = 2, /*!< No response, timeout */
    SDM_Fail_Unexpected_Symbol = 3, /*!< Unexpected symbol received */
    SDM_Fail_Unsupported_Protocol_ID = 4, /*!< Unsupported remote platform ID */
    SDM_Fail_User_Credentials = 5, /*!< Invalid user credentials for the debugged platform */
    SDM_Fail_IO = 6, /*!< Failed to transmit/receive data to/from the device */
    SDM_Fail_Internal = 7, /*!< An unspecified internal error occurred */
} SDMReturnCode;

enum _SDMFlags {
    SDM_IsFilePathValid = (1 << 0), /*!< The userSelectedFilePath is valid */
};

typedef enum SDMMemorySize {
    SDM_Memory8,
    SDM_Memory16,
    SDM_Memory32,
} SDMMemorySize;

typedef struct _SDMOpaqueHandle * SDMHandle;

/**
 * @brief Item details for selectCredential callback.
 */
typedef struct _SDMItemInfo {
    const char* itemShortName;
    const char* itemLongDescription; /*!< Can be NULL. */
} SDMItemInfo;

/**
 * \brief Collection of callback functions provided by the debugger.
 *
 * This interface is not designed for performance but for simplicity.
 *
 * Only bulk 8-bit r/w callbacks are provided to keep the set of callbacks simple. The implementation
 * must guarantee that aligned/sized transfers. [TODO]
 */
typedef struct SDMCallbacks {
    // TODO: revisit progress steps
    void (*updateProgress)(const char *progressMessage, uint8_t percentComplete, void* refcon); /*!<  Progress report callback*/
    SDMReturnCode (*resetStart)(void* refcon); /*!<  nSRST stage 1 callback  */
    SDMReturnCode (*resetFinish)(void* refcon); /*!<  nSRST stage 2 callback  */

// are these callbacks relative to the comm channel? -> YES
//  - makes more sense
//  - don't encode the AP address

// also support absolute addresses

    // TODO deal with >32 bit AP addrs in ADIv6
    // separate AP base from reg offset?
    SDMReturnCode (*apRead)(uint64_t device, uint64_t registerAddress, uint32_t* value, void* refcon);
    SDMReturnCode (*apWrite)(uint64_t device, uint64_t registerAddress, const uint32_t* value, void* refcon);

    // Addresses must be aligned to transfer size.
    // - need to specify AP?

    // specify the device you want to access
    // address in a different parameter

    // mem-window @ 0x1a00_0000 (MEM-AP#2)

    // readMemory(SDMDefaultCommChannel, 0x100)

    SDMReturnCode (*readMemory)(uint64_t device, uint64_t address, SDMMemorySize transferSize, void* value, void* refcon);
    SDMReturnCode (*writeMemory)(uint64_t device, uint64_t address, SDMMemorySize transferSize, const void* value, void* refcon);
    SDMReturnCode (*readMemoryBulk)(uint64_t device, uint64_t address, uint32_t byteCount, uint8_t* data, void* refcon);
    SDMReturnCode (*writeMemoryBulk)(uint64_t device, uint64_t address, uint32_t byteCount, const uint8_t* data, void* refcon);

    /*! Ask the user to choose an item from the list */
    SDMReturnCode (*selectCredential)(const char* title, uint32_t count, const SDMItemInfo* items); // TODO return selection!!
} SDMCallbacks;

/**
 * \brief Collection of connection details for SDM_Init() and EComPort_Init()
 */
typedef struct SDMOpenParameters {
    uint32_t version; /*!< Client interface version, should be set to #SDM_CurrentVersion. */
    uint32_t flags;
    const char* userSelectedFilePath; /*!< Path to file chosen by the user in connection config. Not valid if NULL. */
    uint64_t commChannelBaseAddress; /*!< Base address of the communications channel. This can be an (APSEL<<24), AP address, or memory window base address. The comm link shared library must be selected to match the comm channel type represented by this address */
    SDMCallbacks* callbacks; /*!< Callback collection */
    void* refcon; /*!< Value passed to each of the callbacks. */
} SDMOpenParameters;

typedef struct SDMAuthenticateParams {
    SDMExecutionContext expectedExecutionContext;
} SDMAuthenticateParams;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function is called by the debugger to start a secure debug session with the remote platform
 *
 * The caller is expected to set the resetType to a value other than link SDM_none when
 * it knows that the debugged platform implements the secure debug certificate processing in its ROM.
 * In the case of SDM_nSRSTReset, if the External COM port driver implementation does not have
 * nSRST capability the caller can provide callbacks pDebugIf->callbacks->f_nSRSTStage1 and
 * pDebugIf->callbacks->f_nSRSTStage2.
 *
 * If the caller wants to get progress indications from SDMInit, then the pDebugIF->callbacks->f_progressIndicationCallback
 * must not be NULL. In such case {@link SDM_Init} will call this callback function with the relevant
 * {@link SDMInitStep}.
 *
 * The pDebugIf->pTopologyDetails parameter is to provide to the Secure Debug Manager and
 * External COM port driver the connection details for the I/O driver. This is I/O driver specific.
 * In the case of the CSAPBCOM I/O driver this should be {@link CSAPBCOMConnectionDescription}.
 *
 * @param[in] pDebugIF Connection details for {@link SDM_Init} and {@link EComPort_Init}.
 * @param[out] handle New handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_Open(SDMHandle* handle, SDMOpenParameters* params);
// -> why was resetType removed? what implications does this have?

SDM_EXTERN SDMReturnCode SDM_PreAuthentication(SDMHandle handle);

/**
 * @brief Unlock debug access.
 *
 * @param[in] handle Handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_Authenticate(SDMHandle handle, const SDMAuthenticateParams *params);
// deal with
// - reset type/exec context
// - multiple auths

/**
 * This function is called by the debugger to resume the boot of the remote platform.
 * It is typically called after the debugger places its breakpoints at the booting debugged system.
 * It is only useful if the debugged system supports the introduction of debug certificate in the
 * early boot stages, otherwise if the debugged system processes the secure debug certificate at runtime,
 * it does not wait for the resume command.
 *
 * The debugger can know this fact at the response of introduction of the secure debug certificate, which is
 * also the return value of {@link SDM_Init}. In case it happened at early boot then {@link SDM_Init} will
 * return SDM_SUCCESS_WAIT_RESUME. Otherwise if introduced at run time then {@link SDM_Init} will
 * return SDM_SUCCESS.
 *
 * In the 1st case, after the return of {@link SDM_Init} the user set breakpoints while the debugged system
 * is still waiting. After breakpoints are set, the debugger can call {@link SDM_ResumeBoot}
 *
 * @param[in] handle Handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_ResumeBoot(SDMHandle handle);

/**
 * This function is called by the debugger to end a secure debug session with the remote platform.
 * Function should reset the link then power off the Internal COM Port.
 *
 * @param[in] handle Handle to the SDM instance.
 * @param[in] resetType Required reset type. Should be the same as used with {@link SDM_Init}.
 */
SDM_EXTERN SDMReturnCode SDM_End(SDMHandle handle, SDMResetType resetType);
    // TODO what is difference between SDM_End() and SDM_Close()?

/**
 * @brief Free resources allocated by the SDM.
 *
 * @param[in] handle Handle to the SDM instance.
 */
SDM_EXTERN SDMReturnCode SDM_Close(SDMHandle handle);

#ifdef __cplusplus
}
#endif

#endif /* _SECURE_DEBUG_MANAGER_H_ */
