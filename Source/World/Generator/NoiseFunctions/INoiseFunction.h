#pragma once

class INoiseFunction {
	virtual float Sample(double x, double y, double z) { return 0.0f; };
};
