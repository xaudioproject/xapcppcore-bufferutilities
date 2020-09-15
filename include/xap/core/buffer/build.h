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

//
//  Reference from Chromium source code:
//  https://chromium.googlesource.com/chromium/src/+/refs/heads/master/build/build_config.h
//
//  Processor architecture detection.  For more info on what's defined, see:
//    http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//    http://www.agner.org/optimize/calling_conventions.pdf
//    or with gcc, run: "echo | gcc -E -dM -"
//
#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86_64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__s390x__)
#define ARCH_CPU_S390_FAMILY 1
#define ARCH_CPU_S390X 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_BIG_ENDIAN 1
#elif defined(__s390__)
#define ARCH_CPU_S390_FAMILY 1
#define ARCH_CPU_S390 1
#define ARCH_CPU_31_BITS 1
#define ARCH_CPU_BIG_ENDIAN 1
#elif (defined(__PPC64__) || defined(__PPC__)) && defined(__BIG_ENDIAN__)
#define ARCH_CPU_PPC64_FAMILY 1
#define ARCH_CPU_PPC64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_BIG_ENDIAN 1
#elif defined(__PPC64__)
#define ARCH_CPU_PPC64_FAMILY 1
#define ARCH_CPU_PPC64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__ARMEL__)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARMEL 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARM64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__pnacl__) || defined(__asmjs__) || defined(__wasm__)
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#elif defined(__MIPSEL__)
#if defined(__LP64__)
#define ARCH_CPU_MIPS_FAMILY 1
#define ARCH_CPU_MIPS64EL 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#else
#define ARCH_CPU_MIPS_FAMILY 1
#define ARCH_CPU_MIPSEL 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_LITTLE_ENDIAN 1
#endif
#elif defined(__MIPSEB__)
#if defined(__LP64__)
#define ARCH_CPU_MIPS_FAMILY 1
#define ARCH_CPU_MIPS64 1
#define ARCH_CPU_64_BITS 1
#define ARCH_CPU_BIG_ENDIAN 1
#else
#define ARCH_CPU_MIPS_FAMILY 1
#define ARCH_CPU_MIPS 1
#define ARCH_CPU_32_BITS 1
#define ARCH_CPU_BIG_ENDIAN 1
#endif
#else
#error Please add support for your architecture in xap/core/buffer/build.h
#endif

#if ARCH_CPU_LITTLE_ENDIAN
# define XAP_CORE_BUFFER_LITTLE_ENDIAN
#elif ARCH_CPU_BIG_ENDIAN
# define XAP_CORE_BUFFER_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
//
//  Bytes in 16-bit words are laid out in a 
//  little-endian fashion, whereas the 16-bit 
//  subwords of a 32-bit quantity are laid out 
//  in big-endian fashion.
//
//  See: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
//
# define XAP_CORE_BUFFER_POP_ENDIAN
#else
# error "Cannot check compiler endian order."
#endif

#endif  //  #ifndef XAP_CORE_BUFFER_BUILD_H__