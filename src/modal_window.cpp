#include "modal_window.h"
#include <SDL2/SDL.h>
#include "main_window.h"
#include <SDL2/SDL_image.h>
#include "console.h"

std::string zooName; 

std::vector<Scene> scenes = {
	{
		"И так!\nШо ты воин прибыл из другой галактики!\n"
		"Ну хуй с ним давай я тебе покажу шо тут а как, ты ток внимай внимательно!",
		"misc/image/88ec3916f3f1df6e4dfe252ec12c5a66.jpg"
	},
	{
		"Добро пожаловать, директор, в ваш новый зоопарк!\n"
		"Я буду твоим помошником на всем твоем пути!",
		"misc/image/maxresdefault.jpg"
	},
	{
		"Здес мы будем сра.. кхм собирать самыех эксклюзивных существ.\n"
		"Для начала, давай придумаем название для нашего зоопарка.\n"
		"Как насчэт \"Моий Остроовов в Океане\"?",
		"misc/image/7e76bbe1fea72737301ed58017475e77.jpg",
		0,
		true,
		"Название зоопарка",
		"",
		100,
		[&zooName](const std::string& input) {
			// Сохраняем введенное название в переменную
			zooName = input;
		}
	},
	{
		"Отрично название есть!\n"
		"Я позову своих фанатов и у нас появяться посетители.",
		"misc/image/screenshot_44.jpg"
	}
};

bool ModalWindow::LoadTexture(const std::string& path, GLuint& textureID, int& width, int& height)  {
	// Инициализация SDL_image
	if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
		return false;
	}
	
	// Загрузка изображения через SDL_image
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (!surface) {
		return false;
	}
	
	width = surface->w;
	height = surface->h;
	
	// Конвертирование в RGBA если нужно
	SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
	SDL_FreeSurface(surface);
	
	if (!rgbaSurface) {
		return false;
	}
	
	// Создание OpenGL текстуры
	if (textureID) {
		glDeleteTextures(1, &textureID);
	}
	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
	
	// Параметры текстуры
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Освобождение ресурсов
	SDL_FreeSurface(rgbaSurface);
	
	return textureID != 0;
}

void ModalWindow::HandleInputConfirmed() {
	this->statistic.open = true;
}

void ModalWindow::AddScene(const Scene& scene) {
	Scene newScene = scene;
	int w = 0, h = 0;
	LoadTexture(newScene.imagePath, newScene.textureID, w, h);
	scenes.push_back(newScene);
}

void ModalWindow::GoToScene(int sceneIndex) {
	if (sceneIndex >= 0 && sceneIndex < scenes.size()) {
		currentScene = sceneIndex;
	}
}

void ModalWindow::NextScene() {
	if (currentScene < scenes.size() - 1) {
		currentScene++;
	} else {
		// Последняя сцена, закрываем окно
		open = false;
	}
}

void ModalWindow::PreviousScene() {
	if (currentScene > 0) {
		currentScene--;
	}
}

void ModalWindow::Open() { 
	open = true; 
	currentScene = 0;
}

void ModalWindow::Render() {
	if (open) {
		ImGui::OpenPopup("Санбой");
	}

	if (ImGui::BeginPopupModal("Санбой", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove)) {
		if (currentScene < scenes.size()) {
			Scene& scene = scenes[currentScene];

			if (scene.textureID) {
				ImGui::Image((ImTextureID)(intptr_t)scene.textureID, ImVec2(128, 128));
			}
			
			ImGui::SameLine();
			ImGui::Text("%s", scene.text.c_str());
			
			// Если сцена требует ввода - добавляем элементы ввода
			if (scene.requiresInput) {
				ImGui::Separator();
				
				// Создаем буфер для ввода текста, если его нет
				if (scene.inputBuffer.empty()) {
					scene.inputBuffer.resize(scene.inputBufferSize, '\0');
				}
				
				// Поле ввода текста
				ImGui::Text("%s", scene.inputLabel.c_str());
				
				// Уникальный идентификатор для виджета, чтобы ImGui мог отслеживать его состояние
				char inputId[32];
				snprintf(inputId, sizeof(inputId), "##input%d", currentScene);
				
				if (ImGui::InputText(inputId, &scene.inputBuffer[0], scene.inputBufferSize)) {
					// Обработка изменения текста (если нужно)
				}
				if (scene.inputBuffer[0] == '\0') {
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Введи название");
					}
				// Стандартные кнопки навигации
				if (currentScene > 0) {
					if (ImGui::Button("Назад")) {
						PreviousScene();
					}
					ImGui::SameLine();
				}
				// Кнопка подтверждения ввода
				if (ImGui::Button("Подтвердить") || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
					
					if (scene.inputBuffer[0] != '\0') {
						// Trimming the input (removing trailing null characters)
						std::string inputValue = scene.inputBuffer.c_str();
						
						// Вызов функции обратного вызова для обработки введенных данных
						if (scene.onInputConfirmed) {
							scene.onInputConfirmed(inputValue);
						}
						
						// Переход к следующей сцене после подтверждения
						NextScene();
					}
				}
			} else {
				// Стандартные кнопки навигации
				if (currentScene > 0) {
					if (ImGui::Button("Назад")) {
						PreviousScene();
					}
					ImGui::SameLine();
				}
				
				if (currentScene < scenes.size() - 1) {
					// Если не последняя сцена
					if (ImGui::Button("Далее!")) {
						NextScene();
					}
				} else {
					// Если последняя сцена
					if (ImGui::Button("Завершить")) {
						open = false;
						ImGui::CloseCurrentPopup();
						HandleInputConfirmed();
						getZoo()->setZooName(zooName);
						ConsoleCout() << "Зоопарк " << zooName << " создан!" << std::endl;
						ConsoleCout() << "Санбой: Название так себе на самом деле." << std::endl;
					}
				}
			}
		}
		// Устанавливаем положение окна
		ImGui::SetWindowPos(
			ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
			(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
		);
		ImGui::EndPopup();
	}
}
