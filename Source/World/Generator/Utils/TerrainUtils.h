#pragma once
#include <cmath>
#include "../../../External/glm/glm.hpp"
#include "../../../External/glm/vec3.hpp"

//Terraces : https://www.desmos.com/calculator/swkpbrq1xy


namespace TUtils {

static float saturate (float x) {
	return glm::clamp(x, 0.0f, 1.0f);
}

static float lerp(float x, float a, float b) {
	return a + (b - a) * x;
}

static float lerpClamp(float x, float a, float b) {
	return saturate(lerp(x, a, b));
}

static float smoothClamp(float x) {
	x = saturate(x);
	return 3 * x * x - 2 * x * x * x;
}

static float invLerp (float x, float a, float b) {
	return (x - a) / (b - a);
}

static float invLerpClamp (float x, float a, float b) {
	return saturate(invLerp(x, a, b));
}


static float modulo(float x, float range) {
	return x - floor(x / range) * range;
}

static float flerp(float x, float a, float b) {
	return a + (b - a) * x;
}

static glm::lowp_vec3 vec3lerp(float x, glm::lowp_vec3 a, glm::lowp_vec3 b) {
	return a + (b - a) * x;
}

static glm::lowp_vec3 vec3Bilinear(
	float x, float y, glm::lowp_vec3 v00, glm::lowp_vec3 v01,
	glm::lowp_vec3 v10, glm::lowp_vec3 v11) {
	return vec3lerp(y, vec3lerp(x, v00, v10), vec3lerp(x, v01, v11));
}

static float fBilinear(
	float x, float y, float v00, float v01, float v10, float v11) {
	return flerp(y, flerp(x, v00, v10), flerp(x, v01, v11));
}

static float terraceStep1(float x) {
	return abs(x - 0.5) - 0.5;
}

static float terraceStep2(float x) {
	float offsetX = x - 0.5;
	return (offsetX * offsetX) - 0.25;
}

static float terraceStep3(float x) {
	float offsetX = x - 0.5;
	return (0.25 < x && x < 0.75) ?
		(2 * (offsetX * offsetX) - 0.375) :
		terraceStep1(x);
}

static float smoothAbs (double x) {
	x = abs(x);
	return 3 * x * x - 2 * x * x * x;
}

static float terrace (float x, float scale, float offset, float gap) {
	return scale *
		terraceStep3(
			saturate(
				modulo(
					x / scale + offset,
					1 + abs(gap)
				))) + x;
}

static float crater(float x, float thick, float offset) {
	float value = glm::max(0.0f, 1 - thick * abs(x - offset));
	return value * value;
}

static float craterExp(float x, float thick, float offset) {
	return glm::exp2(-abs(thick * (x - offset)));
}

static float craterDepth(float x, float thick, float offset, float depth) {
	float depthFade = smoothClamp(-thick * (x - offset) + 0.5f);
	return lerp(depthFade, depth, 0.0f) + craterExp(x, thick, offset);
}


}