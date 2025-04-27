// Больше не нужен этот древний код

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "zoo.h"
#include "animals.h"
#include "enclosure.h"
#include "workers.h"

void displayMenu(Zoo& zoo) {
    while (true) {
        std::cout << "\n=== День " << zoo.day << " ===\n";
        std::cout << "Название: " << zoo.name << "\n";
        std::cout << "Деньги: " << zoo.money << "\n";
        std::cout << "Еда: " << zoo.food << "\n";
        std::cout << "Популярность: " << zoo.popularity << "\n";
        std::cout << "Животные: " << zoo.animals.size() << "\n";
        std::cout << "Посетители сегодня: " << zoo.getVisitors() << "\n";
        std::cout << "Вольеры: " << zoo.enclosures.size() << "\n";
        std::cout << "Работники: " << zoo.workers.size() << "\n\n";

        std::cout << "1. Менеджмент животных\n";
        std::cout << "2. Менеджмент закупок\n";
        std::cout << "3. Менеджмент построек\n";
        std::cout << "4. Нанять работника\n";
        std::cout << "5. Перейти к следующему дню\n";
        std::cout << "0. Выйти\n";
        std::cout << "Выберите действие: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1: zoo.animalManagement(); break;
            case 2: zoo.purchaseManagement(); break;
            case 3: zoo.constructionManagement(); break;
            case 4: zoo.hireWorkerMenu(); break;
            case 5: zoo.nextDay(); break;
            case 0: return;
            default: std::cout << "Неверный выбор!\n";
        }

        if (zoo.money < 0) {
            std::cout << "Вы банкрот! Игра окончена.\n";
            return;
        }

        if (zoo.day >= 30) {
            std::cout << "Поздравляем! Вы успешно управляли зоопарком 30 дней!\n";
            return;
        }
    }
}

int main() {
    srand(time(0));
    
    std::string name;
    std::cout << "Введите название вашего зоопарка: ";
    std::getline(std::cin, name);
    
    Zoo zoo(name, 10000); // Начальный капитал 10000
    displayMenu(zoo);
    
    return 0;
}