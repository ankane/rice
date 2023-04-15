#ifndef Rice__Module__ipp_
#define Rice__Module__ipp_

#include "../detail/rice_traits.hpp"
#include "../detail/function_traits.hpp"
#include "../detail/Type.hpp"
#include "../detail/NativeFunction.hpp"
#include "../Exception.hpp"

namespace Rice
{
  inline Module::Module() : Object(rb_cObject)
  {
  }

  inline Module::Module(VALUE value) : Object(value)
  {
    if (::rb_type(value) != T_CLASS && ::rb_type(value) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, value)); // TODO: might raise an exception
    }
  }

  //! Construct a Module from an string that references a Module
  inline Module::Module(std::string name, Object under)
  {
    VALUE result = under.const_get(name);

    if (::rb_type(result) != T_MODULE)
    {
      throw Exception(
        rb_eTypeError,
        "Expected a Module but got a %s",
        detail::protect(rb_obj_classname, result)); // TODO: might raise an exception
    }

    this->set_value(result);
  }

  template<bool IsMethod, typename Function_T>
  inline void Module::wrap_native_call(VALUE klass, Identifier name, Function_T&& function, MethodInfo* methodInfo)
  {
    // Make sure the return type and arguments have been previously seen by Rice
    using traits = detail::method_traits<Function_T, IsMethod>;
    detail::verifyType<typename traits::Return_T>();
    detail::verifyTypes<typename traits::Arg_Ts>();

    // Create a NativeFunction instance to wrap this native call and 
    auto* native = new detail::NativeFunction<VALUE, Function_T, IsMethod>(std::forward<Function_T>(function), methodInfo);

    // Now define the method
    using Native_T = typename std::remove_pointer_t<decltype(native)>;
    detail::protect(rb_define_method_id, klass, name.id(), (RUBY_METHOD_FUNC)&Native_T::call, -1);
    detail::Registries::instance.natives.add(klass, name.id(), native);
  }

  inline Module define_module_under(Object module, char const* name)
  {
    return detail::protect(rb_define_module_under, module.value(), name);
  }

  inline Module define_module(char const* name)
  {
    return detail::protect(rb_define_module, name);
  }

  inline Module anonymous_module()
  {
    return detail::protect(rb_module_new);
  }
}

namespace Rice::detail
{
  template<>
  struct Type<Module>
  {
    static bool verify()
    {
      return true;
    }
  };

  template<>
  class To_Ruby<Module>
  {
  public:
    VALUE convert(Module const& x)
    {
      return x.value();
    }
  };

  template<>
  class From_Ruby<Module>
  {
  public:
    Module convert(VALUE value)
    {
      return Module(value);
    }
  };
}
#endif // Rice__Module__ipp_