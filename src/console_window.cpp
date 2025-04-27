#include "console_window.h"

void ConsoleWindow::Render() {
	if (open) {
		ImGui::SetNextWindowDockID(ImGui::GetID("ConsoleDock"), ImGuiCond_FirstUseEver);
		ImGui::Begin("Консоль", &open);
		if (ImGui::Button("Очистить")) {
			ConsoleCout::ClearConsole();
		}
		ImGui::SameLine(); HelpMarker(
			"Тут будет выводиться вся информация, которая связана с парком.\n"
			"Кнопкой \"Отчистить\" можно отчистить консоль."
		);
		ImGui::Separator();
		ImGui::BeginChild("logs");
		
		for (const auto& log : ConsoleCout::GetLogs()) {
			// ImGui::TextUnformatted(log.c_str());
			ImGui::TextWrapped(log.c_str());
		}
		// Автоматическая прокрутка вниз
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
		ImGui::EndChild();
		ImGui::End();
	}
}
