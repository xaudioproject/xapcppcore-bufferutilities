//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <xap/core/buffer/error.h>

namespace xap {
namespace core {
namespace buffer {

//
//  BufferException constructor & destructor.
//

/**
 *  Construct the object.
 *
 *  @param message
 *      The error message.
 */
BufferException::BufferException(
    const char      *message,
    const uint16_t  &code
) noexcept:
    m_message(message),
    m_code(code)
{}

/**
 *  Construct the object.
 *
 *  @param src
 *      The base exception.
 */
BufferException::BufferException(const BufferException &src) noexcept :
    m_message(src.m_message),
    m_code(src.m_code)
{}

/**
 *  Destruct the object.
 */
BufferException::~BufferException() noexcept {}

//
//  BufferException public methods.
//

/**
 *  Get the error message.
 *
 *  @return
 *      The error message.
 */
const char *BufferException::what() const noexcept {
    return this->m_message.c_str();
}

/**
 *  Get the error code.
 *
 *  @return
 *      The error code.
 */
uint16_t BufferException::get_code() const noexcept {
    return this->m_code;
}

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap