/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_TYPE_TRAITS_H_
#define RTC_BASE_TYPE_TRAITS_H_

#include <cstddef>
#include <type_traits>
#include "absl/meta/type_traits.h"
#include "absl/utility/utility.h"
#include "absl/memory/memory.h"

namespace rtc {

// Determines if the given class has zero-argument .data() and .size() methods
// whose return values are convertible to T* and size_t, respectively.
template <typename DS, typename T>
class HasDataAndSize {
 private:
  template <
      typename C,
      typename std::enable_if<
          std::is_convertible<decltype(std::declval<C>().data()), T*>::value &&
          std::is_convertible<decltype(std::declval<C>().size()),
                              std::size_t>::value>::type* = nullptr>
  static int Test(int);

  template <typename>
  static char Test(...);

 public:
  static constexpr bool value = std::is_same<decltype(Test<DS>(0)), int>::value;
};

namespace test_has_data_and_size {

template <typename DR, typename SR>
struct Test1 {
  DR data();
  SR size();
};
static_assert(HasDataAndSize<Test1<int*, int>, int>::value, "");
static_assert(HasDataAndSize<Test1<int*, int>, const int>::value, "");
static_assert(HasDataAndSize<Test1<const int*, int>, const int>::value, "");
static_assert(!HasDataAndSize<Test1<const int*, int>, int>::value,
              "implicit cast of const int* to int*");
static_assert(!HasDataAndSize<Test1<char*, size_t>, int>::value,
              "implicit cast of char* to int*");

struct Test2 {
  int* data;
  size_t size;
};
static_assert(!HasDataAndSize<Test2, int>::value,
              ".data and .size aren't functions");

struct Test3 {
  int* data();
};
static_assert(!HasDataAndSize<Test3, int>::value, ".size() is missing");

class Test4 {
  int* data();
  size_t size();
};
static_assert(!HasDataAndSize<Test4, int>::value,
              ".data() and .size() are private");

}  // namespace test_has_data_and_size

namespace type_traits_impl {

// Determines if the given type is an enum that converts implicitly to
// an integral type.
template <typename T>
struct IsIntEnum {
 private:
  // This overload is used if the type is an enum, and unary plus
  // compiles and turns it into an integral type.
  template <typename X,
            typename std::enable_if<
                std::is_enum<X>::value &&
                std::is_integral<decltype(+std::declval<X>())>::value>::type* =
                nullptr>
  static int Test(int);

  // Otherwise, this overload is used.
  template <typename>
  static char Test(...);

 public:
  static constexpr bool value =
      std::is_same<decltype(Test<typename std::remove_reference<T>::type>(0)),
                   int>::value;
};

}  // namespace type_traits_impl

// Determines if the given type is integral, or an enum that
// converts implicitly to an integral type.
template <typename T>
struct IsIntlike {
 private:
  using X = typename std::remove_reference<T>::type;

 public:
  static constexpr bool value =
      std::is_integral<X>::value || type_traits_impl::IsIntEnum<X>::value;
};

namespace test_enum_intlike {

enum E1 { e1 };
enum { e2 };
enum class E3 { e3 };
struct S {};

static_assert(type_traits_impl::IsIntEnum<E1>::value, "");
static_assert(type_traits_impl::IsIntEnum<decltype(e2)>::value, "");
static_assert(!type_traits_impl::IsIntEnum<E3>::value, "");
static_assert(!type_traits_impl::IsIntEnum<int>::value, "");
static_assert(!type_traits_impl::IsIntEnum<float>::value, "");
static_assert(!type_traits_impl::IsIntEnum<S>::value, "");

static_assert(IsIntlike<E1>::value, "");
static_assert(IsIntlike<decltype(e2)>::value, "");
static_assert(!IsIntlike<E3>::value, "");
static_assert(IsIntlike<int>::value, "");
static_assert(!IsIntlike<float>::value, "");
static_assert(!IsIntlike<S>::value, "");

}  // namespace test_enum_intlike

}  // namespace rtc


//
// c++11 compatiable
//

#if (__cplusplus < 201703L)
namespace std {

template <class Iter>
constexpr std::reverse_iterator<Iter> make_reverse_iterator(Iter i)
{
  return std::reverse_iterator<Iter>(i);
}

template <typename T>
using add_const_t = absl::add_const_t<T>;

template <typename T>
using decay_t = absl::decay_t<T>;

template <bool B, typename T = void>
using enable_if_t = absl::enable_if_t<B, T>;

template <size_t... Ints>
using index_sequence = absl::index_sequence<Ints...>;

template <size_t N>
using make_index_sequence = absl::make_index_sequence<N>;

template <typename T, typename U = T>
T exchange(T& obj, U&& new_value) {
  T old_value = absl::move(obj);
  obj = absl::forward<U>(new_value);
  return old_value;
}

template <typename T, typename... Args>
typename absl::memory_internal::MakeUniqueResult<T>::scalar make_unique(
    Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
template <typename T>
typename absl::memory_internal::MakeUniqueResult<T>::array make_unique(size_t n) {
  return std::unique_ptr<T>(new typename absl::remove_extent_t<T>[n]());
}
template <typename T, typename... Args>
typename absl::memory_internal::MakeUniqueResult<T>::invalid make_unique(
    Args&&... /* args */) = delete;

}
#endif

#if !defined(CONSTEXPR)
  #if (__cplusplus >= 201402)
    #define CONSTEXPR constexpr
  #else
    #define CONSTEXPR
  #endif
#endif


#endif  // RTC_BASE_TYPE_TRAITS_H_
