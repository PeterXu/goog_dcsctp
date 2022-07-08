/*
 *  Copyright 2019 The Chromium Authors. All rights reserved.
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef RTC_BASE_STRONG_ALIAS_H_
#define RTC_BASE_STRONG_ALIAS_H_

#include <type_traits>
#include <utility>
#include "rtc_base/type_traits.h"

namespace webrtc {

// This is a copy of
// https://source.chromium.org/chromium/chromium/src/+/main:base/types/strong_alias.h
// as the API (and internals) are using type-safe integral identifiers, but this
// library can't depend on that file. The ostream operator has been removed
// per WebRTC library conventions, and the underlying type is exposed.

template <typename TagType, typename TheUnderlyingType>
class StrongAlias {
 public:
  using UnderlyingType = TheUnderlyingType;
  CONSTEXPR StrongAlias() = default;
  CONSTEXPR explicit StrongAlias(const UnderlyingType& v) : value_(v) {}
  CONSTEXPR explicit StrongAlias(UnderlyingType&& v) noexcept
      : value_(std::move(v)) {}

  CONSTEXPR UnderlyingType* operator->() { return &value_; }
  CONSTEXPR const UnderlyingType* operator->() const { return &value_; }

  CONSTEXPR UnderlyingType& operator*() & { return value_; }
  CONSTEXPR const UnderlyingType& operator*() const& { return value_; }
  CONSTEXPR UnderlyingType&& operator*() && { return std::move(value_); }
  CONSTEXPR const UnderlyingType&& operator*() const&& {
    return std::move(value_);
  }

  CONSTEXPR UnderlyingType& value() & { return value_; }
  CONSTEXPR const UnderlyingType& value() const& { return value_; }
  CONSTEXPR UnderlyingType&& value() && { return std::move(value_); }
  CONSTEXPR const UnderlyingType&& value() const&& { return std::move(value_); }

  CONSTEXPR explicit operator const UnderlyingType&() const& { return value_; }

  CONSTEXPR bool operator==(const StrongAlias& other) const {
    return value_ == other.value_;
  }
  CONSTEXPR bool operator!=(const StrongAlias& other) const {
    return value_ != other.value_;
  }
  CONSTEXPR bool operator<(const StrongAlias& other) const {
    return value_ < other.value_;
  }
  CONSTEXPR bool operator<=(const StrongAlias& other) const {
    return value_ <= other.value_;
  }
  CONSTEXPR bool operator>(const StrongAlias& other) const {
    return value_ > other.value_;
  }
  CONSTEXPR bool operator>=(const StrongAlias& other) const {
    return value_ >= other.value_;
  }

 protected:
  UnderlyingType value_;
};

}  // namespace webrtc

#endif  // RTC_BASE_STRONG_ALIAS_H_
