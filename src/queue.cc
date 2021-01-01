//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

//
//  Imports.
//
#include <xap/core/buffer/queue.h>

namespace xap {
namespace core {
namespace buffer {

/**
 *  Construct the object.
 */
BufferQueue::BufferQueue() noexcept :
    m_remaining(0U),
    m_queue()
{
    //  Do nothing.
}

/**
 *  Construct (Copy) the object.
 * 
 *  @param src
 *      The source object.
 */
BufferQueue::BufferQueue(const BufferQueue &src) :
    m_remaining(src.m_remaining),
    m_queue(src.m_queue)
{
    //  Do nothing.
}

/**
 *  Destructor the object.
 */
BufferQueue::~BufferQueue() {
    //  Do nothing.
}

//
//  Public operator.
//

/**
 *  Operator '='.
 * 
 *  @param src
 *      The source.
 *  @return
 *      The destination.
 */
BufferQueue& BufferQueue::operator=(const BufferQueue &src) {
    if (this != &src) {
        this->m_queue = src.m_queue;
        this->m_remaining = src.m_remaining;
    }

    return *this;
}

/**
 *  Push buffer to queue.
 * 
 *  @param data
 *      The data.
 */
void BufferQueue::push(const Buffer &data) {
    size_t datalen = data.get_length();
    if (datalen == 0U) {
        return;
    }

    this->m_queue.push_back(BufferFetcher(data));
    this->m_remaining += data.get_length();
}

/**
 *  Pop buffer from queue.
 * 
 *  @throw BufferException
 *      Raised if parameter 'size' was out of range 
 *      (XAPCORE_BUF_ERROR_OVERFLOW).
 *  @param size
 *      The size of buffer.
 *  @return
 *      The buffer.
 */
Buffer BufferQueue::pop(const size_t size) {
    if (size > this->m_remaining) {
        throw BufferException("Out of range.", XAPCORE_BUF_ERROR_OVERFLOW);
    }

    Buffer buffer(size);
    size_t cursor = 0U;
    while (cursor < size) {
        size_t needed = size - cursor;
        auto fetcher = this->m_queue.begin();
        size_t fetcher_remaining = fetcher->get_remaining_size();
        if (fetcher_remaining <= needed) {
            fetcher->fetch_all().copy(buffer, cursor);
            cursor += fetcher_remaining;
            this->m_queue.pop_front();
            continue;
        } else {
            fetcher->fetch_bytes(needed).copy(buffer, cursor);
            cursor += needed;
            continue;
        }
    }

    this->m_remaining -= size;
    return buffer;
}

/**
 *  Pop all data from queue.
 * 
 *  @return
 *      The buffer.
 */
Buffer BufferQueue::pop_all() {
    Buffer out(this->m_remaining);

    size_t cursor = 0U;
    while (!this->m_queue.empty()) {
        auto fetcher = this->m_queue.begin();
        size_t fetcher_remaining = fetcher->get_remaining_size();
        fetcher->fetch_all().copy(out, cursor);
        cursor += fetcher_remaining;
        this->m_queue.pop_front();
    }

    this->m_remaining = 0U;
    return out;
}

/**
 *  Get the remaining size.
 * 
 *  @return
 *      The size of remaining bytes.
 */
size_t BufferQueue::get_remaining_size() const noexcept {
    return this->m_remaining;
}

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap