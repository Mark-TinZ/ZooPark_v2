#pragma once
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "workers.h"
#include "animals.h"
#include "enclosure.h"
#include "console.h"
#include <iostream>
// TODO: сделать zoo.cpp
// Перебросить туда всй важное
/*
В зоопарке есть список показателей (еда, деньги, популярность)
еда тратится на животных (сколько съели = количество животных), 
если животное не покормить - оно может умереть(50% шанс).
еду можно покупать (+еда -деньги)
в зоопарк в зависимости от популярность приходят посетители (посетители = 2*популярность)
в зависимости от количества животных посетители приносят зоопарку деньги 
(деньги в день = количество посетителей * количество животных)
на популярность влияют различные факторы
		повысить рекламой (-деньги, +популярность)
		колеблется случайно (+-10% в день)
		*специфичные для варианта вещи*

*/

class Zoo {
public:
	std::string name = "";
	int day;
	int food;
	int money;
	int popularity;
	std::vector<Animal> animals;
	std::vector<Worker> workers;
	std::vector<Enclosure> enclosures;
	
	// История для графиков
	std::vector<float> moneyHistory;
	std::vector<float> popularityHistory;
	std::vector<float> visitorsHistory;
	std::vector<float> animalCountHistory;
	
	// Переменные для ImGui
	int selectedAnimalIndex;
	int selectedEnclosureIndex;
	int selectedWorkerIndex;
	std::string statusMessage;

	Zoo(std::string _name, int start_money) : 
		name(_name), day(1), food(100), money(start_money), popularity(50),
		selectedAnimalIndex(-1), selectedEnclosureIndex(-1), selectedWorkerIndex(-1) {
		srand(static_cast<unsigned int>(time(0))); // Для рандома подготавливаем генератор
		workers.push_back(Worker("Дядя Саша", 100, WorkerRole::DIRECTOR, 0, workers.size() + 1)); // Создаем главного
		
		// Инициализация истории
		moneyHistory.push_back(money);
		popularityHistory.push_back(popularity);
		visitorsHistory.push_back(getVisitors());
		animalCountHistory.push_back(0);
	}

	int getVisitors() const {
		return 2 * popularity;
	}

	void setZooName(std::string _name) {
		name = _name;
	}

	std::string getZooName() {
		return name;
	}

	bool buyAnimal(const std::string& name, int animalType, int age) {
		if (age < 0) {
			ConsoleCout() << "Возраст животного не может быть отрицательным!" << std::endl;
			return false;
		}
		Diet diet;
		Climate climate;
		int weight, price;
		
		// Настройка параметров животного на основе типа
		switch (animalType) {
			case 0: // Кошка
				diet = Diet::PREDATORS;
				climate = Climate::CONTINENT;
				weight = 5;
				price = 200 + age * 30;
				break;
			case 1: // Пингвин
				diet = Diet::PREDATORS;
				climate = Climate::ARCTIC;
				weight = 30;
				price = 800 + age * 20;
				break;
			case 2: // Собака
				diet = Diet::PREDATORS;
				climate = Climate::CONTINENT;
				weight = 20;
				price = 250 + age * 25;
				break;
			case 3: // Белый медведь
				diet = Diet::PREDATORS;
				climate = Climate::ARCTIC;
				weight = 600;
				price = 1500 + age * 50;
				break;
			case 4: // Жираф
				diet = Diet::HERBIVORES;
				climate = Climate::TROPIC;
				weight = 800;
				price = 1200 + age * 40;
				break;
			case 5: // Слон
				diet = Diet::HERBIVORES;
				climate = Climate::CONTINENT;
				weight = 5000;
				price = 2000 + age * 60;
				break;
			default:
				return false;
		}
		
		if (money < price) {
			ConsoleCout() << "Недостаточно денег для покупки животного!" << std::endl;
			return false;
		}
		
		// Проверяем, есть ли подходящий вольер перед покупкой
		bool hasSuitableEnclosure = false;
		for (const auto& enclosure : enclosures) {
			if (enclosure.climate == climate && enclosure.animals.size() < enclosure.capacity) {
				if (enclosure.animals.empty() || enclosure.animals[0]->diet == diet) {
					hasSuitableEnclosure = true;
					break;
				}
			}
		}

		if (!hasSuitableEnclosure) {
			ConsoleCout() << "Невозможно купить животное: нет подходящего вольера!" << std::endl;
			return false;
		}

		// Создаем животное
		Animal newAnimal(name, age, weight, price, diet, climate, AnimalState::HEALTHY, animals.size() + 1);
		animals.push_back(newAnimal);
		money -= price;

		// Размещаем животное в подходящем вольере
		for (auto& enclosure : enclosures) {
			if (enclosure.climate == climate && enclosure.getCountAnimal() < enclosure.capacity) {
				if (enclosure.animals.empty() || enclosure.animals[0]->diet == diet) {
					enclosure.addAnimal(&animals.back());
					ConsoleCout() << "Животное куплено и помещено в вольер!" << std::endl;
					break;
				}
			}
		}
		
		return true;
	}
	
	// Создаем животное после чпокания
	Animal birthAnimal(const Animal& animal1, const Animal& animal2) {
		if (animal1.age > 10 && animal2.age > 10 
			&& animal1.guy != animal2.guy
			&& animal1.diet == animal2.diet
			&& animal1.climate == animal2.climate
			&& animal1.state == AnimalState::HEALTHY
			&& animal2.state == AnimalState::HEALTHY) {
			return Animal("Рожденный_" + animal1.name + "_" + animal2.name, 1, animal1.weight, 3000, animal1.diet, animal1.climate, AnimalState::HEALTHY, animals.size() + 1);
		}
		return Animal(); // Возвращаем класс по умолчанию
	}

	// Функция процесса чпоканья
	bool startSex(const int id1, const int id2 ) {
		const Animal* animal1 = nullptr;
		const Animal* animal2 = nullptr; 
		for (size_t i = 0; i < animals.size(); i++) {
			if (id1 == i) animal1 = &animals[i];
			if (id2 == i) animal2 = &animals[i];
		}

		// Проверка что животные есть
		if (!animal1 || !animal2) {
			ConsoleCout() << "Ошибка: одно из животных не найдено!" << std::endl;
			return false;
		}

		try {
			int offspringCount = (rand() % 4) + 1;
			for (int i = 0; i < offspringCount; i++) {
				Animal newAnimal = birthAnimal(*animal1, *animal2);
				if (newAnimal.name.empty()) {
					ConsoleCout() << "Условия для рождения животного не выполнены!" << std::endl;
					return false;
				}

				// Проверяем, есть ли подходящий вольер
				bool hasSuitableEnclosure = false;
				for (auto& enclosure : enclosures) {
					if (enclosure.climate == newAnimal.climate && enclosure.animals.size() < enclosure.capacity) {
						if (enclosure.animals.empty() || enclosure.animals[0]->diet == newAnimal.diet) {
							enclosure.addAnimal(&newAnimal);
							hasSuitableEnclosure = true;
							break;
						}
					}
				}

				if (!hasSuitableEnclosure) {
					ConsoleCout() << "Невозможно разместить новорожденное животное: нет подходящего вольера!" << std::endl;
					return false;
				}

				// Добавляем животное в общий список
				animals.push_back(newAnimal);
			}
			return true;
		} catch (const std::exception& e) {
			ConsoleCout() << "Ошибка при размножении: " << e.what() << std::endl;
			return false;
		}
	}

	bool buildEnclosure(const std::string& name, int climateType, int capacity) {
		Climate climate;
		int cost, dailyCost;
		
		switch (climateType) {
			case 0: // Тропический
				climate = Climate::TROPIC;
				cost = 1000 + capacity * 100;
				dailyCost = 50 + capacity * 10;
				break;
			case 1: // Умеренный
				climate = Climate::CONTINENT;
				cost = 1200 + capacity * 100;
				dailyCost = 60 + capacity * 10;
				break;
			case 2: // Полярный
				climate = Climate::ARCTIC;
				cost = 1500 + capacity * 100;
				dailyCost = 70 + capacity * 10;
				break;
			default:
				return false;
		}
		
		if (money < cost) {
			ConsoleCout() << "Недостаточно денег для строительства вольера!" << std::endl;
			return false;
		}
		
		enclosures.push_back(Enclosure(name, capacity, climate, dailyCost));
		money -= cost;
		ConsoleCout() << "Вольер построен!" << std::endl;
		return true;
	}
	
	bool hireWorker(int workerType) {
		WorkerRole role;
		std::string name;
		int price, served;

		// Константный массив имен
		const std::vector<std::string> names = {
			"Виктор Цой", "Егор Летов", "Юрий Клинских", "Михаил Горшенёв", "Набиулина", 
			"Мизулина", "Шаман", "Марат", "Джек Салли", "Греф", "Дверь Киркорова", "Лепс"
		};
		
		name = names[rand() % names.size()];

		switch (workerType) {
			case 0: // Ветеринар
				role = WorkerRole::VETERINARIAN;
				// name = "Ветеринар";
				price = 500;
				served = 2;
				break;
			case 1: // Уборщик
				role = WorkerRole::CLEANER;
				// name = "Уборщик";
				price = 300;
				served = 1;
				break;
			case 2: // Кормилец
				role = WorkerRole::FOODMEN;
				// name = "Кормилец";
				price = 200;
				served = 50;
				break;
			default:
				return false;
		}
		
		if (money < price) {
			ConsoleCout() << "Недостаточно денег для найма работника!" << std::endl;
			return false;
		}
		int id = workers.size() + 1;
		workers.push_back(Worker(name, price, role, served, id));
		money -= price;
		ConsoleCout() << "id: " << std::to_string(id) << " - " << name << " нанят!" << std::endl;
		return true;
	}
	
	bool buyFood(int amount) {
		int cost = amount * 10;
		
		if (money < cost) {
			ConsoleCout() << "Недостаточно денег для покупки еды!" << std::endl;
			return false;
		}
		
		food += amount;
		money -= cost;
		ConsoleCout() << "Еда куплена!" << std::endl;
		return true;
	}
	
	bool orderAdvertising(int budget) {
		if (money < budget) {
			ConsoleCout() << "Недостаточно денег для рекламы!" << std::endl;
			return false;
		}
		
		int popularityGain = budget / 100;
		popularity += popularityGain;
		money -= budget;
		
		if (popularity > 100) popularity = 100;
		
		ConsoleCout() << "Рекламная кампания проведена успешно! Популярность +" << std::to_string(popularityGain) << std::endl;
		return true;
	}
	
	void sellAnimal(int index) {
		if (index < 0 || index >= animals.size()) {
			ConsoleCout() << "Неверный индекс животного!" << std::endl;
			return;
		}
		
		Animal& animal = animals[index];
		
		// Находим вольер животного и удаляем его оттуда
		for (auto& enclosure : enclosures) {
			auto it = std::find(enclosure.animals.begin(), enclosure.animals.end(), &animal);
			if (it != enclosure.animals.end()) {
				enclosure.animals.erase(it);
				break;
			}
		}
		
		// Получаем половину цены
		money += animal.price / 2;
		
		// Удаляем животное
		animals.erase(animals.begin() + index);
		
		// Обновляем индексы всех оставшихся животных
		for (size_t i = 0; i < animals.size(); i++) {
			animals[i].id = i + 1;
		}
		
		ConsoleCout() << "Животное продано!" << std::endl;
	}
	
	void dismissWorker(int index) {
		if (index < 0 || index >= workers.size() || workers[index].role == WorkerRole::DIRECTOR) {
			ConsoleCout() << "Нельзя уволить директора или неверный индекс!" << std::endl;
			return;
		}
		
		workers.erase(workers.begin() + index);
		ConsoleCout() << "Работник уволен!" << std::endl;
	}

	int getCountAnimal() const {
		int totalAnimal = 0;
		for (const auto& enclosure : enclosures) {
			for (const auto& animal : animals) totalAnimal += (animal.state != AnimalState::DEAD) ? 1 : 0;
		}
		return totalAnimal;
	}

	int getCountSickAnimal() const {
		int totalSickAnimal = 0;
		for (const auto& enclosure : enclosures) {
			for (const auto& animal : animals) {
				if (animal.state == AnimalState::HEALTHY || animal.state == AnimalState::SICK) {
					totalSickAnimal += 1;
				}
			}
		}
		return totalSickAnimal;
	}
	
	bool healAnimal(int animalIndex) {
		if (animalIndex < 0 || animalIndex >= animals.size()) {
			return false;
		}
		
		if (animals[animalIndex].state == AnimalState::SICK) {
			// Проверяем, есть ли свободный ветеринар
			for (auto& worker : workers) {
				if (worker.role == WorkerRole::VETERINARIAN && worker.isWorking) {
					animals[animalIndex].state = AnimalState::HEALTHY;
					worker.isWorking = false; // Ветеринар занят до конца дня
					return true;
				}
			}
			ConsoleCout() << "Нет свободных ветеринаров!" << std::endl;
		}
		return false;
	}
	
	void nextDay() {
		day++;
		
		// Сбрасываем состояние работников
		for (auto& worker : workers) {
			worker.update();
		}
		
		// Обновить животных
		for (auto& animal : animals) {
			animal.update();
		}

		// Расходы
		int expenses = 0;
		ConsoleCout() << "Расходы зоопарка " << name << std::endl;
		// Зарплата работникам
		for (auto& worker : workers) expenses += worker.price;
		ConsoleCout() << "Зарплаты: " << expenses << std::endl;
		money -= expenses;
		expenses = 0;
		// Расходы на содержание вольера
		for (auto& enclosure : enclosures) expenses += enclosure.dailyCost;
		ConsoleCout() << "Расходы на содержание вольеров: " << expenses << std::endl;
		money -= expenses;
		expenses = 0;

		// Считаем количество животных больных и здоровых
		int totalAnimal = getCountAnimal();
		int totalSickAnimal = getCountSickAnimal();

		// Считаем сколько еды хавают животные
		int expensesFood = 0;
		for (const auto& enclosure : enclosures) {
			for (const auto& animal : animals) {
				if (animal.state == AnimalState::HEALTHY || animal.state == AnimalState::SICK) {
					expensesFood += animal.eatingFood;
				}
			}
		}

		// Кормильщик экономит еду
		for (auto& worker : workers) {
			if (worker.role == WorkerRole::FOODMEN && worker.isWorking) {
				expensesFood = expensesFood * 0.95;
				worker.isWorking = false;
			}
		}
		ConsoleCout() << "Кормление животных: " << expensesFood << std::endl;
		
		// Кормление животных
		if (food >= expensesFood) {
			food -= expensesFood;
			
			// Сбрасываем счетчик дней без еды
			for (auto& animal : animals) {
				animal.daysWithoutFood = 0;
				animal.happiness = std::max(100.f, animal.happiness+10);
			}
		} else {
			// Недостаточно еды, некоторые животные не поедят
			int fedCount = food;
			food = 0;
			
			for (size_t i = 0; i < animals.size(); i++) {
				if (i < fedCount) {
					animals[i].daysWithoutFood = 0;
				} else {
					animals[i].daysWithoutFood++;
					animals[i].happiness -= 10.0f;
					
					// Если животное не ело 3 дня, оно может умереть с вероятностью 10%
					if (animals[i].daysWithoutFood >= 3 && rand() % 10 == 0) {
						animals[i].state = AnimalState::DEAD;
					}
				}
			}
		}
		
		// Подсчитываем количество больных и мертвых животных
		int deadCount = 0;
		int sickCount = 0;
		for (const auto& animal : animals) {
			if (animal.state == AnimalState::DEAD) {
				deadCount++;
			} else if (animal.state == AnimalState::SICK) {
				sickCount++;
			}
		}
		
		// Обновляем вольеры
		int cleanerCount = 0;
		int dirtyZoo = 0;
		for (auto& worker : workers) {
			if (worker.role == WorkerRole::CLEANER) cleanerCount++;
		}

		for (auto& enclosure : enclosures) {
			enclosure.update();

			if (enclosure.noodClean() && cleanerCount > 0) {
				enclosure.cleanEnclosure();
				cleanerCount--;
			}
			dirtyZoo += enclosure.dirty;
		}
		ConsoleCout() << "Общее загрязнение зоопарка: " << dirtyZoo << std::endl;
		
		// Доход от посетителей
		int visitors = getVisitors();
		int healthyAnimalCount = animals.size() - deadCount;
		money += visitors * healthyAnimalCount;
		
		// Расходы на зарплату
		for (auto& worker : workers) {
			money -= worker.price;
		}
		
		// Расходы на содержание вольеров
		for (auto& enclosure : enclosures) {
			money -= enclosure.dailyCost;
		}
		
		// Влияние болезней и смертей на популярность
		popularity -= sickCount;
		popularity -= deadCount * 3;
		
		// Случайное колебание популярности
		popularity += (rand() % 21) - 10; // +-10
		
		// Ограничение популярности
		if (popularity < 10) popularity = 10;
		if (popularity > 100) popularity = 100;
		
		// Удаляем мертвых животных
		for (auto& enclosure : enclosures) {
			enclosure.animals.erase(
				std::remove_if(enclosure.animals.begin(), enclosure.animals.end(),
							   [](Animal* animal) { return animal->state == AnimalState::DEAD; }),
				enclosure.animals.end());
		}
		
		// Удаляем мертвых животных из общего списка
		for (int i = animals.size() - 1; i >= 0; i--) {
			if (animals[i].state == AnimalState::DEAD) {
				animals.erase(animals.begin() + i);
			}
		}
		
		// Обновляем историю
		moneyHistory.push_back(money);
		popularityHistory.push_back(popularity);
		visitorsHistory.push_back(visitors);
		animalCountHistory.push_back(animals.size());
		
		// Проверка на банкротство
		if (money < 0) {
			ConsoleCout() << "Вы банкрот! Игра окончена." << std::endl;
		} else if (day >= 30) {
			ConsoleCout() << "Поздравляем! Вы успешно управляли зоопарком 30 дней!" << std::endl;
		} else {
			ConsoleCout() << "День " + std::to_string(day) + " завершен!" << std::endl;
		}
	}
};
