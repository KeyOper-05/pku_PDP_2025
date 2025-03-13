#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>
using namespace std;
enum CommanderType {
    RED = 0, BLUE = 1
};
enum Type {
    DRAGON = 0, NINJA = 1, ICEMAN = 2, LION = 3, WOLF = 4
};
string strCommander[2] = {"red", "blue"};
string strWeapon[3] = {"sword", "bomb", "arrow"};
string strType[5] = {"dragon","ninja","iceman","lion","wolf"};
class Weapon;
class Warrior { //Warrior基类
public:
    static int lifeCostBase[5];
    float morale = 0.0;
    static int attackBase[5];
    vector<Weapon*> weapon = {}; //后面对应改
    int loyalty = 0;
    int attack = 0;
    //输出
    Warrior(int time, int commander, int type, int number, int strength, int total) {
        cout << setw(3) << setfill('0') << time <<' ';
        cout << strCommander[commander] << ' ';
        cout << strType[type] << ' ' << number << ' ';
        cout << "born with strength " << strength << ',';
        cout << total << ' ' << strType[type] << " in " << strCommander[commander] << " headquarter" << endl;
    }
};

class Weapon {
public:
    int durablity = 0; //-1意为无耐久
    int attack = 0;
    string weapon_name;
    int number;
    virtual float use(Warrior* self, Warrior* enemy);
};
class arrow: public Weapon{
    arrow(int number): Weapon(){
        this->number = number;
        weapon_name = "arrow";
    }
    virtual float use(Warrior* self, Warrior* enemy) {
        attack = int( self->attack / 5);
    }
};
class sword: public Weapon{
    sword(): Weapon() {

    }
    virtual float use(Warrior* self, Warrior* enemy) {

    }
};
class bomb: public Weapon{
    bomb(): Weapon() {

    }
    virtual float use(Warrior* self, Warrior* enemy) {

    }
};

class cities {
public:
    int number;

};
int Warrior::lifeCostBase[5];
int Warrior::attackBase[5];
class dragon: public Warrior {
public:
    dragon(int time, int commander, int type, int number, int strength, int total, int remainlife): Warrior(time,commander,type,number,strength,total){
        this -> morale = (float)remainlife/(float)strength; //此处不确定初始生命值是否在游戏过程中会修改，因此这里不使用确定的lifecostBase
        this -> weapon.push_back(number % 3);
        attack = attackBase[DRAGON];
        cout << "It has a " << strWeapon[weapon[0]] << ",and it's morale is " << setprecision(2) << fixed << morale << endl;
    }
};
class ninja: public Warrior {
public:
    ninja(int time, int commander, int type, int number, int strength, int total, int remainlife): Warrior(time,commander,type,number,strength,total){
        this -> weapon.push_back(number % 3);
        this -> weapon.push_back((number+1) % 3);
        attack = attackBase[NINJA];
        cout << "It has a " << strWeapon[weapon[0]] << " and a " << strWeapon[weapon[1]] << endl;
    }
};
class iceman: public Warrior {
public:
    iceman(int time, int commander, int type, int number, int strength, int total, int remainlife): Warrior(time,commander,type,number,strength,total){
        this -> weapon.push_back(number % 3);
        attack = attackBase[ICEMAN];
        cout << "It has a " << strWeapon[weapon[0]] << endl;
    }
};
class lion: public Warrior {
public:
    lion(int time, int commander, int type, int number, int strength, int total, int remainlife): Warrior(time,commander,type,number,strength,total){
        this -> loyalty = remainlife;
        attack = attackBase[LION];
        cout << "It's loyalty is " << loyalty << endl;
    }
};
int redSeq[5] = {ICEMAN, LION, WOLF, NINJA, DRAGON};
int blueSeq[5] = {LION, DRAGON, NINJA, ICEMAN, WOLF};
class wolf: public Warrior {
public:
    wolf(int time, int commander, int type, int number, int strength, int total, int remainlife): Warrior(time,commander,type,number,strength,total) {
        attack = attackBase[WOLF];
    }
};
class commander {
public:
    //when construct a warrior, int time, int commander, int type, int number, int strength, int total, int remainlife
    bool availability = true;
    int commandertype = 0;
    int lifebase = 0;
    int count[5] = {};
    int current = -1;
    vector<Warrior*> warriors;
    int seq[5] = {}; //指向数组的指针，不能直接用，和*seq不一样？
    commander(int type, int commanderLife) {
        this -> commandertype = type;
        lifebase = commanderLife;
        if(type == RED) {
            for(int i=0; i<5; i++) seq[i] = redSeq[i];
        }
        if(type == BLUE) {
            for(int i=0; i<5; i++) seq[i] = blueSeq[i];
        }
    }
    ~commander() {
        for(int i=0;i<warriors.size();i++) {
            delete warriors[i];
        }
    }
    void makeWarrior(int time) {
        //when calling construction function for a warrior, pass -> int time, int commander, int type, int number, int strength, int total, int remainlife
        int i = 1;
        for(;i <= 5;i++) {
            current++;
            current%=5;
            if(lifebase >= Warrior::lifeCostBase[seq[current]]) {
                //roundIndex = time + 1
                switch(seq[current]) {
                    case DRAGON:
                        lifebase -= Warrior::lifeCostBase[DRAGON];
                        count[DRAGON]++;
                        warriors.push_back(new dragon(time, commandertype, DRAGON, time+1, Warrior::lifeCostBase[DRAGON], count[DRAGON],lifebase));
                        break;
                    case NINJA:
                        lifebase -= Warrior::lifeCostBase[NINJA];
                        count[NINJA]++;
                        warriors.push_back(new ninja(time, commandertype, NINJA, time+1, Warrior::lifeCostBase[NINJA], count[NINJA],lifebase));
                        break;
                    case ICEMAN:
                        lifebase -= Warrior::lifeCostBase[ICEMAN];
                        count[ICEMAN]++;
                        warriors.push_back(new iceman(time, commandertype, ICEMAN, time+1, Warrior::lifeCostBase[ICEMAN], count[ICEMAN],lifebase));
                        break;
                    case LION:
                        lifebase -= Warrior::lifeCostBase[LION];
                        count[LION]++;
                        warriors.push_back(new lion(time, commandertype, LION, time+1, Warrior::lifeCostBase[LION], count[LION],lifebase));
                        break;
                    case WOLF:
                        lifebase -= Warrior::lifeCostBase[WOLF];
                        count[WOLF]++;
                        warriors.push_back(new wolf(time, commandertype, WOLF, time+1, Warrior::lifeCostBase[WOLF], count[WOLF],lifebase));
                        break;
                }
                break;
            }
        }
        if(i == 6) {
            availability = false;
            cout << setw(3) << setfill('0') << time << ' ';
            cout<<strCommander[commandertype] << " headquarter stops making warriors"<<endl;
        }
    }
};
int main()
{
    int cases = 0;
    cin>>cases;
    for(int j=1;j<=cases;j++) {
        int commanderLife = 0;
        cin >> commanderLife;
        commander red(RED, commanderLife);
        commander blue(BLUE, commanderLife);
        for(int i=0;i<=4;i++) { //注意下标
            cin>>Warrior::lifeCostBase[i];
        }
        cout<<"Case:"<<j<<endl;
        int time=-1;
        while(red.availability || blue.availability) {
            time++;
            if(red.availability) red.makeWarrior(time);
            if(blue.availability) blue.makeWarrior(time);
        }
    }
    return 0;
}
