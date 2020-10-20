/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_MAKE_DISPATCH_HH_included
#define CALL_THAT_MAKE_DISPATCH_HH_included 1

#include "call_that/fwd/call_that-fwd.hh"

#include "call_that/detail/callback_entry.hh"
#include "call_that/detail/callback_table.hh"

#include <memory>

namespace call_that {

// TODO: sort and unique IDs, hana::map dislikes repeated ones;
template <typename... IDs>
auto make_dispatch_table(IDs... ids)
{
   using table = callback_table<callback_entry<IDs>...>;
   return table{};
}

// TODO: sort and unique IDs, hana::map dislikes repeated ones;
template <typename... IDs>
auto alloc_dispatch_table(IDs... ids)
{
   using table = callback_table<callback_entry<IDs>...>;
   return std::make_unique<table>();
}

} // namespace call_that

#endif // !definded(CALL_THAT_MAKE_DISPATCH_HH_included)
