cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project("dpCtl - A lightweight SYCL wrapper for Python")

# The function checks is DPCPP_ROOT is valid and points to a dpcpp installation
function (check_for_dpcpp)
    string(COMPARE EQUAL "${DPCPP_ROOT}" "" no_dpcpp_root)
    if(${no_dpcpp_root})
        message(FATAL_ERROR "Set the DPCPP_ROOT argument providing the path to \
                             a dpcpp installation.")
    endif()

    if(WIN32)
        set (dpcpp_cmd "${DPCPP_ROOT}/bin/dpcpp")
        set (dpcpp_arg "--version")
    elseif(UNIX)
        set (dpcpp_cmd "${DPCPP_ROOT}/bin/dpcpp")
        set (dpcpp_arg "--version")
    else()
        message(FATAL_ERROR "Unsupported system.")
    endif()

    # Check if dpcpp is available
    execute_process(
        COMMAND ${dpcpp_cmd} ${dpcpp_arg}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE dpcpp_result
        OUTPUT_VARIABLE dpcpp_ver
    )

    if(${dpcpp_result} MATCHES "0")
        string(REPLACE "\n" ";" DPCPP_VERSION_LIST "${dpcpp_ver}")
        list(GET DPCPP_VERSION_LIST 0 dpcpp_ver_line)
        foreach(X ${DPCPP_VERSION_LIST})
            message(STATUS "dpcpp ver[${dpcpp_result}]: ${X}")
        endforeach()
    else()
        message(FATAL_ERROR "DPCPP needed to build dppl_sycl_interface")
    endif()
endfunction()

# Check for dpcpp in the specified DPCPP_ROOT
check_for_dpcpp()

if(WIN32)
    set(CMAKE_CXX_COMPILER:PATH "${DPCPP_ROOT}/bin/dpcpp")
    set(CMAKE_C_COMPILER:PATH "${DPCPP_ROOT}/bin/clang-cl")
    set(CMAKE_LINKER:PATH "${DPCPP_ROOT}/bin/lld-link")
    message(STATUS "Resetting CXX compiler to: " ${CMAKE_CXX_COMPILER})
    message(STATUS "Resetting C compiler to: " ${CMAKE_C_COMPILER})
    message(STATUS "Resetting Linker to: " ${CMAKE_LINK})
    set(WARNING_FLAGS "-Wall -Wextra -Winit-self -Wunused-function -Wuninitialized -Wmissing-declarations")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Qstd=c++17")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${WARNING_FLAGS} -ggdb3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARNING_FLAGS} -ggdb3 -DDEBUG -Qstd=c++17")
elseif(UNIX)
    set(SDL_FLAGS "-fstack-protector -fstack-protector-all -fpic -fPIC -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -fno-strict-overflow -fno-delete-null-pointer-checks")
    set(WARNING_FLAGS "-Wall -Wextra -Winit-self -Wunused-function -Wuninitialized -Wmissing-declarations -fdiagnostics-color=auto")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${SDL_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} ${SDL_FLAGS} -std=c++17 -fsycl")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${WARNING_FLAGS} -ggdb3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARNING_FLAGS} -ggdb3 -DDEBUG -std=c++17 -fsycl")
else()
    message(FATAL_ERROR "Unsupported system.")
endif()

add_library(
  DPPLSyclInterface
  SHARED
  source/dppl_sycl_context_interface.cpp
  source/dppl_sycl_device_interface.cpp
  source/dppl_sycl_event_interface.cpp
  source/dppl_sycl_kernel_interface.cpp
  source/dppl_sycl_platform_interface.cpp
  source/dppl_sycl_program_interface.cpp
  source/dppl_sycl_queue_interface.cpp
  source/dppl_sycl_queue_manager.cpp
  source/dppl_sycl_usm_interface.cpp
  source/dppl_utils.cpp
  details/source/dppl_utils_details.cpp
)

# Install DPPLSyclInterface
target_include_directories(
  DPPLSyclInterface
  PRIVATE
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/details/include/
)

if(WIN32)
    message(
        STATUS
        "SYCL_INCLUDE_DIR: "
        ${DPCPP_ROOT}/include/sycl
    )
    target_include_directories(
        DPPLSyclInterface
        PUBLIC
        ${DPCPP_ROOT}/include/sycl
    )
    target_link_libraries(
        DPPLSyclInterface
        PRIVATE ${DPCPP_ROOT}/lib/sycl.lib
        PRIVATE ${DPCPP_ROOT}/lib/OpenCL.lib
    )
endif()

install(
  TARGETS
  DPPLSyclInterface
  LIBRARY DESTINATION
  "${CMAKE_INSTALL_PREFIX}/lib/"
)

# Install all headers
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/*.h*")
foreach(HEADER ${HEADERS})
  install(FILES "${HEADER}" DESTINATION include)
endforeach()

# Install all headers in include/Support
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/include/Support/*.h*")
foreach(HEADER ${HEADERS})
  install(FILES "${HEADER}" DESTINATION include/Support)
endforeach()

# Install all headers in details/include
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/details/include/*.h*")
foreach(HEADER ${HEADERS})
  install(FILES "${HEADER}" DESTINATION details/include)
endforeach()

option(
    BUILD_CAPI_TESTS
    "Build dpctl C API google tests"
    OFF
)

# Enable to build the dpCtl backend test cases
if(BUILD_CAPI_TESTS)
    add_subdirectory(tests)
endif()


# Todo : Add build rules for doxygen
# maybe refer https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/
