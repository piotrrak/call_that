/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_DETAIL_CALLBACK_ENTRY_HH_included
#define CALL_THAT_DETAIL_CALLBACK_ENTRY_HH_included 1

#include "call_that/fwd/call_that-fwd.hh"

#include <hana/type.hpp>

namespace call_that::detail
{

namespace h = boost::hana;

} // namespace call_that::detail

namespace call_that
{

template <typename ID>
struct callback_entry: detail::h::type<ID>
{
   constexpr callback_entry() = default;

   constexpr callback_entry(ID) {}

   constexpr auto id() noexcept { return ID{}; }
};

} // namespace call_that

#endif // !defined(CALL_THAT_DETAIL_CALLBACK_ENTRY_HH_included)
