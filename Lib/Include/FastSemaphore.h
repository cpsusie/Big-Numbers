#pragma once

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

class FastSemaphore {
private:
  std::atomic<int> m_count;
  SlowSemaphore    m_slowSemaphore;
public:
  FastSemaphore(int count = 1) noexcept : m_count(count), m_slowSemaphore(0) {
  }

  void notify() {
    std::atomic_thread_fence(std::memory_order_release);
    const int count = m_count.fetch_add(1, std::memory_order_relaxed);
    if(count < 0) {
      m_slowSemaphore.notify();
    }
  }

  void wait() {
    const int count = m_count.fetch_sub(1, std::memory_order_relaxed);
    if(count < 1) {
      m_slowSemaphore.wait();
    }
    std::atomic_thread_fence(std::memory_order_acquire);
  }
};
