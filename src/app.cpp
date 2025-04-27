#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "window.h"
#include "zoo.h"
#include "main_window.h"
#include "console_window.h"
#include "statistics_window.h"
#include "main_menubar.h"
#include "modal_window.h"
#include "cheat_menu.h"

#include <GLFW/glfw3.h>


class App {
private:
	GLFWwindow* window;
	MainMenuBar menu;
	MainWindow mainWindow;
	ModalWindow modal;
	DemoWindow demo;
	StatisticsWindow statistic;
	ConsoleWindow console;
	CheatWindow cheatmenu;

public:
	App(GLFWwindow* win) 
		:	window(win), 
			menu(statistic, console, demo, cheatmenu), 
			mainWindow(modal, images), 
			modal(statistic, scenes) 
			{
				glfwWindowShouldClose(window);
			}

	void SetupDockspace() {
		// Устанавливаем параметры главного окна с докспейсом
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		
		// Получаем размеры и позицию основного видового окна
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		// Начинаем полноэкранное окно с dockspace
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpaceWindow", nullptr, window_flags); // Окно затычка нужна чисто ради объединения окон
		ImGui::PopStyleVar(3);

		// Создаем DockSpace
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

		static bool first_time = true;
		if (first_time) {
			first_time = false; // Выполняем настройку только один раз

			ImGui::DockBuilderRemoveNode(dockspace_id); // Очищаем предыдущий докинг
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None); // Создаем новый узел
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			// Разделяем главный узел
			ImGuiID dock_main = dockspace_id; // Основное окно
			ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Left, 0.2f, nullptr, &dock_main);
			ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down, 0.25f, nullptr, &dock_main);

			// Разделяем главное окно на вкладки
			ImGuiID dock_main_tab_1 = dock_main;
			ImGuiID dock_main_tab_2 = ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.5f, nullptr, &dock_main_tab_1);

			// Привязываем окна - тут самое сладкое
			ImGui::DockBuilderDockWindow("Главное", dock_main_tab_1);
			ImGui::DockBuilderDockWindow("Читменю", dock_main_tab_1);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", dock_main_tab_2);
			ImGui::DockBuilderDockWindow("Dear ImGui Style Editor", dock_main_tab_2);

			ImGui::DockBuilderDockWindow("Данные", dock_left);
			ImGui::DockBuilderDockWindow("Консоль", dock_bottom);

			ImGui::DockBuilderFinish(dockspace_id);
		}
		
		// Отрисовываем MainMenuBar внутри dockspace
		menu.Render();
		
		ImGui::End(); // Завершаем окно с dockspace
	}

	void Run() {
		ConsoleCout() << "А на краншташ уходить моя яхтааа белый парус на кринивв." << std::endl;
		ConsoleCout() << "Позади меня осталась моя вауухтааа а прамя небо да залиувв." << std::endl;
		ConsoleCout() << "Я на яхте наделал самбрероооо и стал похож на капитана на Нерооо" << std::endl;
		ConsoleCout() << "Я на яхте наделал самбрероооо и стал похож на капитана на Нерооо" << std::endl;
		ConsoleCout() << "Лиш бы тока не НаБанДерууууу. Я стал похож на капитана Нерооо" << std::endl;
		ConsoleCout() << "" << std::endl;
		ConsoleCout() << "Чтож всё отлично!" << std::endl;
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Настраиваем dockspace и главное меню
			SetupDockspace();
			
			// Отрисовываем остальные окна
			mainWindow.Render();
			modal.Render();
			demo.Render();
			statistic.Render();
			console.Render();
			cheatmenu.Render();

			ImGui::Render();
			
			int display_w, display_h;
			glfwGetFramebufferSize(window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
		}
	}
};