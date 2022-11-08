/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef API_UNITS_FREQUENCY_H_
#define API_UNITS_FREQUENCY_H_

#ifdef WEBRTC_UNIT_TEST
#include <ostream>  // no-presubmit-check TODO(webrtc:8982)
#endif              // WEBRTC_UNIT_TEST

#include <cstdlib>
#include <limits>
#include <string>
#include <type_traits>

#include "api/units/time_delta.h"
#include "rtc_base/units/unit_base.h"

namespace webrtc {

class Frequency final : public rtc_units_impl::RelativeUnit<Frequency> {
 public:
  template <typename T>
  static CONSTEXPR Frequency MilliHertz(T value) {
    static_assert(std::is_arithmetic<T>::value, "");
    return FromValue(value);
  }
  template <typename T>
  static CONSTEXPR Frequency Hertz(T value) {
    static_assert(std::is_arithmetic<T>::value, "");
    return FromFraction(1000, value);
  }
  template <typename T>
  static CONSTEXPR Frequency KiloHertz(T value) {
    static_assert(std::is_arithmetic<T>::value, "");
    return FromFraction(1000000, value);
  }

  Frequency() = delete;

  template <typename T = int64_t>
  CONSTEXPR T hertz() const {
    return ToFraction<1000, T>();
  }
  template <typename T = int64_t>
  CONSTEXPR T millihertz() const {
    return ToValue<T>();
  }

 private:
  friend class rtc_units_impl::UnitBase<Frequency>;
  using RelativeUnit::RelativeUnit;
  static CONSTEXPR bool one_sided; // = true;
};

inline CONSTEXPR Frequency operator/(int64_t nominator,
                                     const TimeDelta& interval) {
  CONSTEXPR int64_t kKiloPerMicro = 1000 * 1000000;
  RTC_DCHECK_LE(nominator, std::numeric_limits<int64_t>::max() / kKiloPerMicro);
  RTC_CHECK(interval.IsFinite());
  RTC_CHECK(!interval.IsZero());
  return Frequency::MilliHertz(nominator * kKiloPerMicro / interval.us());
}

inline CONSTEXPR TimeDelta operator/(int64_t nominator,
                                     const Frequency& frequency) {
  CONSTEXPR int64_t kMegaPerMilli = 1000000 * 1000;
  RTC_DCHECK_LE(nominator, std::numeric_limits<int64_t>::max() / kMegaPerMilli);
  RTC_CHECK(frequency.IsFinite());
  RTC_CHECK(!frequency.IsZero());
  return TimeDelta::Micros(nominator * kMegaPerMilli / frequency.millihertz());
}

inline CONSTEXPR double operator*(Frequency frequency, TimeDelta time_delta) {
  return frequency.hertz<double>() * time_delta.seconds<double>();
}
inline CONSTEXPR double operator*(TimeDelta time_delta, Frequency frequency) {
  return frequency * time_delta;
}

std::string ToString(Frequency value);
inline std::string ToLogString(Frequency value) {
  return ToString(value);
}

#ifdef WEBRTC_UNIT_TEST
inline std::ostream& operator<<(  // no-presubmit-check TODO(webrtc:8982)
    std::ostream& stream,         // no-presubmit-check TODO(webrtc:8982)
    Frequency value) {
  return stream << ToString(value);
}
#endif  // WEBRTC_UNIT_TEST

}  // namespace webrtc
#endif  // API_UNITS_FREQUENCY_H_
