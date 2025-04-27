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
		static char zooNameBuffer[128];
		std::string zooName = zoo->getZooName();
		for (size_t i = 0; i < sizeof(zooNameBuffer) - 1 && i < zooName.size(); ++i) {
			zooNameBuffer[i] = zooName[i];
		}
		zooNameBuffer[std::min(sizeof(zooNameBuffer) - 1, zooName.size())] = '\0';
		if (ImGui::InputText("Название зоопарка", zooNameBuffer, sizeof(zooNameBuffer))) {
			zoo->setZooName(std::string(zooNameBuffer));
		}
		ImGui::InputInt("Деньги", &zoo->money);
		ImGui::End();
	}
}

bool CheatWindow::getActiveCheatMode() {
	return cheat_mode_active;
}