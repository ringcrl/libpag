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
  
  repeatEdgePixels = blurEffect->repeatEdgePixels->getValueAt(0);
  blurDimensions = blurEffect->blurDimensions->getValueAt(0);
  
  BlurOptions options = BlurOptions::None;
  
  if (repeatEdgePixels) {
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
  delete upBlurPass;
  delete downBlurPass;
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

void GaussianBlurFilter::update(Frame frame, const tgfx::Rect& contentBounds,
                             const tgfx::Rect& transformedBounds, const tgfx::Point& filterScale) {
  LayerFilter::update(frame, contentBounds, transformedBounds, filterScale);

  auto* blurEffect = static_cast<FastBlurEffect*>(effect);
  blurriness = blurEffect->blurriness->getValueAt(layerFrame);
  auto expandY = blurriness * filterScale.y;
  filtersBounds.clear();
  filtersBounds.emplace_back(contentBounds);
  switch (blurDimensions) {
    case BlurDimensionsDirection::Vertical:
      downBlurPass->update(frame, contentBounds, transformedBounds, filterScale);
      upBlurPass->update(frame, contentBounds, transformedBounds, filterScale);
      break;
    case BlurDimensionsDirection::Horizontal:
      blurFilterH->update(frame, contentBounds, transformedBounds, filterScale);
      break;
    case BlurDimensionsDirection::All:
      auto blurVBounds = contentBounds;
      if (!repeatEdge) {
        blurVBounds.outset(0, expandY);
        blurVBounds.roundOut();
      }
      filtersBounds.emplace_back(blurVBounds);
      blurFilterV->update(frame, contentBounds, blurVBounds, filterScale);
      blurFilterH->update(frame, blurVBounds, transformedBounds, filterScale);
      break;
  }
  filtersBounds.emplace_back(transformedBounds);
}

void GaussianBlurFilter::draw(tgfx::Context* context, const FilterSource* source,
                           const FilterTarget* target) {
  if (source == nullptr || target == nullptr) {
    LOGE("GaussFilter::draw() can not draw filter");
    return;
  }
  switch (blurDirection) {
    case BlurDirection::Vertical:
      blurFilterV->updateParams(blurriness, 1.0, repeatEdge, BlurMode::Picture);
      blurFilterV->draw(context, source, target);
      break;
    case BlurDirection::Horizontal:
      blurFilterH->updateParams(blurriness, 1.0, repeatEdge, BlurMode::Picture);
      blurFilterH->draw(context, source, target);
      break;
    case BlurDirection::Both:
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
      break;
  }
}
}  // namespace pag
