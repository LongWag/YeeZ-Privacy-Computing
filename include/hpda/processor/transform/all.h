#pragma once
#include <hpda/processor/processor_base.h>
#include "ypc/stbox/tsgx/log.h"

namespace hpda {
namespace processor {
namespace internal {

template <typename InputObjType>
class all_impl : public processor_base<InputObjType, InputObjType> {
public:
  all_impl(::hpda::internal::processor_with_output<InputObjType> *upper_stream)
      : processor_base<InputObjType, InputObjType>(upper_stream), nums(0) {}

  virtual ~all_impl() {}

  typedef processor_base<InputObjType, InputObjType> base;

  virtual bool process() {
    if (base::has_input_value()) {
      m_data = base::input_value().make_copy();
      base::consume_input_value();
      return true;
    }
    else {
      return false;
    }
  }

  virtual InputObjType output_value() {
    return m_data;
  }

protected:
  InputObjType m_data;
  size_t m_index;
  uint32_t nums;
};
} // namespace internal
template <typename T> using all_t = internal::all_impl<T>;
} // namespace processor
} // namespace hpda
