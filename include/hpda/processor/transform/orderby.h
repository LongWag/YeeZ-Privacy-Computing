#pragma once
#include <hpda/processor/processor_base.h>
#include "ypc/stbox/tsgx/log.h"

namespace hpda {
namespace processor {
namespace internal {

template <typename InputObjType, typename T>
class orderby_impl : public processor_base<InputObjType, InputObjType> {
public:
  orderby_impl(::hpda::internal::processor_with_output<InputObjType> *upper_stream)
    : processor_base<InputObjType, InputObjType>(upper_stream), p(0), is_sorted(false) {}
    
  virtual ~orderby_impl() {}

  typedef processor_base<InputObjType, InputObjType> base;

  virtual bool process() {
    if (base::has_input_value()) {
      m_datas.push_back(base::input_value().make_copy());
      base::consume_input_value();
      return false;
    } else {
      if (is_sorted == false) {
        std::sort(m_datas.begin(), m_datas.end(), [](InputObjType a, InputObjType b) {return a.template get<T>() < b.template get<T>();});
        is_sorted = true;
      }
      if (p >= m_datas.size()) {
        return false;
      }
      return true;
    }
  }

  virtual InputObjType output_value() {
    InputObjType result = m_datas[p];
    p++;
    return result;
  }
protected:
  std::vector<InputObjType> m_datas;
  uint32_t p;
  bool is_sorted;
}; //class orderby_impl
} //namespace internal
} //namespace processor
} //namespace hpda