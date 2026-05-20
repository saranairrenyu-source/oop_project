#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <iomanip>
#include <stdexcept>
#include <algorithm>

using namespace std;

// ============================================================
// BILL CLASS
// ============================================================
class Bill {
private:
    string patientId;
    vector<pair<string, double>> charges;

public:
    Bill(const string& id) : patientId(id) {}

    void addCharge(const string& description, double amount) {
        charges.push_back({description, amount});
    }

    double getTotal() const {
        double total = 0;
        for (const auto& charge : charges)
            total += charge.second;
        return total;
    }

    void displayBill() const {
        cout << "\n========== BILL ==========" << endl;
        cout << "Patient ID: " << patientId << endl;
        cout << left << setw(25) << "Description"
             << right << setw(10) << "Amount" << endl;

        cout << "------------------------------------" << endl;

        for (const auto& charge : charges) {
            cout << left << setw(25) << charge.first
                 << right << setw(10) << charge.second << endl;
        }

        cout << "------------------------------------" << endl;
        cout << left << setw(25) << "TOTAL"
             << right << setw(10) << getTotal() << endl;
        cout << "===============================\n" << endl;
    }
};

// ============================================================
// ABSTRACT BASE CLASS: PERSON
// ============================================================
class Person {
private:
    string name;
    int age;
    string id;

public:
    Person(const string& n, int a, const string& i)
        : name(n), age(a), id(i) {}

    virtual ~Person() = default;

    string getName() const { return name; }
    int getAge() const { return age; }
    string getId() const { return id; }

    void setName(const string& n) { name = n; }
    void setAge(int a) { age = a; }

    virtual string getRole() const = 0;
    virtual void display() const = 0;

    bool operator==(const Person& other) const {
        return id == other.id;
    }

    friend ostream& operator<<(ostream& os, const Person& p) {
        os << "[" << p.getRole() << "] "
           << p.name << " | ID: " << p.id
           << " | Age: " << p.age;
        return os;
    }
};

// Forward declaration
class Room;

// ============================================================
// PATIENT CLASS
// ============================================================
class Patient : public Person {
protected:
    string condition;
    string admissionDate;
    unique_ptr<Bill> bill;

public:
    Patient(const string& n,
            int a,
            const string& i,
            const string& c,
            const string& date)
        : Person(n, a, i),
          condition(c),
          admissionDate(date) {
        bill = make_unique<Bill>(i);
    }

    virtual ~Patient() = default;

    string getCondition() const { return condition; }
    string getAdmissionDate() const { return admissionDate; }

    Bill* getBill() const {
        return bill.get();
    }

    string getRole() const override {
        return "Patient";
    }

    virtual bool isEmergency() const {
        return false;
    }

    void display() const override {
        cout << *this << endl;
        cout << "Condition      : " << condition << endl;
        cout << "Admission Date : " << admissionDate << endl;
    }
};

// ============================================================
// INPATIENT CLASS
// ============================================================
class InPatient : public Patient {
private:
    Room* assignedRoom;
    int stayDuration;

public:
    InPatient(const string& n,
              int a,
              const string& i,
              const string& c,
              const string& date,
              int days)
        : Patient(n, a, i, c, date),
          assignedRoom(nullptr),
          stayDuration(days) {}

    void assignRoom(Room* room) {
        assignedRoom = room;
    }

    int getStayDuration() const {
        return stayDuration;
    }

    string getRole() const override {
        return "InPatient";
    }

    void display() const override {
        Patient::display();
        cout << "Stay Duration  : " << stayDuration << " days" << endl;
    }
};

// ============================================================
// OUTPATIENT CLASS
// ============================================================
class OutPatient : public Patient {
private:
    bool emergency;

public:
    OutPatient(const string& n,
               int a,
               const string& i,
               const string& c,
               const string& date,
               bool emergencyCase)
        : Patient(n, a, i, c, date),
          emergency(emergencyCase) {}

    bool isEmergency() const override {
        return emergency;
    }

    string getRole() const override {
        return emergency ? "Emergency OutPatient" : "OutPatient";
    }
};

// ============================================================
// ROOM CLASS
// ============================================================
class Room {
private:
    int roomNumber;
    Patient* occupant;

public:
    Room(int number)
        : roomNumber(number), occupant(nullptr) {}

    bool isAvailable() const {
        return occupant == nullptr;
    }

    void assignPatient(Patient* patient) {
        if (!isAvailable())
            throw runtime_error("Room already occupied");

        occupant = patient;
    }

    void dischargePatient() {
        occupant = nullptr;
    }

    int getRoomNumber() const {
        return roomNumber;
    }
};

// ============================================================
// ABSTRACT MEDICAL STAFF CLASS
// ============================================================
class MedicalStaff : public Person {
public:
    MedicalStaff(const string& n,
                 int a,
                 const string& i)
        : Person(n, a, i) {}

    virtual void treat(Patient* p) = 0;
    virtual double calculateFee() const = 0;
};

// ============================================================
// DOCTOR CLASS
// ============================================================
class Doctor : public MedicalStaff {
protected:
    string specialisation;
    vector<Patient*> activePatients;

    static constexpr double BASE_FEE = 1500.0;

public:
    Doctor(const string& n,
           int a,
           const string& i,
           const string& spec)
        : MedicalStaff(n, a, i),
          specialisation(spec) {}

    virtual ~Doctor() = default;

    string getSpecialisation() const {
        return specialisation;
    }

    void treat(Patient* p) override {
        activePatients.push_back(p);

        double fee = calculateFee();

        p->getBill()->addCharge(
            specialisation + " Consultation",
            fee
        );

        cout << getName()
             << " treated "
             << p->getName()
             << endl;
    }

    double calculateFee() const override {
        return BASE_FEE;
    }

    string getRole() const override {
        return "Doctor";
    }

    void display() const override {
        cout << *this
             << " | Specialisation: "
             << specialisation
             << endl;
    }
};

// ============================================================
// SURGEON CLASS
// ============================================================
class Surgeon : public Doctor {
private:
    double multiplier;

public:
    Surgeon(const string& n,
            int a,
            const string& i,
            double mult = 5.0)
        : Doctor(n, a, i, "Surgery"),
          multiplier(mult) {}

    double calculateFee() const override {
        return BASE_FEE * multiplier;
    }

    string getRole() const override {
        return "Surgeon";
    }
};

// ============================================================
// GENERAL PRACTITIONER CLASS
// ============================================================
class GeneralPractitioner : public Doctor {
public:
    GeneralPractitioner(const string& n,
                        int a,
                        const string& i)
        : Doctor(n, a, i, "General Practice") {}

    double calculateFee() const override {
        return BASE_FEE * 0.6;
    }

    string getRole() const override {
        return "General Practitioner";
    }
};

// ============================================================
// NURSE CLASS
// ============================================================
class Nurse : public MedicalStaff {
public:
    Nurse(const string& n,
          int a,
          const string& i)
        : MedicalStaff(n, a, i) {}

    void treat(Patient* p) override {
        p->getBill()->addCharge("Nursing Care", 500);

        cout << getName()
             << " provided nursing care to "
             << p->getName()
             << endl;
    }

    double calculateFee() const override {
        return 500;
    }

    string getRole() const override {
        return "Nurse";
    }

    void display() const override {
        cout << *this << endl;
    }
};

// ============================================================
// DEPARTMENT CLASS
// ============================================================
class Department {
private:
    string name;
    vector<Doctor*> doctors;
    vector<Room*> rooms;

public:
    Department(const string& n)
        : name(n) {}

    void addDoctor(Doctor* doctor) {
        doctors.push_back(doctor);
    }

    void addRoom(Room* room) {
        rooms.push_back(room);
    }

    Doctor* findDoctorBySpecialisation(const string& spec) {
        for (auto doctor : doctors) {
            if (doctor->getSpecialisation() == spec)
                return doctor;
        }

        return nullptr;
    }

    Room* findAvailableRoom() {
        for (auto room : rooms) {
            if (room->isAvailable())
                return room;
        }

        return nullptr;
    }

    void displayDepartment() const {
        cout << "\n========== " << name << " Department ==========" << endl;

        cout << "Doctors:" << endl;
        for (auto doctor : doctors)
            doctor->display();

        cout << "Rooms Available: " << rooms.size() << endl;
    }
};

// ============================================================
// APPOINTMENT CLASS
// ============================================================
class Appointment {
private:
    Patient* patient;
    Doctor* doctor;
    bool priority;

public:
    Appointment(Patient* p,
                Doctor* d,
                bool emergency = false)
        : patient(p),
          doctor(d),
          priority(emergency) {}

    Patient* getPatient() const {
        return patient;
    }

    Doctor* getDoctor() const {
        return doctor;
    }

    bool isPriority() const {
        return priority;
    }

    void display() const {
        cout << "Patient : " << patient->getName() << endl;
        cout << "Doctor  : " << doctor->getName() << endl;
        cout << "Priority: "
             << (priority ? "HIGH" : "NORMAL")
             << endl;
    }
};

// ============================================================
// TEMPLATE QUEUE CLASS
// ============================================================
template<typename T>
class Queue {
private:
    vector<T> items;

public:
    void enqueue(const T& item) {
        items.push_back(item);
    }

    void priorityEnqueue(const T& item) {
        items.insert(items.begin(), item);
    }

    T dequeue() {
        if (isEmpty())
            throw runtime_error("Queue is empty");

        T front = items.front();
        items.erase(items.begin());

        return front;
    }

    bool isEmpty() const {
        return items.empty();
    }

    int size() const {
        return static_cast<int>(items.size());
    }
};

// ============================================================
// HOSPITAL MANAGEMENT SYSTEM
// ============================================================
class HospitalManagementSystem {
private:
    vector<unique_ptr<Patient>> patients;
    vector<unique_ptr<Doctor>> doctors;
    vector<unique_ptr<Nurse>> nurses;
    vector<unique_ptr<Room>> rooms;

    Queue<Appointment> appointments;

public:
    HospitalManagementSystem() {
        seedData();
    }

    void seedData() {
        doctors.push_back(
            make_unique<Surgeon>(
                "Dr. Wanjiku",
                45,
                "D001"
            )
        );

        doctors.push_back(
            make_unique<GeneralPractitioner>(
                "Dr. Otieno",
                39,
                "D002"
            )
        );

        nurses.push_back(
            make_unique<Nurse>(
                "Nurse Achieng",
                30,
                "N001"
            )
        );

        rooms.push_back(make_unique<Room>(101));
        rooms.push_back(make_unique<Room>(102));
        rooms.push_back(make_unique<Room>(103));
    }

    void registerPatient() {
        string name;
        int age;
        string condition;
        int type;

        cout << "\nEnter patient name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter age: ";
        cin >> age;

        cout << "Enter condition: ";
        cin.ignore();
        getline(cin, condition);

        cout << "\n1. InPatient\n2. OutPatient\nChoice: ";
        cin >> type;

        string id = "P" + to_string(patients.size() + 1);

        if (type == 1) {
            int days;
            cout << "Enter stay duration(days): ";
            cin >> days;

            auto patient = make_unique<InPatient>(
                name,
                age,
                id,
                condition,
                "2026-05-20",
                days
            );

            Room* availableRoom = nullptr;

            for (auto& room : rooms) {
                if (room->isAvailable()) {
                    availableRoom = room.get();
                    break;
                }
            }

            if (availableRoom) {
                availableRoom->assignPatient(patient.get());
                patient->assignRoom(availableRoom);

                patient->getBill()->addCharge(
                    "Room Charges",
                    days * 3000
                );

                cout << "Assigned Room: "
                     << availableRoom->getRoomNumber()
                     << endl;
            }
            else {
                cout << "No room available." << endl;
            }

            patients.push_back(move(patient));
        }
        else {
            bool emergency;

            cout << "Emergency case? (1 = YES, 0 = NO): ";
            cin >> emergency;

            patients.push_back(
                make_unique<OutPatient>(
                    name,
                    age,
                    id,
                    condition,
                    "2026-05-20",
                    emergency
                )
            );
        }

        cout << "Patient registered successfully." << endl;
        cout << "Patient ID: " << id << endl;
    }

    void addDoctor() {
        int type;
        string name;
        int age;

        cout << "\n1. Surgeon\n2. General Practitioner\nChoice: ";
        cin >> type;

        cout << "Enter doctor name: ";
        cin.ignore();
        getline(cin, name);

        cout << "Enter age: ";
        cin >> age;

        string id = "D" + to_string(doctors.size() + 1);

        if (type == 1) {
            doctors.push_back(
                make_unique<Surgeon>(
                    name,
                    age,
                    id
                )
            );
        }
        else {
            doctors.push_back(
                make_unique<GeneralPractitioner>(
                    name,
                    age,
                    id
                )
            );
        }

        cout << "Doctor added successfully." << endl;
    }

    void scheduleAppointment() {
        if (patients.empty() || doctors.empty()) {
            cout << "Patients or doctors unavailable." << endl;
            return;
        }

        cout << "\nAvailable Patients:" << endl;

        for (size_t i = 0; i < patients.size(); ++i) {
            cout << i + 1 << ". "
                 << patients[i]->getName()
                 << endl;
        }

        int patientChoice;
        cout << "Select patient: ";
        cin >> patientChoice;

        if (patientChoice < 1 ||
            patientChoice > static_cast<int>(patients.size())) {
            cout << "Invalid patient." << endl;
            return;
        }

        cout << "\nAvailable Doctors:" << endl;

        for (size_t i = 0; i < doctors.size(); ++i) {
            cout << i + 1 << ". ";
            doctors[i]->display();
        }

        int doctorChoice;
        cout << "Select doctor: ";
        cin >> doctorChoice;

        if (doctorChoice < 1 ||
            doctorChoice > static_cast<int>(doctors.size())) {
            cout << "Invalid doctor." << endl;
            return;
        }

        Patient* patient = patients[patientChoice - 1].get();
        Doctor* doctor = doctors[doctorChoice - 1].get();

        Appointment appointment(
            patient,
            doctor,
            patient->isEmergency()
        );

        if (patient->isEmergency()) {
            appointments.priorityEnqueue(appointment);
            cout << "Emergency appointment prioritised." << endl;
        }
        else {
            appointments.enqueue(appointment);
            cout << "Appointment scheduled." << endl;
        }
    }

    void processAppointment() {
        if (appointments.isEmpty()) {
            cout << "No appointments available." << endl;
            return;
        }

        Appointment appointment = appointments.dequeue();

        cout << "\nProcessing Appointment..." << endl;
        appointment.display();

        appointment.getDoctor()->treat(
            appointment.getPatient()
        );
    }

    void displayPatients() const {
        if (patients.empty()) {
            cout << "No patients registered." << endl;
            return;
        }

        cout << "\n========== PATIENTS ==========" << endl;

        for (const auto& patient : patients) {
            patient->display();
            cout << "-----------------------------------" << endl;
        }
    }

    void displayDoctors() const {
        if (doctors.empty()) {
            cout << "No doctors available." << endl;
            return;
        }

        cout << "\n========== DOCTORS ==========" << endl;

        for (const auto& doctor : doctors)
            doctor->display();
    }

    void generateBill() {
        if (patients.empty()) {
            cout << "No patients available." << endl;
            return;
        }

        cout << "\nSelect Patient:" << endl;

        for (size_t i = 0; i < patients.size(); ++i) {
            cout << i + 1 << ". "
                 << patients[i]->getName()
                 << endl;
        }

        int choice;
        cout << "Choice: ";
        cin >> choice;

        if (choice < 1 ||
            choice > static_cast<int>(patients.size())) {
            cout << "Invalid choice." << endl;
            return;
        }

        patients[choice - 1]->getBill()->displayBill();
    }

    void menu() {
        int choice;

        do {
            cout << "\n====================================" << endl;
            cout << "   HOSPITAL MANAGEMENT SYSTEM" << endl;
            cout << "====================================" << endl;
            cout << "1. Register Patient" << endl;
            cout << "2. Add Doctor" << endl;
            cout << "3. Schedule Appointment" << endl;
            cout << "4. Process Appointment" << endl;
            cout << "5. Display Patients" << endl;
            cout << "6. Display Doctors" << endl;
            cout << "7. Generate Bill" << endl;
            cout << "0. Exit" << endl;
            cout << "Choice: ";

            cin >> choice;

            switch (choice) {
                case 1:
                    registerPatient();
                    break;

                case 2:
                    addDoctor();
                    break;

                case 3:
                    scheduleAppointment();
                    break;

                case 4:
                    processAppointment();
                    break;

                case 5:
                    displayPatients();
                    break;

                case 6:
                    displayDoctors();
                    break;

                case 7:
                    generateBill();
                    break;

                case 0:
                    cout << "Exiting system..." << endl;
                    break;

                default:
                    cout << "Invalid choice." << endl;
            }

        } while (choice != 0);
    }
};

// ============================================================
// MAIN FUNCTION
// ============================================================
int main() {
    HospitalManagementSystem system;
    system.menu();

    return 0;
}
