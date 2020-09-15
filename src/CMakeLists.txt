#
#  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
#  Use of this source code is governed by a BSD-style license that can be
#  found in the LICENSE.md file.
#

#  Project name.
project(xapcppcore-bufferutilities VERSION 1.0.0)

#  Add static library.
add_library(
    xapcppcore-bufferutilities-static
    STATIC
    buffer.cc
    error.cc
    fetcher.cc
    queue.cc
)
target_include_directories(
    xapcppcore-bufferutilities-static 
    PUBLIC 
    ${CMAKE_BINARY_DIR}/include
)

#  Add shared library.
add_library(
    xapcppcore-bufferutilities
    SHARED
    buffer.cc
    error.cc
    fetcher.cc
    queue.cc
)
target_include_directories(
    xapcppcore-bufferutilities
    PUBLIC 
    ${CMAKE_BINARY_DIR}/include
)