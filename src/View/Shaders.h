//
// Created by Jonathan Richard on 2023-08-09.
//

#ifndef INFORMATICHATS_SHADERS_H
#define INFORMATICHATS_SHADERS_H

#include "GL/glew.h"
#include "pxlui/ShaderProgram.h"

static const char *VERT_SHADER =
        R"(#version 300 es

precision highp float;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTexIndex;

out vec4 vColor;
out vec2 vTexCoord;
out float vTexIndex;

void main(void) {
    vColor = aColor;
    vTexCoord = aTexCoord;
    vTexIndex = aTexIndex;
    gl_Position = vec4(aPosition, 1.);
}
)";

static const char *FRAG_SHADER = R"(#version 300 es

#define PI 3.1415926535897932384626433832795

precision highp float;

uniform sampler2D uTextures[16];
uniform float uBlur;
uniform float uCurvature;
uniform float uChroma;
uniform float uScanlineWidth;
uniform float uScanlineIntensity;
uniform float uVignette;
uniform bool uCrtEnabled;
uniform float iTime;
uniform vec2 iResolution;
uniform float iResFactor;

layout(location = 0) out vec4 oColor;

in vec4 vColor;
in vec2 vTexCoord;
in float vTexIndex;


vec2 curve(vec2 uv) {
  uv = (uv - 0.5) * 2.0;
  uv *= 1.1;
  uv.x *= 1.0 + pow((abs(uv.y) * uCurvature), 2.0);
  uv.y *= 1.0 + pow((abs(uv.x) * uCurvature), 2.0);
  uv /= 1.1;
  uv = (uv / 2.0) + 0.5;
  return uv;
}

vec4 sampleTexture(vec2 uv) {

    int index = int(vTexIndex);

    vec4 color; // Color variable to store the sampled color

    switch (index) {
        case 0:
            return texture(uTextures[0], uv);
            break;
        case 1:
            color = texture(uTextures[1], uv);
            break;
        case 2:
            color = texture(uTextures[2], uv);
            break;
        case 3:
            color = texture(uTextures[3], uv);
            break;
        case 4:
            color = texture(uTextures[4], uv);
            break;
        case 5:
            color = texture(uTextures[5], uv);
            break;
        case 6:
            color = texture(uTextures[6], uv);
            break;
        case 7:
            color = texture(uTextures[7], uv);
            break;
        case 8:
            color = texture(uTextures[8], uv);
            break;
        case 9:
            color = texture(uTextures[9], uv);
            break;
        case 10:
            color = texture(uTextures[10], uv);
            break;
        case 11:
            color = texture(uTextures[11], uv);
            break;
        case 12:
            color = texture(uTextures[12], uv);
            break;
        case 13:
            color = texture(uTextures[13], uv);
            break;
        case 14:
            color = texture(uTextures[14], uv);
            break;
        case 15:
            color = texture(uTextures[15], uv);
            break;
        default:
            color = vec4(0.0, 0.0, 0.0, 1.0); // Default color if index is out of range
            break;
    }
    return color;
}

ivec2 getTextureSize() {

    int index = int(vTexIndex);

    vec4 color; // Color variable to store the sampled color

    // Switch statement based on the index value
    switch (index) {
        case 0:
            return textureSize(uTextures[0], 0).xy;
            break;
        case 1:
            return textureSize(uTextures[1], 0).xy;
            break;
        case 2:
            return textureSize(uTextures[2], 0).xy;
            break;
        case 3:
            return textureSize(uTextures[3], 0).xy;
            break;
        case 4:
            return textureSize(uTextures[4], 0).xy;
            break;
        case 5:
            return textureSize(uTextures[5], 0).xy;
            break;
        case 6:
            return textureSize(uTextures[6], 0).xy;
            break;
        case 7:
            return textureSize(uTextures[7], 0).xy;
            break;
        case 8:
            return textureSize(uTextures[8], 0).xy;
            break;
        case 9:
            return textureSize(uTextures[9], 0).xy;
            break;
        case 10:
            return textureSize(uTextures[10], 0).xy;
            break;
        case 11:
            return textureSize(uTextures[11], 0).xy;
            break;
        case 12:
            return textureSize(uTextures[12], 0).xy;
            break;
        case 13:
            return textureSize(uTextures[13], 0).xy;
            break;
        case 14:
            return textureSize(uTextures[14], 0).xy;
            break;
        case 15:
            return textureSize(uTextures[15], 0).xy;
            break;
        default:
            return ivec2(100, 100);
            break;
    }
}

float density = 1.3;
float opacityScanline = .3;
float opacityNoise = .2;
float flickering = 0.03;

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float blend(const in float x, const in float y) {
	return (x < 0.5) ? (2.0 * x * y) : (1.0 - 2.0 * (1.0 - x) * (1.0 - y));
}

vec3 blend(const in vec3 x, const in vec3 y, const in float opacity) {
	vec3 z = vec3(blend(x.r, y.r), blend(x.g, y.g), blend(x.b, y.b));
	return z * opacity + x * (1.0 - opacity);
}

vec4 scanlines(in vec2 fragCoord, in vec2 iResolution, in float iTime, in vec4 icol)
{
    vec2 uv = fragCoord/iResolution.xy;
    vec3 col = icol.rgb;//sampleTexture(uv).rgb;

    float count = iResolution.y * density;
    vec2 sl = vec2(sin(uv.y * count), cos(uv.y * count));
	vec3 scanlines = vec3(sl.x, sl.y, sl.x);

    col += col * scanlines * opacityScanline;
//    col += col * vec3(random(uv * iTime)) * opacityNoise;
//    col += col * sin(110.0 * iTime) * flickering;

    return vec4(col,1.0);
}

void main() {

    if (uCrtEnabled)
    {
        ivec2 size = getTextureSize();
//        vec2 iResolution = vec2(float(size.x)/2., float(size.y)/2.);//vec2(640.0, 360.0);
        vec2 q = vTexCoord;
        vec2 fragCoord = vTexCoord;
        vec2 uv = gl_FragCoord.xy/iResolution.xy;
        uv = curve(uv);

        // Outside of range is black
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            oColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }

        vec4 col = vec4(1.);

        // Chromatic aberration
        // col = texture(wTexture, uv.xy);
        col.r = 0.7 * sampleTexture(vec2(uv.x + 0.001 * uChroma, uv.y + 0.001 * uChroma)).r;
        col.g = 0.7 * sampleTexture(vec2(uv.x + 0.000 * uChroma, uv.y - 0.002 * uChroma)).g;
        col.b = 0.7 * sampleTexture(vec2(uv.x - 0.002 * uChroma, uv.y + 0.000 * uChroma)).b;

        // Blur
        col += 0.05 * sampleTexture(vec2(uv.x - 2.0 * uBlur / iResolution.x, uv.y));
        col += 0.10 * sampleTexture(vec2(uv.x - 1.0 * uBlur / iResolution.x, uv.y));
        col += 0.10 * sampleTexture(vec2(uv.x + 1.0 * uBlur / iResolution.x, uv.y));
        col += 0.05 * sampleTexture(vec2(uv.x + 2.0 * uBlur / iResolution.x, uv.y));

        // Vignette
        col *= pow(16.0 * uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y), uVignette);

//        float scanline = floor(sin(gl_FragCoord.y * PI * iResFactor * 2.0));
        // Scanlines
//        col *= 1.0 - scanline*0.2;
        col *= clamp(1.0 + uScanlineWidth * sin(uv.y * iResolution.y * PI), 1.0 - uScanlineIntensity, 1.0);
        oColor = col;
        return;
    }
    else
    {
        vec4 col = sampleTexture(vTexCoord) * vColor;
        oColor = col;
        return;
    }
}
)";

#endif//INFORMATICHATS_SHADERS_H
