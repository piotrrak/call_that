/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_THAT_CALL_HH_included
#define CALL_THAT_THAT_CALL_HH_included 1

#include "call_that/fwd/call_that-fwd.hh"

#include "call_that/detail/that_call_id.hh"
#include "call_that/detail/callback_entry.hh"
#include "call_that/detail/callback_table.hh"
#include "call_that/detail/dispatch_builder.hh"

#include <hana/at_key.hpp>
#include <hana/for_each.hpp>

namespace call_that
{

template <typename... Callbacks, typename CallID, typename ThatTy>
void attach_cb(callback_table<Callbacks...> &tbl, callback_entry<CallID> entry, ThatTy* that)
{
   static_assert(sizeof(call_that::user_ptr<ThatTy>) != 0,
                 "call_that::user_ptr<> specialiation must be provided!");

   if (that == nullptr) throw std::logic_error("that instance pointer is null!");

   void* uptr = static_cast<void* >(&tbl.map);

   auto has_cb_entry = detail::h::contains(tbl.map, entry);
   static_assert(has_cb_entry, "callback_table must have an entry for callback");

   call_that::user_ptr<ThatTy>::set(CallID{}, that, uptr);

   auto id = entry.id();
   auto cb = detail::make_c_dispatch(id, tbl);

   id.set(that, cb, uptr);
}

template <typename... Callbacks, typename CallID, typename ThatTy,
          typename = std::enable_if_t<detail::is_that_call_id(CallID{})>>
void attach_cb(callback_table<Callbacks...> &tbl, CallID, ThatTy* that)
{
   attach_cb(tbl, callback_entry{CallID{}}, that);
}

template <typename... Callbacks, typename ThatTy>
std::size_t attach_all_cbs(callback_table<Callbacks...> &tbl, ThatTy* that)
{
   static_assert(sizeof(call_that::user_ptr<ThatTy>) != 0,
                 "call_that::user_ptr<> specialiation must be provided!");

   using namespace detail;

   if (that == nullptr) throw std::logic_error("that instance pointer is null!");

   std::size_t active{0};

   auto keys_for_that = h::filter(h::keys(tbl.map), [=](auto k) {
      return k.id().that_t() == h::type<ThatTy>{};
   });

   h::for_each(keys_for_that, [&] (auto k) {
      attach_cb(tbl, k, that);
      ++active;
   });

   return active;
}

template <typename... Callbacks, typename CallID, typename ThatTy>
void detach_cb(callback_table<Callbacks...>& tbl, callback_entry<CallID> entry, ThatTy* that)
{
   static_assert(sizeof(call_that::user_ptr<ThatTy>) != 0,
                 "call_that::user_ptr<> specialiation must be provided!");

   if (that == nullptr) throw std::logic_error("that instance pointer is null!");

   auto has_cb_entry = detail::h::contains(tbl.map, entry);

   static_assert(has_cb_entry, "callback_table must have an entry for callback");

   auto id = entry.id();
   id.set(that, nullptr, nullptr);
}

template <typename... Callbacks, typename CallID, typename ThatTy,
          typename = std::enable_if_t<detail::is_that_call_id(CallID{})>>
void detach_cb(callback_table<Callbacks...>& tbl, CallID id, ThatTy* that)
{
   detach_cb(tbl, callback_entry{id}, that);
}

template <typename... Callbacks, typename ThatTy>
std::size_t detach_all_cbs(callback_table<Callbacks...> &tbl, ThatTy* that)
{
   static_assert(sizeof(call_that::user_ptr<ThatTy>) != 0,
                 "call_that::user_ptr<> specialiation must be provided!");

   // TODO: there is namely few character diff to attach_all_cbs, make common impl
   using namespace detail;

   if (that == nullptr) throw std::logic_error("that instance pointer is null!");

   std::size_t inactive{0};

   auto keys_for_that = h::filter(h::keys(tbl.map), [=](auto k) {
      return k.id().that_t() == h::type<ThatTy>{};
   });

   h::for_each(keys_for_that, [&] (auto k) {
      detach_cb(tbl, k, that);
      ++inactive;
   });

   return inactive;
}

template <typename CallID, typename... Callbacks, typename Fn_>
void on(callback_table<Callbacks...>& tbl, callback_entry<CallID> entry, Fn_ f)
{
   auto has_entry = detail::h::contains(tbl.map, entry);

   static_assert(has_entry, "Must have an entry for callback");

   auto id = CallID{};

   // Type of std::function for c++ callback
   using function = typename decltype(+id.function_t())::type;

   static_assert(std::is_constructible_v<function, Fn_>,
                "Signature of c++ function must be compatible");

   tbl.map[entry].emplace_back(f);
}

template <typename CallID, typename... Callbacks, typename Fn_,
          typename = std::enable_if<detail::is_that_call_id(CallID{})>>
void on(callback_table<Callbacks...>& tbl, CallID id, Fn_ f)
{
   on(tbl, callback_entry{id}, std::forward<Fn_>(f));
}

template <typename CallID, typename... Callbacks>
std::size_t count_cbs(callback_table<Callbacks...>& tbl, callback_entry<CallID> entry)
{
   auto has_entry = detail::h::contains(tbl.map, entry);

   static_assert(has_entry, "Must have an entry for callback");
   return tbl.map[entry].size();
}


template <typename CallID, typename... Callbacks,
          typename = std::enable_if<detail::is_that_call_id(CallID{})>>
std::size_t count_cbs(callback_table<Callbacks...>& tbl, CallID)
{
   return count_cbs(tbl, callback_entry<CallID>{});
}

template <typename CallID, typename... Callbacks>
std::size_t remove_cb(callback_table<Callbacks...>& tbl, callback_entry<CallID> entry, std::size_t index)
{
   auto has_entry = detail::h::contains(tbl.map, entry);

   static_assert(has_entry, "Must have an entry for callback");

   auto& callbacks = tbl.map[entry];

   if (index >= callbacks.size())
      throw std::out_of_range("callback index out of range");

   auto it = callbacks.begin();
   std::advance(it, index);

   callbacks.erase(it);

   return callbacks.size();
}

template <typename CallID, typename... Callbacks,
          typename = std::enable_if<detail::is_that_call_id(CallID{})>>
std::size_t remove_cb(callback_table<Callbacks...>& tbl, CallID, std::size_t number)
{
   return remove_cb(tbl, callback_entry<CallID>{}, number);
}

} // namespace call_that

#endif // !defined(CALL_THAT_THAT_CALL_HH_included)
