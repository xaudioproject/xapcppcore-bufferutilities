//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_CORE_BUFFER_FETCHER_H__
#define XAP_CORE_BUFFER_FETCHER_H__

//
//  Imports.
//
#include <memory>
#include <stdint.h>
#include <xap/core/buffer/buffer.h>
#include <xap/core/buffer/error.h>

namespace xap {
namespace core {
namespace buffer {

//
//  Classes.
//

class BufferFetcher {
public:
    //
    //  Constructor & destructor.
    //

    /**
     *  Cosntruct the object.
     * 
     *  @param buffer
     *      The buffer which would be fetched.
     */
    BufferFetcher(const Buffer &buffer);

    /**
     *  Construct (Copy) the object.
     * 
     *  @param src
     *      The source fetcher.
     */
    BufferFetcher(const BufferFetcher &src) noexcept;

    /**
     *  Destruct the object.
     */
    ~BufferFetcher() noexcept;

    //
    //  Public operators.
    //

    /**
     *  Operator '='.
     * 
     *  @param src
     *      The source fetcher.
     *  @return
     *      The destination fetcher.
     */
    BufferFetcher& operator=(const BufferFetcher& src) noexcept;

    //
    //  Public methods.
    //

    /**
     *  Check whether the fetcher is ended.
     * 
     *  @return
     *      True if so.
     */
    bool is_end() const noexcept;

    /**
     *  Reset the fetcher. Move the cursor to the begin position.
     */
    void reset() noexcept;

    /**
     *  Fetch one byte.
     * 
     *  @throw BufferException
     *      Raised if the buffer fetcher was ended.
     *  @return
     *      The byte.
     */
    uint8_t fetch();

    /**
     *  Fetch bytes to buffer.
     * 
     *  @note
     *      Nothing would be done if destination size is zero.
     *  @throw BufferException
     *      Riased if the buffer fetcher was ended (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param destination
     *      The destination buffer.
     *  @return
     *      The number of bytes fetched.
     */
    size_t fetch_to(Buffer &destination);

    /**
     *  Fetch bytes to buffer.
     * 
     *  @note
     *      Nothing would be done if destination size is zero.
     *  @throw BufferException
     *      Raised if the buffer fetcher was ended (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param destination  
     *      The destination buffer.
     *  @param destination_offset
     *      The offset of destination buffer.
     *  @return
     *      The number of bytes fetched.
     */
    size_t fetch_to(Buffer &destination, const size_t destination_offset);

    /**
     *  Fetch all bytes in buffer.
     * 
     *  @note
     *      Return zero-size buffer if fetcher is ended.
     *  @return
     *      The destination buffer.
     */
    Buffer fetch_all();

    /**
     *  Fetch bytes in buffer.
     * 
     *  @throw BufferException
     *      Parameter 'count' was out of range (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param count
     *      The count of bytes would be fetched.
     *  @return
     *      The destination count.
     */
    Buffer fetch_bytes(const size_t count);

    /**
     *  Skip bytes.
     * 
     *  @note
     *      Nothing would be done if 'count' = 0U.
     *  @throw BufferException
     *      Raised if parameter 'buffer' was out of range 
     *      (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param count
     *      The count of bytes would be skiped.
     */
    void skip(const size_t count);

    /**
     *  Get the remaining size.
     * 
     *  @return
     *      The remaining size.
     */
    size_t get_remaining_size() const noexcept;

    /**
     *  Replace (reset) the fetch with another new buffer.
     * 
     *  @param new_buffer
     *      The buffer.
     */
    void replace(const Buffer &new_buffer);

private:
    //
    //  Private functions.
    //

    /**
     *  Expected fetcher was not ended.
     * 
     *  @throw BufferException
     *      Raised if not (XAPCORE_BUF_ERROR_OVERFLOW).
     */
    void assert_not_eof();

    //
    //  Members.
    //
    size_t                   m_cursor;
    Buffer                  *m_buffer;
    size_t                   m_buffer_length;
    std::shared_ptr<Buffer>  m_buffer_shared_pointer;
};

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap


#endif  //  #ifndef XAP_CORE_BUFFER_FETCHER_H__