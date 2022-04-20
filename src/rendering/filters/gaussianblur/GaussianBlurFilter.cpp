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

#include "GaussianBlurFilter.h"
#include "rendering/filters/utils/FilterHelper.h"

namespace pag {
GaussianBlurFilter::GaussianBlurFilter(Effect* effect) : effect(effect) {
  auto* blurEffect = static_cast<FastBlurEffect*>(effect);
  
  blurParam.repeatEdgePixels = blurEffect->repeatEdgePixels->getValueAt(0);
  auto blurDimensions = blurEffect->blurDimensions->getValueAt(0);
  
  BlurOptions options = BlurOptions::None;
  
  if (blurParam.repeatEdgePixels) {
    options |= BlurOptions::RepeatEdgePixels;
  }
  
  if (blurDimensions == BlurDimensionsDirection::Vertical) {
    options |= BlurOptions::Vertical;
  } else if (blurDimensions == BlurDimensionsDirection::Horizontal) {
    options |= BlurOptions::Horizontal;
  } else {
    options |= BlurOptions::Horizontal | BlurOptions::Vertical;
  }
  
  BlurOptions downOptions = options | BlurOptions::Down;
  BlurOptions upOptions = options | BlurOptions::Up;

  downBlurPass = new GaussianBlurFilterPass(downOptions);
  upBlurPass = new GaussianBlurFilterPass(upOptions);
}

GaussianBlurFilter::~GaussianBlurFilter() {
  delete downBlurPass;
  delete upBlurPass;
}

bool GaussianBlurFilter::initialize(tgfx::Context* context) {
  if (!downBlurPass->initialize(context)) {
    return false;
  }
  if (!upBlurPass->initialize(context)) {
    return false;
  }
  return true;
}

void GaussianBlurFilter::updateBlurParam(float blurriness) {
  blurriness = blurriness < BLUR_MODE_MUTI_PASS_SCALE_LIMIT ?
               blurriness : BLUR_MODE_MUTI_PASS_SCALE_LIMIT;
  if (blurriness < BLUR_MODE_GRADUALLY_SCALE_LIMIT) {
    blurParam.blurMode = BlurMode::GraduallyScale;
    blurParam.blurDepth = 1;
    blurParam.blurValue = blurriness;
  } else {
    blurParam.blurMode = BlurMode::MutiPassScale;
    blurParam.blurDepth = 2;
    blurParam.blurValue = blurriness;
  }
}

void GaussianBlurFilter::update(Frame frame, const tgfx::Rect& contentBounds,
                                const tgfx::Rect& transformedBounds, const tgfx::Point& filterScale) {
  LayerFilter::update(frame, contentBounds, transformedBounds, filterScale);

  auto* blurEffect = static_cast<FastBlurEffect*>(effect);
  auto blurriness = blurEffect->blurriness->getValueAt(layerFrame);
  updateBlurParam(blurriness);

  tgfx::Rect expendBounds = {0, 0, 0, 0};
  if (!blurParam.repeatEdgePixels) {
    expendBounds.setWH(blurriness * filterScale.x, blurriness * filterScale.y);
  }
  filtersBounds.clear();
  filtersBounds.emplace_back(contentBounds);
  
  filtersBounds.emplace_back(transformedBounds);
}

void GaussianBlurFilter::draw(tgfx::Context* context, const FilterSource* source,
                           const FilterTarget* target) {
  if (source == nullptr || target == nullptr) {
    LOGE("GaussFilter::draw() can not draw filter");
    return;
  }
  blurFilterV->updateParams(blurriness, 1.0, repeatEdge, BlurMode::Picture);
  auto contentBounds = filtersBounds[0];
  auto blurVBounds = filtersBounds[1];
  auto targetWidth = static_cast<int>(ceilf(blurVBounds.width() * source->scale.x));
  auto targetHeight = static_cast<int>(ceilf(blurVBounds.height() * source->scale.y));
  if (blurFilterBuffer == nullptr || blurFilterBuffer->width() != targetWidth ||
      blurFilterBuffer->height() != targetHeight) {
    blurFilterBuffer = FilterBuffer::Make(context, targetWidth, targetHeight);
  }
  if (blurFilterBuffer == nullptr) {
    return;
  }
  blurFilterBuffer->clearColor();

  auto offsetMatrix =
      tgfx::Matrix::MakeTrans((contentBounds.left - blurVBounds.left) * source->scale.x,
                              (contentBounds.top - blurVBounds.top) * source->scale.y);
  auto targetV = blurFilterBuffer->toFilterTarget(offsetMatrix);
  blurFilterV->draw(context, source, targetV.get());

  auto sourceH = blurFilterBuffer->toFilterSource(source->scale);
  blurFilterH->updateParams(blurriness, 1.0f, repeatEdge, BlurMode::Picture);
  tgfx::Matrix revertMatrix =
      tgfx::Matrix::MakeTrans((blurVBounds.left - contentBounds.left) * source->scale.x,
                              (blurVBounds.top - contentBounds.top) * source->scale.y);
  auto targetH = *target;
  PreConcatMatrix(&targetH, revertMatrix);
  blurFilterH->draw(context, sourceH.get(), &targetH);
}
}  // namespace pag
