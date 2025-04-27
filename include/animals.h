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
enum class AnimalState { HEALTHY, SICK, DEAD };

// Посмотрел сюда 10 баллов дополниельно поставил.
class Animal {
public:
	std::string name;
	Climate climate;
	AnimalState state;
	Diet diet;
	int age;
	int weight;
	int price;
	int id; // ид 
	float happiness; // довольство у животных
	int daysWithoutFood; 

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
};