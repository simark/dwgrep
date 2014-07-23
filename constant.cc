/*
   Copyright (C) 2014 Red Hat, Inc.
   This file is part of dwgrep.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   dwgrep is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#include <sstream>
#include <iostream>
#include <bitset>
#include <vector>
#include "constant.hh"

void
numeric_constant_dom_t::show (mpz_class const &v, std::ostream &o) const
{
  o << v;
}

numeric_constant_dom_t dec_constant_dom_obj ("dec");
constant_dom const &dec_constant_dom = dec_constant_dom_obj;

numeric_constant_dom_t column_number_dom_obj ("column number");
constant_dom const &column_number_dom = column_number_dom_obj;

numeric_constant_dom_t line_number_dom_obj ("line number");
constant_dom const &line_number_dom = line_number_dom_obj;


static struct
  : public constant_dom
{
  void
  show (mpz_class const &v, std::ostream &o) const override
  {
    std::ios::fmtflags f {o.flags ()};
    o << std::hex << std::showbase << v;
    o.flags (f);
  }

  bool
  safe_arith () const override
  {
    return true;
  }

  std::string name () const override
  {
    return "hex";
  }
} hex_constant_dom_obj;

constant_dom const &hex_constant_dom = hex_constant_dom_obj;


static struct
  : public constant_dom
{
  void
  show (mpz_class const &v, std::ostream &o) const override
  {
    std::ios::fmtflags f {o.flags ()};
    o << std::oct << std::showbase << v;
    o.flags (f);
  }

  bool
  safe_arith () const override
  {
    return true;
  }

  std::string name () const override
  {
    return "oct";
  }
} oct_constant_dom_obj;

constant_dom const &oct_constant_dom = oct_constant_dom_obj;


static struct
  : public constant_dom
{
  void
  show (mpz_class const &t, std::ostream &o) const override
  {
    if (t == 0)
      o << '0';
    else
      {
	mpz_class v = t < 0 ? mpz_class (-t) : t;
	size_t sz = mpz_sizeinbase (v.get_mpz_t (), 2);
	std::vector <char> chars (sz + 1, '\0');
	for (size_t i = 0; i < sz; ++i)
	  *(chars.rbegin () + i + 1)
	    = mpz_tstbit (v.get_mpz_t (), i) == 0 ? '0' : '1';
	o << (t < 0 ? "-" : "") << "0b" << &*chars.begin ();
      }
  }

  bool
  safe_arith () const override
  {
    return true;
  }

  std::string name () const override
  {
    return "bin";
  }
} bin_constant_dom_obj;

constant_dom const &bin_constant_dom = bin_constant_dom_obj;


static struct
  : public constant_dom
{
  void
  show (mpz_class const &v, std::ostream &o) const override
  {
    std::ios::fmtflags f {o.flags ()};
    o << std::boolalpha << (v != 0);
    o.flags (f);
  }

  std::string name () const override
  {
    return "bool";
  }
} bool_constant_dom_obj;

constant_dom const &bool_constant_dom = bool_constant_dom_obj;


std::ostream &
operator<< (std::ostream &o, constant cst)
{
  cst.dom ()->show (cst.value (), o);
  return o;
}

bool
constant::operator< (constant that) const
{
  if (dom () == nullptr && that.dom () != nullptr)
    return true;
  if (dom () != nullptr && that.dom () == nullptr)
    return false;

  if (dom () != nullptr && dom ()->safe_arith () && that.dom ()->safe_arith ())
    return value () < that.value ();
  else
    return std::make_pair (dom (), value ())
	< std::make_pair (that.dom (), that.value ());
}

bool
constant::operator> (constant that) const
{
  return that < *this;
}

bool
constant::operator== (constant that) const
{
  return ! (*this != that);
}

bool
constant::operator!= (constant that) const
{
  return *this < that || that < *this;
}

void
check_arith (constant const &cst_a, constant const &cst_b)
{
  // If a named constant partakes, warn.
  if (! cst_a.dom ()->safe_arith () || ! cst_b.dom ()->safe_arith ())
    std::cerr << "Warning: doing arithmetic with " << cst_a << " and "
	      << cst_b << " is probably not meaningful.\n";
}

void
check_constants_comparable (constant const &cst_a, constant const &cst_b)
{
  auto dom1 = cst_a.dom ();
  auto dom2 = cst_b.dom ();
  if (dom1 != dom2 && ! dom1->safe_arith () && ! dom2->safe_arith ())
    std::cerr << "Warning: comparing " << cst_a << " to " << cst_b
	      << " is probably not meaningful (domains are "
	      << dom1->name () << " and " << dom2->name () << ").\n";
}
