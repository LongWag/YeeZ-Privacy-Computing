#pragma once
#include <hpda/processor/processor_base.h>
#include "ypc/stbox/tsgx/log.h"

namespace hpda {
namespace processor {
namespace internal {

template <typename InputObjType, typename OutputObjType>
class aggregation_impl : public processor_base<std::vector<InputObjType>, OutputObjType> {
public:
  aggregation_impl(::hpda::internal::processor_with_output<std::vector<InputObjType>> *upper_stream)
    : processor_base<std::vector<InputObjType>, OutputObjType>(upper_stream) {}

  virtual ~aggregation_impl() {}

  typedef processor_base<std::vector<InputObjType>, OutputObjType> base;

  virtual bool process() {
    if (base::has_input_value()) {
      std::vector<InputObjType> tmp_vec = base::input_value().make_copy();
      base::consume_input_value();
      int tmp_sum_ID = 0;
      int tmp_ID = 0;
      for (auto data : tmp_vec) {
        tmp_sum_ID += data.template get<ID>();
        tmp_ID = data.template get<ID>();
      }
      m_data.template set<ID>(tmp_ID);
      m_data.template set<sum_ID>(tmp_sum_ID);
      return true;
    } else {
      return false;
    }
  }

  virtual OutputObjType output_value() {
    return m_data;
  }
protected:
  OutputObjType m_data;
}; // class aggregation_impl
} // namespace internal
} // namespace processor
} // namespace hpda