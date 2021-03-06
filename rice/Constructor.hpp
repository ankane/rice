#ifndef Rice__Constructor__hpp_
#define Rice__Constructor__hpp_

#include "detail/Wrapper.hpp"
#include "Object_defn.hpp"

namespace Rice
{
  //! Define a Type's Constructor and it's arguments.
  /*! E.g. for the default constructor on a Type:
      \code
        define_class<Test>()
          .define_constructor(Constructor<Test>());
      \endcode
  *
  *  The first template type must be the type being wrapped.
  *  Afterwards any extra types must match the appropriate constructor
  *  to be used in C++ when constructing the object.
  *
  *  For more information, see Rice::Data_Type::define_constructor.
  */
  template<typename T, typename ...Arg_T>
  class Constructor
  {
  public:
    static void construct(Object self, Arg_T... args)
    {
      T* data = new T(args...);
      detail::replace<T>(self.value(), Data_Type<T>::rb_type(), data, true);
    }
  };

  //! Special-case Constructor used when defining Directors.
  template<typename T, typename ...Arg_T>
  class Constructor<T, Object, Arg_T...>
  {
    public:
      static void construct(Object self, Arg_T... args)
      {
        T* data = new T(self, args...);
        detail::replace<T>(self.value(), Data_Type<T>::rb_type(), data, true);
      }
  };
}

#endif // Rice__Constructor__hpp_

