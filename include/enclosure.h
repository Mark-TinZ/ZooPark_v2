#pragma once
#include <vector>
#include <string>
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
	int dirty;
	int dailyCost;
	bool selected; // Выбран ли вольер в интерфейсе
	std::string name;
	int capacity;
	Climate climate;
	std::vector<Animal*> animals;

	Enclosure(std::string _name, int _capacity, Climate _climate, int _dailyCost) : 
		name(_name), capacity(_capacity), climate(_climate), dailyCost(_dailyCost), dirty(0), selected(false) {}

	// Добавить животное
	bool addAnimal(Animal* animal) {
		if (getCountAnimal() >= capacity) return false;
		if (!animals.empty() && animal->diet != animals[0]->diet) return false;
		if (animal->climate != climate) return false;
		animals.push_back(animal);
		return true;
	}

	// Удалить животное
	void removeAnimal(Animal* animal) {
		for (auto it = animals.begin(); it != animals.end(); ++it) {
			if (*it == animal) {
				animals.erase(it);
				return;
			}
		}
	}

	// Нужно убрать вольер
	bool noodClean() {
		if (dirty > 10) return true;
		return false;
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
		dirty += 2;

		// Механика болезней
		if (!animals.empty() && rand() % 10 == 0) { // 10% шанс заражения
			int idx = rand() % animals.size();
			if (animals[idx]->state == AnimalState::HEALTHY) {
				animals[idx]->state = AnimalState::SICK;
			}
		}

		// Распространение болезни
		int totalAnimal = 0;
		int totalSickAnimal = 0;
		for (const auto* animal : animals) {
			totalAnimal += (animal->state == AnimalState::HEALTHY) ? 1 : 0;
			totalSickAnimal += (animal->state == AnimalState::SICK) ? 1 : 0;
		}
		totalAnimal += totalSickAnimal;
		// Если есть больное животное, то вирус распространяется на 2 других животных
		if (totalSickAnimal) {
			for (int i = 0; i < 2; i++) {
				for (auto* animal : animals) {
					if (animal->state == AnimalState::HEALTHY) {
						animal->state = AnimalState::SICK;
						break;
					}
				}
			}
		}


		// Смерть от болезни
		// Тут мы убиваем животное если в вольере болеет больше 50% с шансом 50 на 50
		if ((totalAnimal - totalSickAnimal) < totalSickAnimal) {
			for (auto* animal : animals) {
				if (animal->state == AnimalState::SICK && rand() % 2 == 0) {
					ConsoleCout() << "ID: " << animal->id << " | Имя: " << animal->name << " умерло.\n";
					animal->state = AnimalState::DEAD;
				}
			}
		}
	}

	int getCountAnimal() const {
		int countAnimal = 0;
		for (const auto* animal : animals) {
			if (animal->state != AnimalState::DEAD && animal->state != AnimalState::SELL) {
				countAnimal++;
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