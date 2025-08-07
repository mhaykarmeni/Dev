#pragma once

#include <iostream>
#include <vector>
#include <atomic>

namespace Common {
  template<typename T>
  class LFQueue final {
  public:
    LFQueue(std::size_t num_elems) :
        m_storage(num_elems, T()) /* pre-allocation of vector storage. */ {
    }

    auto getNextToWriteTo() noexcept {
      return &m_storage[m_nextWriteIdx];
    }

    auto updateWriteIndex() noexcept {
      m_nextWriteIdx = (m_nextWriteIdx + 1) % m_storage.size();
      m_numElements++;
    }

    auto getNextToRead() const noexcept -> const T * {
      return (size() ? &m_storage[m_nextReadIdx] : nullptr);
    }

    auto updateReadIndex() noexcept {
      m_nextReadIdx = (m_nextReadIdx + 1) % m_storage.size();
      ASSERT(m_numElements != 0, "Read an invalid element in:" + std::to_string(pthread_self()));
      m_numElements--;
    }

    auto size() const noexcept {
      return m_numElements.load();
    }

    // Deleted default, copy & move constructors and assignment-operators.
    LFQueue() = delete;

    LFQueue(const LFQueue &) = delete;

    LFQueue(const LFQueue &&) = delete;

    LFQueue &operator=(const LFQueue &) = delete;

    LFQueue &operator=(const LFQueue &&) = delete;

  private:
    std::vector<T> m_storage;

    std::atomic<size_t> m_nextWriteIdx = {0};
    std::atomic<size_t> m_nextReadIdx = {0};

    std::atomic<size_t> m_numElements = {0};
  };
}
