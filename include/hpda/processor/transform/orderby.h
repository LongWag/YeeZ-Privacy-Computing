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

  void quick_sort(std::vector<InputObjType> &datas, int left, int right) {
    int i = left, j = right;
    if (i > j) {
      return;
    }
    InputObjType key = datas[left].make_copy();
    while (i < j) {
      while (i < j && datas[j].template get<T>() >= key.template get<T>()) {
        j--;
      }
      datas[i] = datas[j].make_copy();
      while (i < j && datas[i].template get<T>() <= key.template get<T>()) {
        i++;
      }
      datas[j] = datas[i].make_copy();
    }
    datas[i] = key.make_copy();
    quick_sort(datas, left, i - 1);
    quick_sort(datas, i + 1, right);
  }

  virtual bool process() {
    if (base::has_input_value()) {
      m_datas.push_back(base::input_value().make_copy());
      base::consume_input_value();
      return false;
    } else {
      if (is_sorted == false) {
        quick_sort(m_datas, 0, m_datas.size() - 1);
        is_sorted = true;
      }
      if (p >= m_datas.size()) {
        return false;
      }
      return true;
    }
  }

  virtual InputObjType output_value() {
    InputObjType result = m_datas[p].make_copy();
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