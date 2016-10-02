/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <thrift/thrift-config.h>

#if USE_STD_THREAD || USE_BOOST_THREAD

#include <thrift/concurrency/Exception.h>
#include <thrift/concurrency/StdThreadFactory.h>

#include <cassert>

#include <boost/enable_shared_from_this.hpp>
#include <boost/smart_ptr.hpp>

#if USE_STD_THREAD
#include <thread>
using std::thread;
using std::this_thread::get_id;
#else
#include <boost/thread.hpp>
using boost::thread;
using boost::this_thread::get_id;
// using boost::this_thread;
#endif

namespace apache {
namespace thrift {
namespace concurrency {

/**
 * The std or boost thread class.
 *
 * Note that we use boost shared_ptr rather than std shared_ptrs here
 * because the Thread/Runnable classes use those and we don't want to
 * mix them.
 */
class ThreadImpl : public Thread, public boost::enable_shared_from_this<ThreadImpl> {
public:
  enum STATE { uninitialized, starting, started, stopping, stopped };

  static void threadMain(boost::shared_ptr<ThreadImpl> thread);

private:
  boost::scoped_ptr<thread> thread_;
  STATE state_;
  bool detached_;

public:
  ThreadImpl(bool detached, boost::shared_ptr<Runnable> runnable)
    : state_(uninitialized), detached_(detached) {
    this->Thread::runnable(runnable);
  }

  ~ThreadImpl() {
    if (!detached_) {
      try {
        join();
      } catch (...) {
        // We're really hosed.
      }
    }
  }

  void start() {
    if (state_ != uninitialized) {
      return;
    }

    boost::shared_ptr<ThreadImpl> selfRef = shared_from_this();
    state_ = starting;

    thread_.reset(new thread(threadMain, selfRef));

    if (detached_)
      thread_->detach();
  }

  void join() {
    if (!detached_ && state_ != uninitialized) {
      thread_->join();
    }
  }

  Thread::id_t getId() { return thread_.get() ? thread_->get_id() : thread::id(); }

  boost::shared_ptr<Runnable> runnable() const { return Thread::runnable(); }

  void runnable(boost::shared_ptr<Runnable> value) { Thread::runnable(value); }
};

void ThreadImpl::threadMain(boost::shared_ptr<ThreadImpl> t) {
  if (!t || t->state_ != starting) {
    return;
  }

  t->state_ = started;
  t->runnable()->run();

  if (t->state_ != stopping && t->state_ != stopped) {
    t->state_ = stopping;
  }
}

StdThreadFactory::StdThreadFactory(bool detached) : ThreadFactory(detached) {}

boost::shared_ptr<Thread> StdThreadFactory::newThread(boost::shared_ptr<Runnable> runnable) const {
  boost::shared_ptr<ThreadImpl> result(new ThreadImpl(isDetached(), runnable));
  runnable->thread(result);
  return result;
}

Thread::id_t StdThreadFactory::getCurrentThreadId() const {
  return get_id();
}
}
}
} // apache::thrift::concurrency

#endif // USE_STD_THREAD || USE_BOOST_THREAD
