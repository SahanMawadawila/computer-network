#include <iostream>
#include<map>
#include<fstream>
#include<string>
#include<cstdlib>
#include<iomanip>

using namespace std;

class Packet {  
public:
    string sourceIp, destinationIp, sourceMac, destinationMac;
    Packet():sourceIp(""), destinationIp(""), sourceMac(""), destinationMac("") {}
    void packetDetails() {
        cout << "\tPacket details  : " << endl;
        cout << "\tSource Ip       : " << sourceIp << endl;
        cout << "\tDestination IP  : " << destinationIp << endl;
        cout << "\tSource MAC      : " << sourceMac << endl;
        cout << "\tDestination MAC : " << destinationMac << endl;
    }
};


class NetworkNode {
private:
    string name;
    string macAddress;
public:
    NetworkNode(string name1,string macAddress1):name(name1),macAddress(macAddress1) {}
    NetworkNode(string name1):name(name1){}
   
    string getName() {
        return name;
    }
    string getMacAddress() {
        return macAddress;
    }

};
class Computer:public NetworkNode {
private:
    map<string, string> routing_table;
    string ipAddress;
    string defaultGateway;
public:
    Packet p1;
    string portno;
    map<string, string> arp_table;
    Computer(string name1,string macAddress1,string defaultGateway1,string ipaddress1,map<string,string>arp,string portno_):NetworkNode(name1,macAddress1) {
        ipAddress = ipaddress1;
        defaultGateway = defaultGateway1;
        arp_table = arp;
        portno = portno_;
    }
    void printArrival() {
        cout << "Packet arrived to computer " << getName() << " from " << portno << "." << endl;
    }
    void printLeave() {
        cout << "Packet leaving computer " << getName() <<" from "<<portno<<"." << endl;
    }
    void printDetails() {
        cout << ipAddress << endl;
        cout << defaultGateway << endl;
        cout << getName() << endl;
        cout << getMacAddress() << endl;
        for (auto& pair : arp_table) {
            cout << pair.first << "," << pair.second << endl;
        }
    }
    string getIp() {
        return ipAddress;
    }
    string getDefaultGateway() {
        return defaultGateway;
    }
  
};



class Router:public NetworkNode {
public:
    map<string, string> routing_table;
    map<string, string>arpTable;
    Router(string name_,map<string,string>routingTable_,map<string,string> arpTable_):NetworkNode(name_),routing_table(routingTable_),arpTable(arpTable_) {}
    void printArrival(string portNo) {
        cout << "Packet arrived to Router " << getName() <<" from "<<portNo<<"." << endl;
    }
    void printLeave(string portNo) {
        cout << "Packet leaving Router " << getName() <<" from " << portNo << "." << endl;
    }
};

class Switch:public NetworkNode {
public:
    map<string, string> macTable;
    Switch(string name_,map<string,string>macTable_):NetworkNode(name_),macTable(macTable_) {}
    void printArrival(string portNo) {
        cout << "Packet arrived to switch " << getName()<< " from " << portNo << "." << endl;
    }
    void printLeave(string portNo) {
        cout << "Packet leaving Switch " << getName() << " from " << portNo << "." << endl;
    }
};
void simulatteSwitch(string port6, Packet& pkt, Switch& s1, Switch& s2, Computer* ptr[], Router& r);

void computerTxt(string fileName,string& name_,string& macAddress_, string& defaultGateway_,string& ipAddress_,map<string, string>& arpTable,string& portNo_) {
    string fileLine;
    ifstream file;
    file.open(fileName);
    bool shouldRead = false;
    while (getline(file, fileLine)) {
        size_t colonPos = fileLine.find(':');
        string key = fileLine.substr(0, colonPos);
        size_t valueStart = colonPos + 1;
        while (valueStart < fileLine.size() && fileLine[valueStart] == ' ') {
            valueStart++; // Skip spaces after ':'
        }
        string value = fileLine.substr(valueStart);

        if (key == "Name") {
            name_ = value;
        }
        else if (key == "MAC_Address") {
            macAddress_ = value;
        }
        else if (key == "Default_Gateway") {
            defaultGateway_ = value;
        }
        else if (key == "IP_Address") {
            ipAddress_ = value;
        }
        else if (key == "Port") {
            portNo_ = value;
        }

        else if (fileLine.find("ARP_Table") != string::npos) {
            shouldRead = true;
            continue; // Skip the line with the keyword
        }

        if (shouldRead) {
            size_t spacePos = fileLine.find(',');
            string ip = fileLine.substr(0, spacePos);
            string mac = fileLine.substr(spacePos + 1);
            arpTable[ip] = mac;

        }
    }
    file.close();
}

void SwitchTxt(string fileName,string& name_,map<string,string>& macTable_) {
    ifstream file;
    string fileLine;
    file.open(fileName);
    bool shouldRead = false;
    while (getline(file, fileLine)) {
        size_t colonPos = fileLine.find(':');
        string key = fileLine.substr(0, colonPos);
        size_t valueStart = colonPos + 1;
        while (valueStart < fileLine.size() && fileLine[valueStart] == ' ') {
            valueStart++; // Skip spaces after ':'
        }
        string value = fileLine.substr(valueStart);
        if (key == "Name") {
            name_ = value;
        }
        else if (fileLine.find("MAC_Table") != string::npos) {
            shouldRead = true;
            continue; // Skip the line with the keyword
        }

        if (shouldRead) {
            size_t spacePos = fileLine.find(',');
            string mac = fileLine.substr(0, spacePos);
            string port = fileLine.substr(spacePos + 1);
            macTable_[mac] = port;

        }

    }
}

void routerTxt(string fileName, string& name_, map<string, string>& routingTable,map<string,string>& arpTable) {
    ifstream file;
    string fileLine;
    file.open(fileName);
    bool shouldRead = false;
    bool shouldRead2 = false;
    while (getline(file, fileLine)) {
        if (fileLine == "") {
            continue;
        }
        size_t colonpos = fileLine.find(':');
        string key = fileLine.substr(0, colonpos);
        size_t valueStart = colonpos + 1;
        while (valueStart < fileLine.size() && fileLine[valueStart] == ' ') {
            valueStart++; // Skip spaces after ':'
        }
        string value = fileLine.substr(valueStart);
        if (key == "Name") {
            name_ = value;
        }
        else if (fileLine.find("Routing_Table")!=string::npos) {
            shouldRead = true;
            continue;
        }
        
        else if (fileLine.find("ARP_Table") != string::npos) {
            shouldRead = false;
            shouldRead2 = true;
            continue; // Skip the line with the keyword
        }

        if (shouldRead2) {
            size_t spacePos = fileLine.find(',');
            string ip = fileLine.substr(0, spacePos);
            string mac = fileLine.substr(spacePos + 1);
            arpTable[ip] = mac;

        }
        if (shouldRead) {
            size_t spacePos = fileLine.find(',');
            string networkAddress = fileLine.substr(0, spacePos);
            string port = fileLine.substr(spacePos + 1);
            routingTable[networkAddress] = port;
        }
    }

}

void selectComputer(Computer** ptr2,Computer* obj1,Computer* obj2,Computer* obj3, Computer* obj4) {
    int choice;
    cout << "Select your computer:" << endl;
    cout << "1." << obj1->getName() << endl;
    cout << "2." << obj2->getName() << endl;
    cout << "3." << obj3->getName() << endl;
    cout << "4." << obj4->getName() << endl;
    cin >> choice;
    if (choice == 1) {
        *ptr2 = obj1;
    }
    else if (choice == 2) {
        *ptr2 = obj2;
    }
    else if (choice == 3) {
        *ptr2 = obj3;
    }
    else if (choice == 4) {
        *ptr2 = obj4;
    }
    system("cls");
    cout << "Your device is:" << (*ptr2)->getName() << endl << endl;
    cout << "******* Connected devices Details *******" << endl;
    cout << "Device Name" << setw(16) << "IP Address" << setw(17) << "MAC Address" << endl;
    cout << obj1->getName() << setw(23) << obj1->getIp() << setw(13) << obj1->getMacAddress() << endl;
    cout << obj2->getName() << setw(23) << obj2->getIp() << setw(14) << obj2->getMacAddress() << endl;
    cout << obj3->getName() << setw(23) << obj3->getIp() << setw(13) << obj3->getMacAddress() << endl;
    cout << obj4->getName() << setw(23) << obj4->getIp() << setw(14) << obj4->getMacAddress() << endl;

}
bool isANumber(string str) {
    bool final = true;
    for (char c : str) {
        int x = int(c);
        if (!(x >= 48 && x <= 57)) {
            final = false;
        }
    }
    return final;
}
bool isBetween0And255(string str) {
    int value = stoi(str);
    if (value >= 0 && value <= 255) {
        return true;
    }
    return false;
}
bool isValidIp(string str) {
    //checking there 3 "."
    int count = 0;
    for (char c : str) {
        if (c == '.') {
            count++;
        }
    }
    if (count == 3) {
        size_t colonpos = str.find('.');
        string first = str.substr(0, colonpos);
        string remaining = str.substr(colonpos + 1, string::npos);
        colonpos = remaining.find('.');
        string second = remaining.substr(0, colonpos);
        remaining = remaining.substr(colonpos + 1, string::npos);
        colonpos = remaining.find('.');
        string third = remaining.substr(0, colonpos);
        remaining = remaining.substr(colonpos + 1, string::npos);
        colonpos = remaining.find('.');
        string fouth = remaining.substr(0, colonpos);
        if (!first.empty() && !second.empty() && !third.empty() && !fouth.empty()) {
            if (isANumber(first) && isANumber(second) && isANumber(third) && isANumber(fouth)) {
                if (isBetween0And255(first) && isBetween0And255(second) && isBetween0And255(third) && isBetween0And255(fouth)) {
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
        
    }
    else {
        return false;
    }
}
string GetIpFromUser() {
    string ipAdd;
    cout << "\nThis simulator will show how packet moves in a real network." << endl;
    cout << "You can enter: \n\t#Invalid Ips \n\t#Your device IP \n\t#Connected devices IPs in same subnet \n\t#Not-Connected devices IPs in same subnet "
        "\n\t#Connected devices IPs in other subnet \n\t#Not-Connected devices IPs in other subnet \n\t#Not-connected subnets Ips " << endl;
    cout << "Enter the IP address you want to Ping: " << endl;
    cin >> ipAdd;

    //checking is it a valid ip or not
    if (!isValidIp(ipAdd)) {
        cout << "Error..Invalid IP" << endl;
        return "";
    }
    else
        return ipAdd;
}

void simulateRouter(Packet& pkt,Router& r,string port4, Switch& s1, Switch& s2, Computer* ptr[]) {
    r.printArrival(port4);
    pkt.packetDetails();
    pkt.sourceMac = pkt.destinationMac;

    //getting first 3 potions of IP
    int count = 0;
    int position1 = 0;
    for (char c : pkt.destinationIp) {
        position1++;
        if (c == '.') {
            count++;
        }
        if (count == 3) {
            break;
        }
    }
    string port3;
    string destinationNetworkIp = pkt.destinationIp.substr(0, position1 - 1) + ".0";
    bool x = false;
    for (auto& pair : r.routing_table) {
        if (destinationNetworkIp == pair.first) {
            port3 = pair.second;
            x = true;
            break;
        }
   }
    if (!x) {
        cout << pkt.destinationIp << " is unreachable. There is no matching subnet called " << destinationNetworkIp << " in the routing table of " << r.getName() << ".Packet is dropped at the router. " <<endl;
        return;
    }
    bool y = false;
    for (auto& pair : r.arpTable) {
        if (pkt.destinationIp == pair.first) {
            pkt.destinationMac = pair.second;
            y = true;
            break;
        }
    }
    if (!y) {
        cout << "Cannot find any matching entry of the arp table of router " << r.getName() << ".\nThere is no device having Ip "<<pkt.destinationIp<< " connected to subnet " << destinationNetworkIp << ".\nPacket is dropped at the router. " << endl;
        return;
    }
    r.printLeave(port3);
    pkt.packetDetails();
    return simulatteSwitch(port3, pkt, s1, s2, ptr,r);

}
void simulatteSwitch(string port6,Packet& pkt,Switch& s1,Switch& s2,Computer* ptr[],Router& r) {
    Switch* arr[] = { &s1,&s2 };
    Switch* currentSwitch = NULL;
    string port2 ;
    for (int i = 0; i <= 1; i++) {
        for (auto& pair : arr[i]->macTable) {
            if (port6 == pair.second) {
                currentSwitch = arr[i];
                break;
            }
        }
        if (currentSwitch != NULL) {
            break;
        }
    }
    
    currentSwitch->printArrival(port6);
    pkt.packetDetails();
    for (auto& pair2 : currentSwitch->macTable) {
        if (pkt.destinationMac == pair2.first) {
            port2 = pair2.second;
            break;
        }
        
    }

   
    currentSwitch->printLeave(port2);
    pkt.packetDetails();
    for (int i = 0; i <= 3; i++) {
        if (ptr[i]->portno == port2) {
            ptr[i]->printArrival();
            pkt.packetDetails();
            return;
       }
    }
    return simulateRouter(pkt, r,port2, s1, s2, ptr);

}

void simulatePacket(Computer* ptr[], Computer* userPc, string usergivenIp, Switch& s1, Switch& s2,Router& r) {
    userPc->p1.sourceIp = userPc->getIp();
    userPc->p1.destinationIp = usergivenIp;
    userPc->p1.sourceMac = userPc->getMacAddress();
    
    //getting first 3 potions of IP
    int count = 0;
    int position1 = 0;
    for (char c : usergivenIp) {
        position1++;
        if (c == '.') {
            count++;
        }
        if (count == 3) {
            break;
        }
    }

    count = 0;
    int position2 = 0;
    string userPcIp = userPc->getIp();
    for (char c : userPcIp) {
        position2++;
        if (c == '.') {
            count++;
        }
        if (count == 3) {
            break;
        }
    }

    if (userPc->getIp() == usergivenIp) {
        cout << "Destination reached. Pinging to own device." << endl;
        return;
    }
    else if (usergivenIp.substr(0, position1 - 1) == userPcIp.substr(0, position2 - 1)) {
        bool x = true;
        for (auto& pair : userPc->arp_table) {
            if (pair.first == usergivenIp) {
                userPc->p1.destinationMac = pair.second;
                x = false;
                break;
            }
        }
        if (x) {
            cout << "Destination unreachable. Packet doesnot leave from " << userPc->getName() << ". There is no device having the Ip " << usergivenIp << " of the subnet " << usergivenIp.substr(0, position1 - 1) << ".0" << endl;
            return;
        }
    }
    else {
        for (auto& pair : userPc->arp_table) {
            if (pair.first == userPc->getDefaultGateway()) {
                userPc->p1.destinationMac = pair.second;
                break;
            }
        }
    }
    userPc->printLeave();
    userPc->p1.packetDetails();
    string port5 = userPc->portno;
    return simulatteSwitch(port5,userPc->p1, s1, s2,ptr,r);
  
}

int main() {

    string name_, macAddress_, defaultGateway_, ipAddress_, portNo_;
    map<string, string> arpTable;
    string userGivingIp;
    
    //creating computer objects
    computerTxt("computer1_details_arp_table.txt", name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    Computer pc1(name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    arpTable.clear();
    computerTxt("computer2_details_arp_table.txt", name_, macAddress_, defaultGateway_, ipAddress_, arpTable, portNo_);
    Computer pc2(name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    arpTable.clear();
    computerTxt("computer3_details_arp_table.txt", name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    Computer pc3(name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    arpTable.clear();
    computerTxt("computer4_details_arp_table.txt", name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    Computer pc4(name_, macAddress_, defaultGateway_, ipAddress_, arpTable,portNo_);
    arpTable.clear();

    //creating switch objects
    string switchName;
    map<string, string>MacTable;
    SwitchTxt("switch1_details_MAC_table.txt", switchName, MacTable);
    Switch s1(switchName, MacTable);
    MacTable.clear();
    SwitchTxt("switch2_details_MAC_table.txt", switchName, MacTable);
    Switch s2(switchName, MacTable);
    MacTable.clear();
    

    //creating router object
    string routername;
    map<string, string>routingTable;
    map<string, string>arpTable_;
    routerTxt("router_routing_and_arp_table.txt", routername, routingTable,arpTable_);
    Router r(routername, routingTable,arpTable_);
    routingTable.clear();
    arpTable_.clear();

    Computer* selectedComputer = NULL;
    selectComputer(&selectedComputer, &pc1, &pc2, &pc3, &pc4);
    Computer* arr[] = { &pc1, &pc2,&pc3,&pc4 };
    
    char option;
    do {
        userGivingIp = GetIpFromUser();
        if (!userGivingIp.empty()) {
            simulatePacket(arr, selectedComputer, userGivingIp, s1, s2, r);
        }
        cout << "\n\nDo you want to ping again? (y/n):";
        cin >> option;
    } while (option == 'y' || option == 'Y');
   
    return 0;
} 
