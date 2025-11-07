#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip> // Для форматирования вывода

class RadioComponent {
public:
    double MTBF; // Наработка до отказа
    double failureRate; // Интенсивность отказов
    double reliability; // Достоверность
    double failureTolerance; // Допустимый уровень отказов
    std::string name; // Имя компонента
    double nominalValue; // Номинальная величина

    RadioComponent(double mtbf, double fr, double rel, double ft, const std::string& name, double value)
        : MTBF(mtbf), failureRate(fr), reliability(rel), failureTolerance(ft), name(name), nominalValue(value) {}

    virtual void getDetails() const {
        std::cout << name << ": " << nominalValue << '\n';
    }

    virtual ~RadioComponent() {}
};

class Resistor : public RadioComponent {
public:
    double resistance;
    double tolerance;

    Resistor(double mtbf, double fr, double rel, double ft, const std::string& name, double value, double r, double t)
        : RadioComponent(mtbf, fr, rel, ft, name, value), resistance(r), tolerance(t) {}
};

class Capacitor : public RadioComponent {
public:
    double capacity;
    double voltage;

    Capacitor(double mtbf, double fr, double rel, double ft, const std::string& name, double value, double vol)
        : RadioComponent(mtbf, fr, rel, ft, name, value), capacity(value), voltage(vol) {}
};

class Transistor : public RadioComponent {
public:
    double gain;
    double voltage;

    Transistor(double mtbf, double fr, double rel, double ft, const std::string& name, double value, double g, double vol)
        : RadioComponent(mtbf, fr, rel, ft, name, value), gain(g), voltage(vol) {}
};

class Diode : public RadioComponent {
public:
    double ratedVoltage;
    double rating;

    Diode(double mtbf, double fr, double rel, double ft, const std::string& name, double value, double vol, double r)
        : RadioComponent(mtbf, fr, rel, ft, name, value), ratedVoltage(vol), rating(r) {}
};

class Inductor : public RadioComponent {
public:
    double inductance;

    Inductor(double mtbf, double fr, double rel, double ft, const std::string& name, double value, double ind)
        : RadioComponent(mtbf, fr, rel, ft, name, value), inductance(ind) {}
};

struct Node {
    RadioComponent* component;
    Node* next;
};

class HashTable {
private:
    Node** table;
    size_t size;

public:
    HashTable(size_t s) : size(s) {
        table = new Node*[size];
        for (size_t i = 0; i < size; ++i) table[i] = nullptr;
    }

    ~HashTable() {
        for (size_t i = 0; i < size; ++i) {
            Node* node = table[i];
            while (node) {
                Node* temp = node;
                node = node->next;
                delete temp->component; // Освобождаем динамическую память
                delete temp;
            }
        }
        delete[] table;
    }

    void insert(int key, RadioComponent* component) {
        size_t index = key % size;
        Node* newNode = new Node{component, table[index]};
        table[index] = newNode;
    }

    void output() const {
        for (size_t i = 0; i < size; ++i) {
            Node* node = table[i];
            while (node) {
                node->component->getDetails();
                node = node->next;
            }
        }
    }
};

class Scheme {
private:
    std::string name;
    HashTable componentTable;
    std::vector<RadioComponent*> components; // Список компонентов для расчетов

public:
    Scheme(const std::string& schemeName, size_t hashTableSize) 
        : name(schemeName), componentTable(hashTableSize) {}

    void addComponent(int key, RadioComponent* component) {
        componentTable.insert(key, component);
        components.push_back(component); // Сохраняем компонент для расчетов
    }

    void displayComponents() const {
        std::cout << "Схема: " << name << '\n';
        componentTable.output();
    }
 void displayConnectionScheme() const {
        std::cout << "Схема подключения компонентов:\n";
        for (const auto& component : components) {
            std::cout << component->name << " (Сопротивление: " << std::fixed << std::setprecision(2) << component->nominalValue << " Ом)\n";
        }
    }

    double calculateReliabilitySequential() const {
        double totalFailureRate = 0.0;
        for (const auto& component : components) {
            totalFailureRate += component->failureRate;
        }
        return totalFailureRate; // Общая интенсивность отказов для последовательной схемы
    }

    double calculateReliabilityParallel() const {
        double totalReliability = 1.0;
        for (const auto& component : components) {
            totalReliability *= (1 - component->failureRate);
        }
        return 1 - totalReliability; // Вероятность отказа в параллельной схеме
    }

    void calculateAndDisplayReliability() {
        int connectionType;
        std::cout << "Выберите тип подключения (1 - Последовательное, 2 - Параллельное): ";
        std::cin >> connectionType;

        double reliability;
        if (connectionType == 1) {
            reliability = 1.0 / calculateReliabilitySequential();
            std::cout << "Вероятность безотказности для последовательной схемы: " << reliability << '\n';
        } else if (connectionType == 2) {
            reliability = calculateReliabilityParallel();
            std::cout << "Вероятность безотказности для параллельной схемы: " << reliability << '\n';
        } else {
            std::cout << "Некорректный выбор!" << std::endl;
            return;
        }

        // Вычисление и вывод дополнительных показателей
         double failureProbability = 1 - reliability;
    double densityOfFailure = failureProbability / reliability; // Примерная формула
    double intensityOfFailure = calculateReliabilitySequential(); // Интенсивность отказов

    double totalMTBF = 0.0;
    for (const auto& component : components) {
        totalMTBF += component->MTBF; // Суммируем MTBF всех компонентов
    }
    double averageMTBF = totalMTBF / components.size(); // Средняя наработка до отказа
    double gammaPercentMTBF = totalMTBF * reliability; // Гамма-процентная наработка до отказа

    std::cout << std::fixed << std::setprecision(5);
    std::cout << "Вероятность отказа: " << failureProbability << '\n';
    std::cout << "Плотность распределения отказов: " << densityOfFailure << '\n';
    std::cout << "Интенсивность отказов: " << intensityOfFailure << '\n';
    std::cout << "Средняя наработка до отказа (MTBF): " << averageMTBF << '\n';
    std::cout << "Гамма-процентная наработка до отказа: " << gammaPercentMTBF << '\n';
}

    void inputComponent() {
        int componentCount;
        std::cout << "Введите количество компонентов: ";
        std::cin >> componentCount;

        for (int i = 0; i < componentCount; ++i) {
            std::string componentType;
            std::cout << "Введите номер компонента (1. резистор, 2. конденсатор, 3. транзистор, 4. диод, 5.катушка): ";
            std::cin >> componentType;

            double mtbf, fr, rel, ft, value;
            std::cout << "Введите наработку до отказа (MTBF): ";
            std::cin >> mtbf;
            std::cout << "Введите интенсивность отказов: ";
            std::cin >> fr;
            std::cout << "Введите вероятность безотказности: ";
            std::cin >> rel;
            std::cout << "Введите допустимый уровень отказов: ";
            std::cin >> ft;
            std::cout << "Введите номинальную величину: ";
            std::cin >> value;

            RadioComponent* component = nullptr;

            if (componentType == "1") {
                double resistance, tolerance;
                std::cout << "Введите сопротивление: ";
                std::cin >> resistance;
                std::cout << "Введите допустимую погрешность: ";
                std::cin >> tolerance;
                component = new Resistor(mtbf, fr, rel, ft, "Резистор", value, resistance, tolerance);
            } else if (componentType == "2") {
                double voltage;
                std::cout << "Введите напряжение: ";
                std::cin >> voltage;
                component = new Capacitor(mtbf, fr, rel,ft, "Конденсатор", value, voltage);
            } else if (componentType == "3") {
                double gain, voltage;
                std::cout << "Введите коэффициент усиления: ";
                std::cin >> gain;
                std::cout << "Введите напряжение: ";
                std::cin >> voltage;
                component = new Transistor(mtbf, fr, rel, ft, "Транзистор", value, gain, voltage);
            } else if (componentType == "4") {
                double ratedVoltage, rating;
                std::cout << "Введите допустимое напряжение: ";
                std::cin >> ratedVoltage;
                std::cout << "Введите рейтинг: ";
                std::cin >> rating;
                component = new Diode(mtbf, fr, rel, ft, "Диод", value, ratedVoltage, rating);
            } else if (componentType == "5") {
                double inductance;
                std::cout << "Введите индуктивность: ";
                std::cin >> inductance;
                component = new Inductor(mtbf, fr, rel, ft, "Катушка", value, inductance);
            } else {
                std::cout << "Неизвестный тип компонента!\n";
                continue; // Пропускаем итерацию в случае некорректного типа
            }
            // Добавление компонента в хэш-таблицу (выбор уникального ключа)
            addComponent(i + 1, component); // Используем i + 1 в качестве ключа
        }
    }
};

int main() {
    Scheme myScheme("Моя электронная схема", 10);

    myScheme.inputComponent(); // Ввод компонент
    myScheme.displayComponents(); // Отображение компонентов
    myScheme.displayConnectionScheme(); // Отображение схемы подключения
    myScheme.calculateAndDisplayReliability(); // Расчет показателей надежности

    return 0;
}
