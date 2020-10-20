/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_FWD_CALL_THAT_FWD_HH_included
#define CALL_THAT_FWD_CALL_THAT_FWD_HH_included 1

#include "call_that/config.hh"

#include <hana/fwd/at_key.hpp>
#include <hana/fwd/basic_tuple.hpp>
#include <hana/fwd/contains.hpp>
#include <hana/fwd/for_each.hpp>
#include <hana/fwd/map.hpp>
#include <hana/fwd/pair.hpp>
#include <hana/fwd/type.hpp>

namespace call_that
{

template <typename...> struct callback_table;
template <typename>    struct callback_entry;
template <auto>        struct that_call_id;
template <typename>    struct user_ptr;
}

#endif // !defined(CALL_THAT_FWD_CALL_THAT_FWD_HH_included)
