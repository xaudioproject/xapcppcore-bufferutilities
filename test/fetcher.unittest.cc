//
//  Copyright 2019 - 2020 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//
#include "common.h"

#include <xap/core/buffer/error.h>
#include <xap/core/buffer/fetcher.h>

//
//  Entry.
//
int main() {
    //
    //  Case 1.
    //
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    xap::core::buffer::Buffer buf(data, sizeof(data));
    xap::core::buffer::BufferFetcher fetcher(buf);

    xap::test::assert_ok(
        !fetcher.is_end(),
        "fetcher.is_end()"
    );
    
    uint8_t byte = fetcher.fetch();
    xap::test::assert_equal<uint8_t>(
        byte,
        0x01,
        "byte != 0x01"
    );

    xap::core::buffer::Buffer dst(3U, false);
    fetcher.fetch_to(dst);
    xap::test::assert_ok(
        dst == buf.slice(1U, 3U),
        "dst != buf.slice(1U, 3U)"
    );
    fetcher.reset();
    fetcher.fetch_to(dst, 1U);
    xap::test::assert_ok(
        dst.slice(1U) == buf.slice(0U, 2U),
        "dst.slice(1U) != buf.slice(0U, 2U)"
    );

    fetcher.reset();
    dst = fetcher.fetch_all();
    xap::test::assert_ok(
        dst.is_equal(data, sizeof(data)),
        "fetch_all(): !dst.is_equal(data, sizeof(data))"
    );

    xap::test::assert_throw<xap::core::buffer::BufferException>([&] {
        fetcher.fetch();
    });
    xap::test::assert_throw<xap::core::buffer::BufferException>([&] {
        fetcher.fetch_all();
    });
    xap::test::assert_throw<xap::core::buffer::BufferException>([&] {
        fetcher.fetch_bytes(1U);
    });
    xap::test::assert_throw<xap::core::buffer::BufferException>([&] {
        xap::core::buffer::Buffer buf(0);
        fetcher.fetch_to(buf);
    });
    xap::test::assert_throw<xap::core::buffer::BufferException>([&] {
        fetcher.skip(1U);
    });

    fetcher.reset();
    dst = fetcher.fetch_bytes(3U);
    xap::test::assert_ok(
        dst == buf.slice(0U, 3U),
        "fetch_bytes(3U): dst != buf.slice(0U, 3U)"
    );
    xap::test::assert_ok(
        fetcher.get_remaining_size() == 5U,
        "fetcher.get_remaining_size() != 5U"
    );

    fetcher.reset();
    fetcher.skip(1U);
    xap::test::assert_ok(
        fetcher.fetch() == 0x02,
        "skip(1U): fetcher.fetch() != 5U"
    );
    xap::test::assert_ok(
        fetcher.get_remaining_size() == sizeof(data) - 2U,
        "get_remaining_size(): fetcher.get_remaining_size() != sizeof(data)-2U"
    );

    const uint8_t data2[] = {0xA0, 0xB1, 0xC2, 0xD3};
    fetcher.replace(xap::core::buffer::Buffer(data2, sizeof(data2)));
    xap::test::assert_ok(
        fetcher.fetch_all().is_equal(data2, sizeof(data2)),
        "replace(): !fetcher.fetch_all().is_equal(data2, sizeof(data2))"
    );

    return 0;
}