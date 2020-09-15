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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
# define XAP_CORE_BUFFER_LITTLE_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
# define XAP_CORE_BUFFER_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
//
//  Bytes in 16-bit words are laid out in a 
//  little-endian fashion, whereas the 16-bit 
//  subwords of a 32-bit quantity are laid out 
//  in big-endian fashion.
//
# define XAP_CORE_BUFFER_POP_ENDIAN
#else
# error "Cannot check compiler endian order."
#endif

#endif  //  #ifndef XAP_CORE_BUFFER_BUILD_H__