//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_CORE_BUFFER_BUFFER_H__
#define XAP_CORE_BUFFER_BUFFER_H__

//
//  Imports.
//
#include <memory>
#include <stdint.h>
#include <xap/core/buffer/build.h>
#include <xap/core/buffer/error.h>

namespace xap {
namespace core {
namespace buffer {

//
//  Classes.
//
class Buffer {

public:
    //
    //  Constructor & desctructor.
    //

    /**
     *  Construct the object (, and initial all zero).
     * 
     *  @param length
     *      The length of buffer.
     */
    Buffer(const size_t length = 0U);

    /**
     *  Construct (copy) the object.
     * 
     *  @param source
     *      The source buffer.
     */
    Buffer(const Buffer &source);

    /**
     *  Construct the object.
     * 
     *  @param length
     *      The length of buffer.
     *  @param unsafe
     *      True if not initialze with zero.
     */
    Buffer(const size_t length, const bool unsafe);

    /**
     *  Construct (copy) the object.
     * 
     *  @param data
     *      The source data.
     *  @param datalen
     *      The length of source data.
     */
    Buffer(const uint8_t *data, const size_t datalen);

    /**
     *  Destruct the object.
     */
    ~Buffer() noexcept;
    
    //
    //  Public operators.
    //

    /**
     *  Operator '='.
     * 
     *  @param source
     *      The source buffer.
     *  @return
     *      The target buffer.
     */
    Buffer& operator=(const Buffer &source);

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
    uint8_t& operator[](const size_t offset) const;

    /**
     *  Operator '=='. Whether buffer is equal to others.
     * 
     *  @param other
     *      The other buffer.
     *  @return
     *      True if equal.
     */
    bool operator==(const Buffer& other) const noexcept;

    /**
     *  Operator '!='. Whether buffer is not equal to others.
     * 
     *  @param other
     *      The other buffer.
     *  @return
     *      True if not equal.
     */
    bool operator!=(const Buffer& other) const noexcept;
    
    //
    //  Public methods.
    //

    /**
     *  Get the length of buffer.
     * 
     *  @return
     *      The length.
     */
    size_t get_length() const noexcept;

    /**
     *  Get the raw pointer of buffer.
     * 
     *  @return
     *      The raw pointer.
     */
    uint8_t* get_pointer() noexcept;

    /**
     *  Get the raw pointer of buffer.
     * 
     *  @return
     *      The raw pointer.
     */
    const uint8_t* get_pointer() const noexcept;

    /**
     *  Return a new Buffer that references the same memory as the original, but
     *  offset and cropped by the 'offset' indices.
     * 
     *  @throw BufferException
     *      Raised if 'offset' and 'length' is out of range 
     *      (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param offset
     *      The offset.
     *  @return
     *      The new buffer.
     */
    Buffer slice(const size_t offset) const;

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
    Buffer slice(const size_t offset, const size_t length) const;
    
    /**
     *  Copies data to destination.
     * 
     *  @param destination
     *      The buffer to copy into.
     *  @return
     *      The number of bytes copied.
     */
    size_t copy(Buffer &destination) const noexcept;
    
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
    size_t copy(Buffer &destination, const size_t destination_offset) const;
    
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
    size_t copy(
        Buffer &destination, 
        const size_t destination_offset, 
        const size_t src_offset
    ) const;

    /**
     *  Fills buffer with the specified.
     * 
     *  @param value
     *      The value with which to fill buffer.
     */
    void fill(const uint8_t value) noexcept;

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
    void fill(const uint8_t value, const size_t offset, const size_t length);
    
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
    uint8_t read_uint8(const size_t offset = 0U) const;
    
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
    uint16_t read_uint16_be(const size_t offset = 0U) const;

    /**
     *  Read an unsigned 16-bit integer with little-endian.
     * 
     *  @throw BufferException
     *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param offset
     *      The offset.
     *  @return
     *      The unsigned 16-bit integer.
     */
    uint16_t read_uint16_le(const size_t offset = 0U) const;

    /**
     *  Read a signed 16-bit integer with little-endian.
     * 
     *  @throw BufferException
     *      Raised if 'offset' is out of range (XAPCORE_BUF_ERROR_OVERFLOW).
     *  @param offset
     *      The offset.
     *  @return
     *      The signed 16-bit integer.
     */
    int16_t read_sint16_le(const size_t offset = 0U) const;

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
    uint32_t read_uint32_be(const size_t offset = 0U) const;

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
    uint32_t read_uint32_le(const size_t offset = 0U) const;

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
    uint64_t read_uint64_be(const size_t offset = 0U) const;

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
    uint64_t read_uint64_le(const size_t offset = 0U) const;
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
    float read_float_be(const size_t offset = 0U) const;

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
    float read_float_le(const size_t offset = 0U) const;

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
    double read_double_be(const size_t offset = 0U) const;

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
    double read_double_le(const size_t offset = 0U) const;

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
    void write_uint8(const uint8_t value, const size_t offset = 0U);

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
    void write_uint16_be(const uint16_t value, const size_t offset = 0U);

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
    void write_uint16_le(const uint16_t value, const size_t offset = 0U);

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
    void write_uint32_be(const uint32_t value, const size_t offset = 0U);

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
    void write_uint32_le(const uint32_t value, const size_t offset = 0U);

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
    void write_uint64_be(const uint64_t value, const size_t offset = 0U);

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
    void write_uint64_le(const uint64_t value, const size_t offset = 0U);

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
    void write_float_be(const float value, const size_t offset = 0U);

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
    void write_float_le(const float value, const size_t offset = 0U);

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
    void write_double_be(const double value, const size_t offset = 0U);

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
    void write_double_le(const double value, const size_t offset = 0U);
    
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
    bool is_equal(const uint8_t *other, const size_t other_len) const noexcept;
    
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
    static Buffer concat(
        const Buffer buffers[], 
        const size_t count
    );
    
private:
    //
    //  Private constructor.
    //

    /**
     *  Construct the object.
     * 
     *  @param buffer
     *      The shared pointer to buffer.
     *  @param length
     *      The length of buffer.
     */
    Buffer(std::shared_ptr<uint8_t> buffer, const size_t length);
    
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
    Buffer(
        std::shared_ptr<uint8_t>    buffer,
        const size_t                offset,
        const size_t                length
    );

    //
    //  Private methods.
    //

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
    void check_access(
        const size_t offset,
        const size_t length
    ) const;
    
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
    void prepare(
        std::shared_ptr<uint8_t>    buffer,
        const size_t                offset,
        const size_t                length
    );

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
    float read_ieee_754_float(
        const bool isle, 
        const size_t offset = 0U
    ) const;

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
    double read_ieee_754_double(
        const bool isle, 
        const size_t offset = 0U
    ) const;

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
    void write_ieee_754_float(
        const float value,
        const bool isle,
        const size_t offset = 0U
    );

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
    void write_ieee_754_double(
        const double value, 
        const bool isle, 
        const size_t offset = 0U
    );

    //
    //  Members.
    //
    std::shared_ptr<uint8_t> m_buffer;
    uint8_t                 *m_bufferstart;
    uint8_t                 *m_bufferend;
    size_t                   m_bufferlength;
};

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap


#endif  //  #ifndef XAP_CORE_BUFFER_BUFFER_H__
