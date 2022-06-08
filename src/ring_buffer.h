#pragma once
#include "main.h"
#include <cstdint>
#include <array>

template <class T = uint8_t, uint16_t N = 64>
class RingBuffer {
private:
  using data_t = T;
  std::array<data_t, N> buff_;
  volatile uint16_t head_{}, tail_{};
  bool is_full_ = false;
  uint16_t reserve_index_ = 0;

public:
  void push(const data_t& d) {
    buff_[head_] = d;
    head_ = (head_ + 1) % N;
    is_full_ = is_full_ || (head_ == tail_);
  }

  data_t pop() {
    assert_param(!is_empty());

    data_t ret = buff_[tail_];
    tail_ = (tail_ + 1) % N;
    is_full_ = false;
    return ret;
  }

  const data_t& peek() const {
    assert_param(!is_empty());
    return buff_[tail_];
  }

  bool is_full() const {
    return is_full_;
  }
  bool is_empty() const {
    return ((not is_full_) && (head_ == tail_));
  }

  uint16_t get_num_occupied() const {
    if (is_full()) {
      return N;
    }

    if (head_ >= tail_) {
      return (head_ - tail_);
    } else {
      return (N + head_ - tail_);
    }
  }

  uint16_t get_num_free() const {
    return N - get_num_occupied();
  }

  bool reserve_n(uint16_t num) {
    if (num > get_num_free()) {
      return false;
    }
    reserve_index_ = head_;

    head_ += num;
    head_ = head_ % N;

    return true;
  }

  void push_to_reserved(const data_t* data, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
      buff_[reserve_index_] = data[i];
      reserve_index_ = (reserve_index_ + 1) % N;
    }
  }

  void reset() {
    is_full_ = false;
    head_ = 0;
    tail_ = 0;
  }
};
