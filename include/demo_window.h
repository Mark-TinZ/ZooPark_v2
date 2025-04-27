#pragma once
#include "window.h"

class DemoWindow : public BaseWindow {
private:
	bool open = false;
public:
	void Open();
	
	void Render() override;
};