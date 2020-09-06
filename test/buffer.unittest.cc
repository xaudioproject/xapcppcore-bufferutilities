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

#include <stdio.h>

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

    //
    //  Case 2.
    //
    xap::core::buffer::Buffer buf2 = buf1.slice(1U, 2U);
    xap::core::buffer::Buffer buf3(data + 1U, 2U);
    xap::test::assert_ok(
        buf2 == buf3,
        "buf2 != buf3"
    );

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

    return 0;
}