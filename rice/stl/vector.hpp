#ifndef Rice__stl__vector__hpp_
#define Rice__stl__vector__hpp_

#include "../detail/ruby.hpp"

namespace Rice
{
  template<typename T>
  Data_Type<T> define_vector(std::string name);

  template<typename T>
  Data_Type<T> define_vector_under(Object module, std::string name);
}

#include "vector.ipp"

#endif // Rice__stl__vector__hpp_