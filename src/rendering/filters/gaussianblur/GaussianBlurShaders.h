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
static const char BLUR_FRAGMENT_SHADER[] = R"(
    #version 100
    precision highp float;
    uniform sampler2D uTextureInput;
    uniform float uRadius;
    uniform vec2 uLevel;
    uniform float uRepeatEdge;

    uniform vec3 uColor;
    uniform float uColorValid;
    uniform float uAlpha;

    varying vec2 vertexColor;

    float maxEdge = -999.0;
    float edge = 0.005;

    float Curve (float value) {
        value = value * 2.0 - 1.0;
        return dot(vec3(-value, 2.0, 1.0),vec3(abs(value), value, 1.0)) * 0.5;
    }

    void main()
    {
        vec4 color = vec4(0.0);
        vec4 sum = vec4(0.0);

        float divisor = 0.0;
        float weight = 0.0;
        
        vec2 point = vec2(0.0);
        vec2 isVaild = vec2(0.0);

        float radiusMultiplier = 1.0 / uRadius;

        for (float i = 0.0; i < 1000.0; i++)
        {
            float value = i - uRadius;
            if (value > uRadius) {
                break;
            }
            point = vertexColor + value * uLevel;
            vec2 target = clamp(point, edge + maxEdge * (1.0 - uRepeatEdge), 1.0 - edge + (1.0 - maxEdge) * (1.0 - uRepeatEdge));
            color = texture2D(uTextureInput, target);
            isVaild = abs(step(vec2(1.0), point) - vec2(1.0)) * step(vec2(0.0), point);
            color *= step(1.0, isVaild.x * isVaild.y + uRepeatEdge);
            weight = Curve(1.0 - (abs(value) * radiusMultiplier));
            sum += color * weight;
            divisor += weight;
        }

        color = sum / divisor;
        gl_FragColor = vec4(mix(color.rgb, uColor * color.a, uColorValid), color.a) * uAlpha;
    }
    )";
}  // namespace pag
