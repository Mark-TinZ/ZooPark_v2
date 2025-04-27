#pragma once
#include "window.h"
#include "console.h"
#include "main_window.h"
#include <string>

class CheatWindow : public BaseWindow {
private:
	bool cheat_mode_active = false;
public: 
	bool open = false;

	void Render() override;
	bool getActiveCheatMode();
};