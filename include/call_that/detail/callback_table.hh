/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_DETAIL_CALLBACK_TABLE_HH_included
#define CALL_THAT_DETAIL_CALLBACK_TABLE_HH_included 1

#include "call_that/detail/callback_entry.hh"

#include <hana/map.hpp>
#include <hana/pair.hpp>

#include <vector>

namespace call_that::detail
{

namespace h = boost::hana;

} // namespace call_that::detail

namespace call_that {

template<typename... IDs>
struct callback_table<callback_entry<IDs>...>
{
   // Maps: callback_entry -> std::vector<std::function<...>>
   detail::h::map<
     detail::h::pair<
       callback_entry<IDs>,
       std::vector<typename decltype(+IDs{}.function_t())::type>
     >...
   > map;
};

} // namespace call_that

#endif // !defined(CALL_THAT_DETAIL_CALLBACK_TABLE_HH_included)
