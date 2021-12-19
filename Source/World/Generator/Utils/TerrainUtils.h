#pragma once
#include <cmath>
#include "../../../External/glm/glm.hpp"
#include "../../../External/glm/vec3.hpp"

//Terraces : https://www.desmos.com/calculator/swkpbrq1xy

namespace TUtils {

static double saturate (double x) {
	return glm::clamp(x, 0.0, 1.0);
}

static double lerp(double x, double a, double b) {
	return a + (b - a) * x;
}

static double lerpClamp(double x, double a, double b) {
	return saturate(lerp(x, a, b));
}

static double invLerp (double x, double a, double b) {
	return (x - a) / (b - a);
}

static double invLerpClamp (double x, double a, double b) {
	return saturate(invLerp(x, a, b));
}


static double modulo(double x, double range) {
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

static double terraceStep1(double x) {
	return abs(x - 0.5) - 0.5;
}

static double terraceStep2(double x) {
	double offsetX = x - 0.5;
	return (offsetX * offsetX) - 0.25;
}

static double terraceStep3(double x) {
	double offsetX = x - 0.5;
	return (0.25 < x && x < 0.75) ?
		(2 * (offsetX * offsetX) - 0.375) :
		terraceStep1(x);
}

static double smoothAbs (double x) {
	x = abs(x);
	return 3 * x * x - 2 * x * x * x;
}

static double terrace (double x, double scale, double offset, double gap) {
	return scale *
		terraceStep3(
			saturate(
				modulo(
					x / scale + offset,
					1 + abs(gap)
				))) + x;
}

}