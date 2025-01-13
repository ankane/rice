#ifndef Rice__Encoding__hpp_
#define Rice__Encoding__hpp_

namespace Rice
{
  //! A wrapper for a Ruby encoding
  class Encoding
  {
  public:
    static Encoding utf8();

    //! Wrap an existing encoding.
    Encoding(rb_encoding* encoding);

    //! Implicit conversion to VALUE.
    operator rb_encoding* () const
    {
      return this->encoding_;
    }

    operator VALUE () const
    {
      return detail::protect(rb_enc_from_encoding, this->encoding_);
    }

  private:
    rb_encoding* encoding_;
  };
} // namespace Rice

#endif // Rice__Encoding__hpp_

