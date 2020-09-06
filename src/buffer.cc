//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <xap/core/buffer/error.h>
#include <xap/core/buffer/buffer.h>

namespace xap {
namespace core {
namespace buffer {

//
//  Private functions.
//

/**
 *  Free buffer space.
 * 
 *  @param ptr
 *      The pointer to buffer which would be free.
 */
static void buffer_free_space(uint8_t *ptr) noexcept;

//
//  Public class methods (also includes constructors, destructor and operators).
//

/**
 *  Construct (copy) the object.
 * 
 *  @param source
 *      The source buffer.
 */
Buffer::Buffer(const Buffer &source) noexcept {
    this->m_buffer = source.m_buffer;
    this->m_bufferstart = source.m_bufferstart;
    this->m_bufferend = source.m_bufferend;
    this->m_bufferlength = source.m_bufferlength;
}

/**
 *  Construct the object (, and initial all zero).
 * 
 *  @throw BufferException
 *      Raised if memory allocation was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param length
 *      The length of buffer.
 */
Buffer::Buffer(const size_t length) {
    try {
        uint8_t *data;
        if (length == 0U) {
            data = new uint8_t[1];
        } else {
            data = new uint8_t[length];
        }
        std::shared_ptr<uint8_t> buffer(data, buffer_free_space);
        this->prepare(buffer, 0, length);
        this->fill(0x00);
    } catch (std::bad_alloc &error) {
        throw BufferException(error.what(), XAPCORE_BUF_ERROR_ALLOC);
    }
}

/**
 *  Construct the object.
 * 
 *  @throw BufferException
 *      Raised if memory alloction was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param length
 *      The length of buffer.
 *  @param unsafe
 *      True if not initialze with zero.
 */
Buffer::Buffer(const size_t length, const bool unsafe) {
    try {
        uint8_t *data = new uint8_t[length];
        std::shared_ptr<uint8_t> buffer(data, buffer_free_space);
        this->prepare(buffer, 0, length);
        if (!unsafe) {
            this->fill(0x00);
        }
    } catch (std::bad_alloc &error) {
        throw BufferException(error.what(), XAPCORE_BUF_ERROR_ALLOC);
    }
}

/**
 *  Construct (copy) the object.
 * 
 *  @throw BufferException
 *      Raised if memory alloction was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param data
 *      The source data.
 *  @param datalen
 *      The length of source data.
 */
Buffer::Buffer(const uint8_t *data, const size_t datalen) {
    try {
        uint8_t *inner = new uint8_t[datalen];
        memcpy(inner, data, datalen);
        std::shared_ptr<uint8_t> buffer(inner, buffer_free_space);
        this->prepare(buffer, 0, datalen);
    } catch (std::bad_alloc &error) {
        throw BufferException(error.what(), XAPCORE_BUF_ERROR_ALLOC);
    }
}

/**
 *  Destruct the object.
 */
Buffer::~Buffer() noexcept {}

/**
 *  Operator '='.
 * 
 *  @param source
 *      The source buffer.
 *  @return
 *      The target buffer.
 */
Buffer& Buffer::operator=(const Buffer &source) noexcept {
    if (this != &source) {
        this->m_buffer = source.m_buffer;
        this->m_bufferstart = source.m_bufferstart;
        this->m_bufferend = source.m_bufferend;
        this->m_bufferlength = source.m_bufferlength;
    }
    return *this;
}

/**
 *  Operator '[]'. Get the buffer value of specified position.
 * 
 *  @throw BufferException
 *      Raised if offset is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The value of buffer in specified position. 
 */
uint8_t& Buffer::operator[](const size_t offset) const {
    this->check_access(offset, 1U);
    return *(this->m_bufferstart + offset);
}

/**
 *  Operator '=='. Whether buffer is equal to others.
 * 
 *  @param other
 *      The other buffer.
 *  @return
 *      True if equal.
 */
bool Buffer::operator==(const Buffer& other) const noexcept {
    if (other.m_bufferlength != this->m_bufferlength) {
        return false;
    }

    for (size_t i = 0; i < this->m_bufferlength; ++i) {
        if (this->m_bufferstart[i] != other.m_bufferstart[i]) {
            return false;
        }
    }
    return true;
}

/**
 *  Operator '!='. Whether buffer is not equal to others.
 * 
 *  @param other
 *      The other buffer.
 *  @return
 *      True if not equal.
 */
bool Buffer::operator!=(const Buffer& other) const noexcept {
    return !((*this) == other);
}

/**
 *  Get the length of buffer.
 * 
 *  @return
 *      The length.
 */
size_t Buffer::get_length() const noexcept {
    return this->m_bufferlength;
}

/**
 *  Get the raw pointer of buffer.
 * 
 *  @return
 *      The raw pointer.
 */
uint8_t* Buffer::get_pointer() noexcept {
    return this->m_bufferstart;
}

/**
 *  Get the raw pointer of buffer.
 * 
 *  @return
 *      The raw pointer.
 */
const uint8_t* Buffer::get_pointer() const noexcept {
    return this->m_bufferstart;
}

/**
 *  Return a new Buffer that references the same memory as the original, but
 *  offset and cropped by the 'offset' indices.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The new buffer.
 */
Buffer Buffer::slice(const size_t offset) const {
    return this->slice(offset, this->get_length() - offset);
}

/**
 *  Return a new Buffer that references the same memory as the original, but
 *  offset and cropped by the 'offset' and 'length' indices.
 * 
 *  @throw BufferException
 *      Raised in the follow situations:
 *      
 *          - XAPCORE_BUF_ERROR_OVERFLOW: 
 *              Raised if 'offset' and 'length' is out of range.
 *          - XAPCORE_BUF_ERROR_ALLOC: 
 *              Raised if memory allocation was failed.
 * 
 *  @param offset
 *      The offset.
 *  @param length
 *      The length
 *  @return
 *      The new buffer.
 */
Buffer Buffer::slice(const size_t offset, const size_t length) const {
    this->check_access(offset, length);
    return Buffer(this->m_buffer, offset, length);
}

/**
 *  Copies data to destination.
 * 
 *  @param destination
 *      The buffer to copy into.
 *  @return
 *      The number of bytes copied.
 */
size_t Buffer::copy(Buffer &destination) const noexcept {
    uint8_t *dst_ptr = destination.get_pointer();
    const uint8_t *src_ptr = this->get_pointer();
    size_t dst_len = destination.get_length();
    size_t src_len = this->get_length();
    size_t copy_len = std::min(src_len, dst_len);
    memcpy(dst_ptr, src_ptr, copy_len);
    return copy_len;
}

/**
 *  Copies data to destination.
 * 
 *  @throw BufferException
 *      Raised if the offset of destination buffer is out of range 
 *      (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param destination
 *      The buffer to copy into.
 *  @param destination_offset
 *      The offset of destination buffer.
 *  @return
 *      The number of bytes copied.
 */
size_t Buffer::copy(Buffer &destination, const size_t destination_offset) const{
    size_t dst_len = destination.get_length();
    if (destination_offset > dst_len) {
        throw BufferException(
            "Invalid destination offset.",
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    } else {
        dst_len -= destination_offset;
    }
    size_t src_len = this->get_length();
    uint8_t *dst_ptr = destination.get_pointer() + destination_offset;
    const uint8_t *src_ptr = this->get_pointer();
    size_t copy_len = std::min(src_len, dst_len);
    memcpy(dst_ptr, src_ptr, copy_len);
    return copy_len;
}

/**
 *  Copies data to destination.
 * 
 *  @throw BufferException
 *      Raised if the offset of destination or source buffer is out of 
 *      range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param destination
 *      The buffer to copy into.
 *  @param destination_offset
 *      The offset of destination buffer.
 *  @param src_offset
 *      The offset of source buffer.
 *  @return
 *      The number of bytes copied.
 */
size_t Buffer::copy(
    Buffer &destination,
    const size_t destination_offset,
    const size_t src_offset
) const {
    size_t dst_len = destination.get_length();
    if (destination_offset > dst_len) {
        throw BufferException(
            "Invalid destination offset.",
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    } else {
        dst_len -= destination_offset;
    }
    size_t src_len = this->get_length();
    if (src_offset > src_len) {
        throw BufferException(
            "Invalid source offset.", 
            XAPCORE_BUF_ERROR_OVERFLOW
        );
    } else {
        src_len -= src_offset;
    }
    uint8_t *dst_ptr = destination.get_pointer() + destination_offset;
    const uint8_t *src_ptr = this->get_pointer() + src_offset;
    size_t copy_len = std::min(src_len, dst_len);
    memcpy(dst_ptr, src_ptr, copy_len);
    return copy_len;
}

/**
 *  Fills buffer with the specified.
 * 
 *  @param value
 *      The value with which to fill buffer.
 */
void Buffer::fill(const uint8_t value) noexcept {
    memset(this->m_bufferstart, value, this->m_bufferlength);
}

/**
 *  Fills buffer with the specified.
 * 
 *  @throw XAPRexException
 *      Raised if 'offset' or 'length' is out of range 
 *      (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The value with which to fill buffer.
 *  @param offset
 *      Number of bytes to skip before starting to fill buffer.
 *  @param length
 *      The length of buffer to fill.
 */
void Buffer::fill(
    const uint8_t value,
    const size_t offset,
    const size_t length
) {
    this->check_access(offset, length);
    memset(this->m_bufferstart + offset, value, length);
}

/**
 *  Read an unsigned 8-bit integer.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 8-bit integer value.
 */
uint8_t Buffer::read_uint8(const size_t offset) const {
    this->check_access(offset, 1U);
    return this->m_bufferstart[offset];
}

/**
 *  Read an unsigned 16-bit integer with big-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 16-bit integer with big-endian.
 */
uint16_t Buffer::read_uint16_be(const size_t offset) const {
    this->check_access(offset, 2U);
    return (uint16_t)(
        ((uint16_t)(this->m_bufferstart[offset]) << 8U) |
        ((uint16_t)(this->m_bufferstart[offset + 1]))
    );
}

/**
 *  Write unsigned 8-bit integer at the specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 8-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint8(const uint8_t value, const size_t offset) {
    this->check_access(offset, 1U);
    this->m_bufferstart[offset] = value;
}

/**
 *  Write unsigned 16-bit integer with big-endian at the specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 8-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint16_be(const uint16_t value, const size_t offset) {
    this->check_access(offset, 2U);
    this->m_bufferstart[offset] = (uint8_t)((value & 0xFF00) >> (uint16_t)8U);
    this->m_bufferstart[offset + 1] = (uint8_t)(value & (uint16_t)0x00FF);
}

/**
 *  Check whether the binary data is equal to self.
 * 
 *  @param other
 *      The other buffer.
 *  @param other_len
 *      The length of other buffer.
 *  @return
 *      True if equal.
 */
bool Buffer::is_equal(
    const uint8_t *other, 
    const size_t other_len
) const noexcept {
    if (other_len != this->m_bufferlength) {
        return false;
    }

    for (size_t i = 0; i < other_len; ++i) {
        if (other[i] != this->m_bufferstart[i]) {
            return false;
        }
    }
    return true;
}

//
//  Static functions.
//

/**
 *  Return a new buffer which is the result of concatenating all buffer 
 *  instances in array together.
 * 
 *  @throw BufferException
 *      Raised if memory allocation was failed (XAPCORE_BUF_ERROR_ALLOC).
 *  @param buffers
 *      The buffer instances to concatenate.
 *  @param count
 *      The count of buffer instances.
 *  @return
 *      The new buffer.
 */
Buffer Buffer::concat(const Buffer buffers[], const size_t count) {
    size_t datalen = 0;
    for (size_t i = 0U; i < count; ++i) {
        datalen += buffers[i].get_length();
    }
    
    Buffer rst(datalen, true);
    size_t cursor = 0;
    for (size_t i = 0U; i < count; ++i) {
        buffers[i].copy(rst, cursor);
        cursor += buffers[i].get_length();
    }
    return rst;
}

//
//  Private methods.
//

/**
 *  Construct the object.
 * 
 *  @param buffer
 *      The shared pointer to buffer.
 *  @param length
 *      The length of buffer.
 */
Buffer::Buffer(std::shared_ptr<uint8_t> buffer, const size_t length) noexcept {
    this->prepare(buffer, 0, length);
}

/**
 *  Construct the object.
 * 
 *  @param buffer
 *      The shared pointer to buffer.
 *  @param offset
 *      The offset.
 *  @param length
 *      The length of buffer.
 */
Buffer::Buffer(
    std::shared_ptr<uint8_t> buffer,
    const size_t offset,
    const size_t length
) noexcept {
    this->prepare(buffer, offset, length);
}

/**
 *  Check if 'offset' or 'length is out of range.
 * 
 *  @throw BufferException
 *      Raisd if 'offset' or 'length is out of range 
 *      (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @param length
 *      The length.
 */
void Buffer::check_access(const size_t offset, const size_t length) const {
    if (length == 0) {
        return;
    }
    if (offset >= this->m_bufferlength) {
        throw BufferException("Offset overflowed.", XAPCORE_BUF_ERROR_OVERFLOW);
    }
    if (offset + length > this->m_bufferlength) {
        throw BufferException("Offset overflowd.", XAPCORE_BUF_ERROR_OVERFLOW);
    }
}

/**
 *  Prepare the buffer.
 * 
 *  @param buffer
 *      The shared pointer to buffer.
 *  @param offset
 *      The offset.
 *  @param length
 *      The length of buffer.
 */
void Buffer::prepare(
    std::shared_ptr<uint8_t> buffer,
    const size_t offset,
    const size_t length
) noexcept {
    this->m_buffer = buffer;
    this->m_bufferstart = buffer.get() + offset;
    this->m_bufferend = this->m_bufferstart + length;
    this->m_bufferlength = length;
}

//
//  Private functions.
//

/**
 *  Free buffer space.
 * 
 *  @param ptr
 *      The pointer to buffer which would be free.
 */
static void buffer_free_space(uint8_t *ptr) noexcept {
    delete[] ptr;
}

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap
