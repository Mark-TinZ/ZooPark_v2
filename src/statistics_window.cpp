#include "statistics_window.h"
#include "main_window.h"
#include "zoo.h"

void StatisticsWindow::Render() {
	Zoo* zoo = getZoo();
	if (open) {
		ImGui::Begin("Данные", &open);
		if (!zoo) {
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Зоопарк\nпока не создан");
			ImGui::End();
			return;
		}
		ImGui::Text("Статистика");
		ImGui::Separator();
		// Отображение статистики
		ImGui::Text("Дни: %d", zoo->day);
		ImGui::SameLine(); HelpMarker("Количество прожитых дней");
		ImGui::Text("Деньги: %d", zoo->money);
		ImGui::SameLine(); HelpMarker(
			"Деньги вы можете потратить на покупку товаров\n"
			"Зарабатывать деньги можно с поситителей зоопарка"
		);
		ImGui::Text("Еда: %d", zoo->food);
		ImGui::SameLine(); HelpMarker("Еда требуется животным. Без еды животные будут умирать.");
		ImGui::Text("Популярность: %d", zoo->popularity);
		ImGui::SameLine(); HelpMarker(
			"Популярность зоопарка зависит от вас!\n"
			"На популярность влияют множество факторов:\n"
			"Достаток еды, посетители, животные и др."
		);
		ImGui::Text("Животные: %zu", zoo->animals.size());
		ImGui::SameLine(); HelpMarker("Общее количество всех ваших животных.");
		ImGui::Text("Посетители: %d", zoo->getVisitors());
		ImGui::SameLine(); HelpMarker("Посетители приносят деньги вашему зоопарку.");
		ImGui::Text("Вольеры: %zu", zoo->enclosures.size());
		ImGui::SameLine(); HelpMarker("Общее кол-во вольеров.");
		ImGui::Text("Работники: %zu", zoo->workers.size());
		ImGui::SameLine(); HelpMarker("Общее кол-во работников.");

		ImGui::Separator();
		// Кнопка следующий день
		if (ImGui::Button("Следующий день", ImVec2(0, 40))) {zoo->nextDay();}
		ImGui::End();
	}
}