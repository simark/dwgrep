#ifndef _CST_H_
#define _CST_H_

#include <cassert>
#include <iosfwd>

class constant;

class constant_dom
{
public:
  virtual void format (constant c, std::ostream &o) const = 0;
  virtual ~constant_dom () {}
};

struct untyped_constant_dom_t
  : public constant_dom
{
  virtual void format (constant c, std::ostream &o) const;
};

extern untyped_constant_dom_t const untyped_constant_dom;

class constant
{
  uint64_t m_value;
  constant_dom const *m_dom;

public:
  constant (uint64_t value, constant_dom const *dom)
    : m_value (value)
    , m_dom (dom)
  {}

  constant_dom const *dom () const
  {
    return m_dom;
  }

  uint64_t value () const
  {
    return m_value;
  }

  // Parse constant.
  static constant parse (std::string str);

  // Parse attribute reference.
  static constant parse_attr (std::string str);

  // Parse tag reference.
  static constant parse_tag (std::string str);
};

#endif /* _CST_H_ */
