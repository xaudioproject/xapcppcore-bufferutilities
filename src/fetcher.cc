//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <xap/core/buffer/error.h>
#include <xap/core/buffer/fetcher.h>

namespace xap {
namespace core {
namespace buffer {

/**
 *  Cosntruct the object.
 * 
 *  @throw BufferException
 *      Raised if memory allocation was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param buffer
 *      The buffer which would be fetched.
 */
BufferFetcher::BufferFetcher(const Buffer &buffer) {
    try {
        Buffer *copied_buffer = new Buffer(buffer);
        this->m_buffer_shared_pointer = std::shared_ptr<Buffer>(copied_buffer);
        this->m_buffer = copied_buffer;
        this->m_buffer_length = copied_buffer->get_length();
        this->m_cursor = 0;
    } catch (std::bad_alloc exception) {
        throw BufferException("Bad alloc.", XAPCORE_BUF_ERROR_ALLOC);
    }
}

/**
 *  Construct (Copy) the object.
 * 
 *  @param src
 *      The source fetcher.
 */
BufferFetcher::BufferFetcher(const BufferFetcher &src) noexcept {
    this->m_buffer_shared_pointer = src.m_buffer_shared_pointer;
    this->m_buffer = src.m_buffer;
    this->m_buffer_length = src.m_buffer_length;
    this->m_cursor = src.m_cursor;
}

/**
 *  Destruct the object.
 */
BufferFetcher::~BufferFetcher() noexcept {
    this->m_buffer_shared_pointer.reset();
    this->m_buffer = nullptr;
    this->m_buffer_length = 0;
    this->m_cursor = 0;
}

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
BufferFetcher& BufferFetcher::operator=(const BufferFetcher& src) noexcept {
    if (this != &src) {
        this->m_buffer_shared_pointer = src.m_buffer_shared_pointer;
        this->m_buffer = src.m_buffer;
        this->m_buffer_length = src.m_buffer_length;
        this->m_cursor = src.m_cursor;
    }
    return *this;
}

//
//  Public methods.
//

/**
 *  Check whether the fetcher is ended.
 * 
 *  @return
 *      True if so.
 */
bool BufferFetcher::is_end() const noexcept {
    return this->m_cursor == this->m_buffer_length;
}

/**
 *  Reset the fetcher. Move the cursor to the begin position.
 */
void BufferFetcher::reset() noexcept {
    this->m_cursor = 0;
}

/**
 *  Fetch one byte.
 * 
 *  @throw BufferException
 *      Raised if the buffer fetcher was ended (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @return
 *      The byte.
 */
uint8_t BufferFetcher::fetch() {
    this->assert_not_eof();
    return this->m_buffer->read_uint8(this->m_cursor++);
}

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
size_t BufferFetcher::fetch_to(Buffer &destination) {
    return this->fetch_to(destination, 0U);
}

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
size_t BufferFetcher::fetch_to(
    Buffer &destination, 
    const size_t destination_offset
) {
    if (destination.get_length() == 0) {
        return 0U;
    }

    this->assert_not_eof();

    size_t copy_len = this->m_buffer->copy(
        destination, 
        destination_offset, 
        this->m_cursor
    );
    this->m_cursor += copy_len;
    return copy_len;
}

/**
 *  Fetch all bytes in buffer.
 * 
 *  @note
 *      Return zero-size buffer if fetcher is ended.
 *  @throw BufferException
 *      Raised in the following situations:
 * 
 *          - XAPCORE_BUF_ERROR_ALLOC: 
 *              Raised if memory allocation was failed.
 * 
 *  @return
 *      The destination buffer.
 */
Buffer BufferFetcher::fetch_all() {
    if (this->is_end()) {
        return Buffer(0);
    }
    
    Buffer out = this->m_buffer->slice(this->m_cursor);
    this->m_cursor += out.get_length();
    return out;
}

/**
 *  Fetch bytes in buffer.
 * 
 *  @throw BufferException
 *      Raised in the following situations:
 * 
 *          - XAPCORE_BUF_ERROR_OVERFLOW:
 *              Parameter 'count' was out of range.
 *          - XAPCORE_BUF_ERROR_ALLOC: 
 *              Raised if memory allocation was failed.
 *      
 *  @param count
 *      The count of bytes would be fetched.
 *  @return
 *      The destination count.
 */
Buffer BufferFetcher::fetch_bytes(const size_t count) {
    if (count == 0U) {
        return Buffer(0);
    }

    const size_t remaining = this->get_remaining_size();
    if (count > remaining) {
        throw BufferException(
            "Out of range.", 
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    }

    Buffer out = this->m_buffer->slice(this->m_cursor, count);
    this->m_cursor += count;
    return out;
}

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
void BufferFetcher::skip(const size_t count) {
    if (count == 0U) {
        return;
    }

    this->assert_not_eof();

    const size_t remaining = this->get_remaining_size();
    if (count > remaining) {
        throw BufferException(
            "Out of range.", 
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    }

    this->m_cursor += count;
}

/**
 *  Get the remaining size.
 * 
 *  @return
 *      The remaining size.
 */
size_t BufferFetcher::get_remaining_size() const noexcept {
    return this->m_buffer_length - this->m_cursor;
}

/**
 *  Replace (reset) the fetch with another new buffer.
 * 
 *  @throw BufferException
 *      Raised if memory allocation was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param new_buffer
 *      The buffer.
 */
void BufferFetcher::replace(const Buffer &new_buffer) {
    try {
        Buffer *copied_buffer = new Buffer(new_buffer);
        this->m_buffer_shared_pointer = std::shared_ptr<Buffer>(copied_buffer);
        this->m_buffer = copied_buffer;
        this->m_buffer_length = copied_buffer->get_length();
        this->m_cursor = 0U;
    } catch (std::bad_alloc &error) {
        throw BufferException(error.what(), XAPCORE_BUF_ERROR_ALLOC);
    }
}

//
//  Private methods.
//

/**
 *  Expected fetcher was not ended.
 * 
 *  @throw BufferException
 *      Raised if not (XAPCORE_BUF_ERROR_OVERFLOW).
 */
void BufferFetcher::assert_not_eof() {
    if (this->is_end()) {
        throw BufferException(
            "Reached the end of the buffer.", 
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    }
}

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap