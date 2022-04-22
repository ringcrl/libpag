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

#include "Geometry.h"
#include "core/utils/Log.h"

namespace tgfx {

bool IsFinite(float a) {
  return (a * 0 == 0);
}

static inline bool sk_floats_are_finite(const float array[], int count) {
  float prod = 0;
  for (int i = 0; i < count; ++i) {
    prod *= array[i];
  }
  // At this point, prod will either be NaN or 0
  return prod == 0;   // if prod is NaN, this check will return false
}

static inline bool SkScalarsAreFinite(const float array[], int count) {
  return sk_floats_are_finite(array, count);
}

static bool AreFinite(const Point array[], int count) {
  return SkScalarsAreFinite(&array[0].x, count << 1);
}

/*
 *  "High order approximation of conic sections by quadratic splines"
 *      by Michael Floater, 1993
 */
#define AS_QUAD_ERROR_SETUP                                         \
    float a = fW - 1;                                            \
    float k = a / (4 * (2 + a));                                 \
    float x = k * (fPts[0].x - 2 * fPts[1].x + fPts[2].x);    \
    float y = k * (fPts[0].y - 2 * fPts[1].y + fPts[2].y);

// Limit the number of suggested quads to approximate a conic
#define kMaxConicToQuadPOW2     5

int SkConic::computeQuadPOW2(float tol) const {
  if (tol < 0 || !IsFinite(tol) || !AreFinite(fPts, 3)) {
    return 0;
  }

  AS_QUAD_ERROR_SETUP

  float error = std::sqrt(x * x + y * y);
  int pow2;
  for (pow2 = 0; pow2 < kMaxConicToQuadPOW2; ++pow2) {
    if (error <= tol) {
      break;
    }
    error *= 0.25f;
  }
  // float version -- using ceil gives the same results as the above.
  if ((false)) {
    auto err = std::sqrt(x * x + y * y);
    if (err <= tol) {
      return 0;
    }
    auto tol2 = tol * tol;
    if (tol2 == 0) {
      return kMaxConicToQuadPOW2;
    }
    auto fpow2 = std::log2((x * x + y * y) / tol2) * 0.25f;
    int altPow2 = static_cast<int>(std::ceil(fpow2));
    if (altPow2 != pow2) {
      LOGE("pow2 %d altPow2 %d fbits %g err %g tol %g\n", pow2, altPow2, fpow2, err, tol);
    }
    pow2 = altPow2;
  }
  return pow2;
}

// This was originally developed and tested for pathops: see SkOpTypes.h
// returns true if (a <= b <= c) || (a >= b >= c)
static bool between(float a, float b, float c) {
  return (a - b) * (c - b) <= 0;
}

template <typename T> static inline T SkTAbs(T value) {
  if (value < 0) {
    value = -value;
  }
  return value;
}

static Point* subdivide(const SkConic& src, Point pts[], int level) {
  if (0 == level) {
    memcpy(pts, &src.fPts[1], 2 * sizeof(Point));
    return pts + 2;
  } else {
    SkConic dst[2];
    src.chop(dst);
    const auto startY = src.fPts[0].y;
    auto endY = src.fPts[2].y;
    if (between(startY, src.fPts[1].y, endY)) {
      // If the input is monotonic and the output is not, the scan converter hangs.
      // Ensure that the chopped conics maintain their y-order.
      auto midY = dst[0].fPts[2].y;
      if (!between(startY, midY, endY)) {
        // If the computed midpoint is outside the ends, move it to the closer one.
        float closerY = SkTAbs(midY - startY) < SkTAbs(midY - endY) ? startY : endY;
        dst[0].fPts[2].y = dst[1].fPts[0].y = closerY;
      }
      if (!between(startY, dst[0].fPts[1].y, dst[0].fPts[2].y)) {
        // If the 1st control is not between the start and end, put it at the start.
        // This also reduces the quad to a line.
        dst[0].fPts[1].y = startY;
      }
      if (!between(dst[1].fPts[0].y, dst[1].fPts[1].y, endY)) {
        // If the 2nd control is not between the start and end, put it at the end.
        // This also reduces the quad to a line.
        dst[1].fPts[1].y = endY;
      }
      // Verify that all five points are in order.
//      SkASSERT(between(startY, dst[0].fPts[1].fY, dst[0].fPts[2].fY));
//      SkASSERT(between(dst[0].fPts[1].fY, dst[0].fPts[2].fY, dst[1].fPts[1].fY));
//      SkASSERT(between(dst[0].fPts[2].fY, dst[1].fPts[1].fY, endY));
    }
    --level;
    pts = subdivide(dst[0], pts, level);
    return subdivide(dst[1], pts, level);
  }
}

static inline bool SkScalarsAreFinite(float a, float b) {
  return IsFinite(a) && IsFinite(b);
}

static bool CanNormalize(float dx, float dy) {
  return SkScalarsAreFinite(dx, dy) && (dx || dy);
}

static bool EqualsWithinTolerance(const Point& p1, const Point& p2) {
  return !CanNormalize(p1.x - p2.x, p1.y - p2.y);
}

// Limit the number of suggested quads to approximate a conic
#define kMaxConicToQuadPOW2     5

int SkConic::chopIntoQuadsPOW2(Point pts[], int pow2) const {
  *pts = fPts[0];
  if (pow2 == kMaxConicToQuadPOW2) {  // If an extreme weight generates many quads ...
    SkConic dst[2];
    this->chop(dst);
    // check to see if the first chop generates a pair of lines
    if (EqualsWithinTolerance(dst[0].fPts[1], dst[0].fPts[2]) &&
        EqualsWithinTolerance(dst[1].fPts[0], dst[1].fPts[1])) {
      pts[1] = pts[2] = pts[3] = dst[0].fPts[1];  // set ctrl == end to make lines
      pts[4] = dst[1].fPts[2];
      pow2 = 1;
      goto commonFinitePtCheck;
    }
  }
  subdivide(*this, pts + 1, pow2);
commonFinitePtCheck:
  const int quadCount = 1 << pow2;
  const int ptCount = 2 * quadCount + 1;
  if (!AreFinite(pts, ptCount)) {
    // if we generated a non-finite, pin ourselves to the middle of the hull,
    // as our first and last are already on the first/last pts of the hull.
    for (int i = 1; i < ptCount - 1; ++i) {
      pts[i] = fPts[1];
    }
  }
  return 1 << pow2;
}
}  // namespace pag
