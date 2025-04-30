#pragma once
#include <string>

/*
В зоопарке есть работники 
(имя, должность, зарплата, количество обслуживаемых животных) 
((не забудьте добавить себя, товарищ Директор))

Кто-то кажется начал кашлять? В зоопарк пришёл тиаравирус, теперь некоторые животные болеют и их нужно лечить. 
Каждый день может заболеть одно случайное животное в том же вольере, если его не лечить, на следующий день каждое больное животное заразит ещё двух. 
Как только заражены будут больше половины животных в вольере - они начнут умирать в случайном порядке.
Каждое зараженное животное уменьшает популярность ежедневно (популярность -1 за каждое больное животное)
*/

enum class WorkerRole { DIRECTOR, VETERINARIAN, CLEANER, FOODMEN };

class Worker {
public:
	std::string name;
	WorkerRole role;
	int price;
	int served;
	bool isWorking; // Отображение работает ли рабочий.
	int id;

	Worker(std::string _name, int _price, WorkerRole _role, int _served, int _id) :
		name(_name), price(_price), role(_role), served(_served), isWorking(true), id(_id) {}
	
	// Отображение для интерфейса
	std::string getRoleString() const {
		switch (role) {
			case WorkerRole::DIRECTOR: return "Директор";
			case WorkerRole::VETERINARIAN: return "Ветеринар";
			case WorkerRole::CLEANER: return "Уборщик";
			case WorkerRole::FOODMEN: return "Кормилец";
			default: return "Неизвестно";
		}
	}

	void update() {
		isWorking = true;
	}
};