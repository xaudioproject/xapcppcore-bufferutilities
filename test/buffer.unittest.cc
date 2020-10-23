//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "common.h"

#include "xap/core/buffer/buffer.h"

#include <cmath>
#include <limits>
#include <stdio.h>

//
//  Functions.
//

/**
 *  Assert buffer read signal-precision float-point value with big-endian
 *  is equal to given float-point value.
 * 
 *  @param data
 *      The raw buffer data (which bytes other than float-point must be 0).
 *  @param datalen
 *      The length of raw buffer data.
 *  @param offset
 *      The offset.
 *  @param value
 *      The float-point value.
 *  @param message
 *      The message which would be shown if not equal.
 */
void xap_assert_buffer_equal_float_be(
    const uint8_t *data,
    const size_t datalen,
    const size_t offset,
    const float value,
    const char *message = nullptr
) {
    xap::core::buffer::Buffer buffer1(data, datalen);
    xap::test::assert_equal<float>(
        buffer1.read_float_be(offset),
        value,
        message
    );

    xap::core::buffer::Buffer buffer2(datalen, false);
    buffer2.write_float_be(value, offset);
    xap::test::assert_equal<xap::core::buffer::Buffer>(
        buffer1,
        buffer2,
        message
    );
}

/**
 *  Assert buffer read signal-precision float-point value with little-endian
 *  is equal to given float-point value.
 * 
 *  @param data
 *      The raw buffer data.
 *  @param datalen
 *      The length of raw buffer data.
 *  @param offset
 *      The offset.
 *  @param value
 *      The float-point value.
 *  @param message
 *      The message which would be shown if not equal.
 */
void xap_assert_buffer_equal_float_le(
    const uint8_t *data,
    const size_t datalen,
    const size_t offset,
    const float value,
    const char *message = nullptr
) {
    xap::core::buffer::Buffer buffer1(data, datalen);
    xap::test::assert_equal<float>(
        buffer1.read_float_le(offset),
        value,
        message
    );

    xap::core::buffer::Buffer buffer2(datalen, false);
    buffer2.write_float_le(value, offset);
    xap::test::assert_equal<xap::core::buffer::Buffer>(
        buffer1,
        buffer2,
        message
    );
}

/**
 *  Assert buffer read double-precision float-point value with big-endian
 *  is equal to given float-point value.
 * 
 *  @param data
 *      The raw buffer data (which bytes other than float-point must be 0).
 *  @param datalen
 *      The length of raw buffer data.
 *  @param offset
 *      The offset.
 *  @param value
 *      The float-point value.
 *  @param message
 *      The message which would be shown if not equal.
 */
void xap_assert_buffer_equal_double_be(
    const uint8_t *data,
    const size_t datalen,
    const size_t offset,
    const double value,
    const char *message = nullptr
) {
    xap::core::buffer::Buffer buffer1(data, datalen);
    xap::test::assert_equal<float>(
        buffer1.read_double_be(offset),
        value,
        message
    );

    xap::core::buffer::Buffer buffer2(datalen, false);
    buffer2.write_double_be(value, offset);
    xap::test::assert_equal<xap::core::buffer::Buffer>(
        buffer1,
        buffer2,
        message
    );
}

/**
 *  Assert buffer read double-precision float-point value with little-endian
 *  is equal to given float-point value.
 * 
 *  @param data
 *      The raw buffer data.
 *  @param datalen
 *      The length of raw buffer data.
 *  @param offset
 *      The offset.
 *  @param value
 *      The float-point value.
 *  @param message
 *      The message which would be shown if not equal.
 */
void xap_assert_buffer_equal_double_le(
    const uint8_t *data,
    const size_t datalen,
    const size_t offset,
    const double value,
    const char *message = nullptr
) {
    xap::core::buffer::Buffer buffer1(data, datalen);
    xap::test::assert_equal<float>(
        buffer1.read_double_le(offset),
        value,
        message
    );

    xap::core::buffer::Buffer buffer2(datalen, false);
    buffer2.write_double_le(value, offset);
    xap::test::assert_equal<xap::core::buffer::Buffer>(
        buffer1,
        buffer2,
        message
    );
}

void print_hex(const xap::core::buffer::Buffer &data) {
    for (size_t i = 0; i < data.get_length(); ++i) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

//
//  Entry.
//
int main() {
    //
    //  Case 1.
    //
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    xap::core::buffer::Buffer buf1(data, sizeof(data));
    xap::test::assert_ok(
        buf1.get_length() == sizeof(data),
        "buf1.get_length() != sizeof(data)"
    );

    {
        //
        //  Case 2.
        //
        xap::core::buffer::Buffer buf2 = buf1.slice(1U, 5U);
        xap::core::buffer::Buffer buf3 = buf2.slice(1U, 4U);
        xap::core::buffer::Buffer buf4(data + 2U, 4U);
        xap::test::assert_ok(
            buf3 == buf4,
            "Case2: buf3 != buf4"
        );
    }

    //
    //  Case 3.
    //
    xap::core::buffer::Buffer buf4(buf1.get_length(), true);
    buf1.copy(buf4);
    xap::test::assert_ok(
        buf1 == buf4,
        "buf1 != buf4"
    );

    //
    //  Case 4.
    //
    xap::core::buffer::Buffer buf5(32U, false);
    xap::core::buffer::Buffer buf6(32U, false);
    xap::test::assert_ok(
        buf5 == buf6,
        "buf5 != buf6"
    );

    //
    //  Case 6.
    //
    xap::test::assert_ok(
        buf1.read_uint8(0) == 0x01,
        "buf1.read_uint8(0) != 0x01"
    );
    xap::test::assert_ok(
        buf1.read_uint8(1) == 0x02,
        "buf1.read_uint8(1) != 0x02"
    );
    xap::test::assert_ok(
        buf1.read_uint8(2) == 0x03,
        "buf1.read_uint8(2) != 0x03"
    );
    xap::test::assert_ok(
        buf1.read_uint16_be(0) == 0x0102,
        "buf1.read_uint16_be(0) != 0x0102"
    );
    xap::test::assert_ok(
        buf1.read_uint16_be(1) == 0x0203,
        "buf1.read_uint16_be(1) != 0x0203"
    );

    //
    //  Case 7.
    //
    xap::core::buffer::Buffer buf7(data, sizeof(data));
    buf7.write_uint8(0x91, 0);
    buf7.write_uint8(0x65, 1);
    xap::test::assert_ok(
        buf7.read_uint8(0) == 0x91,
        "buf7.read_uint8(0) != 0x91"
    );
    xap::test::assert_ok(
        buf7.read_uint8(1) == 0x65,
        "buf7.read_uint8(1) != 0x65"
    );
    buf7.write_uint16_be(0x4562, 3);
    xap::test::assert_ok(
        buf7.read_uint16_be(3) == 0x4562,
        "buf7.read_uint16_be(3) != 0x4562"
    );

    //
    //  Case 8.
    //
    const uint8_t data2[] = {0x03, 0x05, 0x07};
    const uint8_t data3[] = {0x01};
    const uint8_t data4[] = {0x02, 0x90, 0xF1, 0x6A};
    const uint8_t data5[] = {0x03, 0x05, 0x07, 0x01, 0x02, 0x90, 0xF1, 0x6A};
    xap::core::buffer::Buffer buf8(data2, sizeof(data2));
    xap::core::buffer::Buffer buf9(data3, sizeof(data3));
    xap::core::buffer::Buffer buf10(data4, sizeof(data4));
    xap::core::buffer::Buffer buf11[] = {buf8, buf9, buf10};
    xap::core::buffer::Buffer buf12 = 
        xap::core::buffer::Buffer::concat(buf11, 3);
    xap::core::buffer::Buffer buf13(data5, sizeof(data5));
    xap::test::assert_ok(
        buf12 == buf13,
        "buf12 != buf13"
    );

    //
    //  Case 9: Signal-precision float-point.
    //
    {
        const uint8_t dat1[] = {0x3f, 0x80, 0x00, 0x00};
        xap_assert_buffer_equal_float_be(
            dat1,
            sizeof(dat1),
            0U,
            1.0,
            "Case 9: dat1 test failed."
        );

        const uint8_t dat2[] = {0x00, 0x00, 0x00, 0x80, 0x3F};
        xap_assert_buffer_equal_float_le(
            dat2,
            sizeof(dat2),
            1U,
            1.0,
            "Case 9: dat2 test failed."
        );

        const uint8_t dat3[] = {0xC0, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_float_be(
            dat3,
            sizeof(dat3),
            0U,
            -2.0,
            "Case 9: dat3 test failed."
        );
        const uint8_t dat4[] = {0x00, 0x00, 0x00, 0xC0};
        xap_assert_buffer_equal_float_le(
            dat4,
            sizeof(dat4),
            0U,
            -2.0,
            "Case 9: dat4 test failed."
        );

        const uint8_t dat5[] = {0x00, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_float_be(
            dat5,
            sizeof(dat5),
            0U,
            0.0,
            "Case 9: dat5 test failed."
        );
        const uint8_t dat6[] = {0x00, 0x00, 0x00, 0x80};
        xap_assert_buffer_equal_float_le(
            dat6,
            sizeof(dat6),
            0U,
            -0.0,
            "Case 9: dat6 test failed."
        );

        const uint8_t dat7[] = {0xFF, 0xC0, 0x00, 0x01};
        xap::core::buffer::Buffer buf7(dat7, sizeof(dat7));
        float f7 = buf7.read_float_be(0U);
        xap::test::assert_ok(std::isnan(f7), "Case 9: f7 is not NaN.");

        const uint8_t dat8[] = {0x01, 0x00, 0x80, 0xFF};
        xap::core::buffer::Buffer buf8(dat8, sizeof(dat8));
        float f8 = buf8.read_float_le(0U);
        xap::test::assert_ok(std::isnan(f8), "Case 9: f8 is not NaN.");

        const uint8_t dat9[] = {0x7F, 0x80, 0x00, 0x00};
        xap_assert_buffer_equal_float_be(
            dat9,
            sizeof(dat9),
            0U,
            INFINITY,
            "Case 9: dat9 test failed."
        );

        const uint8_t dat10[] = {0x00, 0x00, 0x80, 0xFF};
        xap_assert_buffer_equal_float_le(
            dat10,
            sizeof(dat10),
            0U,
            -INFINITY,
            "Case 9: dat10 test failed."
        );

        const uint8_t dat11[] = {0x43, 0x3D, 0xBE, 0xB8};
        xap_assert_buffer_equal_float_be(
            dat11,
            sizeof(dat11),
            0U,
            189.745,
            "Case 9: dat11 test failed."
        );

        const uint8_t dat12[] = {0xA4, 0xB0, 0xCC, 0xC2};
        xap_assert_buffer_equal_float_le(
            dat12,
            sizeof(dat12),
            0U,
            -102.345,
            "Case 9: dat12 test failed."
        );
    }

    //
    //  Case 10: Double-precision double-point.
    //
    {
        const uint8_t dat1[] = {0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 
                                0x00, 0x00};
        xap_assert_buffer_equal_double_be(
            dat1,
            sizeof(dat1),
            2U,
            1.0,
            "Case 10: dat1 test failed."
        );

        const uint8_t dat2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                0xF0, 0x3F};
        xap_assert_buffer_equal_double_le(
            dat2,
            sizeof(dat2),
            1U,
            1.0,
            "Case 10: dat2 test failed."
        );

        const uint8_t dat3[] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_double_be(
            dat3,
            sizeof(dat3),
            0U,
            -2.0,
            "Case 10: dat3 test failed."
        );

        const uint8_t dat4[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0};
        xap_assert_buffer_equal_double_le(
            dat4,
            sizeof(dat4),
            0U,
            -2.0,
            "Case 10: dat4 test failed."
        );

        const uint8_t dat5[] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_double_be(
            dat5,
            sizeof(dat5),
            0U,
            -0.0,
            "Case 10: dat5 test failed."
        );

        const uint8_t dat6[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_double_le(
            dat6,
            sizeof(dat6),
            0U,
            0.0,
            "Case 10: dat6 test failed."
        );

        const uint8_t dat7[] = {0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        xap_assert_buffer_equal_double_be(
            dat7,
            sizeof(dat7),
            0U,
            INFINITY,
            "Case 10: dat7 test failed."
        );
        const uint8_t dat8[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF};
        xap_assert_buffer_equal_double_le(
            dat8,
            sizeof(dat8),
            0U,
            -INFINITY,
            "Case 10: dat8 test failed."
        );

        //  sNaN
        const uint8_t dat9[] = {0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
        xap::core::buffer::Buffer buf9(dat9, sizeof(dat9));
        float f9 = buf9.read_double_be(0U);
        xap::test::assert_ok(std::isnan(f9), "Case 10: f9 is not NaN.");

        //  qNaN
        const uint8_t dat10[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 
                                 0x7F};
        xap::core::buffer::Buffer buf10(dat10, sizeof(dat10));
        float f10 = buf10.read_double_le(0U);
        xap::test::assert_ok(std::isnan(f10), "Case 10: f10 is not NaN.");

        //  An alternative encoding of NaN
        const uint8_t dat11[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                 0x7F};
        xap::core::buffer::Buffer buf11(dat11, sizeof(dat11));
        double f11 = buf11.read_double_le(0U);
        xap::test::assert_ok(std::isnan(f11), "Case 10: f11 is not NaN.");

        const uint8_t dat12[] = {0x28, 0x2A, 0x1B, 0xD6, 0xD4, 0x57, 0x8B, 
                                 0xC0};
        xap_assert_buffer_equal_double_le(
            dat12,
            sizeof(dat12),
            0U,
            -874.978924,
            "Case 10: dat12 test failed."
        );
        
        const uint8_t dat13[] = {0x41, 0x2E, 0x02, 0xE8, 0x3F, 0x42, 0x35, 
                                 0x2F};
        xap_assert_buffer_equal_double_be(
            dat13,
            sizeof(dat13),
            0U,
            983412.123552,
            "Case 10: dat13 test failed."
        );
    }

    //
    //  Case 11: unsigned 16-bit integer.
    //
    {
        const uint8_t expected[] = {0x00, 0x01, 0x02, 0x02, 0x01};
        xap::core::buffer::Buffer buf(5U, false);
        xap::core::buffer::Buffer excepted_buf(expected, sizeof(expected));
        buf.write_uint16_be(0x0102, 1U);
        buf.write_uint16_le(0x0102, 3U);
        xap::test::assert_equal<xap::core::buffer::Buffer>(
            buf,
            excepted_buf,
            "Case 11: buf != expected_buf"
        );
        
        xap::test::assert_equal<uint16_t>(
            excepted_buf.read_uint16_be(1U),
            0x0102,
            "Case 11: excepted_buf.read_uint16_be(1U) != 0x0102"
        );
        xap::test::assert_equal<uint16_t>(
            excepted_buf.read_uint16_le(3U),
            0x0102,
            "Case 11: expected_buf.read_uint16_le(3U) != 0x0102"
        );
    }

    //
    //  Case 12: unsigned 32-bit integer.
    //
    {
        const uint8_t expected[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x04, 0x03, 
                                    0x02, 0x01};
        xap::core::buffer::Buffer buf(9U, false);
        xap::core::buffer::Buffer excepted_buf(expected, sizeof(expected));
        buf.write_uint32_be(0x01020304, 1U);
        buf.write_uint32_le(0x01020304, 5U);
        xap::test::assert_equal<xap::core::buffer::Buffer>(
            buf,
            excepted_buf,
            "Case 12: buf != expected_buf"
        );
        
        xap::test::assert_equal<uint32_t>(
            excepted_buf.read_uint32_be(1U),
            0x01020304,
            "Case 12: excepted_buf.read_uint32_be(1U) != 0x01020304"
        );
        xap::test::assert_equal<uint32_t>(
            excepted_buf.read_uint32_le(5U),
            0x01020304,
            "Case 12: excepted_buf.read_uint32_le(5U) != 0x01020304"
        );
    }

    //
    //  Case 13: unsigned 64-bit integer.
    //
    {
        const uint8_t expected[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 
                                    0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
                                    0x0E, 0x0F, 0x10};
        xap::core::buffer::Buffer buf(sizeof(expected), false);
        xap::core::buffer::Buffer excepted_buf(expected, sizeof(expected));
        buf.write_uint64_be(0x0102030405060708, 1U);
        buf.write_uint64_le(0x100F0E0D0C0B0A09, 9U);
        xap::test::assert_equal<xap::core::buffer::Buffer>(
            buf,
            excepted_buf,
            "Case 13: buf != expected_buf"
        );
        xap::test::assert_equal<uint64_t>(
            excepted_buf.read_uint64_be(1U),
            0x0102030405060708,
            "Case 13: excepted_buf.read_uint64_be(1U) != 0x0102030405060708"
        );
        xap::test::assert_equal<uint64_t>(
            excepted_buf.read_uint64_le(9U),
            0x100F0E0D0C0B0A09,
            "Case 13: excepted_buf.read_uint64_le(9U) != 0x100F0E0D0C0B0A09"
        );
    }

    return 0;
}