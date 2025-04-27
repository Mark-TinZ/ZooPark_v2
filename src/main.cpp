#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "app.cpp"
#include "steam_style.h"
#include <GLFW/glfw3.h>

int main() {
	if (!glfwInit()) return 1;

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Park", nullptr, nullptr);
	if (window == nullptr) return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	#define IMGUI_DEFINE_MATH_OPERATORS
	// #define IMGUI_ENABLE_DOCKING

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Включаем поддержку Docking

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Загружаем шрифт и настраиваем его на кирилицу
	static const ImWchar ranges[] = {
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
	0,
	};
	io.Fonts->AddFontFromFileTTF("misc/fonts/SeymourOne-Regular.ttf", 20.0f, nullptr, ranges);

	// Добавляем нашу тему из hl2
	StyleClassicSteam();

	App app(window);
	app.Run();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
