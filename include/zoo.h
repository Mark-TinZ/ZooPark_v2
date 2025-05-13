#pragma once
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <memory>
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
	std::string name = "";							// Название зоопарка
	int day;										// Сколько дней работает зоопарк
	int food;										// Сколько запасов еды в зоопарке
	int money;										// Сколько денег в зоопарке
	int popularity;									// Популярность зоопарка
	std::vector<std::shared_ptr<Animal>> animals;	// Все животные которые есть в зоопарке
	std::vector<Worker> workers;					// Все работники в зоопарке
	std::vector<Enclosure> enclosures;				// Все вольеры в зоопарке
	
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
			if (enclosure.climate == climate && enclosure.getCountAnimal() < enclosure.capacity) {
				bool allAnimalsMatchDiet = true;
				for (const auto& weakAnimal : enclosure.animals) {
					// Проверяем, что указатель действителен
					if (auto animal = weakAnimal.lock()) {
						if (animal->diet != diet) {
							allAnimalsMatchDiet = false;
							break;
						}
					}
				}
				if (enclosure.animals.empty() || allAnimalsMatchDiet) {
					hasSuitableEnclosure = true;
					break;
				}
			}
		}
		
		if (!hasSuitableEnclosure) {
			ConsoleCout() << "Нет подходящего вольера для этого животного!" << std::endl;
			return false;
		}
		
		// Создаем новое животное
		auto newAnimal = std::make_shared<Animal>();
		newAnimal->name = name;
		newAnimal->age = age;
		newAnimal->weight = weight;
		newAnimal->diet = diet;
		newAnimal->climate = climate;
		newAnimal->state = AnimalState::HEALTHY;
		newAnimal->id = animals.empty() ? 1 : animals.back()->id + 1;
		
		// Добавляем животное в зоопарк
		animals.push_back(newAnimal);
		
		// Поиск подходящего вольера и добавление животного туда
		for (auto& enclosure : enclosures) {
			if (enclosure.climate == climate && enclosure.getCountAnimal() < enclosure.capacity) {
				bool canAddToEnclosure = true;
				for (const auto& weakAnimal : enclosure.animals) {
					if (auto animal = weakAnimal.lock()) {
						if (animal->diet != diet) {
							canAddToEnclosure = false;
							break;
						}
					}
				}
				
				if (enclosure.animals.empty() || canAddToEnclosure) {
					enclosure.addAnimal(newAnimal);
					break;
				}
			}
		}
		
		// Вычитаем стоимость животного
		money -= price;
		
		ConsoleCout() << "Животное " << name << " успешно куплено и помещено в вольер!" << std::endl;
		return true;
	}

	// Проверяем, могут ли животные размножаться
	bool canBreed(const std::shared_ptr<Animal>& animal1, const std::shared_ptr<Animal>& animal2) {
		if (!animal1 || !animal2) return false;
		
		return (animal1->age > 10 && animal2->age > 10 
				&& animal1->guy != animal2->guy
				&& animal1->diet == animal2->diet
				&& animal1->climate == animal2->climate
				&& animal1->state == AnimalState::HEALTHY
				&& animal2->state == AnimalState::HEALTHY);
	}
	
	// Создаем новое животное в результате чпокания
	std::shared_ptr<Animal> createOffspring(const std::shared_ptr<Animal>& animal1, const std::shared_ptr<Animal>& animal2) {
		auto offspring = std::make_shared<Animal>();
		offspring->name = "Рожденный_" + animal1->name + "_" + animal2->name;
		offspring->age = 1;
		offspring->weight = animal1->weight; // Можно использовать среднее значение если нужно
		offspring->diet = animal1->diet;
		offspring->climate = animal1->climate;
		offspring->state = AnimalState::HEALTHY;
		offspring->id = animals.empty() ? 1 : animals.back()->id + 1;
		offspring->guy = (rand() % 2 == 0) ? animal1->guy : animal2->guy; // Случайный пол
		
		return offspring;
	}

	// Функция процесса размножения
	bool startBreeding(const int id1, const int id2) {
		std::shared_ptr<Animal> animal1 = nullptr;
		std::shared_ptr<Animal> animal2 = nullptr;
		
		// Находим животных по id
		for (const auto& animal : animals) {
			if (animal->id == id1) animal1 = animal;
			if (animal->id == id2) animal2 = animal;
		}
		
		// Проверка что животные найдены
		if (!animal1 || !animal2) {
			ConsoleCout() << "Ошибка: одно из животных не найдено!" << std::endl;
			return false;
		}
		
		// Проверяем, могут ли животные размножаться
		if (!canBreed(animal1, animal2)) {
			ConsoleCout() << "Условия для размножения не выполнены!" << std::endl;
			return false;
		}
		
		try {
			// Определяем количество потомства (от 1 до 4)
			int offspringCount = (rand() % 4) + 1;
			int successfulBirths = 0;
			
			for (int i = 0; i < offspringCount; i++) {
				// Создаем нового детеныша
				auto newAnimal = createOffspring(animal1, animal2);
				
				// Проверяем, есть ли подходящий вольер
				bool hasSuitableEnclosure = false;
				for (auto& enclosure : enclosures) {
					if (enclosure.climate == newAnimal->climate && enclosure.getCountAnimal() < enclosure.capacity) {
						bool canAddToEnclosure = true;
						
						// Проверяем совместимость с существующими животными в вольере
						for (const auto& weakAnimal : enclosure.animals) {
							if (auto existingAnimal = weakAnimal.lock()) {
								if (existingAnimal->diet != newAnimal->diet) {
									canAddToEnclosure = false;
									break;
								}
							}
						}
						
						if (enclosure.animals.empty() || canAddToEnclosure) {
							// Сначала добавляем животное в зоопарк
							animals.push_back(newAnimal);
							
							// Затем добавляем в вольер
							enclosure.addAnimal(newAnimal);
							
							hasSuitableEnclosure = true;
							successfulBirths++;
							ConsoleCout() << "Родилось новое животное: " << newAnimal->name << " (ID: " << newAnimal->id << ")" << std::endl;
							break;
						}
					}
				}
				
				if (!hasSuitableEnclosure) {
					ConsoleCout() << "Невозможно разместить новорожденное животное: нет подходящего вольера!" << std::endl;
					// Прерываем цикл, если мы не можем разместить больше животных
					break;
				}
			}
			
			if (successfulBirths > 0) {
				ConsoleCout() << "Успешно родилось " << successfulBirths << " животных!" << std::endl;
				return true;
			} else {
				ConsoleCout() << "Не удалось разместить ни одного новорожденного животного." << std::endl;
				return false;
			}
			
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
	
	void sellAnimal(int animalId) {
		// Находим животное по ID (не по индексу)
		auto it = std::find_if(animals.begin(), animals.end(),
							[animalId](const std::shared_ptr<Animal>& animal) {
								return animal->id == animalId;
							});
		
		if (it == animals.end()) {
			ConsoleCout() << "Животное с ID " << animalId << " не найдено!" << std::endl;
			return;
		}
		
		std::shared_ptr<Animal> animal = *it;
		
		// Находим вольер животного и удаляем его оттуда
		for (auto& enclosure : enclosures) {
			enclosure.removeAnimal(animal); // Используем метод removeAnimal, который мы ранее модифицировали
		}
		
		// Получаем половину цены
		money += animal->price / 2;

		// Указываем что животное продано, но сохраняем его для статистики
		animal->state = AnimalState::SELL;
		
		ConsoleCout() << "Животное " << animal->name << " (ID: " << animal->id << ") продано за " 
					<< (animal->price / 2) << " денег!" << std::endl;
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
			for (const auto& animal : animals) totalAnimal += (animal->state != AnimalState::DEAD) ? 1 : 0;
		}
		return totalAnimal;
	}

	int getCountSickAnimal() const {
		int totalSickAnimal = 0;
		for (const auto& enclosure : enclosures) {
			for (const auto& animal : animals) {
				if (animal->state == AnimalState::HEALTHY || animal->state == AnimalState::SICK) {
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
		
		if (animals[animalIndex]->state == AnimalState::SICK) {
			// Проверяем, есть ли свободный ветеринар
			for (auto& worker : workers) {
				if (worker.role == WorkerRole::VETERINARIAN && worker.isWorking) {
					animals[animalIndex]->state = AnimalState::HEALTHY;
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
			animal->update();  // Используем -> вместо . для shared_ptr
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
		for (const auto& animal : animals) {
			if (animal->state == AnimalState::HEALTHY || animal->state == AnimalState::SICK) {
				expensesFood += animal->eatingFood;
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
				animal->daysWithoutFood = 0;
				animal->happiness = std::max(100.f, animal->happiness+10);
			}
		} else {
			// Недостаточно еды, некоторые животные не поедят
			int fedCount = food;
			food = 0;
			
			for (size_t i = 0; i < animals.size(); i++) {
				if (i < fedCount) {
					animals[i]->daysWithoutFood = 0;
				} else {
					animals[i]->daysWithoutFood++;
					animals[i]->happiness -= 10.0f;
					
					// Если животное не ело 3 дня, оно может умереть с вероятностью 10%
					if (animals[i]->daysWithoutFood >= 3 && rand() % 10 == 0) {
						animals[i]->state = AnimalState::DEAD;
					}
				}
			}
		}
		
		// Подсчитываем количество больных и мертвых животных
		int deadCount = 0;
		int sickCount = 0;
		for (const auto& animal : animals) {
			if (animal->state == AnimalState::DEAD) {
				deadCount++;
			} else if (animal->state == AnimalState::SICK) {
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
		int healthyAnimalCount = totalAnimal - deadCount;  // Используем totalAnimal вместо animals.size()
		money += visitors * healthyAnimalCount;
		
		// Расходы на зарплату уже учтены выше, убираем дублирование
		
		// Влияние болезней и смертей на популярность
		popularity -= sickCount;
		popularity -= deadCount * 3;
		
		// Случайное колебание популярности
		popularity += (rand() % 21) - 10; // +-10
		
		// Ограничение популярности
		if (popularity < 10) popularity = 10;
		if (popularity > 100) popularity = 100;
		
		// Очищаем слабые указатели на мертвых животных в вольерах
		for (auto& enclosure : enclosures) {
			enclosure.cleanupExpiredAnimals();  // Используем нашу новую функцию для очистки
			
			// Удаляем мертвых животных из вольера
			enclosure.animals.erase(
				std::remove_if(enclosure.animals.begin(), enclosure.animals.end(),
							[](const std::weak_ptr<Animal>& weakAnimal) { 
								auto animal = weakAnimal.lock();
								return !animal || animal->state == AnimalState::DEAD; 
							}),
				enclosure.animals.end());
		}
		
		// // Удаляем мертвых животных из общего списка
		// animals.erase(
		// 	std::remove_if(animals.begin(), animals.end(),
		// 				[](const std::shared_ptr<Animal>& animal) { 
		// 					return animal->state == AnimalState::DEAD; 
		// 				}),
		// 	animals.end());
		
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
