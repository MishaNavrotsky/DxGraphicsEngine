function(target_namespace_headers TARGET NAMESPACE)
    set(SOURCE_DIRS ${ARGN})
    set(VIRTUAL_ROOT "${CMAKE_CURRENT_BINARY_DIR}/gen_inc/${TARGET}")
    set(DEST_DIR "${VIRTUAL_ROOT}/${NAMESPACE}")

    foreach(SRC_DIR ${SOURCE_DIRS})
        if(NOT IS_DIRECTORY "${SRC_DIR}")
            continue()
        endif()

        file(COPY "${SRC_DIR}/"
                DESTINATION "${DEST_DIR}"
                FILES_MATCHING
                PATTERN "*.h"
                PATTERN "*.hpp"
                PATTERN "*.inl")
    endforeach()

    get_target_property(T_TYPE ${TARGET} TYPE)
    set(SCOPE "PUBLIC")
    if(T_TYPE STREQUAL "INTERFACE_LIBRARY")
        set(SCOPE "INTERFACE")
    endif()

    target_include_directories(${TARGET} ${SCOPE} "${VIRTUAL_ROOT}")
endfunction()