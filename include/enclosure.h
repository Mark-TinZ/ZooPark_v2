#pragma once
#include <vector>
#include <string>
#include "animals.h"

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
	std::string name;
	int capacity;
	Climate climate;
	std::vector<Animal*> animals;
	int dirty;
	int dailyCost;
	bool selected; // Выбран ли вольер в интерфейсе

	Enclosure(std::string _name, int _capacity, Climate _climate, int _dailyCost) : 
		name(_name), capacity(_capacity), climate(_climate), dailyCost(_dailyCost), dirty(0), selected(false) {}

	// Добавить животное
	bool addAnimal(Animal* animal) {
		if (animals.size() >= capacity) return false;
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

	// Перерасчет данных вольера.
	void update(bool haveCleaner) {
		dirty += 2;
		
		// Если есть уборщик, очищаем вольер
		if (haveCleaner && dirty > 0) {
			dirty -= 50;
			if (dirty < 0) dirty = 0;
		}

		// Уменьшение счастья у животных из-за грязи
		if (dirty > 50) {
			for (auto* animal : animals) {
				animal->happiness -= 5.0f;
				if (animal->happiness < 0) animal->happiness = 0;
			}
		}

		// Механика болезней
		if (!animals.empty() && rand() % 10 == 0) { // 10% шанс заражения
			int idx = rand() % animals.size();
			if (animals[idx]->state == AnimalState::HEALTHY) {
				animals[idx]->state = AnimalState::SICK;
			}
		}

		// Распространение болезни
		for (auto* animal : animals) {
			if (animal->state == AnimalState::SICK && rand() % 2 == 0) {
				for (auto* other : animals) {
					if (other->state == AnimalState::HEALTHY && rand() % 2 == 0) {
						other->state = AnimalState::SICK;
					}
				}
			}
		}

		// Смерть от болезни
		int sickCount = 0;
		for (auto* animal : animals) {
			if (animal->state == AnimalState::SICK) sickCount++;
		}

		if (sickCount > 0 && sickCount >= animals.size() / 2) {
			for (auto* animal : animals) {
				if (animal->state == AnimalState::SICK && rand() % 3 == 0) {
					animal->state = AnimalState::DEAD;
				}
			}
		}
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