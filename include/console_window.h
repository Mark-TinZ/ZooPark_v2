#pragma once
#include "window.h"
#include "console.h"

class ConsoleWindow : public BaseWindow {
public: 
	bool open = true;

	void Render() override;
};