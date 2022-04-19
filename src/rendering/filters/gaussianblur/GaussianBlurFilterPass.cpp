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

#include "GaussianBlurShaders.h"
#include "GaussianBlurFilterPass.h"

namespace pag {


GaussianBlurFilterPass::GaussianBlurFilterPass(BlurOptions options) : options(options) {
}

std::string GaussianBlurFilterPass::onBuildFragmentShader() {
  return BLUR_FRAGMENT_SHADER;
}

void GaussianBlurFilterPass::onPrepareProgram(tgfx::Context* context, unsigned int program) {
  auto gl = tgfx::GLFunctions::Get(context);
  blurrinessHandle = gl->getUniformLocation(program, "uBlurriness");
  repeatEdgeHandle = gl->getUniformLocation(program, "uRepeatEdge");
  specifiedColorHandle = gl->getUniformLocation(program, "uSpecifiedColor");
}

void GaussianBlurFilterPass::updateParams(float blurValue, tgfx::Color blurColor) {
  blurriness = blurValue;
  specifiedColor = blurColor;
}

void GaussianBlurFilterPass::onUpdateParams(tgfx::Context* context, const tgfx::Rect& contentBounds,
                                          const tgfx::Point& filterScale) {
  auto gl = tgfx::GLFunctions::Get(context);
  gl->uniform1f(repeatEdgeHandle, (options & BlurOptions::RepeatEdgePixels) != BlurOptions::None);
  gl->uniform4f(specifiedColorHandle, specifiedColor.red, specifiedColor.green, specifiedColor.blue,
                                      specifiedColor.alpha);
}

std::vector<tgfx::Point> GaussianBlurFilterPass::computeVertices(const tgfx::Rect& inputBounds,
                                                                 const tgfx::Rect& outputBounds,
                                                                 const tgfx::Point& filterScale) {
  if ((options & BlurOptions::RepeatEdgePixels) != BlurOptions::None) {
    return LayerFilter::computeVertices(inputBounds, outputBounds, filterScale);
  }
  std::vector<tgfx::Point> vertices = {};
  tgfx::Point contentPoint[4] = {{outputBounds.left, outputBounds.bottom},
                                 {outputBounds.right, outputBounds.bottom},
                                 {outputBounds.left, outputBounds.top},
                                 {outputBounds.right, outputBounds.top}};

  auto deltaX = direction == BlurDirection::Horizontal ? -blurriness * filterScale.x : 0;
  auto deltaY = direction == BlurDirection::Vertical ? -blurriness * filterScale.y : 0;

  tgfx::Point texturePoints[4] = {
      {deltaX, (outputBounds.height() + deltaY)},
      {(outputBounds.width() + deltaX), (outputBounds.height() + deltaY)},
      {deltaX, deltaY},
      {(outputBounds.width() + deltaX), deltaY}};
  for (int ii = 0; ii < 4; ii++) {
    vertices.push_back(contentPoint[ii]);
    vertices.push_back(texturePoints[ii]);
  }
  return vertices;
}
}  // namespace pag
