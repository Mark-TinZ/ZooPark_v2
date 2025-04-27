#pragma once
#include "window.h"

class StatisticsWindow : public BaseWindow {
public:
	bool open = false;

	void Render() override;
};
