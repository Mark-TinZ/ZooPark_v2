#pragma once
#include "window.h"
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include <functional>
#include "statistics_window.h"
extern std::string zooName; 
// Сцены модальных окон
struct Scene {
	std::string text;
	std::string imagePath;
	GLuint textureID = 0;
	bool requiresInput = false;  // Флаг, нужен ли ввод
	std::string inputLabel = ""; // Метка для поля ввода
	std::string inputBuffer = ""; // Буфер для ввода текста
	size_t inputBufferSize = 100; // Максимальный размер ввода
	
	// Функция обратного вызова при подтверждении ввода
	std::function<void(const std::string&)> onInputConfirmed = nullptr;
};


extern std::vector<Scene> scenes;


class ModalWindow : public BaseWindow {
private:
	StatisticsWindow& statistic;
	bool open = false;
	int currentScene = 0; // Текущая сцена
	std::vector<Scene> scenes; // Массив сцен
	bool LoadTexture(const std::string&, GLuint& textureID, int& width, int& height);
	void HandleInputConfirmed();
public:
	GLuint textureID = 0;
	int width = 0;
	int height = 0;

	ModalWindow(StatisticsWindow& stat, const std::vector<Scene>& initialScenes) : statistic(stat), scenes(initialScenes) {
		// Загружаем все текстурки для сцен.
		for (auto& scene : scenes) {
			int w = 0, h = 0;
			LoadTexture(scene.imagePath, scene.textureID, w, h);
		}
	}

	~ModalWindow() {
		// Удаляем все текстуры
		for (auto& scene : scenes) {
			if (scene.textureID) {
				glDeleteTextures(1, &scene.textureID);
			}
		}
	}

	// Добавить новую сцену
	void AddScene(const Scene& scene);
	
	// Перейти к определенной сцене
	void GoToScene(int sceneIndex);
	
	// Перейти к следующей сцене
	void NextScene();
	
	// Перейти к предыдущей сцене
	void PreviousScene();

	// Открыть модальное окно
	void Open();

	// Отрисовка окна
	void Render() override;
};
