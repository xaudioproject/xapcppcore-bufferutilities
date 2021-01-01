//
//  Copyright 2019 - 2021 The XOrange Studio. All rights reserved.
//  Use of this source code is governed by a BSD-style license that can be
//  found in the LICENSE.md file.
//

#ifndef XAP_CORE_BUFFER_QUEUE_H__
#define XAP_CORE_BUFFER_QUEUE_H__

//
//  Imports.
//
#include <list>
#include <xap/core/buffer/buffer.h>
#include <xap/core/buffer/error.h>
#include <xap/core/buffer/fetcher.h>

namespace xap {
namespace core {
namespace buffer {

class BufferQueue {
public:
    //
    //  Public methods.
    //

    /**
     *  Construct the object.
     */
    BufferQueue() noexcept;

    /**
     *  Construct (Copy) the object.
     * 
     *  @param src
     *      The source object.
     */
    BufferQueue(const BufferQueue &src);

    /**
     *  Destructor the object.
     */
    ~BufferQueue();

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
    BufferQueue& operator=(const BufferQueue &src);

    /**
     *  Push buffer to queue.
     * 
     *  @param data
     *      The data.
     */
    void push(const Buffer &data);
    
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
    Buffer pop(const size_t size);
    
    /**
     *  Pop all data from queue.
     * 
     *  @return
     *      The buffer.
     */
    Buffer pop_all();

    /**
     *  Get the remaining size.
     * 
     *  @return
     *      The size of remaining bytes.
     */
    size_t get_remaining_size() const noexcept;

private:

    //
    //  Members.
    //
    size_t m_remaining;
    std::list<BufferFetcher> m_queue;
};

}  //  namespace buffer
}  //  namespace core
}  //  namespace xap


#endif  //  #ifndef XAP_CORE_BUFFER_QUEUE_H__