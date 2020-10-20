/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_DETAIL_DISPATCH_BULDER_HH_included
#define CALL_THAT_DETAIL_DISPATCH_BULDER_HH_included 1

#include "call_that/fwd/call_that-fwd.hh"

#include "call_that/user_ptr.hh"

#include <hana/basic_tuple.hpp>
#include <hana/contains.hpp>
#include <hana/type.hpp>

#include <stdexcept>

namespace call_that::detail
{

namespace h = boost::hana;

template <typename CallId>
struct dispatch_builder
{
   using thunk_type = typename decltype(+CallId{}.c_cb_t())::type;

   template <typename That, typename Map, typename... Args>
   constexpr static auto c_thunk(h::basic_type<That> that,
                                 Map& m,
                                 h::basic_tuple<Args...> args) -> thunk_type
   {
      auto entry = callback_entry<CallId>{};
      auto has_cb_entry = h::contains(m, entry);

      static_assert(has_cb_entry, "callback_table must have an entry for callback");

      // Create lambda function convertible to c-fuction ptr
      return [] (That* that, Args... args) -> void {

         if (that == nullptr)
            throw std::logic_error("that instance pointer is null!");

         void* uptr = call_that::user_ptr<That>::get(CallId{}, that, std::forward<Args>(args)...);

         if (uptr == nullptr)
            throw std::logic_error("user_ptr not set!");

         auto entry = callback_entry<CallId>{};

         auto& map = *static_cast<Map*>(uptr);

         for (auto &cb : map[entry])
         {
            cb(std::forward<Args>(args)...);
         }
      };
   }
};


template <typename CallID, typename Table>
auto make_c_dispatch(CallID id, Table& tbl)
{
   static_assert(detail::is_that_call_id(id));

   return dispatch_builder<CallID>::c_thunk(id.that_t(), tbl.map, id.args_t());
}

}

#endif // !defined(CALL_THAT_DETAIL_DISPATCH_BULDER_HH_included)
