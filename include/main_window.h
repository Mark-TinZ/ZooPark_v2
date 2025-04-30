#pragma once
#include "window.h"
#include "modal_window.h"
#include "console.h"
#include "zoo.h"

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct Img_scene {
	std::string name;
	std::string imagePath;
	GLuint textureID = 0;
};

extern std::vector<Img_scene> images;

class MainWindow : public BaseWindow {
private:
	ModalWindow& modalwin;
	std::vector<Img_scene> images;
	bool start_game = false;
	bool LoadTexture(const std::string&, GLuint& textureID, int& width, int& height);
public:
	MainWindow(ModalWindow& modal, const std::vector<Img_scene>& initialImages) : modalwin(modal), images(initialImages), start_game(false) {
		for (auto& image : images) {
			int w = 0, h = 0;
			LoadTexture(image.imagePath, image.textureID, w, h);
		}
	}
	Img_scene* findImageByName(const std::vector<Img_scene>& imageList, const std::string& name);

	void Begin();
	void Game();
	void Render() override; // Объявление метода Render
	void ShowPopups();
	void Draw();
};

Zoo* getZoo();