#pragma once

class IRenderable {
public:
	virtual ~IRenderable() {}
	virtual void Render() {};
};
