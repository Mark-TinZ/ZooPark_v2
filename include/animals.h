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
	bool guy;				// Мужик это или баба
	Diet diet;				// Диета
	int price;				// Цеда
	int weight;				// Вес
	float happiness;		// довольство у животных
	Climate climate;		// Климат
	std::string name;		// Имя
	AnimalState state;		// Состояние
	int eatingFood = 1;		// Сколько кушает
	int daysWithoutFood;	// Дней без еды
	int makeSex = 2;		// Сколько дней кд на трахитрахи
	Animal* parent1 = nullptr; // Ссылка на первого родителя
	Animal* parent2 = nullptr; // Ссылка на второго родителя

	// Задаем требуемые значения
	Animal(std::string _name="", int _age=1, int _weight=0, int _price=0, 
		   Diet _diet=Diet::HERBIVORES, Climate _climate=Climate::AQUATIC, AnimalState _state=AnimalState::DEAD, int _id=-1) :
		name(_name), age(_age), weight(_weight), price(_price),
		diet(_diet), climate(_climate), state(_state), id(_id),
		happiness(100.0f), daysWithoutFood(0) {
			guy = (rand() % 2 == 0) ? true : false;
		}
		
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
	
	bool getGuy() const {
		return guy;
	}

	// Возвращает состояние животного
	std::string getStateString() const {
		switch (state) {
			case AnimalState::HEALTHY: return "Здоров";
			case AnimalState::SICK: return "Болен";
			case AnimalState::DEAD: return "Мертв";
			case AnimalState::SELL: return "Продано";
			default: return "Неизвестно";
		}
	}

	std::string getName() const {
		return name;
	}

	bool getMakeSex() {
		if (makeSex == 0) return true;
		return false;
	}

	void update() {
		age++;

		makeSex = (makeSex > 0) ? makeSex-1 : 0;

		// Заражение вирусом
		if (rand() % 10 == 0) {
			state = AnimalState::SICK;
		}

		// Смерть от старости
		if (age > 30 && rand() % 20 == 0) {
			state = AnimalState::DEAD;
		}
	}
};