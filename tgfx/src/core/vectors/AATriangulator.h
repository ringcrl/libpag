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

#include "Triangulator.h"

namespace tgfx {
// Triangulates the given path in device space with a mesh of alpha ramps for antialiasing.
class AATriangulator : private Triangulator {
 public:
  static std::vector<float> PathToAATriangles(const Path& path, float tolerance, const Rect& clipBounds) {
    AATriangulator aaTriangulator(path);
    aaTriangulator.fRoundVerticesToQuarterPixel = true;
    aaTriangulator.fEmitCoverage = true;
    bool isLinear;
    auto [ polys, success ] = aaTriangulator.pathToPolys(tolerance, clipBounds, &isLinear);
    if (!success) {
      return {};
    }
    return aaTriangulator.polysToAATriangles(polys);
  }

  // Structs used by GrAATriangulator internals.
  struct SSEdge;
  struct EventList;
  struct Event {
    Event(SSEdge* edge, const Point& point, uint8_t alpha)
        : fEdge(edge), fPoint(point), fAlpha(alpha) {}
    SSEdge* fEdge;
    Point fPoint;
    uint8_t fAlpha;
    void apply(VertexList* mesh, const Comparator&, EventList* events, AATriangulator*);
  };
  struct EventComparator {
    enum class Op { kLessThan, kGreaterThan };
    EventComparator(Op op) : fOp(op) {}
    bool operator() (Event* const &e1, Event* const &e2) {
      return fOp == Op::kLessThan ? e1->fAlpha < e2->fAlpha
                                  : e1->fAlpha > e2->fAlpha;
    }
    Op fOp;
  };

 private:
  AATriangulator(const Path& path) : Triangulator(path) {}

  // For screenspace antialiasing, the algorithm is modified as follows:
  //
  // Run steps 1-5 above to produce polygons.
  // 5b) Apply fill rules to extract boundary contours from the polygons:
  void extractBoundary(EdgeList* boundary, Edge* e) const;
  void extractBoundaries(const VertexList& inMesh, VertexList* innerVertices,
                         const Comparator&);

  // 5c) Simplify boundaries to remove "pointy" vertices that cause inversions:
  void simplifyBoundary(EdgeList* boundary, const Comparator&);

  // 5d) Displace edges by half a pixel inward and outward along their normals. Intersect to find
  //     new vertices, and set zero alpha on the exterior and one alpha on the interior. Build a
  //     new antialiased mesh from those vertices:
  void strokeBoundary(EdgeList* boundary, VertexList* innerMesh, const Comparator&);

  // Run steps 3-6 above on the new mesh, and produce antialiased triangles.
  std::tuple<Poly*, bool> tessellate(const VertexList& mesh, const Comparator&) override;
  std::vector<float> polysToAATriangles(Poly*) const;

  // Additional helpers and driver functions.
  void makeEvent(SSEdge*, EventList* events) const;
  void makeEvent(SSEdge*, Vertex* v, SSEdge* other, Vertex* dest, EventList* events,
                 const Comparator&) const;
  void connectPartners(VertexList* mesh, const Comparator&);
  void removeNonBoundaryEdges(const VertexList& mesh) const;
  void connectSSEdge(Vertex* v, Vertex* dest, const Comparator&);
  bool collapseOverlapRegions(VertexList* mesh, const Comparator&, EventComparator comp);

  // FIXME: fOuterMesh should be plumbed through function parameters instead.
  mutable VertexList fOuterMesh;
};
}  // namespace pag
