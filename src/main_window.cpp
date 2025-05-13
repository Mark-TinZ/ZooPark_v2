#include "main_window.h"
#include "modal_window.h"
#include <functional>
#include <algorithm>
#include <cstring>

std::vector<Img_scene> images = {
	{
		"shop",
		"misc/image/hmelisuneli.jpg"
	},
	{
		"enclosure",
		"misc/image/HupZWHmblfXMD1y9JWwFZbPMnjg.jpg"
	},
	{
		"animal",
		"misc/image/qzjNmDQvorI.jpg"
	},
	{
		"market",
		"misc/image/JZF3CcrAAy6Ou5xK4ykbW.jpg"
	},
	{
		"worker",
		"misc/image/88ec3916f3f1df6e4dfe252ec12c5a66.jpg"
	},
	{
		"statistic",
		"misc/image/hqdefault.jpg"
	}
 };

static Zoo* zoo = nullptr;
Zoo* getZoo() {
	return zoo;
}

bool MainWindow::LoadTexture(const std::string& path, GLuint& textureID, int& width, int& height)  {
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

Img_scene* MainWindow::findImageByName(const std::vector<Img_scene>& imageList, const std::string& name) {
	auto it = std::find_if(imageList.begin(), imageList.end(), [&name](const Img_scene& img) {
		return img.name == name;
	});

	if (it != imageList.end()) {
		return &(*it); // Возвращаем указатель на найденный элемент
	} else {
		return nullptr; // Если не найдено, возвращаем nullptr
	}
}

void MainWindow::Begin() {
	ImGui::SetWindowFontScale(1.5f);
	ImGui::Text("Лор игры:");
	ImGui::SetWindowFontScale(1.f);
	ImGui::Separator();
	ImGui::TextWrapped(
		"Неожиданно вас подхватил пространственно-временной вихрь, бац, \nи вы директор инопланетного зоопарка на искусственной планете, "
		"вращающейся вокруг Тау Кита.\n"
		"Инопланетные технологии позволяют вам очень гибко изменять самих животных, \nи вообще жить в кайф, но вот с системой учёта там совсем беда."
	);
	ImGui::Spacing();
	if (ImGui::Button("Начать игру")) {
		ConsoleCout() << 
			"Санбой: и так, д06р0 п0жал0вать, дирэкт0р, в ваш н0выи э00парк на искусствэнн0и планэтэ тау кита! я ваш вэрныи п0м0щник, и вмэстэ мы с0эдадим нэчт0 удивитэльн0э!\n"
			"Санбой: для начала, даваитэ придумаэм наэваниэ для нашэx0 э00парка. как насчэт \"э00сфэра тау\"? эдэсь мы 6удэм с06ирать самыэ экэ0тичэскиэ сущэства иэ всэх уx0лк0в всэлэнн0и и с0эдавать уникальныэ усл0вия для их жиэни." << std::endl;
		modalwin.Open();
		
		// Создаем новый зоопарк
		zoo = new Zoo(zooName, 10000);
		
		start_game = true;
	} 
	ImGui::SetItemTooltip("Прочитал гей.");
}

void MainWindow::Game() {
	if (!zoo) return;

	ImGui::BeginTabBar("Игровая сцена");

	// Главное меню
	if (ImGui::BeginTabItem("Главное")) {
		// Статус-бар с сообщениями
		if (!zoo->statusMessage.empty()) {
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "%s", zoo->statusMessage.c_str());
			ImGui::Separator();
		}

		
		// ImGui::Separator();

		// Информация зоопарка
		ImGui::SetWindowFontScale(1.5f);
		ImGui::Text("Зоопарк: %s", zoo->getZooName().c_str());
		ImGui::SetWindowFontScale(1.f);


		// Выводить данные о животных
		if (ImGui::TreeNode("Информация о животных:")) {
			// Графики по животным
			if (!zoo->animalCountHistory.empty()) {
				ImGui::Text("Количество животных:");
				ImGui::PlotHistogram("##AnimalsCount", 
									zoo->animalCountHistory.data(), 
									zoo->animalCountHistory.size(), 
									0, NULL, 0.0f, 
									*std::max_element(zoo->animalCountHistory.begin(), zoo->animalCountHistory.end()) * 1.1f, 
									ImVec2(0, 40.0f));
			}
			
			// Больные животные
			int sickCount = 0;
			for (const auto& animal : zoo->animals) {
				if (animal->state == AnimalState::SICK) sickCount++;
			}
			ImGui::Text("Больных животных: %d", sickCount);
			
			ImGui::TreePop();
		}

		// Выводить данные о работниках
		if (ImGui::TreeNode("Информация о рабочих:")) {
			int vetCount = 0, cleanerCount = 0, foodmanCount = 0;
			for (const auto& worker : zoo->workers) {
				if (worker.role == WorkerRole::VETERINARIAN) vetCount++;
				else if (worker.role == WorkerRole::CLEANER) cleanerCount++;
				else if (worker.role == WorkerRole::FOODMEN) foodmanCount++;
			}
			
			ImGui::Text("Ветеринары: %d", vetCount);
			ImGui::Text("Уборщики: %d", cleanerCount);
			ImGui::Text("Кормильцы: %d", foodmanCount);
			
			ImGui::TreePop();
		}

		// Выводить данные о посетителях и экономике
		if (ImGui::TreeNode("Экономика и посетители:")) {
			if (!zoo->moneyHistory.empty()) {
				ImGui::Text("Деньги:");
				ImGui::PlotHistogram("##MoneyHistory", 
									 zoo->moneyHistory.data(), 
									 zoo->moneyHistory.size(), 
									 0, NULL, 0.0f, 
									 *std::max_element(zoo->moneyHistory.begin(), zoo->moneyHistory.end()) * 1.0f, 
									 ImVec2(0, 40.0f));
				ImGui::SameLine(); HelpMarker(
					"Числа больше 10к будут выводится кратко\n"
					"Крч для того чтобы решить эту проблему нужно подключать библиотеку ImPlot\n"
					"Я бы хотел но я не хочу ждать дохрена времени компиляции."
				);
			} 
			if (!zoo->popularityHistory.empty()) {
				ImGui::Text("Популярность:");
				ImGui::PlotHistogram("##PopularityHistory",
									zoo->popularityHistory.data(),
									zoo->popularityHistory.size(),
									0, NULL, 0.0f,
									*std::max_element(zoo->popularityHistory.begin(), zoo->popularityHistory.end()) * 1.1f,
									ImVec2(0, 40.0f));
			}
			if (!zoo->visitorsHistory.empty()) {
				ImGui::Text("Посетители:");
				ImGui::PlotHistogram("##VisitorsHistory", 
									  zoo->visitorsHistory.data(), 
									  zoo->visitorsHistory.size(), 
									  0, NULL, 0.0f, 
									  *std::max_element(zoo->visitorsHistory.begin(), zoo->visitorsHistory.end()) * 1.1f, 
									  ImVec2(0, 40.0f));
			}
			ImGui::TreePop();
		}

		// Кнопка следующий день
		// if (ImGui::Button("Следующий день", ImVec2(0, 40))) {zoo->nextDay();}
		ImGui::EndTabItem();
	}

	// Вкладка рынка животных
	if (ImGui::BeginTabItem("Животные")) {
		// Настройки
		enum WidgetType {WidgetType_TreeNode};
		static WidgetType widget_type = WidgetType_TreeNode;
		static bool show_menu_sex_processin = false;
		static int id1 = -1;
		static int id2 = -1;

		static bool show_advice_animal = false;
		// Отображаем изображение рынка животных если оно есть
		Img_scene* marketImg = findImageByName(images, "animal");
		if (marketImg && marketImg->textureID != 0 && !show_advice_animal) {
			ImGui::OpenPopup("Санбой");

			if (ImGui::BeginPopupModal("Санбой", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				ImGui::Image((void*)(intptr_t)marketImg->textureID, ImVec2(250, 250));
				ImGui::SameLine();
				ImGui::TextWrapped(
					"В данном разделе ты сможешь купить животных.\n"
					"Перед покупкой животных ты должен купить вольер, не забывай!\n"
					"Эх Олех олех Олехх попросился ко вдове на ночлехх\n"
					"Эх олехъ Олехъ Олеххх затем тебе ее вещи на греехх?\n"
					"Ммммм....\n"
					"Не забывай делать осмысленные имена, они будут переходится детишкам.\n"
					"Цена животных зависит от параметров животных."
				);
			
				if (ImGui::Button("Спасибо дядя Санбой")) {
					show_advice_animal = true;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}

		ImGui::Text("Купить новое животное:");
		
		// Форма для добавления животного
		static char animalName[64] = "Животное";
		static int selectedAnimalType = 0;
		static int animalAge = 1;
		
		const char* animalTypes[] = { "Кошка", "Пингвин", "Собака", "Белый медведь", "Жираф", "Слон", "Рыба" };
		
		ImGui::InputText("Имя", animalName, IM_ARRAYSIZE(animalName));
		ImGui::Combo("Тип животного", &selectedAnimalType, animalTypes, IM_ARRAYSIZE(animalTypes));

		// Toolbox с всей этой шляпой
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			switch (selectedAnimalType) {
				case 0: // Кошка
					ImGui::Text("Кошка:");
					ImGui::BulletText("Климат: Умереный");
					ImGui::BulletText("Характеристика: Легко адаптируется.");
					break;
				case 1: // Пингвин
					ImGui::Text("Пингвин:");
					ImGui::BulletText("Климат: Арктика");
					ImGui::BulletText("Характеристика: Любит холод.");
					break;
				case 2: // Собака
					ImGui::Text("Собака:");
					ImGui::BulletText("Климат: Умереный");
					ImGui::BulletText("Характеристика: Верный друг.");
					break;
				case 3: // Белый медведь
					ImGui::Text("Белый медведь:");
					ImGui::BulletText("Климат: Арктика");
					ImGui::BulletText("Характеристика: Требует много еды.");
					break;
				case 4: // Жираф
					ImGui::Text("Жираф:");
					ImGui::BulletText("Климат: Тропический");
					ImGui::BulletText("Характеристика: Высокий и грациозный.");
					break;
				case 5: // Слон
					ImGui::Text("Слон:");
					ImGui::BulletText("Климат: Умереный");
					ImGui::BulletText("Характеристика: Очень умный.");
					break;
				case 6: // Рыба
					ImGui::Text("Рыба:");
					ImGui::BulletText("Климат: Водный");
					ImGui::BulletText("Характеристика: Живет в воде.");
					break;
			}
			ImGui::EndTooltip();
		}
		ImGui::SliderInt("Возраст", &animalAge, 1, 20);
		
		// Расчет стоимости
		int animalPrice = 0;
		switch (selectedAnimalType) {
			case 0: animalPrice = 200 + (20 - animalAge) * 30; break; // Кошка
			case 1: animalPrice = 800 + (20 - animalAge) * 20; break; // Пингвин
			case 2: animalPrice = 250 + (20 - animalAge) * 25; break; // Собака
			case 3: animalPrice = 1500 + (20 - animalAge) * 50; break; // Медведь
			case 4: animalPrice = 1200 + (20 - animalAge) * 40; break; // Жираф
			case 5: animalPrice = 2000 + (20 - animalAge) * 60; break; // Слон
			case 6: animalPrice = 2000 + (20 - animalAge) * 60; break; // Рыба
		}
		
		ImGui::Text("Стоимость: %d", animalPrice);
		
		if (ImGui::Button("Купить", ImVec2(100, 30))) {
			if (zoo->buyAnimal(animalName, selectedAnimalType, animalAge)) {
				// Сброс имени после покупки
				strcpy(animalName, "Животное");
				animalAge = 1;
			}
		}
		
		ImGui::Separator();
		
		// Список имеющихся животных
		ImGui::Text("Имеющиеся животные:");
		static int selectedAnimalIndex = -1; // Индекс выбранного животного
		if (ImGui::BeginTable("AnimalsTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Имя");
			ImGui::TableSetupColumn("Тип", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Пол");
			ImGui::TableSetupColumn("Возраст");
			ImGui::TableSetupColumn("Состояние");
			ImGui::TableSetupColumn("Счастье");
			ImGui::TableSetupColumn("Действия", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			
			for (int i = 0; i < zoo->animals.size(); i++) {
				std::shared_ptr<Animal>& animal = zoo->animals[i];

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%s", animal->getName());

				// Остальная часть кода для отображения животного
				ImGui::TableNextColumn();
				ImGui::TextWrapped("%s/%s", animal->getDietString().c_str(), animal->getClimateString().c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", animal->guy ? "Мужик" : "Баба");

				ImGui::TableNextColumn();
				ImGui::Text("%d", animal->age);

				ImGui::TableNextColumn();

				ImVec4 stateColor;
				if (animal->state == AnimalState::HEALTHY) {
					stateColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				} else if (animal->state == AnimalState::SICK) {
					stateColor = ImVec4(1.0f, 0.75f, 0.0f, 1.0f);
				} else {
					stateColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
				}
				ImGui::TextColored(stateColor, "%s", animal->getStateString().c_str());

				ImGui::TableNextColumn();
				ImGui::ProgressBar(std::max(0.f, std::min(animal->happiness / 100.0f, 1.f)), ImVec2(-1, 0));

				ImGui::TableNextColumn();
				ImGui::PushID(i); // Уникальный ID для каждой строки
				if (ImGui::Button("Действие")) {
					selectedAnimalIndex = i; // Устанавливаем выбранное животное
					ImGui::OpenPopup("AnimalActions");
				}

				if (ImGui::BeginPopup("AnimalActions")) {
					if (ImGui::Selectable("Лечить")) {
						zoo->healAnimal(animal->id);  // Предполагаем, что метод теперь принимает ID
					}
					if (ImGui::Selectable("Продать")) {
						zoo->sellAnimal(animal->id);   // Предполагаем, что метод теперь принимает ID
					}
					if (ImGui::Selectable("Спаривать")) {
						id1 = animal->id;  // Храним ID животного, а не индекс в векторе
						// zoo->breedAnimal(i);
						// ВНИМАНИЕ ГОВНОКОД!!!
						show_menu_sex_processin = true;
					}
					if (ImGui::Selectable("Закрыть")) {
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				ImGui::PopID(); // Убедитесь, что PopID вызывается в любом случае
			}
			
			ImGui::EndTable();
		}
		
		if (show_menu_sex_processin) {
			static int e = 0;
			ImGui::OpenPopup("Спаривание");

			if (ImGui::BeginPopupModal("Спаривание", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				
				if (ImGui::BeginTable("EnclosureAnimalsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
					ImGui::TableSetupColumn("Имя");
					ImGui::TableSetupColumn("Тип");
					ImGui::TableSetupColumn("Пол");
					ImGui::TableSetupColumn("Выбор");
					ImGui::TableHeadersRow();

					for (size_t j = 0; j < zoo->animals.size(); j++) {
						// Получаем указатель на Animal и разыменовываем его
						std::shared_ptr<Animal> animalPtr = zoo->animals[j];
						if (!animalPtr) continue; // Проверяем, что указатель не пустой

						Animal& animal = *animalPtr; // Разыменовываем указатель, чтобы получить ссылку на Animal
						if (animal.state == AnimalState::DEAD || animal.state == AnimalState::SELL) continue;

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						ImGui::TextWrapped("%s", animal.getName().c_str());

						ImGui::TableNextColumn();
						ImGui::TextWrapped("%s/%s", animal.getDietString().c_str(), animal.getClimateString().c_str());

						ImGui::TableNextColumn();
						ImGui::Text("%s", animal.guy ? "Мужик" : "Баба");

						ImGui::TableNextColumn();
						ImGui::PushID(static_cast<int>(j));
						ImGui::RadioButton("##radio", &e, j);
						ImGui::PopID();
					}

					ImGui::EndTable();
				} else {
					ImGui::Text("Вольер пуст");
				}

			
				if (ImGui::Button("Закрыть")) {
					show_menu_sex_processin = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Чпокаться")) {
					id2 = e;
					if (id1 != -1 && id2 != -1) zoo->startBreeding(id1, id2);
					else ConsoleCout() << "Ошибка не удалось определить животных" << std::endl;

					show_menu_sex_processin = false;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}

		ImGui::EndTabItem();
	}

	// Вкладка вольеров
	if (ImGui::BeginTabItem("Вольеры")) {
		static bool show_advice_enclosure = false;
		// Изображение вольера
		Img_scene* enclosureImg = findImageByName(images, "enclosure");
		
		if (enclosureImg && enclosureImg->textureID != 0 && !show_advice_enclosure) {
			ImGui::OpenPopup("Санбой");

			if (ImGui::BeginPopupModal("Санбой", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				ImGui::Image((void*)(intptr_t)enclosureImg->textureID, ImVec2(250, 250));
				ImGui::SameLine();
				ImGui::TextWrapped(
					"Ну шобы у тебя всё было чики пуки я тут поискал брихаду зумеров которые построят тебе вольеры"
					"Конечно они не особо требовательны им только денег дай и они тебе всё чо хочешь постоят.\n"
					"Только учитывай сносить вольеры нельзя! На этой планате есть местное провительсво, они хотят чтобы ты только расширял зоопарк."
				);
				if (ImGui::Button("Хорошо спасибо тебе велекодушный товарищ Санбой")) {
					show_advice_enclosure = true;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}

		ImGui::Text("Построить новый вольер:");
		
		// Форма для создания вольера
		static char enclosureName[64] = "Вольер";
		static int selectedClimateType = 0;
		static int enclosureCapacity = 3;
		
		const char* climateTypes[] = { "Тропический", "Умеренный", "Полярный", "Водный"};
		
		ImGui::InputText("Название", enclosureName, IM_ARRAYSIZE(enclosureName));
		ImGui::Combo("Климат", &selectedClimateType, climateTypes, IM_ARRAYSIZE(climateTypes));
		ImGui::SliderInt("Вместимость", &enclosureCapacity, 1, 10);
		
		// Расчет стоимости
		int enclosurePrice = 0;
		switch (selectedClimateType) {
			case 0: enclosurePrice = 1000 + enclosureCapacity * 100; break; // Тропический
			case 1: enclosurePrice = 1200 + enclosureCapacity * 100; break; // Умеренный
			case 2: enclosurePrice = 1500 + enclosureCapacity * 100; break; // Полярный
			case 3: enclosurePrice = 2000 + enclosureCapacity * 100; break; // Водный
		}
		
		ImGui::Text("Стоимость: %d", enclosurePrice);
		ImGui::Text("Ежедневные расходы: %d", (selectedClimateType == 0 ? 50 : selectedClimateType == 1 ? 60 : 70) + enclosureCapacity * 10);
		
		if (ImGui::Button("Построить", ImVec2(0, 30))) {
			if (zoo->buildEnclosure(enclosureName, selectedClimateType, enclosureCapacity)) {
				strcpy(enclosureName, "Вольер");
				enclosureCapacity = 3;
			}
		}
		
		ImGui::Separator();
		
		// Список имеющихся вольеров
		ImGui::Text("Имеющиеся вольеры:");
		
		if (ImGui::BeginTable("EnclosuresTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Название");
			ImGui::TableSetupColumn("Климат");
			ImGui::TableSetupColumn("Вместимость");
			ImGui::TableSetupColumn("Заполненность");
			ImGui::TableSetupColumn("Чистота");
			ImGui::TableHeadersRow();
			
			for (int i = 0; i < zoo->enclosures.size(); i++) {
				Enclosure& enclosure = zoo->enclosures[i];
				
				ImGui::TableNextRow();
				ImGui::PushID(i);
				
				ImGui::TableNextColumn();
				if (ImGui::Selectable(enclosure.name.c_str(), zoo->selectedEnclosureIndex == i, ImGuiSelectableFlags_SpanAllColumns)) {
					zoo->selectedEnclosureIndex = i;
				}
				
				ImGui::TableNextColumn();
				ImGui::Text("%s", enclosure.getClimateString().c_str());
				
				ImGui::TableNextColumn();
				ImGui::Text("%d", enclosure.capacity);
				
				ImGui::TableNextColumn();
				ImGui::Text("%llu/%d", static_cast<unsigned long long>(enclosure.animals.size()), enclosure.capacity);
				
				ImGui::TableNextColumn();
				ImGui::ProgressBar(1.0f - (enclosure.dirty / 100.0f), ImVec2(-1, 0));
				
				ImGui::PopID();
			}
			
			ImGui::EndTable();
		}
		
		// Отображение информации о выбранном вольере
		if (zoo && zoo->selectedEnclosureIndex >= 0 && zoo->selectedEnclosureIndex < static_cast<int>(zoo->enclosures.size())) {
			Enclosure& selectedEnclosure = zoo->enclosures[zoo->selectedEnclosureIndex];
			
			ImGui::Separator();
			ImGui::Text("Информация о вольере \"%s\":", selectedEnclosure.getName().c_str());
			ImGui::Text("Климат: %s", selectedEnclosure.getClimateString().c_str());
			ImGui::Text("Животные в вольере:");
			
			if (selectedEnclosure.animals.empty()) {
				ImGui::Text("Вольер пуст");
			} else {
				// Итерируем по weak_ptr животных
				for (const auto& weakAnimal : selectedEnclosure.animals) {
					// Пытаемся получить shared_ptr из weak_ptr
					if (auto animal = weakAnimal.lock()) {
						// Проверяем состояние животного
						if (animal->state == AnimalState::DEAD || animal->state == AnimalState::SELL) continue;
						
						// Получаем данные о животном
						animalData data = animal->getAnimalData();
						ImGui::Text("%d. %s (%s)", data.id, data.name.c_str(), animal->getClimateString().c_str());
					}
				}
			}
		}
		
		ImGui::EndTabItem();
	}

	// Вкладка управления персоналом
	if (ImGui::BeginTabItem("Персонал")) {
		static bool show_advice_worker = false;
		// Изображение магазина
		Img_scene* shopImg = findImageByName(images, "worker");
		if (shopImg && shopImg->textureID != 0 && !show_advice_worker) {
			ImGui::OpenPopup("Санбой");

			if (ImGui::BeginPopupModal("Санбой", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				ImGui::Image((void*)(intptr_t)shopImg->textureID, ImVec2(250, 250));
				ImGui::SameLine();
				ImGui::TextWrapped(
					"Мир стоит в тишине или бранном тумане на кроваввой войне на любовном обмане на любовном любовном любовном любовном обмане любовном любовном любовном любовном обмане...\n"
					"Собрал тут для тебя стаю фанатов которые могли бы работать у тебя в зоопарке.\n"
					"Тут ты сможешь нанять себе работников для зоопарка. Учитывай что они каждый день получают зарплату."
				);
				if (ImGui::Button("Понял принял ")) {
					show_advice_worker = true;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}

		ImGui::Text("Нанять нового работника:");
		
		static int selectedWorkerType = 0;
		const char* workerTypes[] = { "Ветеринар", "Уборщик", "Кормилец" };

		ImGui::Combo("Должность", &selectedWorkerType, workerTypes, IM_ARRAYSIZE(workerTypes));
		
		// Расчет зарплаты
		int workerSalary = 0;
		switch (selectedWorkerType) {
			case 0: workerSalary = 500; break; // Ветеринар
			case 1: workerSalary = 300; break; // Уборщик
			case 2: workerSalary = 200; break; // Кормилец
		}
		
		ImGui::Text("Зарплата: %d в день", workerSalary);
		
		if (ImGui::Button("Нанять", ImVec2(100, 30))) {
			zoo->hireWorker(selectedWorkerType);
		}
		
		ImGui::Separator();
		
		// Список имеющихся работников
		ImGui::Text("Имеющиеся работники:");
		
		if (ImGui::BeginTable("WorkersTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
			ImGui::TableSetupColumn("Имя");
			ImGui::TableSetupColumn("Должность");
			ImGui::TableSetupColumn("Зарплата");
			ImGui::TableSetupColumn("Статус");
			ImGui::TableSetupColumn("Действия");
			ImGui::TableHeadersRow();
			
			for (int i = 0; i < zoo->workers.size(); i++) {
				Worker& worker = zoo->workers[i];
				
				ImGui::TableNextRow();
				ImGui::PushID(i);
				
				ImGui::TableNextColumn();
				ImGui::Text("%s", worker.name.c_str());

				ImGui::TableNextColumn();
				ImGui::Text("%s", worker.getRoleString().c_str());
				
				ImGui::TableNextColumn();
				ImGui::Text("%d", worker.price);
				
				ImGui::TableNextColumn();
				ImGui::Text("%s", worker.isWorking ? "Работает" : "Отдыхает");
				
				ImGui::TableNextColumn();
				if (worker.role != WorkerRole::DIRECTOR) {
					if (ImGui::Button("Уволить")) {
						zoo->dismissWorker(i);
						ImGui::PopID();
						break;  // Выходим из цикла, т.к. изменился размер массива
					}
				} else {
					ImGui::Text("Нельзя уволить");
				}
				
				ImGui::PopID();
			}
			
			ImGui::EndTable();
		}
		
		ImGui::EndTabItem();
	}

	// Вкладка магазина (еда и реклама)
	if (ImGui::BeginTabItem("Магазин")) {
		static bool show_advice_store = false;
		// Изображение магазина
		Img_scene* shopImg = findImageByName(images, "shop");
		if (shopImg && shopImg->textureID != 0 && !show_advice_store) {
			ImGui::OpenPopup("Санбой");

			if (ImGui::BeginPopupModal("Санбой", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				ImGui::Image((void*)(intptr_t)shopImg->textureID, ImVec2(250, 250));
				ImGui::SameLine();
				ImGui::TextWrapped(
					"Тут ты сможешь купить еды у моих знакомых поставщиков. Они немного жадные но торгуют всяким.\n"
					"А еще ты сможешь заказать у меня рекламу, я организую концерт который принесет тебе популяреость."
				);
				if (ImGui::Button("Понял принял ")) {
					show_advice_store = true;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}
		
		ImGui::Separator();
		
		// Покупка еды
		ImGui::Text("Купить еду для животных:");
		static int foodAmount = 50;
		
		ImGui::SliderInt("Количество еды", &foodAmount, 10, 200);
		ImGui::Text("Стоимость: %d", foodAmount * 10);
		
		if (ImGui::Button("Купить еду", ImVec2(0, 30))) {
			zoo->buyFood(foodAmount);
		}
		
		ImGui::Separator();
		
		// Реклама зоопарка
		ImGui::Text("Заказать рекламу зоопарка:");
		static int advertisingBudget = 500;
		
		ImGui::SliderInt("Бюджет рекламы", &advertisingBudget, 100, 2000);
		ImGui::Text("Прирост популярности: ~ %d", advertisingBudget / 100);
		
		if (ImGui::Button("Заказать рекламу", ImVec2(0, 30))) {
			zoo->orderAdvertising(advertisingBudget);
		}
		
		ImGui::EndTabItem();
	}

	// Вкладка статистики
	if (ImGui::BeginTabItem("Статистика")) {
		/*
		Добавить в это меню все данные, сколько заработали, сколько сколько было максимум работников ну и другие полузные штуки.
		*/

		static bool show_advice_statistic = false;
		// Изображение магазина
		Img_scene* shopImg = findImageByName(images, "statistic");
		if (shopImg && shopImg->textureID != 0 && !show_advice_statistic) {
			ImGui::OpenPopup("Обама");

			if (ImGui::BeginPopupModal("Обама", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
				ImGui::SetWindowSize(ImVec2(900, 330)); // Устанавливаем фиксированный размер окна
				ImGui::Image((void*)(intptr_t)shopImg->textureID, ImVec2(250, 250));
				ImGui::SameLine();
				ImGui::TextWrapped(
					"Айм сори за мой шпрехен русиш. Я работать твой БУХАТЬер. Я записать твой деньга и нарисовать картинка "
					"красивай картинка."
				);
				if (ImGui::Button("Понял принял ")) {
					show_advice_statistic = true;
				}
				
				// Устанавливаем положение окна
				ImGui::SetWindowPos(
					ImVec2((ImGui::GetIO().DisplaySize.x - ImGui::GetWindowSize().x) * 0.5f, 
					(ImGui::GetIO().DisplaySize.y - ImGui::GetWindowSize().y) * 0.5f)
				);
				ImGui::EndPopup();
			}
		}

		// Графики
		if (!zoo->moneyHistory.empty()) {
			ImGui::Text("Деньги:");
			ImGui::PlotHistogram("##MoneyHistoryFull", 
								  zoo->moneyHistory.data(), 
								  zoo->moneyHistory.size(), 
								  0, NULL, 0.0f, 
								  *std::max_element(zoo->moneyHistory.begin(), zoo->moneyHistory.end()) * 1.1f, 
								  ImVec2(0, 80.0f));
		}
		
		ImGui::Separator();
		
		if (!zoo->popularityHistory.empty()) {
			ImGui::Text("Популярность зоопарка:");
			ImGui::PlotHistogram("##PopularityHistoryFull", 
								  zoo->popularityHistory.data(), 
								  zoo->popularityHistory.size(), 
								  0, NULL, 0.0f, 100.0f, 
								  ImVec2(0, 80.0f));
		}
		
		ImGui::Separator();
		
		if (!zoo->visitorsHistory.empty()) {
			ImGui::Text("Посетители:");
			ImGui::PlotHistogram("##VisitorsHistoryFull", 
								  zoo->visitorsHistory.data(), 
								  zoo->visitorsHistory.size(), 
								  0, NULL, 0.0f, 
								  *std::max_element(zoo->visitorsHistory.begin(), zoo->visitorsHistory.end()) * 1.1f, 
								  ImVec2(0, 80.0f));
		}
		
		ImGui::Separator();
		
		if (!zoo->animalCountHistory.empty()) {
			ImGui::Text("Количество животных:");
			ImGui::PlotHistogram("##AnimalsCountFull", 
								  zoo->animalCountHistory.data(), 
								  zoo->animalCountHistory.size(), 
								  0, NULL, 0.0f, 
								  *std::max_element(zoo->animalCountHistory.begin(), zoo->animalCountHistory.end()) * 1.1f, 
								  ImVec2(0, 80.0f));
		}
		
		ImGui::EndTabItem();
	}


	// Пока не реализованно
	if (ImGui::BeginTabItem("Рынок")) {
		// использовать grid для создания вариантов товаров.
		Img_scene* image = findImageByName(images, "market");
		for (int y = 0; y < 4; y++)
				for (int x = 0; x < 4; x++)
				{
					if (x > 0)
						ImGui::SameLine();
					ImGui::PushID(y * 4 + x);
					ImGui::BeginChild("rinok", ImVec2(256.f, 0.f));
					
					if (image != nullptr) {
						if (image->textureID) {
							ImGui::Image((ImTextureID)(intptr_t)image->textureID, ImVec2(256, 256));
						}
					}
					ImGui::TextWrapped(
						"Тут самый важный текст тут описывается товар\n"
						"Heal: 100\n"
						"age: 40\nand etc.%s",
						(y*4+x == 2 ? "\n\n\nTUUUT" : "")
					);
					if (ImGui::Button("Купить", ImVec2(252, 40))) {}
					ImGui::EndChild();
					ImGui::PopID();
				}

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();
}

void MainWindow::Render() {
	ImGui::Begin("Главное");
	if (start_game) {
		Game();
		ImGui::End();
		return;
	}
	Begin();
	ImGui::End();
}

// Всплывающие окна для важных сообщений (уведомления, банкротство и т.д.)
// void MainWindow::ShowPopups() {
// 	if (zoo && (zoo->money < 0 || zoo->day >= 30)) {
// 		ImGui::OpenPopup("Конец игры");
// 	}
	
// 	// Модальное окно конца игры
// 	if (ImGui::BeginPopupModal("Конец игры", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
// 		if (zoo->money < 0) {
// 			ImGui::Text("Вы обанкротились! Ваш зоопарк закрыт.");
// 			ImGui::Text("Прожито дней: %d", zoo->day);
// 		} else if (zoo->day >= 30) {
// 			ImGui::Text("Поздравляем! Вы успешно управляли зоопарком 30 дней!");
// 			ImGui::Text("Итоговый капитал: %d", zoo->money);
// 			ImGui::Text("Количество животных: %zu", zoo->animals.size());
// 			ImGui::Text("Популярность: %d", zoo->popularity);
// 		}
		
// 		if (ImGui::Button("Начать заново", ImVec2(150, 30))) {
// 			// Пересоздаем зоопарк
// 			delete zoo;
// 			zoo = new Zoo("Зоосфера Тау", 10000);
// 			ImGui::CloseCurrentPopup();
// 		}
		
// 		ImGui::EndPopup();
// 	}
// }

// Модальное окно для начала игры
// void ModalWin::Draw() {
// 	// Предотвращаем закрытие окна кликом вне него
// 	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), 
// 						   ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	
// 	if (ImGui::BeginPopupModal("Начало игры", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoClose)) {
// 		ImGui::Text("Санбой: Привет, новый директор зоопарка!");
// 		ImGui::Text("Как будет называться ваш зоопарк?");
		
// 		static char zooName[64] = "Зоосфера Тау";
// 		ImGui::InputText("Название", zooName, IM_ARRAYSIZE(zooName));
		
// 		if (ImGui::Button("Подтвердить", ImVec2(120, 30))) {
// 			if (zoo) {
// 				zoo->name = zooName;
// 			}
// 			ImGui::CloseCurrentPopup();
// 			isOpen = false;
// 		}
		
// 		ImGui::EndPopup();
// 	}
	
// 	if (isOpen) {
// 		ImGui::OpenPopup("Начало игры");
// 	 }
// }