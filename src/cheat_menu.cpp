#include "cheat_menu.h"
// #include <cstring>

void CheatWindow::Render() {
	Zoo* zoo = getZoo();
	if (open) {
		if (!cheat_mode_active) {
			cheat_mode_active = true;
			ConsoleCout() << "Санбой: Фу какой дурной поступок. Я запомнил твой какашечный поступок." << std::endl;
		}
		ImGui::Begin("Читменю", &open);
		if (!zoo) {
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Зоопарк\nпока не создан");
			ImGui::End();
			return;
		}
		ImGui::Text("Чит меню:");
		ImGui::InputText("Название зоопарка", &zoo->name[0], 128);
		ImGui::InputInt("Деньги", &zoo->money);
		ImGui::End();
	}
}

bool CheatWindow::getActiveCheatMode() {
	return cheat_mode_active;
}