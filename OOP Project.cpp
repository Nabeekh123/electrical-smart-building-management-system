#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
using namespace std;

string dataError = "";
const double EMISSION_FACTOR = 0.45;
const double COOLING_RATE = 3.0;
const double HEATING_RATE = 3.0;

class ElectricalDevice
{
protected:
    int deviceID;
    string name;
    double ratedPower;
    bool isOn;
    double setting;
public:
    ElectricalDevice(int id, string n, double power,bool status, double s)
    {
        this->deviceID = id;
        this->name = n;
        this->ratedPower = power;
        this->isOn = status;
        this->setting = s;
    }
    virtual ~ElectricalDevice()
    {
    }
    int getID()
    {
        return deviceID;
    }
    string getName()
    {
        return name;
    }
    double getRatedPower()
    {
        return ratedPower;
    }
    bool getStatus()
    {
        return isOn;
    }
    double getSetting()
    {
        return setting;
    }
    ElectricalDevice& turnOn()
    {
        this->isOn = true;
        return *this;
    }
    ElectricalDevice& turnOff()
    {
        this->isOn = false;
        return *this;
    }
    ElectricalDevice& setSetting(double s)
    {
        this->setting = s;
        return *this;
    }
    bool operator==(ElectricalDevice& other)
    {
        return this->deviceID == other.deviceID;
    }
    bool operator<(ElectricalDevice& other)
    {
        return this->calculatePower() < other.calculatePower();
    }
    friend ostream& operator<<(ostream& out, ElectricalDevice& d);
    virtual double calculatePower() = 0;
    virtual void updateBehavior(double hours) = 0;
    virtual string getType() = 0;
    virtual string getSettingName() = 0;
    virtual bool isValidSetting(double s) = 0;
    virtual void display()
    {
        cout << *this << endl;
    }
    virtual string saveData(int roomID)
    {
        string powerText = to_string(ratedPower);
        string settingText = to_string(setting);
        while(powerText.length() > 1 && powerText[powerText.length()-1] == '0')
        {
        	 powerText.erase(powerText.length()-1);
		}
        if(powerText[powerText.length()-1] == '.')
        {
        	 powerText.erase(powerText.length()-1);
		}
        while(settingText.length() > 1 && settingText[settingText.length()-1] == '0')
        {
        	settingText.erase(settingText.length()-1);
		}
        if(settingText[settingText.length()-1] == '.')
        {
        	settingText.erase(settingText.length()-1);
		}
        string result = getType() + ",";
        result += to_string(deviceID) + ",";
        result += to_string(roomID) + ",";
        result += name + ",";
        result += powerText;
        result += ",";
        result += (isOn ? "1" : "0");
        result += ",";
        result += getSettingName() + ",";
        result += settingText;
        return result;
    }
};

ostream& operator<<(ostream& out, ElectricalDevice& d)
{
    out << "ID: " << d.deviceID
        << " | Name: " << d.name
        << " | Type: " << d.getType()
        << " | Rated Power: " << d.ratedPower << " W"
        << " | Status: " << (d.isOn ? "ON" : "OFF")
        << " | " << d.getSettingName() << ": " << d.setting
        << " | Current Power: " << d.calculatePower() << " W";
    return out;
}

class AirConditioner : public ElectricalDevice
{
public:
    AirConditioner(int id, string n, double p,bool status, double s): ElectricalDevice(id, n, p, status, s)
    {
    }
    double calculatePower()
    {
        if(!isOn)
        {
        	return 0;
		}
        double factor = 1.0 - 0.04 * (setting - 16.0);
        if(factor < 0.4)
        {
        	factor = 0.4;
		}
        return ratedPower * factor;
    }
    void updateBehavior(double)
    {
    }
    string getType()
    {
        return "AC";
    }
    string getSettingName()
    {
        return "targetTemperatureC";
    }
    bool isValidSetting(double s)
    {
        return s >= 16 && s <= 30;
    }
};

class Light : public ElectricalDevice
{
public:
    Light(int id, string n, double p,bool status, double s): ElectricalDevice(id, n, p, status, s)
    {
    }
    double calculatePower()
    {
        if(!isOn)
        {
        	return 0;
		}
        return ratedPower * setting / 100.0;
    }
    void updateBehavior(double)
    {
    }
    string getType()
    {
        return "LIGHT";
    }
    string getSettingName()
    {
        return "brightnessPercent";
    }
    bool isValidSetting(double s)
    {
        return s >= 1 && s <= 100;
    }
};

class Fan : public ElectricalDevice
{
public:
    Fan(int id, string n, double p,bool status, double s): ElectricalDevice(id, n, p, status, s)
    {
    }
    double calculatePower()
    {
        if(!isOn)
        {
        	return 0;
		}
        return ratedPower * setting / 3.0;
    }
    void updateBehavior(double)
    {
    }
    string getType()
    {
        return "FAN";
    }
    string getSettingName()
    {
        return "speedLevel";
    }
    bool isValidSetting(double s)
    {
        return s >= 1 && s <= 3 && s == (int)s;
    }
};

class Heater : public ElectricalDevice
{
public:
    Heater(int id, string n, double p,bool status, double s): ElectricalDevice(id, n, p, status, s)
    {
    }
    double calculatePower()
    {
        if(!isOn)
        {
        	return 0;
		}
        return ratedPower * setting / 100.0;
    }
    void updateBehavior(double)
    {
    }
    string getType()
    {
        return "HEATER";
    }
    string getSettingName()
    {
        return "powerLevelPercent";
    }
    bool isValidSetting(double s)
    {
        return s >= 0 && s <= 100;
    }
};

class Room
{
private:
    int roomID;
    string roomName;
    double temperature;
    double minTemperature;
    double maxTemperature;
    static const int MAX_DEVICES = 20;
    ElectricalDevice* devices[MAX_DEVICES];
    int deviceCount;
public:
    Room()
    {
        roomID = 0;
        roomName = "";
        temperature = 25;
        minTemperature = 20;
        maxTemperature = 27;
        for(int i = 0; i < MAX_DEVICES; i++)
        {
        	devices[i] = nullptr;
		}
        deviceCount = 0;
    }
    Room(int id, string n, double temp,double minT, double maxT)
    {
        roomID = id;
        roomName = n;
        temperature = temp;
        minTemperature = minT;
        maxTemperature = maxT;
        for(int i = 0; i < MAX_DEVICES; i++)
        {
        	devices[i] = nullptr;
		}
        deviceCount = 0;
    }
    ~Room()
    {
        for(int i = 0; i < MAX_DEVICES; i++)
        {
        	delete devices[i];
		}
    }
    Room(const Room&) = delete;
    Room& operator=(const Room&) = delete;
    void clear()
    {
        for(int i = 0; i < MAX_DEVICES; i++)
        {
            delete devices[i];
            devices[i] = nullptr;
        }
        deviceCount = 0;
        roomID = 0;
        roomName = "";
        temperature = 25;
        minTemperature = 20;
        maxTemperature = 27;
    }
    void moveFrom(Room& other)
    {
        if(this == &other)
        {
        	return;
		}
        clear();
        roomID = other.roomID;
        roomName = other.roomName;
        temperature = other.temperature;
        minTemperature = other.minTemperature;
        maxTemperature = other.maxTemperature;
        deviceCount = other.deviceCount;
        for(int i = 0; i < MAX_DEVICES; i++)
        {
            devices[i] = other.devices[i];
            other.devices[i] = nullptr;
        }
        other.deviceCount = 0;
        other.roomID = 0;
        other.roomName = "";
    }
    void setRoomData(int id, string n, double temp, double minT, double maxT)
    {
        roomID = id;
        roomName = n;
        temperature = temp;
        minTemperature = minT;
        maxTemperature = maxT;
    }
    int getID()
    {
        return roomID;
    }
    string getName()
    {
        return roomName;
    }
    double getTemperature()
    {
        return temperature;
    }
    Room& setTemperature(double t)
    {
        this->temperature = t;
        return *this;
    }
    bool operator>(Room& other)
    {
        return this->calculateTotalPower() > other.calculateTotalPower();
    }
    double getMinTemperature()
    {
        return minTemperature;
    }
    double getMaxTemperature()
    {
        return maxTemperature;
    }
    int getDeviceCount()
    {
        return deviceCount;
    }
    ElectricalDevice* getDevice(int index)
    {
        if(index<0||index>=deviceCount)
        {
        	return nullptr;
		}
        return devices[index];
    }
    ElectricalDevice* findDevice(int id)
    {
        for(int i = 0; i < deviceCount; i++)
        {
            if(devices[i]->getID()==id)
            {
            	return devices[i];
			}
        }
        return nullptr;
    }
    bool addDevice(ElectricalDevice* device)
    {
        if(deviceCount>=MAX_DEVICES)
        {
            cout << "Room device limit reached!" << endl;
            delete device;
            return false;
        }
        for(int i = 0; i < deviceCount; i++)
        {
            if(*devices[i] == *device)
            {
                cout << "A device with this ID already exists in the room!" << endl;
                delete device;
                return false;
            }
        }
        devices[deviceCount] = device;
        deviceCount++;
        return true;
    }
    bool removeDevice(int id)
    {
        for(int i = 0; i < deviceCount; i++)
        {
            if(devices[i]->getID()==id)
            {
                delete devices[i];
                for(int j = i; j < deviceCount - 1; j++)
                {
                    devices[j] = devices[j + 1];
                    devices[j + 1] = nullptr;
                }
                devices[deviceCount - 1] = nullptr;
                deviceCount--;
                return true;
            }
        }
        return false;
    }
    double calculateTotalPower()
    {
        double total = 0;
        for(int i = 0; i < deviceCount; i++)
        {
            total += devices[i]->calculatePower();
        }
        return total;
    }
    double calculateEnergy(double hours)
    {
        return (calculateTotalPower() / 1000.0) * hours;
    }
    double calculateEnergy()
    {
        return calculateEnergy(1.0);
    }
    bool isComfortable()
    {
        return temperature >= minTemperature && temperature <= maxTemperature;
    }
    void simulateTemperature(double hours)
    {
        for(int i = 0; i < deviceCount; i++)
        {
            ElectricalDevice* d = devices[i];
            d->updateBehavior(hours);
            if(!d->getStatus())
            {
            	continue;
			}
            if(d->getType()=="AC")
            {
                if(temperature>d->getSetting())
                {
                    temperature -= COOLING_RATE * hours;
                    if(temperature<d->getSetting())
                    {
                    	temperature = d->getSetting();
					}
                }
            }
            else if(d->getType()=="HEATER")
            {
                if(temperature<maxTemperature)
                {
                    temperature += HEATING_RATE * hours * d->getSetting() / 100.0;
                    if(temperature>maxTemperature)
                    {
                    	temperature = maxTemperature;
					}
                }
            }
        }
    }
    void optimize()
    {
        for(int i = 0; i < deviceCount; i++)
        {
            ElectricalDevice* d = devices[i];
            if(d->getType()=="LIGHT")
            {
                d->turnOn();
                if(d->getSetting()>50)
                {
                	d->setSetting(50);
				}
            }
            else if(d->getType()=="FAN")
            {
                d->turnOn();
                if(d->getSetting()>1)
                {
                	d->setSetting(1);
				}
            }
            else if(d->getType()=="AC")
            {
                if(!d->getStatus())
                {
                	continue;
				}
                double target = maxTemperature - 0.5;
                if(target<minTemperature)
                {
                	target = (minTemperature + maxTemperature) / 2.0;
				}
                if(target<16)
                {
                	target = 16;
				}
                if(target>30)
                {
                	target = 30;
				}
                if(d->getSetting()<target||d->getSetting()>maxTemperature)
                {
                	d->setSetting(target);
				}
            }
            else if(d->getType()=="HEATER")
            {
                if(!d->getStatus())
                {
                	continue;
				}
                if(temperature>=minTemperature&&d->getSetting()>50)
                {
                	d->setSetting(50);
				}
            }
        }
    }
    void printAdvice()
    {
        if(isComfortable())
        {
        	return;
		}
        bool tooHot;
        if(temperature > maxTemperature)
        {
        	tooHot=true;
		}
		else 
		{
			tooHot=false;
		}
        string wanted;
		if(tooHot)
		{
		    wanted = "AC";
		}
		else
		{
            wanted = "HEATER";
		}
        bool hasDevice = false;
        bool running = false;
        for(int i = 0; i < deviceCount; i++)
        {
            if(devices[i]->getType()==wanted)
            {
                hasDevice = true;
                if(devices[i]->getStatus())
                {
                	running = true;
				}
            }
        }
        cout << "Room " << roomID << " (" << roomName << ") is ";
        if(tooHot)
		{
		    cout<<"too hot.";
		}
		else
		{
            cout<<"too cold.";
		}
        if(!hasDevice)
        {
        	cout << "No " << wanted << " installed in this room." << endl;
		}
        else if(!running)
        {
        	cout << "Switch the " << wanted << " ON manually (option 5)." << endl;
		}
        else
        {
        	cout << "The " << wanted<< " is running; it needs more time to reach the comfort range." << endl;
		}
    }
    void display()
    {
        cout << endl << "Room ID: " << roomID
             << endl << "Room Name: " << roomName
             << endl << "Temperature: " << temperature << " C"
             << endl << "Comfort Range: "
             << minTemperature << " - "
             << maxTemperature << " C"
             << endl << "Total Power: "
             << calculateTotalPower() << " W"
             << endl << "Energy (1 hour): " << calculateEnergy() << " kWh"
             << endl << "Comfort Status: "
             << (isComfortable() ? "COMFORTABLE" : "OUTSIDE RANGE")
             << endl << "Devices: " << deviceCount << endl;
        for(int i = 0; i < deviceCount; i++)
        {
            cout << *devices[i] << endl;
        }
    }
    friend class DataLogger;
};

ostream& operator<<(ostream& out, Room& r)
{
    out << "Room " << r.getID() << " (" << r.getName() << ") | "
        << r.getTemperature() << " C | Range "
        << r.getMinTemperature() << "-" << r.getMaxTemperature() << " C | "
        << r.calculateTotalPower() << " W | "
        << (r.isComfortable() ? "COMFORTABLE" : "OUTSIDE RANGE");
    return out;
}

class Building
{
private:
    static const int MAX_ROOMS = 20;
    Room rooms[MAX_ROOMS];
    int roomCount;
    int buildingID;
    string buildingName;
    double electricityRate;
public:
    Building(int id = 1,string name = "Smart_Building", double rate = 50)
    {
        buildingID = id;
        buildingName = name;
        electricityRate = rate;
        roomCount = 0;
    }
    Building(const Building&) = delete;
    Building& operator=(const Building&) = delete;
    void moveFrom(Building& other)
    {
        if(this == &other) return;
        buildingID = other.buildingID;
        buildingName = other.buildingName;
        electricityRate = other.electricityRate;
        roomCount = other.roomCount;
        for(int i = 0; i < MAX_ROOMS; i++)
            rooms[i].moveFrom(other.rooms[i]);
        other.roomCount = 0;
    }
    int getRoomCount()
    {
        return roomCount;
    }
    Room* getRoom(int index)
    {
        if(index<0||index>=roomCount)
            return nullptr;
        return &rooms[index];
    }
    Room* findRoom(int id)
    {
        for(int i = 0; i < roomCount; i++)
        {
            if(rooms[i].getID()==id)
            {
            	return &rooms[i];
			}
        }
        return nullptr;
    }
    int getID()
    {
        return buildingID;
    }
    string getName()
    {
        return buildingName;
    }
    double getRate()
    {
        return electricityRate;
    }
    void setName(string name)
    {
        buildingName = name;
    }
    bool setRate(double rate)
    {
        if(rate<=0)
        {
        	return false;
		}
        electricityRate = rate;
        return true;
    }
    bool addRoom(int id, string name, double temp,double minT, double maxT)
    {
        if(roomCount>=MAX_ROOMS)
        {
        	return false;
		}
        if(id<=0||name.empty())
        {
        	return false;
		}
        if(minT>maxT||temp<-20||temp>60)
        {
        	return false;
		}
        if(minT<-20||maxT>60)
        {
        	return false;
		}
        if(findRoom(id)!=nullptr)
        {
        	return false;
		}
        rooms[roomCount].setRoomData(id, name, temp, minT, maxT);
        roomCount++;
        return true;
    }
    bool addRoom(int id, string name)
    {
        return addRoom(id, name, 25, 20, 27);
    }
    bool removeRoom(int id)
    {
        for(int i = 0; i < roomCount; i++)
        {
            if(rooms[i].getID()==id)
            {
                for(int j = i; j < roomCount - 1; j++)
                {
                    rooms[j].moveFrom(rooms[j + 1]);
                }
                rooms[roomCount - 1].clear();
                roomCount--;
                return true;
            }
        }
        return false;
    }
    double calculateTotalPower()
    {
        double total = 0;
        for(int i = 0; i < roomCount; i++)
        {
            total += rooms[i].calculateTotalPower();
        }
        return total;
    }
    double calculateTotalEnergy(double hours)
    {
        return calculateTotalPower() / 1000.0 * hours;
    }
    double calculateTotalEnergy()
    {
        return calculateTotalEnergy(1.0);
    }
    double* saveTemperatures()
    {
    	double* saved;
    	if(roomCount > 0)
    	{
    		saved = new double[roomCount];
		}
		else
		{
			saved = new double[1];
		}
        for(int i = 0; i < roomCount; i++)
        {
        	saved[i] = rooms[i].getTemperature();
		}
        return saved;
    }
    void restoreTemperatures(double* saved)
    {
        for(int i = 0; i < roomCount; i++)
        {
        	rooms[i].setTemperature(saved[i]);
		}
    }
    Room* highestPowerRoom()
    {
        if(roomCount==0)
        {
        	return nullptr;
		}
        Room* best = &rooms[0];
        for(int i = 1; i < roomCount; i++)
        {
            if(rooms[i] > *best)
            {
            	best = &rooms[i];
			}
        }
        return best;
    }
    double calculateCost(double hours)
    {
        return calculateTotalEnergy(hours) * electricityRate;
    }
    int countComfortableRooms()
    {
        int count = 0;
        for(int i = 0; i < roomCount; i++)
        {
            if(rooms[i].isComfortable())
            {
            	count++;
			}
        }
        return count;
    }
    double averageTemperature()
    {
        if(roomCount==0)
        {
        	return 0;
		}
        double total = 0;
        for(int i = 0; i < roomCount; i++)
        {
            total += rooms[i].getTemperature();
        }
        return total / roomCount;
    }
    void simulate(double hours)
    {
        for(int i = 0; i < roomCount; i++)
        {
            rooms[i].simulateTemperature(hours);
        }
    }
    void optimize()
    {
        for(int i = 0; i < roomCount; i++)
        {
            rooms[i].optimize();
        }
    }
    void display()
    {
        cout << endl << "========== BUILDING INFORMATION ==========" << endl;
        cout << "Building ID: " << buildingID << endl;
        cout << "Building: " << buildingName << endl;
        cout << "Rooms: " << roomCount << endl;
        cout << "Total Power: "
             << calculateTotalPower() << " W" << endl;
        cout << "Average Temperature: "
             << averageTemperature() << " C" << endl;
        cout << "Energy per hour: "
             << calculateTotalEnergy() << " kWh" << endl;
        cout << "Comfortable Rooms: "
             << countComfortableRooms()
             << "/" << roomCount << endl;
        Room* top = highestPowerRoom();
        if(top!=nullptr)
        {
        	cout << "Highest power room: " << top->getName() << endl;
		}
        for(int i = 0; i < roomCount; i++)
        {
            rooms[i].display();
        }
    }
    friend class DataLogger;
};

string formatTime(time_t seconds)
{
    char* result = ctime(&seconds);
    if(result == nullptr)
    {
    	return "Unknown time";
	}
    string formatted = result;
    if(!formatted.empty() && formatted[formatted.length() - 1] == '\n')
    {
    	formatted.erase(formatted.length() - 1);
	}
    return formatted;
}

class DataLogger
{
private:
    static string trim(const string& s)
    {
        size_t start = s.find_first_not_of(" \t\r");
        if(start == string::npos)
        {
        	return "";
		}
        size_t end = s.find_last_not_of(" \t\r");
        return s.substr(start, end - start + 1);
    }
    static string toUpper(string s)
    {
        for(int i = 0; i < s.length(); i++)
        {
            if(s[i] >= 'a' && s[i] <= 'z')
            {
            	s[i] = s[i] - 32;
			}
        }
        return s;
    }
    static bool isSkippable(const string& line)
    {
        return (line.empty() || line[0] == '#');
    }
    static int splitLine(const string& line, string fields[], int maxFields)
    {
        int count = 0;
        string part = "";
        for(int i = 0; i <= line.length(); i++)
        {
            if(i == line.length() || line[i] == ',')
            {
                if(count < maxFields)
                {
                	fields[count] = trim(part);
				}
                count++;
                part = "";
            }
            else
            {
            	part += line[i];
			}
        }
        return count;
    }
    static int toInt(const string& text, const string& field, const string& where)
    {
        if(text.empty())
        {
            dataError = where + ": invalid " + field + " '" + text + "'";
            return 0;
        }
        int sign = 1;
        int i = 0;
        long value = 0;
        if(text[0] == '-')
        {
            sign = -1;
            i = 1;
        }
        else if(text[0] == '+')
        {
        	i = 1;
		}
        if(i == text.length())
        {
            dataError = where + ": invalid " + field + " '" + text + "'";
            return 0;
        }
        for(; i < text.length(); i++)
        {
            if(text[i] < '0' || text[i] > '9')
            {
                dataError = where + ": invalid " + field + " '" + text + "'";
                return 0;
            }
            value = value * 10 + (text[i] - '0');
        }
        return (int)(value * sign);
    }
    static double toDouble(const string& text, const string& field, const string& where)
    {
        if(text.empty())
        {
            dataError = where + ": invalid " + field + " '" + text + "'";
            return 0;
        }
        int sign = 1;
        int i = 0;
        double value = 0;
        double decimalPlace = 0.1;
        bool decimalFound = false;
        bool digitFound = false;
        if(text[0] == '-')
        {
            sign = -1;
            i = 1;
        }
        else if(text[0] == '+')
            i = 1;

        for(; i < text.length(); i++)
        {
            if(text[i] == '.' && !decimalFound)
            {
                decimalFound = true;
                continue;
            }

            if(text[i] < '0' || text[i] > '9')
            {
                dataError = where + ": invalid " + field + " '" + text + "'";
                return 0;
            }
            digitFound = true;
            if(decimalFound)
            {
                value += (text[i] - '0') * decimalPlace;
                decimalPlace *= 0.1;
            }
            else
            {
            	value = value * 10 + (text[i] - '0');
			}
        }
        if(!digitFound)
        {
            dataError = where + ": invalid " + field + " '" + text + "'";
            return 0;
        }
        return value * sign;
    }
public:
    static void saveBuilding(Building& b)
    {
        ofstream file("building.txt");
        if(!file)
        {
            cout << "Cannot open building.txt" << endl;
            return;
        }
        file << "# BUILDING,buildingId,buildingName,electricityRatePerKWh,roomCount" << endl;
        file << "BUILDING," << b.buildingID << ","
             << b.buildingName << ","
             << fixed << setprecision(2) << b.electricityRate << ","
             << b.roomCount << endl;
        file << endl;
        file << "# ROOM,roomId,roomName,currentTemperatureC,minComfortC,maxComfortC,deviceCount" << endl;
        int totalDevices = 0;
        for(int i = 0; i < b.roomCount; i++)
        {
            Room& r = b.rooms[i];
            totalDevices += r.deviceCount;
            file << "ROOM," << r.roomID << ","
                 << r.roomName << ","
                 << fixed << setprecision(1)
                 << r.temperature << ","
                 << r.minTemperature << ","
                 << r.maxTemperature << ","
                 << r.deviceCount << endl;
        }
        if(!file)
        {
            cout << "Error while writing building.txt" << endl;
            return;
        }
        file.close();
        ofstream deviceFile("devices.txt");
        if(!deviceFile)
        {
            cout << "Cannot open devices.txt" << endl;
            return;
        }
        deviceFile << "# deviceType,deviceId,roomId,name,powerWatts,isOn,settingName,settingValue" << endl;
        for(int i = 0; i < b.roomCount; i++)
        {
            Room& r = b.rooms[i];
            for(int j = 0; j < r.deviceCount; j++)
            {
                deviceFile << r.devices[j]->saveData(r.roomID) << endl;
            }
        }
        if(!deviceFile)
        {
            cout << "Error while writing devices.txt" << endl;
            return;
        }
        deviceFile.close();
        if(b.roomCount < 6 || totalDevices < 6)
        {
            cout << "The Program can save upto 6 rows in each file (currently "<< b.roomCount << " rooms, " << totalDevices << " devices)." << endl;
        }
        cout << "Building and devices saved successfully!" << endl;
    }
    static void loadBuilding(Building& b)
    {
        Building temp;
        int declared[Building::MAX_ROOMS];
        int expectedRooms = 0;
        bool buildingFound = false;
        string line;
        int lineNo = 0;
        ifstream file("building.txt");
        if(!file)
        {
            cout << "building.txt not found" << endl;
            return;
        }
        while(getline(file, line))
        {
            lineNo++;
            line = trim(line);
            if(isSkippable(line))
            {
            	continue;
			}
            string where = "building.txt line " + to_string(lineNo);
            string f[8];
            int n = splitLine(line, f, 8);
            if(f[0] == "BUILDING")
            {
                if(buildingFound)
                {
                    cout << where << ": duplicate BUILDING record" << endl;
                    return;
                }
                if(n != 5)
                {
                    cout << where << ": BUILDING record needs 5 fields" << endl;
                    return;
                }
                dataError = "";
                int id = toInt(f[1], "building ID", where);
                double rate = toDouble(f[3], "electricity rate", where);
                expectedRooms = toInt(f[4], "room count", where);
                if(!dataError.empty())
                {
                    cout << dataError << endl;
                    return;
                }
                if(id <= 0 || f[2].empty() || rate <= 0 ||expectedRooms < 0 || expectedRooms > Building::MAX_ROOMS)
                {
                    cout << where << ": invalid building data" << endl;
                    return;
                }
                temp.buildingID = id;
                temp.buildingName = f[2];
                temp.electricityRate = rate;
                buildingFound = true;
            }
            else if(f[0] == "ROOM")
            {
                if(!buildingFound)
                {
                    cout << where << ": ROOM record before BUILDING record" << endl;
                    return;
                }
                if(n != 7)
                {
                    cout << where << ": ROOM record needs 7 fields" << endl;
                    return;
                }
                dataError = "";
                int id = toInt(f[1], "room ID", where);
                double t = toDouble(f[3], "temperature", where);
                double minT = toDouble(f[4], "minimum comfort temperature", where);
                double maxT = toDouble(f[5], "maximum comfort temperature", where);
                int dc = toInt(f[6], "device count", where);
                if(!dataError.empty())
                {
                    cout << dataError << endl;
                    return;
                }
                if(dc < 0 || dc > Room::MAX_DEVICES)
                {
                    cout << where << ": invalid device count" << endl;
                    return;
                }
                if(temp.roomCount >= expectedRooms)
                {
                    cout << where << ": more ROOM records than the room count" << endl;
                    return;
                }
                if(!temp.addRoom(id, f[2], t, minT, maxT))
                {
                    cout << where << ": invalid or duplicate room" << endl;
                    return;
                }
                declared[temp.roomCount - 1] = dc;
            }
            else
            {
                cout << where << ": unknown record type '" << f[0] << "'" << endl;
                return;
            }
        }
        file.close();
        if(!buildingFound)
        {
            cout << "building.txt has no BUILDING record" << endl;
            return;
        }
        if(temp.roomCount != expectedRooms)
        {
            cout << "building.txt: room count says "
                 << expectedRooms << " but "
                 << temp.roomCount << " ROOM records found" << endl;
            return;
        }
        ifstream deviceFile("devices.txt");
        if(!deviceFile)
        {
            cout << "devices.txt not found" << endl;
            return;
        }
        lineNo = 0;
        while(getline(deviceFile, line))
        {
            lineNo++;
            line = trim(line);
            if(isSkippable(line))
            {
            	continue;
			}
            string where = "devices.txt line " + to_string(lineNo);
            string f[9];
            int n = splitLine(line, f, 9);
            if(n != 8)
            {
                cout << where << ": device record needs 8 fields" << endl;
                return;
            }
            string type = toUpper(f[0]);
            dataError = "";
            int id = toInt(f[1], "device ID", where);
            int roomID = toInt(f[2], "room ID", where);
            double power = toDouble(f[4], "power", where);
            int state = toInt(f[5], "status", where);
            double setting = toDouble(f[7], "setting value", where);
            if(!dataError.empty())
            {
                cout << dataError << endl;
                return;
            }
            if(id <= 0 || f[3].empty() || power <= 0 ||(state != 0 && state != 1))
            {
                cout << where << ": invalid device data" << endl;
                return;
            }
            Room* r = temp.findRoom(roomID);
            if(r == nullptr)
            {
                cout << where << ": room " << f[2]<< " does not exist in building.txt" << endl;
                return;
            }
            if(r->findDevice(id) != nullptr)
            {
                cout << where << ": duplicate device ID "<< f[1] << " in room " << f[2] << endl;
                return;
            }
            ElectricalDevice* d = nullptr;
            if(type == "AC")
            {
            	d = new AirConditioner(id, f[3], power, state == 1, setting);
			}
            else if(type == "LIGHT")
            {
            	d = new Light(id, f[3], power, state == 1, setting);
			}
            else if(type == "FAN")
            {
            	d = new Fan(id, f[3], power, state == 1, setting);
			}
            else if(type == "HEATER")
            {
            	d = new Heater(id, f[3], power, state == 1, setting);
			}
            else
            {
                cout << where << ": unknown device type '" << f[0] << "'" << endl;
                return;
            }
            if(f[6] != d->getSettingName())
            {
                cout << where << ": setting name should be '"<< d->getSettingName() << "'" << endl;
                delete d;
                return;
            }
            if(!d->isValidSetting(setting))
            {
                cout << where << ": setting value out of range" << endl;
                delete d;
                return;
            }
            if((type == "LIGHT" || type == "FAN") && !d->getStatus())
            {
                cout << "Warning (" << where << "): "<< type << " must remain ON, switched ON." << endl;
                d->turnOn();
            }
            if(!r->addDevice(d))
            {
                cout << where << ": room device limit reached" << endl;
                return;
            }
        }
        deviceFile.close();
        for(int i = 0; i < temp.roomCount; i++)
        {
            if(temp.rooms[i].deviceCount != declared[i])
            {
                cout << "Room " << temp.rooms[i].roomID
                     << ": building.txt declares " << declared[i]
                     << " devices but devices.txt has "
                     << temp.rooms[i].deviceCount << endl;
                return;
            }
        }
        b.moveFrom(temp);
        cout << "Building loaded successfully!" << endl;
    }
    static void exportScenario(Building& b, const string& scenario,const string& timestamp, double hours)
    {
        ifstream check("simulation_results.txt");
        bool needHeader = !check.good() ||check.peek() == ifstream::traits_type::eof();
        check.close();
        ofstream file("simulation_results.txt", ios::app);
        if(!file)
        {
            cout << "Cannot open simulation_results.txt" << endl;
            return;
        }
        if(needHeader)
        {
            file << "# scenario,timestamp,roomId,deviceId,energyKWh,"<< "electricityCost,emissionsKg,roomTemperatureC,"<< "isComfortable" << endl;
        }
        for(int i = 0; i < b.roomCount; i++)
        {
            Room& r = b.rooms[i];
            for(int j = 0; j < r.deviceCount; j++)
            {
                ElectricalDevice* d = r.devices[j];
                double energy = d->calculatePower() / 1000.0 * hours;
                file << scenario << ","
                     << timestamp << ","
                     << r.roomID << ","
                     << d->getID() << ","
                     << fixed << setprecision(3) << energy << ","
                     << setprecision(2) << energy * b.electricityRate << ","
                     << setprecision(3) << energy * EMISSION_FACTOR << ","
                     << setprecision(1) << r.temperature << ","
                     << (r.isComfortable() ? 1 : 0) << endl;
            }
        }
        if(!file)
        {
            cout << "Error while writing simulation_results.txt" << endl;
            return;
        }
        file.close();
    }
    static void loadResults(Building& b)
    {
        ifstream file("simulation_results.txt");
        if(!file)
        {
            cout << "simulation_results.txt not found" << endl;
            return;
        }
        double baseEnergy = 0, baseCost = 0, baseCarbon = 0;
        double optEnergy = 0, optCost = 0, optCarbon = 0;
        int baseRows = 0, optRows = 0, skipped = 0;
        string line;
        int lineNo = 0;
        while(getline(file, line))
        {
            lineNo++;
            line = trim(line);
            if(isSkippable(line))
            {
            	continue;
			}
            string where = "simulation_results.txt line " + to_string(lineNo);
            string f[10];
            int n = splitLine(line, f, 10);
            string error = "";
            if(n != 9)
            {
            	 error = where + ": expected 9 fields but found " + to_string(n);
			}
            string scenario = toUpper(f[0]);
            int roomID = 0, deviceID = 0, comfort = 0;
            double energy = 0, cost = 0, carbon = 0, temperature = 0;
            if(error.empty() && scenario != "BASELINE" && scenario != "OPTIMIZED")
            {
            	error = where + ": unknown scenario '" + f[0] + "'";
			}
            if(error.empty() && f[1].empty())
            {
            	error = where + ": missing timestamp";
			}
            if(error.empty())
            {
                dataError = "";
                roomID = toInt(f[2], "room ID", where);
                deviceID = toInt(f[3], "device ID", where);
                energy = toDouble(f[4], "energy", where);
                cost = toDouble(f[5], "cost", where);
                carbon = toDouble(f[6], "emissions", where);
                temperature = toDouble(f[7], "temperature", where);
                comfort = toInt(f[8], "comfort flag", where);
                if(!dataError.empty())
                {
                	error = dataError;
				}
            }
            if(error.empty() &&
               (energy < 0 || cost < 0 || carbon < 0 ||
                temperature < -20 || temperature > 60 ||
                (comfort != 0 && comfort != 1)))
            {
                error = where + ": value out of range";
            }
            if(error.empty())
            {
                Room* r = b.findRoom(roomID);
                if(r == nullptr || r->findDevice(deviceID) == nullptr)
                {
                    error = where + ": room " + f[2] +" / device " + f[3] +" not found in the current building";
                }
            }
            if(!error.empty())
            {
                skipped++;
                continue;
            }

            if(scenario == "BASELINE")
            {
                baseEnergy += energy;
                baseCost += cost;
                baseCarbon += carbon;
                baseRows++;
            }
            else
            {
                optEnergy += energy;
                optCost += cost;
                optCarbon += carbon;
                optRows++;
            }
        }
        file.close();
        cout << fixed << setprecision(3);
        cout << endl << "========== SAVED RESULTS COMPARISON ==========" << endl;
        cout << "Rows read: baseline " << baseRows
             << ", optimized " << optRows
             << ", skipped " << skipped << endl;
        if(baseRows + optRows == 0)
        {
            cout << "No valid rows found. Load the matching building first (option 10)." << endl;
            return;
        }
        cout << "Baseline  : " << baseEnergy << " kWh | Cost "
             << setprecision(2) << baseCost
             << " | CO2 " << setprecision(3) << baseCarbon << " kg" << endl;
        cout << "Optimized : " << optEnergy << " kWh | Cost "
             << setprecision(2) << optCost
             << " | CO2 " << setprecision(3) << optCarbon << " kg" << endl;
        cout << "Energy Saved: " << baseEnergy - optEnergy << " kWh" << endl;
        if(baseEnergy > 0)
        {
            cout << "Energy Reduction: " << setprecision(2)
                 << (baseEnergy - optEnergy) / baseEnergy * 100
                 << "%" << endl;
        }
    }
};

void discardRestOfLine()
{
    cin.ignore(10000, '\n');
    if(cin.eof())
    {
    	cin.clear();
	}
}

int readInt(string message)
{
    int value;
    while(true)
    {
        cout << message;
        if(cin >> value)
        {
            cin.ignore(10000, '\n');
            if(cin.eof()) cin.clear();
            return value;
        }
        if(cin.eof())
        {
        	return 0;
		}
        cout << "Invalid input. Enter an integer." << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

double readDouble(string message)
{
    double value;
    while(true)
    {
        cout << message;
        if(cin >> value && value >= 0)
        {
            cin.ignore(10000, '\n');
            if(cin.eof())
            {
            	cin.clear();
			}
            return value;
        }
        if(cin.eof()) return 0;
        cout << "Invalid input. Enter a valid number." << endl;
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

string readName(string message)
{
    string value;
    while(true)
    {
        cout << message;
        if(getline(cin, value))
        {
            size_t start = value.find_first_not_of(" \t\r");
            if(start == string::npos)
            {
            	value = "";
			}
            else
            {
            	value = value.substr(start, value.find_last_not_of(" \t\r") - start + 1);
			}
            if(!value.empty() && value.find(',') == string::npos && value[0] != '#')
            {
            	return value;
			}
        }
        else if(cin.eof())
        {
        	return "Building";
		}
        cout << "Invalid name. It cannot be empty, contain commas or start with #." << endl;
        cin.clear();
    }
}

void addRoomMenu(Building& building)
{
    int id = readInt("Enter Room ID: ");
    string name = readName("Enter Room Name: ");
    int useDefault = readInt("Use default temperature 25 C and comfort range 20-27 C? (1=Yes, 0=No): ");
    bool added;
    if(useDefault==1)
    {
        added = building.addRoom(id, name);
    }
    else
    {
        double temp = readDouble("Current Temperature: ");
        double minT = readDouble("Minimum Comfort Temperature: ");
        double maxT = readDouble("Maximum Comfort Temperature: ");
        added = building.addRoom(id, name, temp, minT, maxT);
    }
    if(added)
        cout << "Room added successfully!" << endl;
    else
        cout << "Unable to add room. Check ID, limits and capacity." << endl;
}

void addDeviceMenu(Building& building)
{
    int roomID = readInt("Enter Room ID: ");
    Room* room = building.findRoom(roomID);
    if(room==nullptr)
    {
        cout << "Room not found!" << endl;
        return;
    }
    cout << endl << "1. Air Conditioner";
    cout << endl << "2. Light";
    cout << endl << "3. Fan";
    cout << endl << "4. Heater" << endl;
    int choice = readInt("Select Device: ");
    if(choice<1||choice>4)
    {
        cout << "Invalid selection!" << endl;
        return;
    }
    int id = readInt("Enter Device ID: ");
    if(id<=0)
    {
        cout << "Device ID must be greater than zero!" << endl;
        return;
    }
    if(room->findDevice(id)!=nullptr)
    {
        cout << "Device ID already exists in this room!" << endl;
        return;
    }
    string name = readName("Enter Device Name: ");
    double power = readDouble("Enter Rated Power (Watts): ");
    if(power<=0)
    {
        cout << "Power must be greater than zero!" << endl;
        return;
    }
    string prompt;
    if(choice==1)
    {
    	prompt = "Enter AC Target Temperature (16-30 C): ";
	}
    else if(choice==2)
    {
    	prompt = "Enter Brightness (1-100): ";
	}
    else if(choice==3)
    {
    	prompt = "Enter Fan Speed (1-3): ";
	}
    else
    {
    	prompt = "Enter Heater Setting (0-100): ";
	}
    double setting = readDouble(prompt);
    ElectricalDevice* device = nullptr;
    if(choice==1)
    {
    	device = new AirConditioner(id, name, power, true, setting);
	}
    else if(choice==2)
    {
    	device = new Light(id, name, power, true, setting);
	}
    else if(choice==3)
    {
    	device = new Fan(id, name, power, true, setting);
	}
    else
    {
    	device = new Heater(id, name, power, true, setting);
	}
    if(!device->isValidSetting(setting))
    {
        cout << "Invalid setting!" << endl;
        delete device;
        return;
    }
    if(choice==1||choice==4)
    {
        int state = readInt("Turn device ON? (1=Yes, 0=No): ");
        if(state!=0&&state!=1)
        {
            cout << "Invalid choice!" << endl;
            delete device;
            return;
        }
        if(state==0)
        {
        	device->turnOff();
		}
    }
    if(room->addDevice(device))
    {
    	cout << "Device added successfully!" << endl;
	}
}

void controlDevice(Building& building)
{
    int roomID = readInt("Enter Room ID: ");
    Room* room = building.findRoom(roomID);
    if(room==nullptr)
    {
        cout << "Room not found!" << endl;
        return;
    }
    int id = readInt("Enter Device ID: ");
    ElectricalDevice* device = room->findDevice(id);
    if(device==nullptr)
    {
        cout << "Device not found!" << endl;
        return;
    }
    cout << endl << "1. Turn ON";
    cout << endl << "2. Turn OFF";
    cout << endl << "3. Change Setting" << endl;
    int choice = readInt("Enter Choice: ");
    if(choice==1)
    {
        device->turnOn();
    }
    else if(choice==2)
    {
        if(device->getType()=="LIGHT"||device->getType()=="FAN")
        {
            cout << "Lights and Fans must remain ON!" << endl;
            return;
        }
        device->turnOff();
    }
    else if(choice==3)
    {
        double setting = readDouble("Enter New " + device->getSettingName() + ": ");
        if(!device->isValidSetting(setting))
        {
            cout << "Invalid setting for this device!" << endl;
            return;
        }
        device->setSetting(setting);
    }
    else
    {
        cout << "Invalid choice!" << endl;
        return;
    }
    cout << "Device updated successfully!" << endl;
}

void removeDeviceMenu(Building& building)
{
    int roomID = readInt("Enter Room ID: ");
    Room* room = building.findRoom(roomID);
    if(room==nullptr)
    {
        cout << "Room not found!" << endl;
        return;
    }
    int id = readInt("Enter Device ID: ");
    if(room->removeDevice(id))
    {
    	cout << "Device removed successfully!" << endl;
	}
    else
    {
    	cout << "Device not found!" << endl;
	}
}

void simulationMenu(Building& building)
{
    double hours = readDouble("Enter Simulation Hours: ");
    if(hours<=0)
    {
        cout << "Hours must be greater than zero!" << endl;
        return;
    }
    double power = building.calculateTotalPower();
    double energy = building.calculateTotalEnergy(hours);
    double cost = building.calculateCost(hours);
    cout << fixed << setprecision(2);
    cout << endl << "          SIMULATION RESULTS          " << endl;
    cout << "Total Power: " << power << " W" << endl;
    cout << "Energy Consumption: " << energy << " kWh" << endl;
    cout << "Electricity Cost: " << cost << endl;
    building.simulate(hours);
    cout << endl << "Temperature simulation completed." << endl;
    cout << "Comfortable Rooms: "<< building.countComfortableRooms()<< "/" << building.getRoomCount() << endl;
}

void optimizationMenu(Building& building)
{
    double hours = readDouble("Enter Comparison Hours: ");
    if(hours<=0)
    {
        cout << "Hours must be greater than zero!" << endl;
        return;
    }
    time_t start = time(0);
    string baselineTime = formatTime(start);
    string optimizedTime = formatTime(start + (time_t)(hours * 3600));
    double* startTemps = building.saveTemperatures();
    double before = building.calculateTotalPower();
    double energyBefore = before / 1000.0 * hours;
    double costBefore = energyBefore * building.getRate();
    building.simulate(hours);
    int comfortBefore = building.countComfortableRooms();
    cout << fixed << setprecision(2);
    cout << endl << "          BASELINE RESULTS          " << endl;
    cout << "Power: " << before << " W" << endl;
    cout << "Energy: " << energyBefore << " kWh" << endl;
    cout << "Cost: " << costBefore << endl;
    cout << "Comfortable rooms after " << hours << " h: "<< comfortBefore << "/" << building.getRoomCount() << endl;
    for(int i = 0; i < building.getRoomCount(); i++)
    {
    	cout << "  " << *building.getRoom(i) << endl;
	}
    DataLogger::exportScenario(building, "BASELINE", baselineTime, hours);
    building.restoreTemperatures(startTemps);
    delete[] startTemps;
    building.optimize();
    double after = building.calculateTotalPower();
    double energyAfter = after / 1000.0 * hours;
    double costAfter = energyAfter * building.getRate();
    building.simulate(hours);
    int comfortAfter = building.countComfortableRooms();
    cout << endl << "          OPTIMIZED RESULTS          " << endl;
    cout << "Power: " << after << " W" << endl;
    cout << "Energy: " << energyAfter << " kWh" << endl;
    cout << "Cost: " << costAfter << endl;
    cout << "Comfortable rooms after " << hours << " h: "<< comfortAfter << "/" << building.getRoomCount() << endl;
    for(int i = 0; i < building.getRoomCount(); i++)
    {
    	cout << "  " << *building.getRoom(i) << endl;
	}
    cout << endl << "          SAVINGS         " << endl;
    cout << "Power Saved: " << before - after << " W" << endl;
    cout << "Energy Saved: "<< energyBefore - energyAfter << " kWh" << endl;
    cout << "Cost Saved: " << costBefore - costAfter << endl;
    if(energyBefore>0)
    {
        double percentage =(energyBefore - energyAfter) / energyBefore * 100;
        cout << "Energy Reduction: "<< percentage << "%" << endl;
    }
    cout << "Comfort: " << comfortBefore << " -> " << comfortAfter<< " rooms within range" << endl;
    if(comfortAfter<comfortBefore)
    {
    	cout << "Warning: optimization reduced comfort!" << endl;
	}
    bool advice = false;
    for(int i = 0; i < building.getRoomCount(); i++)
    {
        if(!building.getRoom(i)->isComfortable())
        {
            if(!advice)
            {
                cout << endl << "---------- ROOMS STILL OUTSIDE RANGE ----------" << endl;
                advice = true;
            }
            building.getRoom(i)->printAdvice();
        }
    }
    cout << endl << "Optimization completed!" << endl;
    DataLogger::exportScenario(building, "OPTIMIZED", optimizedTime, hours);
    cout << "Simulation results exported!" << endl;
}

int main()
{
    int buildingID=readInt("Enter Building ID: ");
    while(buildingID<=0)
    {
        if(cin.eof()) return 0;
        cout<<"Building ID must be greater than zero!"<<endl;
        buildingID=readInt("Enter Building ID: ");
    }
    string name=readName("Enter Building Name: ");
    double rate=readDouble("Enter Electricity Rate per kWh: ");
    while(rate<=0)
    {
        if(cin.eof()) return 0;
        cout<<"Rate must be greater than zero!"<<endl;
        rate=readDouble("Enter Electricity Rate: ");
    }
    Building building(buildingID,name,rate);
    int choice=-1;
    do
    {
        cout<<endl<<endl;
        cout<<endl;
        cout<<" SMART BUILDING ENERGY MANAGEMENT"<<endl;
        cout<<endl;
        cout<<"1. Add Room"<<endl;
        cout<<"2. Remove Room"<<endl;
        cout<<"3. Add Electrical Device"<<endl;
        cout<<"4. Remove Electrical Device"<<endl;
        cout<<"5. Control Electrical Device"<<endl;
        cout<<"6. Display Building Information"<<endl;
        cout<<"7. Run Energy Simulation"<<endl;
        cout<<"8. Optimize Energy Consumption"<<endl;
        cout<<"9. Save Building Data"<<endl;
        cout<<"10. Load Building Data"<<endl;
        cout<<"11. Change Electricity Rate"<<endl;
        cout<<"12. View Saved Results Comparison"<<endl;
        cout<<"0. Exit"<<endl;
        cout<<endl;
        choice=readInt("Enter Your Choice: ");
        if(cin.eof()) break;
        switch(choice)
        {
            case 1: addRoomMenu(building);
            {
            	break;
			}
            case 2:
            {
                int id=readInt("Enter Room ID: ");
                if(building.removeRoom(id)) cout<<"Room removed successfully!"<<endl;
                else cout<<"Room not found!"<<endl;
                break;
            }
            case 3: addDeviceMenu(building);
            {
            	break;
			}
            case 4: removeDeviceMenu(building);
            {
            	break;
			}
            case 5: controlDevice(building);
            {
            	break;
			}
            case 6: building.display();
            {
            	break;
			}
            case 7: simulationMenu(building);
            {
            	break;
			}
            case 8: optimizationMenu(building);
            {
            	break;
			}
            case 9: DataLogger::saveBuilding(building);
            {
            	break;
			}
            case 10: DataLogger::loadBuilding(building); 
            {
            	break;
			}
            case 11:
            {
                double newRate=readDouble("Enter New Electricity Rate: ");
                if(building.setRate(newRate))
                {
                	cout<<"Rate updated successfully!"<<endl;
				}
                else
                {
                	cout<<"Invalid electricity rate."<<endl;
				}
                break;
            }
            case 12: DataLogger::loadResults(building);
            {
            	cout<<"Invalid electricity rate."<<endl;
			}
            case 0: cout<<"Exiting Program..."<<endl; 
            {
            	break;
			}
            default: cout<<"Invalid menu choice!"<<endl;
        }
    } while(choice!=0);
    return 0;
}
