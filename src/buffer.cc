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
        this->prepare(buffer, 0U, length);
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
        this->prepare(buffer, 0U, length);
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
        this->prepare(buffer, 0U, datalen);
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->read_float_backgrounds(offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->read_float_forwards(offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->read_float_forwards(offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->read_float_forwards(offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->read_float_backgrounds(offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->read_float_backgrounds(offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->read_double_backwards(offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->read_double_forwards(offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->read_double_forwards(offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->read_double_forwards(offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->read_double_backwards(offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->read_double_backwards(offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->write_float_backgrounds(value, offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->write_float_forwards(value, offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->write_float_forwards(value, offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->write_float_forwards(value, offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->write_float_backgrounds(value, offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->write_float_backgrounds(value, offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->write_double_backgrounds(value, offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->write_double_forwards(value, offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->write_double_forwards(value, offset);
#else
# error "Invalid compiler endian order."
#endif
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
#if defined(XAP_CORE_BUFFER_LITTLE_ENDIAN)
    this->write_double_forwards(value, offset);
#elif defined(XAP_CORE_BUFFER_BIG_ENDIAN)
    this->write_double_backgrounds(value, offset);
#elif defined(XAP_CORE_BUFFER_POP_ENDIAN)
    this->write_double_backgrounds(value, offset);
#else
# error "Invalid compiler endian order."
#endif
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
 *  Read signal-precision float-point in order as following:
 *          
 *                       +-------+---+---+---+-----+----+--------+
 *      Source bites:    | LSb 0 | 1 | 2 | 3 | ... | 30 | MSb 31 |
 *                       +-------+---+---+---+-----+----+--------+
 *      Result bites:    | LSb 0 | 1 | 2 | 3 | ... | 30 | MSb 31 |
 *                       +-------+---+---+---+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The signal-precision float-point value.
 */
float Buffer::read_float_forwards(const size_t offset) const {
    this->check_access(offset, 4U);
    
    float rst;
    uint8_t *rst_ptr = reinterpret_cast<uint8_t *>(&rst);
    rst_ptr[0] = this->m_bufferstart[offset + 0U];
    rst_ptr[1] = this->m_bufferstart[offset + 1U];
    rst_ptr[2] = this->m_bufferstart[offset + 2U];
    rst_ptr[3] = this->m_bufferstart[offset + 3U];
    return rst;
}

/**
 *  Read signal-precision float-point in reverse order as following:
 *          
 *                       +--------+----+----+----+-----+----+--------+
 *      Source bites:    | LSb 0  | 1  | 2  | 3  | ... | 30 | MSb 31 |
 *                       +--------+----+----+----+-----+----+--------+
 *      Result bites:    | MSb 31 | 30 | 29 | 28 | ... | 1  | LSb 0  |
 *                       +--------+----+----+----+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The signal-precision float-point value.
 */
float Buffer::read_float_backgrounds(const size_t offset) const {
    this->check_access(offset, 4U);

    float rst;
    uint8_t *rst_ptr = reinterpret_cast<uint8_t *>(&rst);
    rst_ptr[0] = this->m_bufferstart[offset + 3U];
    rst_ptr[1] = this->m_bufferstart[offset + 2U];
    rst_ptr[2] = this->m_bufferstart[offset + 1U];
    rst_ptr[3] = this->m_bufferstart[offset + 0U];
    return rst;
}

/**
 *  Read double-precision float-point in order as following:
 * 
 *                       +-------+---+---+---+-----+----+--------+
 *      Source bites:    | LSb 0 | 1 | 2 | 3 | ... | 62 | MSb 63 |
 *                       +-------+---+---+---+-----+----+--------+
 *      Result bites:    | LSb 0 | 1 | 2 | 3 | ... | 62 | MSb 63 |
 *                       +-------+---+---+---+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The double-precision float-point value.
 */
double Buffer::read_double_forwards(const size_t offset) const {
    this->check_access(offset, 8U);

    double rst;
    uint8_t *rst_ptr = reinterpret_cast<uint8_t *>(&rst);
    rst_ptr[0] = this->m_bufferstart[offset + 0U];
    rst_ptr[1] = this->m_bufferstart[offset + 1U];
    rst_ptr[2] = this->m_bufferstart[offset + 2U];
    rst_ptr[3] = this->m_bufferstart[offset + 3U];
    rst_ptr[4] = this->m_bufferstart[offset + 4U];
    rst_ptr[5] = this->m_bufferstart[offset + 5U];
    rst_ptr[6] = this->m_bufferstart[offset + 6U];
    rst_ptr[7] = this->m_bufferstart[offset + 7U];
    return rst;
}

/**
 *  Read double-precision float-point in reverse order as following:
 *          
 *                       +--------+----+----+----+-----+----+--------+
 *      Source bites:    | LSb 0  | 1  | 2  | 3  | ... | 62 | MSb 63 |
 *                       +--------+----+----+----+-----+----+--------+
 *      Result bites:    | MSb 63 | 62 | 61 | 60 | ... | 1  | LSb 0  |
 *                       +--------+----+----+----+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param offset
 *      The offset.
 *  @return
 *      The double-precision float-point value.
 */
double Buffer::read_double_backwards(const size_t offset) const {
    this->check_access(offset, 8U);

    double rst;
    uint8_t *rst_ptr = reinterpret_cast<uint8_t *>(&rst);
    rst_ptr[0] = this->m_bufferstart[offset + 7U];
    rst_ptr[1] = this->m_bufferstart[offset + 6U];
    rst_ptr[2] = this->m_bufferstart[offset + 5U];
    rst_ptr[3] = this->m_bufferstart[offset + 4U];
    rst_ptr[4] = this->m_bufferstart[offset + 3U];
    rst_ptr[5] = this->m_bufferstart[offset + 2U];
    rst_ptr[6] = this->m_bufferstart[offset + 1U];
    rst_ptr[7] = this->m_bufferstart[offset + 0U];
    return rst;
}

/**
 *  Write signal-precision float-point in order as following:
 *  
 *                       +-------+---+---+---+-----+----+--------+
 *      Source bites:    | LSb 0 | 1 | 2 | 3 | ... | 30 | MSb 31 |
 *                       +-------+---+---+---+-----+----+--------+
 *      Result bites:    | LSb 0 | 1 | 2 | 3 | ... | 30 | MSb 31 |
 *                       +-------+---+---+---+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The signal-precision float-point value.
 *  @param offset
 *      The offset.
 */
void Buffer::write_float_forwards(
    const float value, 
    const size_t offset
) {
    this->check_access(offset, 4U);

    const uint8_t *rst_ptr = reinterpret_cast<const uint8_t *>(&value);
    this->m_bufferstart[offset + 0U] = rst_ptr[0];
    this->m_bufferstart[offset + 1U] = rst_ptr[1];
    this->m_bufferstart[offset + 2U] = rst_ptr[2];
    this->m_bufferstart[offset + 3U] = rst_ptr[3];
}

/**
 *  Write signal-precision float-point in reverse order as following:
 *          
 *                       +--------+----+----+----+-----+----+--------+
 *      Source bites:    | LSb 0  | 1  | 2  | 3  | ... | 30 | MSb 31 |
 *                       +--------+----+----+----+-----+----+--------+
 *      Result bites:    | MSb 31 | 30 | 29 | 28 | ... | 1  | LSb 0  |
 *                       +--------+----+----+----+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The signal-precision float-point value.
 *  @param offset
 *      The offset.
 */
void Buffer::write_float_backgrounds(
    const float value, 
    const size_t offset
) {
    this->check_access(offset, 4U);

    const uint8_t *rst_ptr = reinterpret_cast<const uint8_t *>(&value);
    this->m_bufferstart[offset + 0U] = rst_ptr[3];
    this->m_bufferstart[offset + 1U] = rst_ptr[2];
    this->m_bufferstart[offset + 2U] = rst_ptr[1];
    this->m_bufferstart[offset + 3U] = rst_ptr[0];
}

/**
 *  Write double-precision float-point in order as following:
 * 
 *                       +-------+---+---+---+-----+----+--------+
 *      Source bites:    | LSb 0 | 1 | 2 | 3 | ... | 62 | MSb 63 |
 *                       +-------+---+---+---+-----+----+--------+
 *      Result bites:    | LSb 0 | 1 | 2 | 3 | ... | 62 | MSb 63 |
 *                       +-------+---+---+---+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The double-precision float-point value.
 *  @param offset
 *      The offset.
 */
void Buffer::write_double_forwards(
    const double value, 
    const size_t offset
) {
    this->check_access(offset, 8U);

    const uint8_t *rst_ptr = reinterpret_cast<const uint8_t *>(&value);
    this->m_bufferstart[offset + 0U] = rst_ptr[0];
    this->m_bufferstart[offset + 1U] = rst_ptr[1];
    this->m_bufferstart[offset + 2U] = rst_ptr[2];
    this->m_bufferstart[offset + 3U] = rst_ptr[3];
    this->m_bufferstart[offset + 4U] = rst_ptr[4];
    this->m_bufferstart[offset + 5U] = rst_ptr[5];
    this->m_bufferstart[offset + 6U] = rst_ptr[6];
    this->m_bufferstart[offset + 7U] = rst_ptr[7];
}

/**
 *  Write double-precision float-point in reverse order as following:
 *          
 *                       +--------+----+----+----+-----+----+--------+
 *      Source bites:    | LSb 0  | 1  | 2  | 3  | ... | 62 | MSb 63 |
 *                       +--------+----+----+----+-----+----+--------+
 *      Result bites:    | MSb 63 | 62 | 61 | 60 | ... | 1  | LSb 0  |
 *                       +--------+----+----+----+-----+----+--------+
 * 
 *  @throw BufferException
 *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param value
 *      The double-precision float-point value.
 *  @param offset
 *      The offset.
 */
void Buffer::write_double_backgrounds(
    const double value, 
    const size_t offset
) {
    this->check_access(offset, 8U);

    const uint8_t *rst_ptr = reinterpret_cast<const uint8_t *>(&value);
    this->m_bufferstart[offset + 0U] = rst_ptr[7];
    this->m_bufferstart[offset + 1U] = rst_ptr[6];
    this->m_bufferstart[offset + 2U] = rst_ptr[5];
    this->m_bufferstart[offset + 3U] = rst_ptr[4];
    this->m_bufferstart[offset + 4U] = rst_ptr[3];
    this->m_bufferstart[offset + 5U] = rst_ptr[2];
    this->m_bufferstart[offset + 6U] = rst_ptr[1];
    this->m_bufferstart[offset + 7U] = rst_ptr[0];
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
