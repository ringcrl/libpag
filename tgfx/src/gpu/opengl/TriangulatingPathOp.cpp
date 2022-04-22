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

#include "TriangulatingPathOp.h"
#include "core/Clock.h"
#include "core/vectors/AATriangulator.h"
#include "gpu/DefaultGeometryProcessor.h"

namespace tgfx {
std::unique_ptr<TriangulatingPathOp> TriangulatingPathOp::Make(const Path& path,
                                                               const Rect& clipBounds) {
  return std::unique_ptr<TriangulatingPathOp>(new TriangulatingPathOp(path, clipBounds));
}

std::unique_ptr<GeometryProcessor> TriangulatingPathOp::getGeometryProcessor(const DrawArgs& args) {
  return DefaultGeometryProcessor::Make(args.renderTarget->width(), args.renderTarget->height());
}

std::vector<float> TriangulatingPathOp::vertices(const DrawArgs&) {
  auto start = Clock::Now();
  auto ret = AATriangulator::PathToAATriangles(path, 0.25f, clipBounds);
  printf("Triangulator: %lld\n", Clock::Now() - start);
  return ret;
}
}  // namespace tgfx
