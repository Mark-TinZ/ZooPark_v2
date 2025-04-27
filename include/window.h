#pragma once
#include "imgui.h"

class BaseWindow {
public:
	// Деструктор реализуется по умолчанию
	virtual ~BaseWindow() = default; // Магия компилятора.
	virtual void Render() = 0;
};

void HelpMarker(const char*);
