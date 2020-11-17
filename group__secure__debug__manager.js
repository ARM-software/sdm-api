var group__secure__debug__manager =
[
    [ "Structure", "index.html#autotoc_md1", null ],
    [ "Status", "index.html#autotoc_md2", null ],
    [ "License", "index.html#autotoc_md3", null ],
    [ "secure_debug_manager.h", "secure__debug__manager_8h.html", null ],
    [ "SDMItemInfo", "group__secure__debug__manager.html#struct_s_d_m_item_info", [
      [ "itemLongDescription", "group__secure__debug__manager.html#a27a5ac935b96c5b55e1ada9947e613a1", null ],
      [ "itemShortName", "group__secure__debug__manager.html#ab0a6a362aa46070841dbbb2f16aa2536", null ]
    ] ],
    [ "SDMArmADICallbacks", "struct_s_d_m_arm_a_d_i_callbacks.html", [
      [ "apRead", "struct_s_d_m_arm_a_d_i_callbacks.html#a7675a9edb0e0c01a69479e0cef451a68", null ],
      [ "apWrite", "struct_s_d_m_arm_a_d_i_callbacks.html#aa42891accc1e7d3ed69f9883219b2a49", null ],
      [ "invokeDebugSequence", "struct_s_d_m_arm_a_d_i_callbacks.html#a9e4b1b6a327538ece7d3cc26632602f2", null ]
    ] ],
    [ "SDMArchitectureCallbacks", "group__secure__debug__manager.html#union_s_d_m_architecture_callbacks", [
      [ "armADICallbacks", "group__secure__debug__manager.html#a0d52a0a54091e9cdddcbfdf3a3bb6d10", null ],
      [ "nexus5001Callbacks", "group__secure__debug__manager.html#aa686a8b510aab1e8b2eb1258a715ba26", null ]
    ] ],
    [ "SDMCallbacks", "struct_s_d_m_callbacks.html", [
      [ "architectureCallbacks", "struct_s_d_m_callbacks.html#a6fcc77c7a98cfbf597532db587f0ff73", null ],
      [ "readMemory", "struct_s_d_m_callbacks.html#acb3f713c4a65337156442ee1b3695310", null ],
      [ "readMemoryBulk", "struct_s_d_m_callbacks.html#ae90337b6ab3c03f98bca0ede3c89154a", null ],
      [ "resetFinish", "struct_s_d_m_callbacks.html#a0097b126305408e4c2cdd62a27834e11", null ],
      [ "resetStart", "struct_s_d_m_callbacks.html#a236c5fcbceda02bea1477d806255bec1", null ],
      [ "selectItem", "struct_s_d_m_callbacks.html#a6bee8763ab200494f9cddbbbec87a0e6", null ],
      [ "updateProgress", "struct_s_d_m_callbacks.html#a04515d10c6ed5da4c23ccaf45c7e4e93", null ],
      [ "writeMemory", "struct_s_d_m_callbacks.html#a8349848e83bd6e60e4539108cecfaf4d", null ],
      [ "writeMemoryBulk", "struct_s_d_m_callbacks.html#ace54045d6f2d1e92ef653b36b7bf9392", null ]
    ] ],
    [ "SDMDefaultDeviceInfo", "group__secure__debug__manager.html#struct_s_d_m_default_device_info", [
      [ "address", "group__secure__debug__manager.html#a45d6f242871de2e5e2dc875ad4aaea87", null ],
      [ "deviceType", "group__secure__debug__manager.html#ade0110b19a03bb398d008844d5ec1485", null ]
    ] ],
    [ "SDMOpenParameters", "group__secure__debug__manager.html#struct_s_d_m_open_parameters", [
      [ "callbacks", "group__secure__debug__manager.html#a049222f334914eb1de700d69158e7f3d", null ],
      [ "debugArchitecture", "group__secure__debug__manager.html#a1fd031834c4d0d33624d65472bce0f67", null ],
      [ "defaultDeviceInfo", "group__secure__debug__manager.html#afcc847df3f0b6e2c0e9c0b7251d97a04", null ],
      [ "flags", "group__secure__debug__manager.html#a930c09fb9c953e28197dc1968fc362f5", null ],
      [ "refcon", "group__secure__debug__manager.html#a0a6e027c6af4ccc805dd3985fd96deb3", null ],
      [ "userSelectedFilePath", "group__secure__debug__manager.html#a8f3ebc7f4c0c3ae51cb8764ff91d3747", null ],
      [ "version", "group__secure__debug__manager.html#a65cc126f899323e386bc2de0e7e37902", null ]
    ] ],
    [ "SDMAuthenticateParameters", "group__secure__debug__manager.html#struct_s_d_m_authenticate_parameters", [
      [ "expectedExecutionContext", "group__secure__debug__manager.html#a8d7f5ff1d919d4c5b3cfd176f618e04d", null ],
      [ "isLastAuthentication", "group__secure__debug__manager.html#a64855f82572a7e799eb2ece4699f12e4", null ]
    ] ],
    [ "SDMDefaultDeviceType", "group__secure__debug__manager.html#gae0fba61349f11982f3705b7ccf65df65", null ],
    [ "SDMExecutionContext", "group__secure__debug__manager.html#ga2d60f4deb8c9e0dc958bcc9e1e9c3839", null ],
    [ "SDMHandle", "group__secure__debug__manager.html#ga6fb95886ee381cc9ad2788621abc824a", null ],
    [ "SDMMemorySize", "group__secure__debug__manager.html#gadcd7979d7ae4d12d95724071791f9160", null ],
    [ "SDMReturnCode", "group__secure__debug__manager.html#gadf7016e48c36fd28734f580629204ff7", [
      [ "SDM_DefaultDevice", "group__secure__debug__manager.html#gga06fc87d81c62e9abb8790b6e5713c55baf7ec567ded54710a6630b00801e848aa", null ]
    ] ],
    [ "SDMDebugArchitecture", "group__secure__debug__manager.html#gac790398366fe86778f79d9caa4ae0776", [
      [ "SDM_Arm_ADIv5", "group__secure__debug__manager.html#ggac790398366fe86778f79d9caa4ae0776aceec1e0f3eb3ba0bff659e911d3752d5", null ],
      [ "SDM_Arm_ADIv6", "group__secure__debug__manager.html#ggac790398366fe86778f79d9caa4ae0776a4f9b19dced8011913f77564206bc2153", null ],
      [ "SDM_Nexus5001", "group__secure__debug__manager.html#ggac790398366fe86778f79d9caa4ae0776a4f4cda6ec6ed88ca67850cc3f6a17c51", null ]
    ] ],
    [ "SDMDefaultDeviceType", "group__secure__debug__manager.html#ga978a697936c36e513c6a3b58e12b10d5", [
      [ "SDM_ArmADI_AP", "group__secure__debug__manager.html#gga978a697936c36e513c6a3b58e12b10d5a8c2808d7ad4746df7da0a21938d0ee0a", null ],
      [ "SDM_ArmADI_MEM_AP", "group__secure__debug__manager.html#gga978a697936c36e513c6a3b58e12b10d5a61b260050fb653d1e85b35d927d912ee", null ],
      [ "SDM_ArmADI_CoreSight_Component", "group__secure__debug__manager.html#gga978a697936c36e513c6a3b58e12b10d5a6eae37866eca5955d05e61066ad88f07", null ]
    ] ],
    [ "SDMExecutionContext", "group__secure__debug__manager.html#ga2b74721a6a1b55e9ad5d2f931f0affcf", [
      [ "SDM_Boot_ROM", "group__secure__debug__manager.html#gga2b74721a6a1b55e9ad5d2f931f0affcfa8ba65954c11f4b2599aad7a46fc7627d", null ],
      [ "SDM_Boot_Loader", "group__secure__debug__manager.html#gga2b74721a6a1b55e9ad5d2f931f0affcfadaa168dae75813ffe2bfc94021159d76", null ],
      [ "SDM_Runtime", "group__secure__debug__manager.html#gga2b74721a6a1b55e9ad5d2f931f0affcfaf709b5d0db8b431b50b78db8a4c2195c", null ]
    ] ],
    [ "SDMFlags", "group__secure__debug__manager.html#ga16b2d0722cf4c18817f96c96edace6ef", [
      [ "SDM_IsDefaultDeviceInfoValid", "group__secure__debug__manager.html#gga16b2d0722cf4c18817f96c96edace6efae0cc0e6b22f2244d92605dda1207fb76", null ]
    ] ],
    [ "SDMMemorySize", "group__secure__debug__manager.html#gafb31e3f81ba5095b795d61610200115a", [
      [ "SDM_Memory8", "group__secure__debug__manager.html#ggafb31e3f81ba5095b795d61610200115aabe9e3e5c2eb0447e8df7b8b16a0e2845", null ],
      [ "SDM_Memory16", "group__secure__debug__manager.html#ggafb31e3f81ba5095b795d61610200115aa8790480e6b205771b986534e1a675f98", null ],
      [ "SDM_Memory32", "group__secure__debug__manager.html#ggafb31e3f81ba5095b795d61610200115aae68f0a7b6a148f5d8e9d939684de71f5", null ],
      [ "SDM_Memory64", "group__secure__debug__manager.html#ggafb31e3f81ba5095b795d61610200115aa7d0c93827be1a73006258a926f0c0a98", null ]
    ] ],
    [ "SDMResetType", "group__secure__debug__manager.html#gadda19af6d42341bbb844ff2b30761274", [
      [ "SDM_NoReset", "group__secure__debug__manager.html#ggadda19af6d42341bbb844ff2b30761274ab98efe809f015eda9e75875431561b2e", null ],
      [ "SDM_HardwareReset", "group__secure__debug__manager.html#ggadda19af6d42341bbb844ff2b30761274a50d89e7083c81f2e2535a0bd6f117d4a", null ],
      [ "SDM_SoftwareReset", "group__secure__debug__manager.html#ggadda19af6d42341bbb844ff2b30761274ae16b00347de0b6f9aa0120fa835118d5", null ]
    ] ],
    [ "SDMReturnCode", "group__secure__debug__manager.html#gaeffcb60ba5124479d40c6b61e0bcf156", [
      [ "SDM_Success", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a1ad266ad718f7d2edbeda115dac29889", null ],
      [ "SDM_Fail_No_Response", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a3fed0832e0ecf1d0a646958e4e64b2ba", null ],
      [ "SDM_Fail_Unsupported_Transfer_Size", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a17e402d6e0e0f78dfeb140d9b930b65b", null ],
      [ "SDM_Fail_User_Credentials", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a8e89dc9ce8608d522fef51c89141d9b6", null ],
      [ "SDM_Fail_IO", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a7956855566085cf54047ca3641563205", null ],
      [ "SDM_Fail_Internal", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156ab7817c0d11cf7c297773895673f88a0b", null ],
      [ "SDM_Fail_Parameter", "group__secure__debug__manager.html#ggaeffcb60ba5124479d40c6b61e0bcf156a37543458048ffd8f262ab5dabec6bff3", null ]
    ] ],
    [ "SDMVersion", "group__secure__debug__manager.html#gab3d80ef5d4f18cf4cf60f8dcb94e34bc", [
      [ "SDM_CurrentVersion", "group__secure__debug__manager.html#ggab3d80ef5d4f18cf4cf60f8dcb94e34bcaf8dc3439be9bad4b28bcf3200ae4a62c", null ]
    ] ],
    [ "SDM_Authenticate", "group__secure__debug__manager.html#ga968907cd0908d0eb0328c5cbaf7aa893", null ],
    [ "SDM_Close", "group__secure__debug__manager.html#ga4801fd4cb869a33d4dd06f3ef4d8ea8c", null ],
    [ "SDM_Open", "group__secure__debug__manager.html#ga6ac7269554ea62d5137542bbf764ffaf", null ],
    [ "SDM_ResumeBoot", "group__secure__debug__manager.html#ga0f719cc0693b1adaa25f5998151ed620", null ]
];