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

#ifndef _THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_
#define _THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_ 1

#include <thrift/concurrency/Thread.h>

#include <boost/shared_ptr.hpp>

namespace apache {
namespace thrift {
namespace concurrency {

/**
 * A thread factory to create posix threads
 *
 * @version $Id:$
 */
class PosixThreadFactory : public ThreadFactory {

public:
  /**
   * POSIX Thread scheduler policies
   */
  class Policy {
  public:
    enum value {
      OTHER       = 0,
      FIFO        = 1,
      ROUND_ROBIN = 2
    };
  };

  /**
   * POSIX Thread scheduler relative priorities,
   *
   * Absolute priority is determined by scheduler policy and OS. This
   * enumeration specifies relative priorities such that one can specify a
   * priority within a giving scheduler policy without knowing the absolute
   * value of the priority.
   */
  class Priority {
  public:
    enum value {
      LOWEST    = 0,
      LOWER     = 1,
      LOW       = 2,
      NORMAL    = 3,
      HIGH      = 4,
      HIGHER    = 5,
      HIGHEST   = 6,
      INCREMENT = 7,
      DECREMENT = 8
    };
  };

  /**
   * Posix thread (pthread) factory.  All threads created by a factory are reference-counted
   * via boost::shared_ptr and boost::weak_ptr.  The factory guarantees that threads and
   * the Runnable tasks they host will be properly cleaned up once the last strong reference
   * to both is given up.
   *
   * Threads are created with the specified policy, priority, stack-size and detachable-mode
   * detached means the thread is free-running and will release all system resources the
   * when it completes.  A detachable thread is not joinable.  The join method
   * of a detachable thread will return immediately with no error.
   *
   * By default threads are not joinable.
   */
  PosixThreadFactory(Policy::value policy,
                     Priority::value priority,
                     int stackSize,
                     bool detached);

  /**
   * Provide a constructor compatible with the other factories
   * The default policy is Policy::ROUND_ROBIN.
   * The default priority is Priority::NORMAL.
   * The default stackSize is 1.
   */
  PosixThreadFactory(bool detached = true);

  // From ThreadFactory;
  boost::shared_ptr<Thread> newThread(boost::shared_ptr<Runnable> runnable) const;

  // From ThreadFactory;
  Thread::id_t getCurrentThreadId() const;

  /**
   * Gets stack size for newly created threads
   *
   * @return int size in megabytes
   */
  virtual int getStackSize() const;

  /**
   * Sets stack size for newly created threads
   *
   * @param value size in megabytes
   */
  virtual void setStackSize(int value);

  /**
   * Gets priority relative to current policy
   */
  virtual Priority::value getPriority() const;

  /**
   * Sets priority relative to current policy
   */
  virtual void setPriority(Priority::value priority);

private:
  Policy::value policy_;
  Priority::value priority_;
  int stackSize_;
};
}
}
} // apache::thrift::concurrency

#endif // #ifndef _THRIFT_CONCURRENCY_POSIXTHREADFACTORY_H_
