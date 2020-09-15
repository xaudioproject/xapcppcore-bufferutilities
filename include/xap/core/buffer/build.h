//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_CORE_BUFFER_BUILD_H__
#define XAP_CORE_BUFFER_BUILD_H__

//
//  Endian flag.
//

#if __LITTLE_ENDIAN__
#define XAP_CORE_BUFFER_LITTLE_ENDIAN
#elif __BIG_ENDIAN__
#define XAP_CORE_BUFFER_BIG_ENDIAN
#else
# error "Cannot check compiler endian order."
#endif

#endif  //  #ifndef XAP_CORE_BUFFER_BUILD_H__