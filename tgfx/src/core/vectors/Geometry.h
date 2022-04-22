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

#include "core/Point.h"
#include "core/Rect.h"
#include "core/Matrix.h"

namespace tgfx {
bool IsFinite(float a);

struct SkConic {
  SkConic() {}
  SkConic(const Point& p0, const Point& p1, const Point& p2, float w) {
    fPts[0] = p0;
    fPts[1] = p1;
    fPts[2] = p2;
    fW = w;
  }
  SkConic(const Point pts[3], float w) {
    memcpy(fPts, pts, sizeof(fPts));
    fW = w;
  }

  Point  fPts[3];
  float fW;

  void set(const Point pts[3], float w) {
    memcpy(fPts, pts, 3 * sizeof(Point));
    fW = w;
  }

  void set(const Point& p0, const Point& p1, const Point& p2, float w) {
    fPts[0] = p0;
    fPts[1] = p1;
    fPts[2] = p2;
    fW = w;
  }

  /**
     *  Given a t-value [0...1] return its position and/or tangent.
     *  If pos is not null, return its position at the t-value.
     *  If tangent is not null, return its tangent at the t-value. NOTE the
     *  tangent value's length is arbitrary, and only its direction should
     *  be used.
   */
  void evalAt(float t, Point* pos, Point* tangent = nullptr) const;
  bool chopAt(float t, SkConic dst[2]) const;
  void chopAt(float t1, float t2, SkConic* dst) const;
  void chop(SkConic dst[2]) const;

  Point evalAt(float t) const;
  Point evalTangentAt(float t) const;

  void computeAsQuadError(Point* err) const;
  bool asQuadTol(float tol) const;

  /**
     *  return the power-of-2 number of quads needed to approximate this conic
     *  with a sequence of quads. Will be >= 0.
   */
  int computeQuadPOW2(float tol) const;

  /**
     *  Chop this conic into N quads, stored continguously in pts[], where
     *  N = 1 << pow2. The amount of storage needed is (1 + 2 * N)
   */
  int chopIntoQuadsPOW2(Point pts[], int pow2) const;

  enum {
    kMaxConicsForArc = 5
  };
};

/**
 *  Help class to allocate storage for approximating a conic with N quads.
 */
class SkAutoConicToQuads {
 public:
  SkAutoConicToQuads() : fQuadCount(0) {}

  /**
     *  Given a conic and a tolerance, return the array of points for the
     *  approximating quad(s). Call countQuads() to know the number of quads
     *  represented in these points.
     *
     *  The quads are allocated to share end-points. e.g. if there are 4 quads,
     *  there will be 9 points allocated as follows
     *      quad[0] == pts[0..2]
     *      quad[1] == pts[2..4]
     *      quad[2] == pts[4..6]
     *      quad[3] == pts[6..8]
   */
  const Point* computeQuads(const SkConic& conic, float tol) {
    int pow2 = conic.computeQuadPOW2(tol);
    fQuadCount = 1 << pow2;
    auto* pts = new Point[1 + 2 * fQuadCount];
    fQuadCount = conic.chopIntoQuadsPOW2(pts, pow2);
    return pts;
  }

  const Point* computeQuads(const Point pts[3], float weight,
                              float tol) {
    SkConic conic;
    conic.set(pts, weight);
    return computeQuads(conic, tol);
  }

  int countQuads() const { return fQuadCount; }

 private:
  enum {
    kQuadCount = 8, // should handle most conics
    kPointCount = 1 + 2 * kQuadCount,
  };
  int fQuadCount; // #quads for current usage
};

}  // namespace pag
