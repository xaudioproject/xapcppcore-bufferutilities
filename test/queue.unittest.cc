//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include "build.h"
#include "common.h"

#include <xap/core/buffer/queue.h>
#include <string>

void check_buffer_with_string(
    const xap::core::buffer::Buffer &buf, 
    const std::string &str,
    const char *message = nullptr
) {
    size_t buflen = buf.get_length();
    xap::test::assert_equal<size_t>(
        buflen * 2U,
        str.size(),
        message
    );

    size_t bufstr_len = buflen * 2U;
    std::string bufstr(bufstr_len, true);
    for (size_t i = 0; i < buflen; ++i) {
#if defined(XAP_TEST_OS_WIN)
        sprintf_s(&bufstr[i * 2U], 4U, "%02X", buf[i]);
#else
        sprintf(&bufstr[i * 2U], "%02X", buf[i]);
#endif
    }
    xap::test::assert_equal<std::string>(
        bufstr,
        str,
        message
    );
}

//
//  Entry.
//
int main() {
    const uint8_t data1[] = {0x01, 0x02, 0x03, 0x04};
    const size_t data1len = sizeof(data1);
    const xap::core::buffer::Buffer buffer1(data1, data1len);

    xap::core::buffer::BufferQueue queue;
    queue.push(buffer1);
    queue.push(buffer1);
    queue.push(buffer1);
    queue.push(buffer1);
    queue.push(buffer1);

    xap::test::assert_ok(
        queue.pop(1)[0] == 0x01,
        "Invalid pop value 1."
    );

    check_buffer_with_string(
        queue.pop(5),
        "0203040102",
        "Invalid pop value 2."
    );
    check_buffer_with_string(
        queue.pop(10),
        "03040102030401020304",
        "Invalid pop value 3."
    );
    check_buffer_with_string(
        queue.pop(3),
        "010203",
        "Invalid pop value 4."
    );

    const uint8_t data2[] = {0xA1, 0xB2, 0xC3, 0xD4};
    const size_t data2len = sizeof(data2);
    const xap::core::buffer::Buffer buffer2(data2, data2len);
    queue.push(buffer2);
    check_buffer_with_string(
        queue.pop_all(),
        "04A1B2C3D4",
        "Invalid pop value 5."
    );

    const uint8_t data3[] = {0x81, 0x82, 0x83};
    const size_t data3len = sizeof(data3);
    const xap::core::buffer::Buffer buffer3(data3, data3len);
    queue.push(buffer1);
    queue.push(buffer2);
    queue.push(buffer3);
    check_buffer_with_string(
        queue.pop(5),
        "01020304A1",
        "Invalid pop value 6."
    );
    check_buffer_with_string(
        queue.pop(6),
        "B2C3D4818283",
        "Invalid pop value 7."
    );

    return 0;
}