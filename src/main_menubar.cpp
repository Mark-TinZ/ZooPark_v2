#include "main_menubar.h"

MainMenuBar::MainMenuBar(StatisticsWindow& stat, ConsoleWindow& log, DemoWindow& demowin, CheatWindow& cheat) 
	: statistic(stat), console(log), demo(demowin), cheatmenu(cheat) {}


void MainMenuBar::Render() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Файл")) {
			// if (ImGui::MenuItem("Новая игра")) {}
			// ImGui::Separator();
			if (ImGui::MenuItem("Выход", "Ctrl+Q")) {
				exit(0);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Вид")) {
			ImGui::MenuItem("Меню данных", nullptr, &statistic.open);
			ImGui::MenuItem("Консоль", nullptr, &console.open);
			if (ImGui::MenuItem("Окно демо")) {
				demo.Open();
			}
			ImGui::MenuItem("Читменю", nullptr, &cheatmenu.open);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}