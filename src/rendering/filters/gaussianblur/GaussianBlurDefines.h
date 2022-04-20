/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Tencent is pleased to support the open source community by making libpag available.
//
//  Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
//  except in compliance with the License. You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  unless required by applicable law or agreed to in writing, software distributed under the
//  license is distributed on an "as is" basis, without warranties or conditions of any kind,
//  either express or implied. see the license for the specific language governing permissions
//  and limitations under the license.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <type_traits>

namespace pag {

#define BLUR_MODE_GRADUALLY_SCALE_LIMIT 40
#define BLUR_MODE_MUTI_PASS_SCALE_LIMIT 300

enum class BlurMode : unsigned {
  None            = 0,
  GraduallyScale  = 1,
  MutiPassScale   = 2
};

struct BlurParam {
  BlurMode blurMode = BlurMode::None;
  float blurValue = 0;
  int blurDepth = 0;
  bool repeatEdgePixels = true;
};

enum class BlurOptions : unsigned {
  None              = 0,
  Up                = 1 << 0,
  Down              = 1 << 1,
  Horizontal        = 1 << 2,
  Vertical          = 1 << 3,
  RepeatEdgePixels  = 1 << 4,
  SpecifiedColor    = 1 << 5,
  SpecifiedAlpha    = 1 << 6
};

BlurOptions operator&(BlurOptions lhs, BlurOptions rhs) {
  return static_cast<BlurOptions>(static_cast<std::underlying_type<BlurOptions>::type>(lhs) &
                                  static_cast<std::underlying_type<BlurOptions>::type>(rhs));
}

BlurOptions operator|(BlurOptions lhs, BlurOptions rhs) {
  return static_cast<BlurOptions>(static_cast<std::underlying_type<BlurOptions>::type>(lhs) |
                                  static_cast<std::underlying_type<BlurOptions>::type>(rhs));
}

BlurOptions& operator|=(BlurOptions& lhs, BlurOptions rhs) {
  lhs = lhs | rhs;
  return lhs;
}

}  // namespace pag
