/*
 * SPDX-FileCopyrightText: 2020 Piotr Rak <piotr.rak@gamil.com>
 * SPDX-License-Identifier: MIT
 */

#ifndef CALL_THAT_DETAIL_THAT_CALL_ID_HH_included
#define CALL_THAT_DETAIL_THAT_CALL_ID_HH_included 1

#include "call_that/fwd/call_that-fwd.hh"

#include <hana/basic_tuple.hpp>
#include <hana/type.hpp>

#include <functional>
#include <type_traits>

namespace call_that::detail {

namespace h = boost::hana;

auto is_that_call_id = h::is_valid([](auto && trialed) -> decltype(trialed.that_t()) {});

}
namespace call_that {

// This bit overwhelming partial template specialization pattern-matches value of function pointer:
//
//   R (*)(Type*, R2(*)(Type*, Args...))
//
// For example if we have:
//
// using fn_type = void (*) (Foo*, int, int);
// void foobar(Foo*, fn_type);
//
// that_call_id<foobar> will match and will figure out:
//
// - That type: Foo*
// - return types of function and function pointer
// - argument list (int, int)
//
// that_call_id is integral_contant of function pointer value, thus it is unique type
// for given function pointer.
template <typename ThatTy_, typename R_, typename RCb_, typename... CbArgs_,
           R_ (Setter) (ThatTy_*, RCb_ (*)(ThatTy_*, CbArgs_...))>
struct that_call_id<Setter> : detail::h::integral_constant<decltype(Setter), Setter>
{
private:
   using _that_type               = ThatTy_;
   using _setter_type             = decltype(Setter);

   using _callback_c_type         = RCb_ (*) (_that_type*, CbArgs_...);
   using _callback_return_type    = std::invoke_result_t<_callback_c_type, _that_type*, CbArgs_...>;
   using _callback_argument_types = detail::h::basic_tuple<CbArgs_...>;
   using _return_type             = std::invoke_result_t<_setter_type, _that_type*, _callback_c_type>;
   using _function_type           = std::function<RCb_ (CbArgs_...)>;

   static inline constexpr _setter_type _setter_function = Setter;

   static_assert(std::is_pointer_v<_setter_type>);
   static_assert(std::is_same_v<void, _callback_return_type>,
                                "Unsupported - would require return value combinators");

   static_assert(sizeof(call_that::user_ptr<ThatTy_>) != 0,
                 "call_that::user_ptr<> specialiation must be provided!");

   // TODO: GLFW returns previous ptr - how do we handle it?
   //static_assert(std::is_same_v<void, _return_type>,
   //                             "Unsupported - we do not handle setter return value in any way");

public:

   // returns hana::basic_tuple of argument types
   static constexpr auto args_t()     noexcept { return _callback_argument_types{}; }
   static constexpr auto return_t()   noexcept { return detail::h::type<_callback_return_type>{}; }
   static constexpr auto that_t()     noexcept { return detail::h::type<_that_type>{}; }
   static constexpr auto function_t() noexcept { return detail::h::type<_function_type>{}; }
   static constexpr auto c_cb_t()     noexcept { return detail::h::type<_callback_c_type>{}; }

   auto set(_that_type* that, _callback_c_type cb, [[maybe_unused]] void *uptr)
   {
      (void) uptr;
      return _setter_function(that, cb);
   }
};

// TODO:
// Same as above but for const 'that' pointer

// TODO: SFINAE disabler
template <auto FPtr>
constexpr auto setter_id() noexcept
{
   return that_call_id<FPtr>{};
}

} // namespace call_that

#endif // !defined(CALL_THAT_DETAIL_THAT_CALL_ID_HH_included)
