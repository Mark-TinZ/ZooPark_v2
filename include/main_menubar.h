#include "imgui.h"
#include "statistics_window.h"
#include "cheat_menu.h"
#include "console_window.h"
#include "demo_window.h"
#include <stdlib.h>

class MainMenuBar {
private:
	StatisticsWindow& statistic;
	ConsoleWindow& console;
	DemoWindow& demo;
	CheatWindow& cheatmenu;
public:
	MainMenuBar(StatisticsWindow& stat, ConsoleWindow& log, DemoWindow& demowin, CheatWindow& cheat);
	void Render();
};