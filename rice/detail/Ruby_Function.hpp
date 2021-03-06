#ifndef Rice__detail__ruby_function__hpp_
#define Rice__detail__ruby_function__hpp_

#include "ruby.hpp"

namespace Rice::detail
{
  /* This is functor class that wraps calls to a Ruby C API method. It is needed because
     rb_protect only supports calling methods that take one argument. Thus 
     we invoke rb_protect telling it to invoke Ruby_Function::call with an 
     instance of a Ruby_Function. That instance then in turn calls the original
     Ruby method passing along its required arguments. */

  template<typename Function_T, typename Return_T, typename...Arg_Ts>
  class Ruby_Function
  {
  public:
    Ruby_Function(Function_T func, const Arg_Ts&... args);
    VALUE operator()();

  private:
    static VALUE invoke(VALUE value);

    Function_T func_;
    std::tuple<Arg_Ts...> args_;
  };

  template<typename Function_T, typename... Arg_Ts>
  decltype(auto) Make_Ruby_Function(Function_T&& func, Arg_Ts&&... args);

  template<typename Function_T, typename ...Arg_Ts>
  VALUE protect(Function_T&& func, Arg_Ts&&... args);
}

namespace Rice
{
  template<typename Function_T, typename ...Arg_Ts>
  [[deprecated("Please use detail::protect")]]
  VALUE protect(Function_T&& func, Arg_Ts&&... args);
}

#include "Ruby_Function.ipp"

#endif // Rice__detail__ruby_function__hpp_