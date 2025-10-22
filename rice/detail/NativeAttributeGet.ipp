#include <array>
#include <algorithm>

namespace Rice::detail
{
  template<typename Attribute_T>
  void NativeAttributeGet<Attribute_T>::define(VALUE klass, std::string name, Attribute_T attribute, Return returnInfo)
  {
    // Verify attribute type
    Native::verify_type<Attr_T>(returnInfo.isBuffer());

    // Create a NativeAttributeGet that Ruby will call to read/write C++ variables
    NativeAttribute_T* nativeAttribute = new NativeAttribute_T(klass, name, std::forward<Attribute_T>(attribute), returnInfo);
    std::unique_ptr<Native> native(nativeAttribute);

    detail::protect(rb_define_method, klass, name.c_str(), (RUBY_METHOD_FUNC)&Native::resolve, -1);

    // Add to native registry. Since attributes cannot be overridden, there is no need to set the
    // matches or calls function pointer. Instead Ruby can call the static call method defined on
    // this class (&NativeAttribute_T::get).
    Identifier identifier(name);
    detail::Registries::instance.natives.add(klass, identifier.id(), native);
  }

  template<typename Attribute_T>
  inline Resolved NativeAttributeGet<Attribute_T>::matches(size_t argc, const VALUE* argv, VALUE self)
  {
    if (argc == 0)
      return Resolved { Convertible::Exact, 1, this };
    else
      return Resolved{ Convertible::None, 0, this };
  }
  
  template<typename Attribute_T>
  NativeAttributeGet<Attribute_T>::NativeAttributeGet(VALUE klass, std::string name, Attribute_T attribute, Return returnInfo)
    : klass_(klass), name_(name), attribute_(attribute), return_(returnInfo)
  {
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::operator()(size_t argc, const VALUE* argv, VALUE self)
  {
    if constexpr (std::is_member_object_pointer_v<Attribute_T>)
    {
      Receiver_T* nativeSelf = From_Ruby<Receiver_T*>().convert(self);

      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(nativeSelf->*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(&this->return_).convert(nativeSelf->*attribute_);
      }
    }
    else
    {
      if constexpr (std::is_fundamental_v<detail::intrinsic_type<To_Ruby_T>>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else if constexpr (std::is_array_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else if constexpr (std::is_pointer_v<To_Ruby_T>)
      {
        return To_Ruby<To_Ruby_T>(&this->return_).convert(*attribute_);
      }
      else
      {
        // If the attribute is an object return a reference to avoid a copy (and avoid issues with
        // attributes that are not assignable, copy constructible or move constructible)
        return To_Ruby<To_Ruby_T&>(&this->return_).convert(*attribute_);
      }
    }
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeGet<Attribute_T>::toString()
  {
    return "";
  }

  template<typename Attribute_T>
  inline std::string NativeAttributeGet<Attribute_T>::name()
  {
    return this->name_;
  }

  template<typename Attribute_T>
  inline NativeKind NativeAttributeGet<Attribute_T>::kind()
  {
    return NativeKind::AttributeReader;
  }

  template<typename Attribute_T>
  inline VALUE NativeAttributeGet<Attribute_T>::returnKlass()
  {
    // Check if an array is being returned
    if (this->return_.isBuffer())
    {
      TypeMapper<Pointer<Attr_T>> typeMapper;
      return typeMapper.rubyKlass();
    }
    else
    {
      TypeMapper<Attr_T> typeMapper;
      return typeMapper.rubyKlass();
    }
  }
}