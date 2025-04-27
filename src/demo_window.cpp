#include "demo_window.h"

void DemoWindow::Open() { open = true; }

void DemoWindow::Render() {
	if (open) {
		ImGui::ShowDemoWindow(&open);  // Указываем состояние для закрытия
	}
}