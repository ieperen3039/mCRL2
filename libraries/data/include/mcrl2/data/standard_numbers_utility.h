// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/standard_numbers_utility.h
/// \brief Provides utilities for working with data expressions of standard sorts

#ifndef MCRL2_DATA_STANDARD_NUMBERS_UTILITY_H
#define MCRL2_DATA_STANDARD_NUMBERS_UTILITY_H

#include <type_traits>
#include "mcrl2/data/bool.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/standard_utility.h"

namespace mcrl2
{

namespace data
{

/// \cond INTERNAL_DOCS
namespace detail
{


// Convert to number represented as character array where each character
// represents a decimal digit
inline std::vector< char > string_to_vector_number(const std::string& s)
{
  assert(s.size() > 0);
  std::vector< char > result;

  result.reserve(s.size());

  for (char i: s)
  {
    assert('0' <= i && i <= '9');

    result.push_back(i - '0');
  }

  return result;
}

// Convert from number represented as character array where each character
// represents a decimal digit
inline std::string vector_number_to_string(const std::vector< char >& v)
{
  assert(v.size() > 0);
  std::string result;

  result.reserve(v.size());

  for (char i: v)
  {
    result.push_back(i + '0');
  }

  return result;
}

/// Type T is an unsigned type
template< typename T >
inline std::string as_decimal_string(T t)
{
  if (t != 0)
  {
    std::string result;

    while (0 < t)
    {
      result.append(1, '0' + static_cast< char >(t % 10));

      t /= 10;
    }

    return std::string(result.rbegin(), result.rend());
  }

  return "0";
}

/// \brief Divides a number in decimal notation represented by an array by two
/// \param[in,out] s the number
/// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
inline void decimal_number_divide_by_two(std::vector< char >& number)
{
  assert(0 < number.size());

  std::vector< char >           result(number.size(), 0);
  std::vector< char >::iterator j(result.begin());

  if (2 <= number[0])
  {
    *(j++) = number[0] / 2;
  }

  for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j)
  {
    // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j, number[b - 1] = *(i - 1)
    *j = 5 * (*(i - 1) % 2) + *i / 2;
  }

  result.resize(j - result.begin());

  number.swap(result);
}

/// \brief Multiplies a number in decimal notation represented by an array by two
/// \param[in,out] number the number
/// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
inline void decimal_number_multiply_by_two(std::vector< char >& number)
{
  assert(0 < number.size());

  std::vector< char >           result(number.size() + 2, 0);
  std::vector< char >::iterator j(result.begin());

  if (5 <= number[0])
  {
    *(j++) = number[0] / 5;
  }

  for (std::vector< char >::const_iterator i = number.begin(); i < number.end(); ++i, ++j)
  {
    // result[a] = 2*(number[b] mod 5) + number[b+1] div 5   where result[a] = *j and number[b] = *(i)
    if (i == number.end() - 1)
    {
      *j = 2 * (*i % 5);
    }
    else
    {
      *j = 2 * (*i % 5) + *(i+1) / 5;
    }
  }

  result.resize(j - result.begin());

  number.swap(result);
}

/// \brief Adds one to a number in decimal notation represented by an array
/// \param[in,out] number the number
/// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
inline void decimal_number_increment(std::vector< char >& number)
{
  assert(0 < number.size());

  for (std::vector< char >::reverse_iterator i = number.rbegin(); i != number.rend(); ++i)
  {
    if (*i < 9)
    {
      ++(*i);

      return;
    }
    else
    {
      *i = 0;
    }
  }

  number.insert(number.begin(), 1);
}

// Check whether a number vector is equal to zero.
inline bool is_zero_number_vector(const std::vector< std::size_t >& v)
{
  for(const std::size_t n: v)
  {
    if (n>0)
    {
      return false;
    }
  }
  // All digits are zero. Thus the number is zero.
  assert(v.size()<2);  // Zero can only occur as a vector of length 0 or 1. 
  return true;
}

// Divide the vector v by two, and yield true if the vector was odd. 
inline bool div2_number_vector(std::vector< std::size_t >& v)
{
  bool carry=false;
  const std::size_t leftmost_carry_mask = (std::size_t)1 << (8*sizeof(std::size_t)-1);

  for(std::size_t i=v.size(); i>0; --i)
  {
    bool next_carry= v.at(i-1) % 2 == 1;
    v[i-1] = v[i-1]/2; 
    if (carry)
    {
      v[i-1] = v[i-1] | leftmost_carry_mask;
    }
    carry = next_carry; 
  }
  if (v.size()>0 && v.back()==0) 
  {
    v.pop_back();
  }
  return carry;
}

// Add the two number vectors. The least significant digit is at position 0.
inline std::vector<std::size_t> add_number_vectors(const std::vector< std::size_t >& v1, 
                                                   const std::vector< std::size_t >& v2)
{
  std::vector<std::size_t> result;
  std::size_t carry=0;
  for(std::size_t i=0; i<std::max(v1.size(),v2.size()); i++)
  {
    std::size_t n1 = (i<v1.size()? v1[i]: 0);
    std::size_t n2 = (i<v2.size()? v2[i]: 0);

    std::size_t sum = n1+n2+carry;
    if (sum<n1)  // There is a carry overflow. 
    {
      assert(sum<n2);
      carry=1;
    }
    result.push_back(sum);
  }
  if (carry==1)
  {
    result.push_back(1);
  }
  return result;
}

inline std::vector<size_t> multiply_by10_and_add(const std::vector< size_t >& v, size_t val)
{
  std::vector<size_t> val_vector(1,val);
  assert(val_vector.size()==1);
  std::vector<size_t> result=add_number_vectors(v,v);   // result is 2 v.
  result = add_number_vectors(result, result);          // result is 4 v.
  result = add_number_vectors(result, v);               // result is 5 v.
  result = add_number_vectors(result, result);          // result is 10 v.
  result = add_number_vectors(result, val_vector);      // result is 10 v + val.
  return result;
}

// Transform a number vector to a string representation of the same number.
inline std::string number_vector_as_string(const std::vector< std::size_t >& v1)
{
  std::vector< std::size_t > v=v1;
  assert(!detail::is_zero_number_vector(v));

  std::vector< char > result = detail::string_to_vector_number("0");

  while (!detail::is_zero_number_vector(v)) 
  {
    detail::decimal_number_multiply_by_two(result);
    if (detail::div2_number_vector(v))
    {
      detail::decimal_number_increment(result);
    }
  }

  return detail::vector_number_to_string(result);
}


// Multiply a vector consisting of size_t's with 10 and add the extra value.
// Convert to number represented as character array where each character
// represents a decimal digit
inline std::vector< size_t > number_string_to_vector_number(const std::string& s)
{
  assert(s.size() > 0);
  std::vector< size_t > result;

  result.reserve(s.size()/18);  // approximately 18 digits fit in one size_t.

  for (char i: s)
  {
    if ('0' <= i && i <= '9')
    {
       result = multiply_by10_and_add(result, i - '0');
    }
    else
    {
      throw mcrl2::runtime_error("The string " + s + " is expected to only consist of digits. ");
    }
  }

  return result;
}
} // namespace detail
/// \endcond

namespace sort_pos
{
/// \brief Constructs expression of type Bool from an integral type
/// Type T is an unsigned integral type.
template < typename T >
inline typename std::enable_if<std::is_integral< T >::value, data_expression>::type
pos(const T t)
{
  static_assert(sizeof(T)<=sizeof(std::size_t),"Can only convert numbers up till a size_t.");
  assert(t>0);
  return most_significant_digit(machine_number(t));
}

/// \brief Constructs expression of type Pos from a string
/// \param n A string
inline data_expression pos(const std::string& n)
{
  std::vector< char > number_as_vector(detail::string_to_vector_number(n));

  std::vector< bool > bits;
  bits.reserve(number_as_vector.size());

  while (0 < number_as_vector.size() && !((number_as_vector.size() == 1) && number_as_vector[0] == 1))   // number != 1
  {
    bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));

    detail::decimal_number_divide_by_two(number_as_vector);
  }

  data_expression result(sort_pos::c1());

  for (std::vector< bool >::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i)
  {
    result = sort_pos::cdub(sort_bool::bool_(*i), result);
  }

  return result;
}

/// \brief Determines whether n is a positive constant
/// \param n A data expression
inline bool is_positive_constant(const data_expression& n)
{
  return (sort_pos::is_most_significant_digit_application(n) &&
          is_machine_number(arg(n))) ||
         (sort_pos::is_concat_digit_application(n) &&
          sort_pos::is_positive_constant(sort_pos::arg1(n)) &&
          is_machine_number(sort_nat::arg2(n)) 
         );
}

/// \brief Return the string representation of a positive number
/// \param n_in A data expression
/// \pre is_positive_constant(n)
/// \return String representation of n
/// Transforms a positive constant n into a character array containing
/// the decimal representation of n.
inline
std::string positive_constant_as_string(const data_expression& n_in)
{
  std::vector<std::size_t> number_vector;
  data_expression n=n_in;

  while (sort_pos::is_concat_digit_application(n))
  {
    number_vector.push_back(atermpp::down_cast<machine_number>(sort_pos::arg2(n)).value());
    n = sort_pos::arg1(n);
  }

  assert(sort_pos::is_most_significant_digit_application(n));
  number_vector.push_back(atermpp::down_cast<machine_number>(sort_pos::arg(n)).value());

  return detail::number_vector_as_string(number_vector);
}
} // namespace sort_pos

namespace sort_nat
{

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
nat(T t)
{
  static_assert(sizeof(T)<=sizeof(std::size_t),"Can only convert numbers up till a size_t.");
  return sort_nat::most_significant_digit_nat(machine_number(t));
}

/// \brief Constructs expression of type Nat from a string
/// \param n A string
inline data_expression nat(const std::string& n)
{
  std::vector<std::size_t> number_vector= detail::number_string_to_vector_number(n);

  if (number_vector.empty())
  {
    return most_significant_digit_nat(machine_number(0));
  }
  data_expression result=most_significant_digit_nat(machine_number(number_vector.back()));
  for(std::size_t i=number_vector.size()-1; i>0; ++i)
  {
    result = sort_nat::concat_digit(sort_nat::nat(),sort_machine_word::machine_word())
                       (result,machine_number(number_vector.at(i-1)));
  }
  return result;
}

/// \brief Determines whether n is a natural constant
/// \param n A data expression
inline bool is_natural_constant(const data_expression& n)
{
  return (sort_nat::is_most_significant_digit_nat_application(n) &&
          is_machine_number(arg(n))) ||
         (sort_nat::is_concat_digit_application(n) &&
          sort_nat::is_natural_constant(sort_nat::arg1(n)) &&
          is_machine_number(sort_nat::arg2(n)) 
         );
}

/// \brief Return the string representation of a natural number
/// \param n A data expression
/// \pre is_natural_constant(n)
/// \return String representation of n
inline std::string natural_constant_as_string(const data_expression& n_in)
{
  std::vector<std::size_t> number_vector;
  data_expression n=n_in;
  
  while (sort_nat::is_concat_digit_application(n))
  { 
    number_vector.push_back(atermpp::down_cast<machine_number>(sort_nat::arg2(n)).value());
    n = sort_nat::arg1(n);
  }
  
  assert(sort_nat::is_most_significant_digit_nat_application(n));
  number_vector.push_back(atermpp::down_cast<machine_number>(sort_nat::arg(n)).value());

  if (detail::is_zero_number_vector(number_vector))
  {
    return "0";
  }

  return detail::number_vector_as_string(number_vector);
}
} // namespace sort_nat

namespace sort_int
{

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value && std::is_unsigned< T >::value, data_expression >::type
int_(T t)
{
  return sort_int::cint(sort_nat::nat(t));
}

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value && std::is_signed< T >::value, data_expression >::type
int_(T t)
{
  if (t<0)
  {
    return sort_int::cneg(sort_pos::pos(typename std::make_unsigned<T>::type(-t)));
  }
  return sort_int::cint(sort_nat::nat(typename std::make_unsigned<T>::type(t)));
}

/// \brief Constructs expression of type Int from a string
/// \param n A string
/// \pre n is of the form ([-]?[0...9][0...9]+)([0...9]+)
inline data_expression int_(const std::string& n)
{
  if (n[0] == '-')
  {
    return sort_int::cneg(sort_pos::pos(n.substr(1)));
  }
  return sort_int::cint(sort_nat::nat(n));
}

/// \brief Determines whether n is an integer constant
/// \param n A data expression
inline bool is_integer_constant(const data_expression& n)
{
  return (sort_int::is_cint_application(n) &&
          sort_nat::is_natural_constant(sort_int::arg(n))) ||
         (sort_int::is_cneg_application(n) &&
          sort_pos::is_positive_constant(sort_int::arg(n))
         );
}

/// \brief Return the string representation of an integer number
/// \param n A data expression
/// \pre is_integer_constant(n)
/// \return String representation of n
inline std::string integer_constant_as_string(const data_expression& n)
{
  assert(is_integer_constant(n));
  if (sort_int::is_cint_application(n))
  {
    return sort_nat::natural_constant_as_string(sort_int::arg(n));
  }
  else
  {
    return "-" + sort_pos::positive_constant_as_string(sort_int::arg(n));
  }
}
} // namespace sort_int

namespace sort_real
{
/// \brief Constructs expression of type pos from an integral type
/// \param t An expression of type T
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
real_(T t)
{
  return sort_real::creal(sort_int::int_(t), sort_pos::c1());
}

/// \brief Constructs expression of type pos from an integral type
/// \param numerator numerator
/// \param denominator denominator
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
real_(T numerator, T denominator)
{
  return sort_real::creal(sort_int::int_(numerator), sort_pos::pos(denominator));
}

/// \brief Constructs expression of type Real from a string
/// \param n A string
/// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
inline data_expression real_(const std::string& n)
{
  return sort_real::creal(sort_int::int_(n), sort_pos::c1());
}
} // namespace sort_real


/// \brief Construct numeric expression from a string representing a number in decimal notation
/// \param s A sort expression
/// \param n A string
/// \pre n is of the form [1]?[0...9]+
inline data_expression number(const sort_expression& s, const std::string& n)
{
  if (s == sort_pos::pos())
  {
    return sort_pos::pos(n);
  }
  else if (s == sort_nat::nat())
  {
    return sort_nat::nat(n);
  }
  else if (s == sort_int::int_())
  {
    return sort_int::int_(n);
  }

  return sort_real::real_(n);
}

/// \brief Returns true if and only if s1 == s2, or if s1 is a less specific numeric type than s2
///
/// \param[in] s1 a sort expression
/// \param[in] s2 a sort expression
inline bool is_convertible(const sort_expression& s1, const sort_expression& s2)
{
  if (s1 != s2)
  {
    if (s2 == sort_real::real_())
    {
      return s1 == sort_int::int_() || s1 == sort_nat::nat() || s1 == sort_pos::pos();
    }
    else if (s2 == sort_int::int_())
    {
      return s1 == sort_nat::nat() || s1 == sort_pos::pos();
    }
    else if (s2 == sort_nat::nat())
    {
      return s1 == sort_pos::pos();
    }

    return false;
  }

  return true;
}

} // namespace data

} // namespace mcrl2

#endif

