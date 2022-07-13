/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef RTC_BASE_UNITS_UNIT_BASE_H_
#define RTC_BASE_UNITS_UNIT_BASE_H_

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <type_traits>

#include "rtc_base/checks.h"
#include "rtc_base/numerics/safe_conversions.h"

namespace webrtc {
namespace rtc_units_impl {

// UnitBase is a base class for implementing custom value types with a specific
// unit. It provides type safety and commonly useful operations. The underlying
// storage is always an int64_t, it's up to the unit implementation to choose
// what scale it represents.
//
// It's used like:
// class MyUnit: public UnitBase<MyUnit> {...};
//
// Unit_T is the subclass representing the specific unit.
template <class Unit_T>
class UnitBase {
 public:
  UnitBase() = delete;
  static CONSTEXPR Unit_T Zero() { return Unit_T(0); }
  static CONSTEXPR Unit_T PlusInfinity() { return Unit_T(PlusInfinityVal()); }
  static CONSTEXPR Unit_T MinusInfinity() { return Unit_T(MinusInfinityVal()); }

  CONSTEXPR bool IsZero() const { return value_ == 0; }
  CONSTEXPR bool IsFinite() const { return !IsInfinite(); }
  CONSTEXPR bool IsInfinite() const {
    return value_ == PlusInfinityVal() || value_ == MinusInfinityVal();
  }
  CONSTEXPR bool IsPlusInfinity() const { return value_ == PlusInfinityVal(); }
  CONSTEXPR bool IsMinusInfinity() const {
    return value_ == MinusInfinityVal();
  }

  CONSTEXPR bool operator==(const UnitBase<Unit_T>& other) const {
    return value_ == other.value_;
  }
  CONSTEXPR bool operator!=(const UnitBase<Unit_T>& other) const {
    return value_ != other.value_;
  }
  CONSTEXPR bool operator<=(const UnitBase<Unit_T>& other) const {
    return value_ <= other.value_;
  }
  CONSTEXPR bool operator>=(const UnitBase<Unit_T>& other) const {
    return value_ >= other.value_;
  }
  CONSTEXPR bool operator>(const UnitBase<Unit_T>& other) const {
    return value_ > other.value_;
  }
  CONSTEXPR bool operator<(const UnitBase<Unit_T>& other) const {
    return value_ < other.value_;
  }
  CONSTEXPR Unit_T RoundTo(const Unit_T& resolution) const {
    RTC_DCHECK(IsFinite());
    RTC_DCHECK(resolution.IsFinite());
    RTC_DCHECK_GT(resolution.value_, 0);
    return Unit_T((value_ + resolution.value_ / 2) / resolution.value_) *
           resolution.value_;
  }
  CONSTEXPR Unit_T RoundUpTo(const Unit_T& resolution) const {
    RTC_DCHECK(IsFinite());
    RTC_DCHECK(resolution.IsFinite());
    RTC_DCHECK_GT(resolution.value_, 0);
    return Unit_T((value_ + resolution.value_ - 1) / resolution.value_) *
           resolution.value_;
  }
  CONSTEXPR Unit_T RoundDownTo(const Unit_T& resolution) const {
    RTC_DCHECK(IsFinite());
    RTC_DCHECK(resolution.IsFinite());
    RTC_DCHECK_GT(resolution.value_, 0);
    return Unit_T(value_ / resolution.value_) * resolution.value_;
  }

 protected:
  template <
      typename T,
      typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  static CONSTEXPR Unit_T FromValue(T value) {
    if (Unit_T::one_sided)
      RTC_DCHECK_GE(value, 0);
    RTC_DCHECK_GT(value, MinusInfinityVal());
    RTC_DCHECK_LT(value, PlusInfinityVal());
    return Unit_T(rtc::dchecked_cast<int64_t>(value));
  }
  template <typename T,
            typename std::enable_if<std::is_floating_point<T>::value>::type* =
                nullptr>
  static CONSTEXPR Unit_T FromValue(T value) {
    if (value == std::numeric_limits<T>::infinity()) {
      return PlusInfinity();
    } else if (value == -std::numeric_limits<T>::infinity()) {
      return MinusInfinity();
    } else {
      RTC_DCHECK(!std::isnan(value));
      return FromValue(rtc::dchecked_cast<int64_t>(value));
    }
  }

  template <
      typename T,
      typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
  static CONSTEXPR Unit_T FromFraction(int64_t denominator, T value) {
    if (Unit_T::one_sided)
      RTC_DCHECK_GE(value, 0);
    RTC_DCHECK_GT(value, MinusInfinityVal() / denominator);
    RTC_DCHECK_LT(value, PlusInfinityVal() / denominator);
    return Unit_T(rtc::dchecked_cast<int64_t>(value * denominator));
  }
  template <typename T,
            typename std::enable_if<std::is_floating_point<T>::value>::type* =
                nullptr>
  static CONSTEXPR Unit_T FromFraction(int64_t denominator, T value) {
    return FromValue(value * denominator);
  }

  template <typename T = int64_t>
  CONSTEXPR typename std::enable_if<std::is_integral<T>::value, T>::type
  ToValue() const {
    RTC_DCHECK(IsFinite());
    return rtc::dchecked_cast<T>(value_);
  }
  template <typename T>
  CONSTEXPR typename std::enable_if<std::is_floating_point<T>::value, T>::type
  ToValue() const {
    return IsPlusInfinity()
               ? std::numeric_limits<T>::infinity()
               : IsMinusInfinity() ? -std::numeric_limits<T>::infinity()
                                   : value_;
  }
  template <typename T>
  CONSTEXPR T ToValueOr(T fallback_value) const {
    return IsFinite() ? value_ : fallback_value;
  }

  template <int64_t Denominator, typename T = int64_t>
  CONSTEXPR typename std::enable_if<std::is_integral<T>::value, T>::type
  ToFraction() const {
    RTC_DCHECK(IsFinite());
    if (Unit_T::one_sided) {
      return rtc::dchecked_cast<T>(
          DivRoundPositiveToNearest(value_, Denominator));
    } else {
      return rtc::dchecked_cast<T>(DivRoundToNearest(value_, Denominator));
    }
  }
  template <int64_t Denominator, typename T>
  CONSTEXPR typename std::enable_if<std::is_floating_point<T>::value, T>::type
  ToFraction() const {
    return ToValue<T>() * (1 / static_cast<T>(Denominator));
  }

  template <int64_t Denominator>
  CONSTEXPR int64_t ToFractionOr(int64_t fallback_value) const {
    return IsFinite() ? Unit_T::one_sided
                            ? DivRoundPositiveToNearest(value_, Denominator)
                            : DivRoundToNearest(value_, Denominator)
                      : fallback_value;
  }

  template <int64_t Factor, typename T = int64_t>
  CONSTEXPR typename std::enable_if<std::is_integral<T>::value, T>::type
  ToMultiple() const {
    RTC_DCHECK_GE(ToValue(), std::numeric_limits<T>::min() / Factor);
    RTC_DCHECK_LE(ToValue(), std::numeric_limits<T>::max() / Factor);
    return rtc::dchecked_cast<T>(ToValue() * Factor);
  }
  template <int64_t Factor, typename T>
  CONSTEXPR typename std::enable_if<std::is_floating_point<T>::value, T>::type
  ToMultiple() const {
    return ToValue<T>() * Factor;
  }

  explicit CONSTEXPR UnitBase(int64_t value) : value_(value) {}

 private:
  template <class RelativeUnit_T>
  friend class RelativeUnit;

  static inline CONSTEXPR int64_t PlusInfinityVal() {
    return std::numeric_limits<int64_t>::max();
  }
  static inline CONSTEXPR int64_t MinusInfinityVal() {
    return std::numeric_limits<int64_t>::min();
  }

  CONSTEXPR Unit_T& AsSubClassRef() { return static_cast<Unit_T&>(*this); }
  CONSTEXPR const Unit_T& AsSubClassRef() const {
    return static_cast<const Unit_T&>(*this);
  }
  // Assumes that n >= 0 and d > 0.
  static CONSTEXPR int64_t DivRoundPositiveToNearest(int64_t n, int64_t d) {
    return (n + d / 2) / d;
  }
  // Assumes that d > 0.
  static CONSTEXPR int64_t DivRoundToNearest(int64_t n, int64_t d) {
    return (n + (n >= 0 ? d / 2 : -d / 2)) / d;
  }

  int64_t value_;
};

// Extends UnitBase to provide operations for relative units, that is, units
// that have a meaningful relation between values such that a += b is a
// sensible thing to do. For a,b <- same unit.
template <class Unit_T>
class RelativeUnit : public UnitBase<Unit_T> {
 public:
  CONSTEXPR Unit_T Clamped(Unit_T min_value, Unit_T max_value) const {
    return std::max(min_value,
                    std::min(UnitBase<Unit_T>::AsSubClassRef(), max_value));
  }
  CONSTEXPR void Clamp(Unit_T min_value, Unit_T max_value) {
    *this = Clamped(min_value, max_value);
  }
  CONSTEXPR Unit_T operator+(const Unit_T other) const {
    if (this->IsPlusInfinity() || other.IsPlusInfinity()) {
      RTC_DCHECK(!this->IsMinusInfinity());
      RTC_DCHECK(!other.IsMinusInfinity());
      return this->PlusInfinity();
    } else if (this->IsMinusInfinity() || other.IsMinusInfinity()) {
      RTC_DCHECK(!this->IsPlusInfinity());
      RTC_DCHECK(!other.IsPlusInfinity());
      return this->MinusInfinity();
    }
    return UnitBase<Unit_T>::FromValue(this->ToValue() + other.ToValue());
  }
  CONSTEXPR Unit_T operator-(const Unit_T other) const {
    if (this->IsPlusInfinity() || other.IsMinusInfinity()) {
      RTC_DCHECK(!this->IsMinusInfinity());
      RTC_DCHECK(!other.IsPlusInfinity());
      return this->PlusInfinity();
    } else if (this->IsMinusInfinity() || other.IsPlusInfinity()) {
      RTC_DCHECK(!this->IsPlusInfinity());
      RTC_DCHECK(!other.IsMinusInfinity());
      return this->MinusInfinity();
    }
    return UnitBase<Unit_T>::FromValue(this->ToValue() - other.ToValue());
  }
  CONSTEXPR Unit_T& operator+=(const Unit_T other) {
    *this = *this + other;
    return this->AsSubClassRef();
  }
  CONSTEXPR Unit_T& operator-=(const Unit_T other) {
    *this = *this - other;
    return this->AsSubClassRef();
  }
  CONSTEXPR double operator/(const Unit_T other) const {
    return UnitBase<Unit_T>::template ToValue<double>() /
           other.template ToValue<double>();
  }
  template <typename T,
            typename std::enable_if_t<std::is_floating_point<T>::value>* = nullptr>
  Unit_T operator/(T scalar) const {
    return UnitBase<Unit_T>::FromValue(std::llround(this->ToValue() / scalar));
  }
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
  CONSTEXPR Unit_T operator/(T scalar) const {
    return UnitBase<Unit_T>::FromValue(this->ToValue() / scalar);
  }
  Unit_T operator*(double scalar) const {
    return UnitBase<Unit_T>::FromValue(std::llround(this->ToValue() * scalar));
  }
  CONSTEXPR Unit_T operator*(int64_t scalar) const {
    return UnitBase<Unit_T>::FromValue(this->ToValue() * scalar);
  }
  CONSTEXPR Unit_T operator*(int32_t scalar) const {
    return UnitBase<Unit_T>::FromValue(this->ToValue() * scalar);
  }
  CONSTEXPR Unit_T operator*(size_t scalar) const {
    return UnitBase<Unit_T>::FromValue(this->ToValue() * scalar);
  }

 protected:
  using UnitBase<Unit_T>::UnitBase;
};

template <class Unit_T>
inline CONSTEXPR Unit_T operator*(double scalar, RelativeUnit<Unit_T> other) {
  return other * scalar;
}
template <class Unit_T>
inline CONSTEXPR Unit_T operator*(int64_t scalar, RelativeUnit<Unit_T> other) {
  return other * scalar;
}
template <class Unit_T>
inline CONSTEXPR Unit_T operator*(int32_t scalar, RelativeUnit<Unit_T> other) {
  return other * scalar;
}
template <class Unit_T>
inline CONSTEXPR Unit_T operator*(size_t scalar, RelativeUnit<Unit_T> other) {
  return other * scalar;
}

template <class Unit_T>
inline CONSTEXPR Unit_T operator-(RelativeUnit<Unit_T> other) {
  if (other.IsPlusInfinity())
    return UnitBase<Unit_T>::MinusInfinity();
  if (other.IsMinusInfinity())
    return UnitBase<Unit_T>::PlusInfinity();
  return -1 * other;
}

}  // namespace rtc_units_impl

}  // namespace webrtc

#endif  // RTC_BASE_UNITS_UNIT_BASE_H_
