//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <cmath>
#include <xap/core/buffer/error.h>
#include <xap/core/buffer/buffer.h>

namespace xap {
namespace core {
namespace buffer {

//
//  Private functions declare.
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
Buffer::Buffer(const Buffer &source) {
    this->m_buffer = source.m_buffer;
    this->m_bufferstart = source.m_bufferstart;
    this->m_bufferend = source.m_bufferend;
    this->m_bufferlength = source.m_bufferlength;
}

/**
 *  Construct the object (, and initial all zero).
 * 
 *  @param length
 *      The length of buffer.
 */
Buffer::Buffer(const size_t length) {
    uint8_t *data;
    if (length == 0U) {
        data = new uint8_t[1];
    } else {
        data = new uint8_t[length];
    }
    std::shared_ptr<uint8_t> buffer(data, buffer_free_space);
    this->prepare(buffer, 0U, length);
    this->fill(0x00);
}

/**
 *  Construct the object.
 * 
 *  @param length
 *      The length of buffer.
 *  @param unsafe
 *      True if not initialze with zero.
 */
Buffer::Buffer(const size_t length, const bool unsafe) {
    uint8_t *data = new uint8_t[length];
    std::shared_ptr<uint8_t> buffer(data, buffer_free_space);
    this->prepare(buffer, 0U, length);
    if (!unsafe) {
        this->fill(0x00);
    }
}

/**
 *  Construct (copy) the object.
 * 
 *  @param data
 *      The source data.
 *  @param datalen
 *      The length of source data.
 */
Buffer::Buffer(const uint8_t *data, const size_t datalen) {
    uint8_t *inner = new uint8_t[datalen];
    memcpy(inner, data, datalen);
    std::shared_ptr<uint8_t> buffer(inner, buffer_free_space);
    this->prepare(buffer, 0U, datalen);
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
Buffer& Buffer::operator=(const Buffer &source) {
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
 *      Raised if 'offset' and 'length' is out of range 
 *      (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @param length
 *      The length
 *  @return
 *      The new buffer.
 */
Buffer Buffer::slice(const size_t offset, const size_t length) const {
    this->check_access(offset, length);
    return Buffer(
        this->m_buffer, 
        (this->m_bufferstart - this->m_buffer.get()) + offset, 
        length
    );
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
 *      The unsigned 16-bit integer.
 */
uint16_t Buffer::read_uint16_be(const size_t offset) const {
    this->check_access(offset, 2U);
    return static_cast<uint16_t>(
        (static_cast<uint16_t>(this->m_bufferstart[offset + 0U]) << 8U) |
        (static_cast<uint16_t>(this->m_bufferstart[offset + 1U]))
    );
}

/**
 *  Read an unsigned 16-bit integer with little-endian.
 * 
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 16-bit integer.
 */
uint16_t Buffer::read_uint16_le(const size_t offset) const {
    this->check_access(offset, 2U);
    return static_cast<uint16_t>(
        (static_cast<uint16_t>(this->m_bufferstart[offset + 1U]) << 8U) |
        (static_cast<uint16_t>(this->m_bufferstart[offset + 0U]))
    );
}

/**
 *  Read an unsigned 32-bit integer with big-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 32-bit integer.
 */
uint32_t Buffer::read_uint32_be(const size_t offset) const {
    this->check_access(offset, 4U);
    return static_cast<uint32_t>(
        (static_cast<uint32_t>(this->m_bufferstart[offset + 0U]) << 24U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 1U]) << 16U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 2U]) <<  8U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 3U]))
    );
}

/**
 *  Read an unsigned 32-bit integer with little-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 32-bit integer.
 */
uint32_t Buffer::read_uint32_le(const size_t offset) const {
    this->check_access(offset, 4U);
    return static_cast<uint32_t>(
        (static_cast<uint32_t>(this->m_bufferstart[offset + 3U]) << 24U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 2U]) << 16U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 1U]) <<  8U) |
        (static_cast<uint32_t>(this->m_bufferstart[offset + 0U]))
    );
}

#if defined(UINT64_MAX)

/**
 *  Read an unsigned 64-bit integer with big-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 64-bit integer.
 */
uint64_t Buffer::read_uint64_be(const size_t offset) const {
    this->check_access(offset, 8U);
    return static_cast<uint64_t>(
        (static_cast<uint64_t>(this->m_bufferstart[offset + 0U]) << 56U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 1U]) << 48U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 2U]) << 40U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 3U]) << 32U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 4U]) << 24U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 5U]) << 16U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 6U]) <<  8U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 7U]))
    );
}

/**
 *  Read an unsigned 64-bit integer with little-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The unsigned 64-bit integer.
 */
uint64_t Buffer::read_uint64_le(const size_t offset) const {
    this->check_access(offset, 8U);
    return static_cast<uint64_t>(
        (static_cast<uint64_t>(this->m_bufferstart[offset + 7U]) << 56U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 6U]) << 48U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 5U]) << 40U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 4U]) << 32U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 3U]) << 24U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 2U]) << 16U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 1U]) <<  8U) |
        (static_cast<uint64_t>(this->m_bufferstart[offset + 0U]))
    );
}

#endif  //  #if defined(UINT64_MAX)

/**
 *  Read a signal-precision float-point value with big-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The signal-precision float-point value.
 */
float Buffer::read_float_be(const size_t offset) const {
    return this->read_ieee_754_float(false, offset);
}

/**
 *  Read a signal-precision float-point value with little-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The signal-precision float-point value.
 */
float Buffer::read_float_le(const size_t offset) const {
    return this->read_ieee_754_float(true, offset);
}

/**
 *  Read a double-precision float-point value with big-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The double-precision float-point value.
 */
double Buffer::read_double_be(const size_t offset) const {
    return this->read_ieee_754_double(false, offset);
}

/**
 *  Read a double-precision float-point value with little-endian.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The double-precision float-point value.
 */
double Buffer::read_double_le(const size_t offset) const {
    return this->read_ieee_754_double(true, offset);
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
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>((value & 0xFF00) >> 8U);
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>(value & 0x00FF);
}

/**
 *  Write unsigned 16-bit integer with little-endian at the specified 
 *  offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 16-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint16_le(const uint16_t value, const size_t offset) {
    this->check_access(offset, 2U);
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>((value & static_cast<uint16_t>(0xFF00)) >> 8U);
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>(value & static_cast<uint16_t>(0x00FF));
}

/**
 *  Write unsigned 32-bit integer with big-endian at the specified 
 *  offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 32-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint32_be(const uint32_t value, const size_t offset) {
    this->check_access(offset, 4U);
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0xFF000000)) >>24U);
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0x00FF0000)) >>16U);
    this->m_bufferstart[offset + 2U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0x0000FF00)) >> 8U);
    this->m_bufferstart[offset + 3U] = 
        static_cast<uint8_t>(value & static_cast<uint32_t>(0x000000FF));
}

/**
 *  Write unsigned 32-bit integer with little-endian at the specified 
 *  offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 32-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint32_le(const uint32_t value, const size_t offset) {
    this->check_access(offset, 4U);
    this->m_bufferstart[offset + 3U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0xFF000000)) >>24U);
    this->m_bufferstart[offset + 2U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0x00FF0000)) >>16U);
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>((value & static_cast<uint32_t>(0x0000FF00)) >> 8U);
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>(value & static_cast<uint32_t>(0x000000FF));
}

#if defined(UINT64_MAX)

/**
 *  Write unsigned 64-bit integer with big-endian at the specified 
 *  offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 64-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint64_be(const uint64_t value, const size_t offset) {
    this->check_access(offset, 8U);
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0xFF00000000000000)) >> 56U
        );
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x00FF000000000000)) >> 48U
        );
    this->m_bufferstart[offset + 2U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x0000FF0000000000)
        ) >> 40U);
    this->m_bufferstart[offset + 3U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x000000FF00000000)) >> 32U
        );
    this->m_bufferstart[offset + 4U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x00000000FF000000)) >> 24U
        );
    this->m_bufferstart[offset + 5U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x0000000000FF0000)) >> 16U
        );
    this->m_bufferstart[offset + 6U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x000000000000FF00)) >>  8U
        );
    this->m_bufferstart[offset + 7U] = 
        static_cast<uint8_t>(
            value & static_cast<uint64_t>(0x00000000000000FF)
        );
}

/**
 *  Write unsigned 64-bit integer with little-endian at the specified 
 *  offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The unsigned 64-bit integer.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_uint64_le(const uint64_t value, const size_t offset) {
    this->check_access(offset, 8U);
    this->m_bufferstart[offset + 7U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0xFF00000000000000)) >> 56U
        );
    this->m_bufferstart[offset + 6U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x00FF000000000000)) >> 48U
        );
    this->m_bufferstart[offset + 5U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x0000FF0000000000)
        ) >> 40U);
    this->m_bufferstart[offset + 4U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x000000FF00000000)) >> 32U
        );
    this->m_bufferstart[offset + 3U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x00000000FF000000)) >> 24U
        );
    this->m_bufferstart[offset + 2U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x0000000000FF0000)) >> 16U
        );
    this->m_bufferstart[offset + 1U] = 
        static_cast<uint8_t>(
            (value & static_cast<uint64_t>(0x000000000000FF00)) >>  8U
        );
    this->m_bufferstart[offset + 0U] = 
        static_cast<uint8_t>(
            value & static_cast<uint64_t>(0x00000000000000FF)
        );
}
#endif  //  #if defined(UINT64_MAX)

/**
 *  Write signal-precision float-point with big-endian at the 
 *  specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The signal-precision float-point value.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_float_be(const float value, const size_t offset) {
    this->write_ieee_754_float(value, false, offset);
}

/**
 *  Write signal-precision float-point with little-endian at the 
 *  specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The signal-precision float-point value.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_float_le(const float value, const size_t offset) {
    this->write_ieee_754_float(value, true, offset);
}

/**
 *  Write double-precision float-point with big-endian at the 
 *  specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The double-precision float-point value.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_double_be(const double value, const size_t offset) {
    this->write_ieee_754_double(value, false, offset);
}

/**
 *  Write double-precision float-point with little-endian at the 
 *  specified offset.
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The double-precision float-point value.
 *  @param offset
 *      The offset (default 0).
 */
void Buffer::write_double_le(const double value, const size_t offset) {
    this->write_ieee_754_double(value, true, offset);
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
 *  @param buffers
 *      The buffer instances to concatenate.
 *  @param count
 *      The count of buffer instances.
 *  @return
 *      The new buffer.
 */
Buffer Buffer::concat(const Buffer buffers[], const size_t count) {
    size_t datalen = 0U;
    for (size_t i = 0U; i < count; ++i) {
        datalen += buffers[i].get_length();
    }
    
    Buffer rst(datalen, true);
    size_t cursor = 0U;
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
Buffer::Buffer(std::shared_ptr<uint8_t> buffer, const size_t length) {
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
) {
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
) {
    this->m_buffer = buffer;
    this->m_bufferstart = buffer.get() + offset;
    this->m_bufferend = this->m_bufferstart + length;
    this->m_bufferlength = length;
}

/**
 *  Read IEEE 754 signal-precision float-point value.
 * 
 *  @param isle
 *      True if with little-endian.
 *  @param offset
 *      The offset.
 *  @return
 *      The signal-precision float-point value.
 */
float Buffer::read_ieee_754_float(
    const bool isle, 
    const size_t offset
) const {
    //  Check access.
    this->check_access(offset, 4U);
    
    const static size_t bytes = 4U;
    const static size_t mantissa_length = 23U;
    const static size_t exponent_length = 8U;
    
    const static double exponent_max  = 255.0;  //  (1U << exponent_length) - 1;
    const static double exponent_bias = 127.0;  //  (exponent_max >> 1U);
    const static size_t rt = std::pow(2, -24) - std::pow(2, -77);

    uint32_t m_bits = 0U;
    uint16_t e_bits = 0U;
    uint8_t s_bits = 0U;
    if (isle) {
        s_bits = (this->m_bufferstart[offset + 3U] & 0x80) >> 7U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 3U] & 0x7F
        ) << 1U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 2U] & 0x80
        ) >> 7U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 2U] & 0x7F
        ) << 16U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 1U]
        ) << 8U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 0U]
        );
    } else {
        s_bits = (this->m_bufferstart[offset + 0U] & 0x80) >> 7U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 0U] & 0x7F
        ) << 1U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 1U] & 0x80
        ) >> 7U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 1U] & 0x7F
        ) << 16U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 2U]
        ) << 8U;
        m_bits |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 3U]
        );
    }

    double s = ((s_bits == 0) ? 1 : -1);
    double e = static_cast<double>(e_bits);
    double m = static_cast<double>(m_bits);

    if (e == 0) {
        e = 1 - exponent_bias;
    } else if (e == exponent_max) {
        if (m == 0) {
            return s * INFINITY;
        } else {
            return std::nan("1");
        }
    } else {
        m += std::pow(2, mantissa_length);
        e -= exponent_bias;
    }

    return s * m * std::pow(2, e - mantissa_length);
}

/**
 *  Read IEEE 754 double-precision float-point value.
 * 
 *  @param isle
 *      True if with little-endian.
 *  @param offset
 *      The offset.
 *  @return
 *      The double-precision float-point value.
 */
double Buffer::read_ieee_754_double(
    const bool isle, 
    const size_t offset
) const {
    //  Check access.
    this->check_access(offset, 8U);

    const static size_t bytes = 8U;
    const static size_t mantissa_length = 52U;
    const static size_t exponent_length = 11U;
    
    const static double exponent_max  = 2047.0; //  (1U << exponent_length) - 1;
    const static double exponent_bias = 1023.0; //  (exponent_max >> 1U);
    const static size_t rt = 0U;

    uint32_t m_bits1 = 0U, m_bits2 = 0U;
    uint16_t e_bits = 0U;
    uint8_t s_bits = 0U;
    if (isle) {
        s_bits = (
            this->m_bufferstart[offset + 7U] & 0x80
        ) >> 7U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 7U] & 0x7F
        ) << 4U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 6U] & 0xF0
        ) >> 4U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 6U] & 0x0F
        ) << 16U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 5U]
        ) << 8U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 4U]
        );
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 3U]
        ) << 24U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 2U]
        ) << 16U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 1U]
        ) << 8U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 0U]
        );
    } else {
        s_bits = (this->m_bufferstart[offset + 0U] & 0x80) >> 7U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 0U] & 0x7F
        ) << 4U;
        e_bits |= static_cast<uint16_t>(
            this->m_bufferstart[offset + 1U] & 0xF0
        ) >> 4U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 1U] & 0x0F
        ) << 16U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 2U]
        ) << 8U;
        m_bits2 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 3U]
        );
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 4U]
        ) << 24U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 5U]
        ) << 16U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 6U]
        ) << 8U;
        m_bits1 |= static_cast<uint32_t>(
            this->m_bufferstart[offset + 7U]
        );
    }

    double s = ((s_bits == 0) ? 1 : -1);
    double e = static_cast<double>(e_bits);
    double m = static_cast<double>(m_bits1);
    m += static_cast<double>(m_bits2) * 0xFFFFFFFF;

    if (e == 0) {
        e = 1 - exponent_bias;
    } else if (e == exponent_max) {
        if (m == 0) {
            return s * INFINITY;
        } else {
            return std::nan("1");
        }
    } else {
        m += std::pow(2, mantissa_length);
        e -= exponent_bias;
    }

    return s * m * std::pow(2, e - mantissa_length);
}

/**
 *  Write IEEE 754 single-precision float-point value.
 * 
 *  @param value    
 *      The single-precision float-point value.
 *  @param isle
 *      True if with little-endian.
 *  @param offset
 *      The offset.
 */
void Buffer::write_ieee_754_float(
    const float value, 
    const bool isle, 
    const size_t offset
) {
    //  Check access.
    this->check_access(offset, 4U);

    const static size_t bytes = 4U;
    const static size_t mantissa_length = 23U;
    const static size_t exponent_length = 8U;
    
    const static double exponent_max  = 255.0;  //  (1U << exponent_length) - 1;
    const static double exponent_bias = 127.0;  //  (exponent_max >> 1U);
    const static double rt = std::pow(2, -24) - std::pow(2, -77);

    double m_value = std::abs(value);

    double e, //  Exponent bits value.
           m; //  Mantissa bits value.
    
    bool isnan = std::isnan(m_value);
    if (isnan || m_value == INFINITY) {
        //
        //  NaN and infinity.
        //
        m = isnan ? 1 : 0;
        e = exponent_max;
    } else {
        e = std::floor(std::log(m_value) / std::log(2));
        double c = std::pow(2, -e);
        if (m_value * c < 1.0) {
            --e;
            c *= 2.0;
        }

        if (e + exponent_bias >= 1.0) {
            m_value += rt / c;
        } else {
            m_value += rt * std::pow(2, 1 - exponent_bias);
        }

        if (m_value * c >= 2.0) {
            ++e;
            c /= 2.0;
        }
        if (e + exponent_bias >= exponent_max) {
            m = 0U;
            e = exponent_max;
        } else if (e + exponent_bias >= 1.0) {
            m = ((m_value * c) - 1.0) * std::pow(2, mantissa_length);
            e += exponent_bias;
        } else {
            m = m_value * 
                std::pow(2, exponent_bias - 1U) * 
                std::pow(2, mantissa_length);
            e = 0U;
        }
    }

    //  Result bits.
    const uint32_t m_bits = static_cast<uint32_t>(m);
    const uint16_t e_bits = static_cast<uint16_t>(e);
    const uint8_t s_bits = 
        ((value < 0) || (value == 0 && 1 / value < 0)) ? 1U : 0U;

    if (isle) {
        this->m_bufferstart[offset + 0U] = 
            static_cast<uint8_t>(m_bits & 0xFF);
        this->m_bufferstart[offset + 1U] = 
            static_cast<uint8_t>((m_bits >> 8U) & 0xFF);
        this->m_bufferstart[offset + 2U] = 
            static_cast<uint8_t>((m_bits >> 16U) & 0x7F);
        this->m_bufferstart[offset + 2U] |= 
            static_cast<uint8_t>((e_bits << 7U) & 0x80);
        this->m_bufferstart[offset + 3U] = 
            static_cast<uint8_t>((e_bits >> 1U) & 0x7F);
        this->m_bufferstart[offset + 3U] |= 
            (s_bits << 7U);
    } else {
        this->m_bufferstart[offset + 3U] = 
            static_cast<uint8_t>(m_bits & 0xFF);
        this->m_bufferstart[offset + 2U] = 
            static_cast<uint8_t>((m_bits >> 8U) & 0xFF);
        this->m_bufferstart[offset + 1U] = 
            static_cast<uint8_t>((m_bits >> 16U) & 0x7F);
        this->m_bufferstart[offset + 1U] |= 
            static_cast<uint8_t>((e_bits << 7U) & 0x80);
        this->m_bufferstart[offset + 0U] = 
            static_cast<uint8_t>((e_bits >> 1U) & 0x7F);
        this->m_bufferstart[offset + 0U] |= 
            (s_bits << 7U);
    }
}

/**
 *  Write IEEE 754 double-precision float-point value.
 * 
 *  @param value    
 *      The double-precision float-point value.
 *  @param isle
 *      True if with little-endian.
 *  @param offset
 *      The offset.
 */
void Buffer::write_ieee_754_double(
    const double value, 
    const bool isle, 
    const size_t offset
) {
    //  Check access.
    this->check_access(offset, 8U);

    const static size_t bytes = 8U;
    const static size_t mantissa_length = 52U;
    const static size_t exponent_length = 11U;
    
    const static double exponent_max  = 2047.0; //  (1U << exponent_length) - 1;
    const static double exponent_bias = 1023.0; //  (exponent_max >> 1U);
    const static double rt = 0U;

    double m_value = std::abs(value);

    double e, //  Exponent bits value.
           m; //  Mantissa bits value.
    
    bool isnan = std::isnan(m_value);
    if (isnan || m_value == INFINITY) {
        //
        //  NaN and infinity.
        //
        m = isnan ? 1 : 0;
        e = exponent_max;
    } else {
        e = std::floor(std::log(m_value) / std::log(2));
        double c = std::pow(2, -e);
        if (m_value * c < 1.0) {
            --e;
            c *= 2.0;
        }
        if (m_value * c >= 2.0) {
            ++e;
            c /= 2.0;
        }
        if (e + exponent_bias >= exponent_max) {
            m = 0U;
            e = exponent_max;
        } else if (e + exponent_bias >= 1.0) {
            m = ((m_value * c) - 1.0) * std::pow(2, mantissa_length);
            e += exponent_bias;
        } else {
            m = m_value * 
                std::pow(2, exponent_bias - 1U) * 
                std::pow(2, mantissa_length);
            e = 0U;
        }
    }

    //  Result bits.
    const uint32_t m_bits1 = static_cast<uint32_t>(m);
    const uint32_t m_bits2 = static_cast<uint32_t>(
        m / static_cast<double>(UINT32_MAX)
    );
    uint16_t e_bits = static_cast<uint16_t>(e);
    uint8_t s_bits = ((value < 0) || (value == 0 && 1 / value < 0)) ? 1U : 0U;

    if (isle) {
        this->m_bufferstart[offset + 0U] = 
            static_cast<uint8_t>(m_bits1 & 0xFF);
        this->m_bufferstart[offset + 1U] = 
            static_cast<uint8_t>((m_bits1 >> 8U) & 0xFF);
        this->m_bufferstart[offset + 2U] = 
            static_cast<uint8_t>((m_bits1 >> 16U) & 0xFF);
        this->m_bufferstart[offset + 3U] = 
            static_cast<uint8_t>((m_bits1 >> 24U) & 0xFF);
        this->m_bufferstart[offset + 4U] = 
            static_cast<uint8_t>((m_bits2 >> 0U) & 0xFF);
        this->m_bufferstart[offset + 5U] = 
            static_cast<uint8_t>((m_bits2 >> 8U) & 0xFF);
        this->m_bufferstart[offset + 6U] = 
            static_cast<uint8_t>((m_bits2 >> 16U) & 0x0F);
        this->m_bufferstart[offset + 6U] |= 
            static_cast<uint8_t>(e_bits & 0x0F) << 4U;
        this->m_bufferstart[offset + 7U] = 
            static_cast<uint8_t>((e_bits >> 4U) & 0x7F);
        this->m_bufferstart[offset + 7U] |= 
            (s_bits << 7U);
    } else {
        this->m_bufferstart[offset + 7U] = 
            static_cast<uint8_t>(m_bits1 & 0xFF);
        this->m_bufferstart[offset + 6U] = 
            static_cast<uint8_t>((m_bits1 >> 8U) & 0xFF);
        this->m_bufferstart[offset + 5U] = 
            static_cast<uint8_t>((m_bits1 >> 16U) & 0xFF);
        this->m_bufferstart[offset + 4U] = 
            static_cast<uint8_t>((m_bits1 >> 24U) & 0xFF);
        this->m_bufferstart[offset + 3U] = 
            static_cast<uint8_t>((m_bits2 >> 0U) & 0xFF);
        this->m_bufferstart[offset + 2U] = 
            static_cast<uint8_t>((m_bits2 >> 8U) & 0xFF);
        this->m_bufferstart[offset + 1U] = 
            static_cast<uint8_t>((m_bits2 >> 16U) & 0x0F);
        this->m_bufferstart[offset + 1U] |= 
            static_cast<uint8_t>(e_bits & 0x0F) << 4U;
        this->m_bufferstart[offset + 0U] = 
            static_cast<uint8_t>((e_bits >> 4U) & 0x7F);
        this->m_bufferstart[offset + 0U] |= 
            (s_bits << 7U);
    }
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
