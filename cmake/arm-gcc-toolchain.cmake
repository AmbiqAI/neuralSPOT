set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)
set(CMAKE_NO_SYSTEM_FROM_IMPORTED   True)

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ar${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_ASM_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_C_COMPILER                ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER              ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ld${CMAKE_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY                   ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_OBJDUMP                   ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-objdump${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-ranlib${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_SIZE                      ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-size${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_STRIP                     ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-strip${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")
set(CMAKE_GCOV                      ${BAREMETAL_ARM_TOOLCHAIN_PATH}arm-none-eabi-gcov${CMAKE_EXECUTABLE_SUFFIX} CACHE INTERNAL "")

set(AMBIQ_COMPILE_FLAGS             "${APP_C_FLAGS} -mthumb -mcpu=${CPU} -mfpu=${FPU} -mfloat-abi=${FABI} -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-exceptions -MMD -MP -Wall" CACHE INTERNAL "")
set(CMAKE_C_FLAGS                   "${AMBIQ_COMPILE_FLAGS} -std=c99" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${AMBIQ_COMPILE_FLAGS} -fno-use-cxa-atexit" CACHE INTERNAL "")

#add_link_options()
set(CMAKE_EXE_LINKER_FLAGS          "-mthumb -mcpu=${CPU} -mfpu=${FPU} -mfloat-abi=${FABI} -nostartfiles -static -lstdc++ -fno-exceptions  -Wl,--gc-sections,--entry,Reset_Handler,-Map,foo.map")

set(CMAKE_C_FLAGS_DEBUG             "-Os -g" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-Os -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_LINK_GROUP_USING_RESCAN "LINKER:--start-group" "LINKER:--end-group")
set(CMAKE_LINK_GROUP_USING_RESCAN_SUPPORTED TRUE)

function (generate_other_files TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME}.axf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET_NAME}.axf> ${TARGET_NAME}.bin
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET_NAME}.axf> > ${TARGET_NAME}.lst
        COMMAND ${CMAKE_SIZE}  $<TARGET_FILE:${TARGET_NAME}.axf> > ${TARGET_NAME}.size
    )
endfunction ()