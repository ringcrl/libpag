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

namespace pag {
static const char BLUR_DOWN_FRAGMENT_SHADER[] = R"(
    #version 100
    precision highp float;
    
    varying vec2 vertexColor;
    uniform sampler2D uTextureInput;
    
    uniform float uBlurriness;
    uniform float uRepeatEdge;
    uniform vec4 uSpecifiedColor;

    void main()
    {
        vec4 sum = texture2D(texture, uv) * 4.0;
        sum += texture2D(texture, uv - halfpixel.xy * offset);
        sum += texture2D(texture, uv + halfpixel.xy * offset);
        sum += texture2D(texture, uv + vec2(halfpixel.x, -halfpixel.y) * offset);
        sum += texture2D(texture, uv - vec2(halfpixel.x, -halfpixel.y) * offset);

        vec4 color = sum / 8.0;
    
        //BLUR_SPECIFIED_COLOR_RGB
        //BLUR_SPECIFIED_COLOR_ALPHA
    
        gl_FragColor = color;
    }
    )";

static const char BLUR_SPECIFIED_COLOR_RGB[] = R"(
        color.rgb = uSpecifiedColor.rgb;
    )";

static const char BLUR_SPECIFIED_COLOR_ALPHA[] = R"(
        color.a *= uSpecifiedColor.a;
    )";

}  // namespace pag
