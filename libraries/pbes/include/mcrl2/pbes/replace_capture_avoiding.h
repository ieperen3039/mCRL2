// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/replace_capture_avoiding.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REPLACE_CAPTURE_AVOIDING_H
#define MCRL2_PBES_REPLACE_CAPTURE_AVOIDING_H

#include "mcrl2/data/replace_capture_avoiding.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/find.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template<template<class> class Builder, class Derived, class Substitution>
struct add_capture_avoiding_replacement
  : public data::detail::add_capture_avoiding_replacement<Builder, Derived, Substitution>
{
  typedef data::detail::add_capture_avoiding_replacement <Builder, Derived, Substitution> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;
  using super::sigma;

  explicit add_capture_avoiding_replacement(data::detail::capture_avoiding_substitution_updater<Substitution>& sigma)
    : super(sigma)
  { }

  pbes_expression apply(const forall& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    pbes_expression result = forall(v1, apply(x.body()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }

  pbes_expression apply(const exists& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variables());
    pbes_expression result = exists(v1, apply(x.body()));
    sigma.remove_fresh_variable_assignments(x.variables());
    return result;
  }

  void update(pbes_equation& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.variable().parameters());
    x.variable() = propositional_variable(x.variable().name(), v1);
    x.formula() = apply(x.formula());
    sigma.remove_fresh_variable_assignments(x.variable().parameters());
  }

  void update(pbes& x)
  {
    data::variable_list v1 = sigma.add_fresh_variable_assignments(x.global_variables());
    x.global_variables() = std::set<data::variable>(v1.begin(), v1.end());
    update(x.equations());
    sigma.remove_fresh_variable_assignments(x.global_variables());
  }
};

} // namespace detail

//--- start generated pbes_system replace_capture_avoiding code ---//
/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the subsitution is applied.
/// \param sigma A substitution.
/// \param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma,
                                        data::set_identifier_generator& id_generator,
                                        typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
std::cout << "replace_capture_avoiding: " << x << sigma << " = ";
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma1).update(x);
std::cout << x << std::endl;
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the substiution is applied.
/// \param sigma A substitution.
/// \param id_generator An identifier generator that generates names that do not appear in x and sigma
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma,
                                     data::set_identifier_generator& id_generator,
                                     typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::detail::capture_avoiding_substitution_updater<Substitution> sigma1(sigma, id_generator);
  auto result = data::detail::apply_replace_capture_avoiding_variables_builder<pbes_system::data_expression_builder, pbes_system::detail::add_capture_avoiding_replacement>(sigma1).apply(x);
std::cout << "replace_capture_avoiding: " << x << sigma << " = " << result << std::endl;
  return result;
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the subsitution is applied.
/// \param sigma A substitution.
template <typename T, typename Substitution>
void replace_variables_capture_avoiding(T& x,
                                        Substitution& sigma,
                                        typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(pbes_system::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  pbes_system::replace_variables_capture_avoiding(x, sigma, id_generator);
}

/// \brief Applies sigma as a capture avoiding substitution to x.
/// \param x The object to which the substiution is applied.
/// \param sigma A substitution.
template <typename T, typename Substitution>
T replace_variables_capture_avoiding(const T& x,
                                     Substitution& sigma,
                                     typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr
)
{
  data::set_identifier_generator id_generator;
  id_generator.add_identifiers(pbes_system::find_identifiers(x));
  for (const data::variable& v: substitution_variables(sigma))
  {
    id_generator.add_identifier(v.name());
  }
  return pbes_system::replace_variables_capture_avoiding(x, sigma, id_generator);
}
//--- end generated pbes_system replace_capture_avoiding code ---//

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REPLACE_CAPTURE_AVOIDING_H
