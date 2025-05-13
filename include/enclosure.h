#pragma once
#include <vector>
#include <string>
#include <memory>
#include "animals.h"
#include "console.h"

/*
В зоопарке есть разные вольеры (количество животных которые могут жить в вольере, тип животных, климат, расходы в день)

Делегация с Суперземли TOI-1452 b очень расстроена отсутствием водных видов животных. 
Они великодушно высылают вам своих самых интересных животных живущих в воде. 
Придется сделать так чтобы водоплавающим был выделен свой участок зоопарка наравне с хищниками и травоядными. 
Тогда жители TOI-1452 b будут ездить к вам с большим удовольствием. 
(каждый аквоид х2 множитель посетителей и денег, но х2 содержание и требуют отдельный вольер)
*/

class Enclosure {
public:
	int id;							// Ид.
	int dirty;						// Загрязнение вольера
	int dailyCost;					// Стоимость обслуживания
	bool selected;					// Выбран ли вольер в интерфейсе
	std::string name;				// Название вольера
	int capacity;					// Вместимость
	Climate climate;				// Климат: Умереный, Тропики, Арктика, Водный
	// std::vector<Animal*> animals;	// Животные которые живут в вольере

	std::vector<std::weak_ptr<Animal>> animals;

	Enclosure(std::string _name, int _capacity, Climate _climate, int _dailyCost) : 
		name(_name), capacity(_capacity), climate(_climate), dailyCost(_dailyCost), dirty(0), selected(false) {}

	// Добавить животное
	bool addAnimal(const std::shared_ptr<Animal>& animal) {
		if (getCountAnimal() >= capacity) return false;

		// Проверяем совместимость с другими животными
		if (!animals.empty()) {
			// Получаем первое живое животное для проверки диеты
			for (const auto& weakAnimal : animals) {
				if (auto firstAnimal = weakAnimal.lock()) {
					if (animal->diet != firstAnimal->diet) return false;
					break;
				}
			}
		}

		if (animal->climate != climate) return false;
		animals.push_back(animal); // Теперь это создает weak_ptr из shared_ptr
		return true;
	}

	// Удалить животное
	void removeAnimal(const std::shared_ptr<Animal>& animal) {
		animals.erase(
			std::remove_if(animals.begin(), animals.end(), 
				[&animal](const std::weak_ptr<Animal>& weakPtr) {
					auto sharedPtr = weakPtr.lock();
					return !sharedPtr || sharedPtr == animal;
				}
			),
			animals.end()
		);
	}

	// Очистка недействительных указателей
	void cleanupExpiredAnimals() {
		animals.erase(
			std::remove_if(animals.begin(), animals.end(),
				[](const std::weak_ptr<Animal>& weakPtr) {
					return weakPtr.expired();
				}
			),
			animals.end()
		);
	}

	// Нужно убрать вольер
	bool noodClean() {
		return dirty > 10;
	}
	
	// Убираемся в вольере
	void cleanEnclosure() {
		dirty -= std::min(dirty, 50);
	}

	std::string getName() const {
		return name;
	}

	// Перерасчет данных вольера.
	void update() {
		// Сначала удаляем недействительные указатели
		cleanupExpiredAnimals();

		dirty += 2;

		// Механика болезней
		std::vector<std::shared_ptr<Animal>> validAnimals;
		for (const auto& weakAnimal : animals) {
			if (auto animal = weakAnimal.lock()) {
				validAnimals.push_back(animal);
			}
		}
		
		if (!validAnimals.empty() && rand() % 10 == 0) { // 10% шанс заражения
			int idx = rand() % validAnimals.size();
			if (validAnimals[idx]->state == AnimalState::HEALTHY) {
				validAnimals[idx]->state = AnimalState::SICK;
			}
		}

		// Распространение болезни
		int totalAnimal = 0;
		int totalSickAnimal = 0;
		for (const auto& animal : validAnimals) {
			totalAnimal += (animal->state == AnimalState::HEALTHY) ? 1 : 0;
			totalSickAnimal += (animal->state == AnimalState::SICK) ? 1 : 0;
		}
		totalAnimal += totalSickAnimal;

		// Если есть больное животное, то вирус распространяется на 2 других животных
		if (totalSickAnimal) {
			int infected = 0;
			for (auto& animal : validAnimals) {
				if (infected >= 2) break;
				if (animal->state == AnimalState::HEALTHY) {
					animal->state = AnimalState::SICK;
					infected++;
				}
			}
		}

		// Смерть от болезни
		// Тут мы убиваем животное если в вольере болеет больше 50% с шансом 50 на 50
		if ((totalAnimal - totalSickAnimal) < totalSickAnimal) {
			for (auto& animal : validAnimals) {
				if (animal->state == AnimalState::SICK && rand() % 2 == 0) {
					ConsoleCout() << "ID: " << animal->id << " | Имя: " << animal->name << " умерло.\n";
					animal->state = AnimalState::DEAD;
				}
			}
		}
	}

	int getCountAnimal() const {
		int countAnimal = 0;
		for (const auto& weakAnimal : animals) {
			if (auto animal = weakAnimal.lock()) {
				if (animal->state != AnimalState::DEAD && animal->state != AnimalState::SELL) {
					countAnimal++;
				}
			}
		}
		return countAnimal;
	}
	
	// Отображение климота вольера
	std::string getClimateString() const {
		switch (climate) {
			case Climate::TROPIC: return "Тропики";
			case Climate::CONTINENT: return "Умеренный";
			case Climate::ARCTIC: return "Полярный";
			case Climate::AQUATIC: return "Водный";
			default: return "Неизвестно";
		}
	}
};