#pragma once

// see https://vorbrodt.blog/2019/02/05/fast-semaphore/
// Fast-Semaphore by Joe Seigh; C++ Implementation by Chris Thomasson:

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <cassert>

class SlowSemaphore {
private:
  int                     m_count;
  std::mutex              m_mutex;
  std::condition_variable m_cv;

public:
  SlowSemaphore(int count) noexcept : m_count(count) {
    assert(count > -1);
  }

  void notify() noexcept {
    std::unique_lock<std::mutex> lock(m_mutex);
    ++m_count;
    m_cv.notify_one();
  }

  void wait() noexcept {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [&]() { return m_count != 0; });
    --m_count;
  }
};

class Semaphore {
private:
  std::atomic<int> m_count;
  SlowSemaphore    m_slowSemaphore;
public:
  // count is the number of threads allowed in critical section (=initial value of internal counter). Assume 0 <= initialCount
  // The state of the semaphore is signaled when counter >= 1, nonsignaled when counter < 1
  Semaphore(int count = 1) noexcept : m_count(count), m_slowSemaphore(0) {
  }

  // Increased internal counter by 1 (typically when a thread leaves a critical section, or a memberfunction of a monitor))
  // If other threads have called wait while state was nonsignaled, hence waiting, they will be notified that state has changed
  void notify() {
    const int count = m_count.fetch_add(1, std::memory_order_release);
    if(count < 0) {
      m_slowSemaphore.notify();
    }
  }

  // Decrease internal counter by 1. If wait is called on a nonsignaled semaphore, the calling thread will be blocked until 
  // notify is called (by anothr thread), turning state signaled
  void wait() {
    const int count = m_count.fetch_sub(1, std::memory_order_acquire);
    if(count < 1) {
      m_slowSemaphore.wait();
    }
  }
};

class TimedSemaphore {
private:
  std::atomic<int> m_count;
  std::timed_mutex m_tmutex;
public:
  // Same functionality as Semaphore, but function wait has a timeout parameter. See below
  TimedSemaphore(int count = 1) noexcept : m_count(count) {
    m_tmutex.lock();
  }

  void notify() noexcept {
    const int count = m_count.fetch_add(1, std::memory_order_release);
    if(count < 0) {
      m_tmutex.unlock();
    }
  }

  // Decreased by 1 whenever a call to wait returns true (not if wait returns false because of timeout)
  // Return true if state is already signaled or it become signaled within the specified period of timeout in milliseconds.
  // Return false, if state does not turn signaled before timeout
  // timeout is in milliseconds
  // If timeout < 0, wait will work just as the function wait() in class Semaphore. See above
  bool wait(int timeout = -1) noexcept {
    const int count = m_count.fetch_sub(1, std::memory_order_acquire);
    if(count >= 1) {
      return true;
    } else if(timeout < 0) {
      m_tmutex.lock();
      return true;
    } else if(m_tmutex.try_lock_for(std::chrono::milliseconds(timeout))) {
      return true;
    } else {
      m_count++;
      return false;
    }
  }
};
