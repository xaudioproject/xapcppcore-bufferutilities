//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_CORE_BUFFER_ERROR_H__
#define XAP_CORE_BUFFER_ERROR_H__

//
//  Imports.
//
#include <exception>
#include <string.h>
#include <string>

namespace xap {
namespace core {
namespace buffer {

//
//  Constants.
//

//  Error code.
const static uint16_t XAPCORE_BUF_ERROR           = 4000U;
const static uint16_t XAPCORE_BUF_ERROR_OVERFLOW  = 4001U;

//
//  Classes.
//

//
//  XAP buffer core exception.
//
class BufferException: public std::exception {
public:
    //
    //  Public methods.
    //
    
    /**
     *  Construct the object.
     *
     *  @param message
     *      The error message.
     */
    explicit BufferException(
        const char      *message,
        const uint16_t  &code
    ) noexcept;
    
    /**
     *  Construct the object.
     *
     *  @param src
     *      The base exception.
     */
    BufferException(const BufferException &src) noexcept;
    
    /**
     *  Destruct the object.
     */
    virtual ~BufferException() noexcept;
    
    /**
     *  Get the error message.
     *
     *  @return
     *      The error message.
     */
    virtual const char* what() const noexcept;
    
    /**
     *  Get the error code.
     *
     *  @return
     *      The error code.
     */
    virtual uint16_t get_code() const noexcept;
    
private:
    //
    //  Members.
    //
    
    //  The error message.
    std::string m_message;
    uint16_t    m_code;
};

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap


#endif  //  XAP_CORE_BUFFER_ERROR_H__
