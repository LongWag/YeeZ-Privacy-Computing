#pragma once
#include <hpda/processor/processor_base.h>
#include "ypc/stbox/tsgx/log.h"

namespace hpda {
namespace processor {
namespace internal {

template <typename InputObjType>
class limit_impl : public processor_base<InputObjType, InputObjType> {
public:
  limit_impl(
      ::hpda::internal::processor_with_output<InputObjType> *upper_stream,
      uint64_t left, uint64_t right)
      : processor_base<InputObjType, InputObjType>(upper_stream),
        m_num_left(left), m_num_right(right), m_cur_num(1) {}

  virtual ~limit_impl() {}

  typedef processor_base<InputObjType, InputObjType> base;

  virtual bool process() {
    if (base::has_input_value() && (m_cur_num <= m_num_right) && (m_cur_num >= m_num_left)) {
      m_data = base::input_value().make_copy();
      base::consume_input_value();
      m_cur_num++;
      return true;
    } else {
      return false;
    }
  }

  virtual InputObjType output_value() {
    return m_data;
  }

protected:
  uint64_t m_num_left;
  uint64_t m_num_right;
  uint64_t m_cur_num;
  InputObjType m_data;
}; // class limit_impl
} // namespace internal
} // namespace processor
} // namespace hpda