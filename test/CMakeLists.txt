#
#  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE.md file.
#

#
#  Private functions.
#
function(add_executable_dependencies PROJ_NAME) 

    #  Add the include directories for compiling.
    target_include_directories(
        ${PROJ_NAME}
        PRIVATE
        ${CMAKE_BINARY_DIR}/include
    )

endfunction()

#  Test case.
add_executable(
    buffer-unittest 
    buffer.unittest.cc
    ${CMAKE_BINARY_DIR}/src/error.cc
    ${CMAKE_BINARY_DIR}/src/buffer.cc
)
add_executable(
    fetcher-unittest 
    fetcher.unittest.cc
    ${CMAKE_BINARY_DIR}/src/error.cc
    ${CMAKE_BINARY_DIR}/src/buffer.cc
    ${CMAKE_BINARY_DIR}/src/fetcher.cc
)
add_executable(
    queue-unittest
    queue.unittest.cc
    ${CMAKE_BINARY_DIR}/src/error.cc
    ${CMAKE_BINARY_DIR}/src/buffer.cc
    ${CMAKE_BINARY_DIR}/src/fetcher.cc
    ${CMAKE_BINARY_DIR}/src/queue.cc
)

add_executable_dependencies(buffer-unittest)
add_executable_dependencies(fetcher-unittest)
add_executable_dependencies(queue-unittest)

add_test(
    NAME                xaptest-buffer
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/buffer-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)
add_test(
    NAME                xaptest-fetcher
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fetcher-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)
add_test(
    NAME                xaptest-queue
    COMMAND             ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/queue-unittest
    WORKING_DIRECTORY   ${CMAKE_BINARY_DIR}
)

#  Timeout.
set_tests_properties(xaptest-buffer PROPERTIES TIMEOUT 3)
set_tests_properties(xaptest-fetcher PROPERTIES TIMEOUT 3)
set_tests_properties(xaptest-queue PROPERTIES TIMEOUT 3)
