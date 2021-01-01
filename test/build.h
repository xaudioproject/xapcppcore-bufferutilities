//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAPTEST_BUILD_H__
#define XAPTEST_BUILD_H__

//  Operating system.
#if defined(WIN32) || defined(_WIN32)
# define XAP_TEST_OS_WIN 1
#elif defined(__linux__)
# define XAP_TEST_OS_LINUX 1
#elif defined(__APPLE__)
# include <TargetConditionals.h>
# if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#  define XAP_TEST_OS_IOS 1
# else
#  define XAP_TEST_OS_MAC 1
# endif
#else
# error Unsupported operating system.
#endif

#endif  //  #ifndef XAPTEST_BUILD_H__