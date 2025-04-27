#pragma once
#include <string>
#include <vector>

/*
В зоопарке есть животные (имя, возраст, вес, предпочтительный климат, цена)
Некоторые животные являются хищниками, некоторые травоядными. 
Травоядные не могут жить с хищниками в одном вольере.
*/

enum class Diet { HERBIVORES, PREDATORS };
enum class Climate { TROPIC, CONTINENT, ARCTIC, AQUATIC };
enum class AnimalState { HEALTHY, SICK, DEAD, SELL };

// Посмотрел сюда 10 баллов дополниельно поставил.
class Animal {
public:
	int id;					// индификатор
	int age;				// Возраст
	Diet diet;				// Диета
	int price;				// Цеда
	int weight;				// Вес
	float happiness;		// довольство у животных
	Climate climate;		// Климат
	std::string name;		// Имя
	AnimalState state;		// Состояние
	int eatingFood = 1;		// Сколько кушает
	int daysWithoutFood;	// Дней без еды

	// Задаем требуемые значения
	Animal(std::string _name, int _age, int _weight, int _price, 
		   Diet _diet, Climate _climate, AnimalState _state, int _id) :
		name(_name), age(_age), weight(_weight), price(_price),
		diet(_diet), climate(_climate), state(_state), id(_id),
		happiness(100.0f), daysWithoutFood(0) {}
		
	// Возвращаем название климата.
	std::string getClimateString() const {
		switch (climate) {
			case Climate::TROPIC: return "Тропики";
			case Climate::CONTINENT: return "Умереный";
			case Climate::ARCTIC: return "Арктика";
			case Climate::AQUATIC: return "Водный";
			default: return "Неизвестно";
		}
	}
	
	// Возвращает что жрет животное траву или мясо
	std::string getDietString() const {
		return diet == Diet::HERBIVORES ? "Травоядный" : "Хищник";
	}
	
	// Возвращает состояние животного
	std::string getStateString() const {
		switch (state) {
			case AnimalState::HEALTHY: return "Здоров";
			case AnimalState::SICK: return "Болен";
			case AnimalState::DEAD: return "Мертв";
			default: return "Неизвестно";
		}
	}

	void update() {
		age++;
		if (rand() % 10 == 0) {
			state = AnimalState::SICK;
		}
		if (age > 30 && rand() % 20 == 0) {
			state = AnimalState::DEAD;
		}
	}
};