#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>

using std::vector;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::stringstream;
using std::ifstream;
using std::unordered_map;

class Car;

class Person {
    private:
        const int id;
        int age;
        string name;
        vector<Car*> cars;

        friend std::ostream& operator<<(std::ostream &strm, const Person &p);
    public:
        void changeName();
        void changeAge();

        Person(const int id, const int age, const string& name) :
            id(id),
            name(name),
            age(age) { }

        void changeName(const string newName) {
            this->name = newName;
        }
        void changeAge(const int age) {
            this->age = age;
        }
        int getAge() const {
            return this->age;
        }
        string getName() const {
            return this->name;
        }
        int getId() const {
            return this->id;
        }
        void addCar(Car* car) {
            this->cars.push_back(car);
        }
        vector<Car*>& getCars() {
            return this->cars;
        }
};
std::ostream& operator<<(std::ostream &strm, const Person &p) {
    return strm << p.name << ", " << p.age;
}
class Car {
    private:
        const string manufacturer;
        const string model;
        const int horsepower;
        Person* owner;
        string registrationNumber;
        friend std::ostream& operator<<(std::ostream &strm, const Car &c);
    public:
        Car(const string manufacturer, const string model, const int horsepower, Person *const owner, const string registrationNumber) :
            manufacturer(manufacturer),
            model(model),
            horsepower(horsepower),
            owner(owner),
            registrationNumber(registrationNumber) {
                owner->addCar(this);
            }

        void changeOwner(Person* const newOwner, string newRegNumber) {
            this->owner = newOwner;
            this->registrationNumber = newRegNumber;
        }

        string getManufacturer() const {
            return this->manufacturer;
        }
        string getModel() const {
            return this->model;
        }
        Person getOwner() const {
            return *this->owner;
        }
        string getRegistrationNumber() const {
            return this->registrationNumber;
        }
        int getHorsepower() const {
            return this->horsepower;
        }

        void changeOwner(Person* owner) {
            this->owner = owner;
        }
};
std::ostream& operator<<(std::ostream &strm, const Car &c) {
    return strm << c.getManufacturer() << " " << c.getModel() << " owned by " << c.getOwner();
}

Person* createPerson() {
    string name;
    int id, age;
    cout << "Creating a new person" << endl;
    cout << "Enter his id: ";
    cin >> id;
    cout << endl << "Enter his name: ";
    cin >> name;
    cout << endl << "Enter his age: ";
    cin >> age;
    cout << endl;

    Person* p = new Person(id, age, name);
    cout << "Created a new person " << *p << endl;
    return p;
}

void printOwners(vector<Person*> people) {
    for (int i = 0; i < people.size(); i++) {
        cout << i << "." << "\t" << *people[i] << endl;
    }
}
void printCars(vector<Car*> cars) {
    for (size_t i = 0; i < cars.size(); i++) {
        cout << i << "." << "\t" << *cars[i] << endl;
    }
}
Car* createCar(vector<Person*> people) {
    string manufacturer, model, registrationNumber;
    int horsepower, ownerId;
    Person* owner;

    cout << "Creating a new car" << endl;
    cout << "Enter the manufacturer: ";
    cin >> manufacturer;
    cout << "Enter the model: ";
    cin >> model;
    cout << "Enter the horsepower: ";
    cin >> horsepower;

    cout << "Choose the owner (by id): " << endl;
    printOwners(people);
    cin >> ownerId;
    while (ownerId < 0 || ownerId >= people.size()) {
        cout << "Invalid owner id! Choose again!" << endl;
        printOwners(people);
        cin >> ownerId;
    }
    owner = people[ownerId];
    cout << "Enter a registration number: ";
    cin >> registrationNumber;
    cout << "Created a new Car " << manufacturer << " " << model << " for " << owner->getName() << endl;

    return new Car(manufacturer, model, horsepower, owner, registrationNumber);
}

void writePeopleAndCars(vector<Person*> people) {
    vector<Car*> cars;
    ofstream file;
    file.open("people_and_cars.txt");

    for (int i = 0; i < people.size(); i++) {
        Person* p = people[i];
        file << p->getId() << " " << p->getAge() << " " << p->getName() << "\n";
        vector<Car*> personCars = p->getCars();

        for (int j = 0; j < personCars.size(); j++) {
            cars.push_back(personCars[j]);
        }
    }
    file << "CAR SEPARATOR" << "\n";
    for (int i = 0; i < cars.size(); i++) {
        Car* c = cars[i];
        file << c->getOwner().getId() << " " << c->getManufacturer() << " " << c->getModel() << " " << c->getHorsepower() << " " << c->getRegistrationNumber() << "\n";
    }
    file.close();
}

vector<Person*> readPeopleAndCars() {
    vector<Person*> people;
    ifstream file;
    file.open("people_and_cars.txt");
    string line;
    bool readingPeople = true;
    unordered_map<int, Person*> peopleByIds;
    while(getline(file, line)) {
        if (line == "CAR SEPARATOR") {
            readingPeople = false;
            continue;
        }
        if (readingPeople) {
            stringstream ss;
            int personId, age;
            string name;
            ss << line;
            ss >> personId >> age >> name;

            Person* tempPerson = new Person(personId, age, name);
            people.push_back(tempPerson);
            peopleByIds.insert(std::make_pair(personId, tempPerson));
        } else {
            stringstream ss;
            string manufacturer, model, registrationNumber;
            int horsepower, ownerId;
            Person* owner;
            ss << line;
            ss >> ownerId >> manufacturer >> model >> horsepower >> registrationNumber;
            unordered_map<int, Person*>::const_iterator findResult = peopleByIds.find(ownerId);
            owner = findResult->second;
            Car* tempCar = new Car(manufacturer, model, horsepower, owner, registrationNumber);
            owner->addCar(tempCar);
        }
    }

    return people;
}
int main() {
    /*
     Create cars and people which then get saved to a text file and read on each further program start so that the data is saved.
     Duplicate IDs will cause the program to behave unexpectedly.
     */
    int choice;
    vector<Person*> people = readPeopleAndCars();
    while (true) {
        cout << "Do you want to: " << endl << "1.Create a car" << endl << "2.Create a person" << endl << "3.Change the owner of a car" << endl;
        cin >> choice;
        if (choice == 1) {
            createCar(people);
        } else if (choice == 2) {
            people.push_back(createPerson());
        } else if (choice == 3) {
            cout << "Choose a person to get the car from" << endl;
            int personId, receiverId;
            int carId;
            printOwners(people);
            cin >> personId;
            while (personId < 0 || personId >= people.size()) {
                cout << "Invalid owner id! Choose again!" << endl;
                printOwners(people);
                cin >> personId;
            }
            Person* person = people[personId];
            vector<Car*>& cars = person->getCars();
            cout << "Choose a car from the person" << endl;
            printCars(cars);
            cin >> carId;
            while (carId < 0 || carId >= cars.size()) {
                cout << "Invalid car id! Choose again!" << endl;
                printCars(cars);
                cin >> carId;
            }
            cout << "Choose a person to register the car to" << endl;
            printOwners(people);
            cin >> receiverId;
            while (receiverId < 0 || receiverId >= people.size()) {
                cout << "Invalid owner id! Choose again!" << endl;
                printOwners(people);
                cin >> receiverId;
            }

            Person* receiver = people[receiverId];
            Car* carToMove = cars[carId];
            cars.erase(cars.begin() + carId);

            carToMove->changeOwner(receiver);
            receiver->addCar(carToMove);
            cout << "Moved " << *carToMove << " from " << *person << " to " << *receiver << endl;
        } else {
            cout << "Invalid choice!" << endl;
            break;
        }
        cout << endl;
    }
    writePeopleAndCars(people);
    return 0;
}