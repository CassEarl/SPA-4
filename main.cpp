//
// Created by earleywinec on 3/28/2025.
//
#include <iostream>
#include <vector>
#include <cctype>
#include <set>
using namespace std;

const int MAP_ROWS = 5;
const int MAP_COLS = 5;
const int NUM_PUB_SAFETY = 2;
const int NUM_RA = 2;
const int NUM_FRIEND = 2;
const int NUM_TASERS = 2;
const int NUM_TREASURE = 2;

class Room;
class Player;

class Hazard {
public:
    virtual string clue() = 0;
    virtual bool trigger(vector<vector<Room>>& map, int row, int col, Player& player) = 0;
    virtual char symbol() const = 0;
    virtual ~Hazard() = default;
};

class Room {
public:
    Hazard* hazard = nullptr;
    class Weapon* weapon = nullptr;
    bool hasTreasure = false;
    bool hasPlayer = false;
};
class Player {
public:
    int row = 0;
    int col = 0;
    int tase = 0;
};
                   
class pub_safety_officer : public Hazard {
    string clue() override {
        return "You hear keys jangling";
    }
    bool trigger(vector<vector<Room>> &map, int row, int col, Player&) override {
        cout << "You have been caught by public safety!\n";
        return true;
    }

    char symbol() const override {
        return '#';
    }
};

class RA : public Hazard {
    string clue() override {
        return "You see an RA in the area, but they don't see you...yet";
    }
    bool trigger(vector<vector<Room>>& map, int row, int col, Player&) override {
        cout << "You have been caught by an RA\n";
        return true;
    }
    char symbol() const override {
        return '@';
    }
};

class best_friend : public Hazard {
    string clue() override {
        return "You hear a laugh nearby";
    }

   bool trigger(vector<vector<Room>>& map, int row, int col, Player& player) override {
        cout << "A friend spots you! They've helped you move to a new safe area!\n" << endl;
        map[row][col].hazard = nullptr;
        map[row][col].hasPlayer = false;

        int newR, newC;
        do {
            newR = rand() % MAP_ROWS;
            newC = rand() % MAP_COLS;
        } while(map[newR][newC].hazard || map[newR][newC].hasPlayer || map[newR][newC].weapon || map[newR][newC].hasTreasure);

        player.row = newR;
        player.col = newC;
        map[newR][newC].hasPlayer = true;
        return false;
    }

    char symbol() const override {
        return '!';
    }
};

class Weapon {
public:
    virtual void use() = 0;
    virtual char symbol() const = 0;
    virtual ~Weapon() = default;
};

class Taser : public Weapon {
public:
    void use() override {
        cout << "You tased someone!\n" << endl;
    }
    char symbol() const override {
        return '>';
    }
};

pair<int, int> getRandomEmptyCell(const vector<vector<Room>>& map) {
    int r, c;
    do {
        r = rand() % MAP_ROWS;
        c = rand() % MAP_COLS;
    } while(map[r][c].hazard || map[r][c].weapon || map[r][c].hasTreasure || map[r][c].hasPlayer);
    return {r, c};
}

vector<vector<Room>> createMap(Player& player) {
    vector<vector<Room>> map(MAP_ROWS, vector<Room>(MAP_COLS));

    player.row = 0;
    player.col = 0;
    map[0][0].hasPlayer = true;

    for(int i = 0; i < NUM_PUB_SAFETY; ++i) {
        map[getRandomEmptyCell(map).first][getRandomEmptyCell(map).second].hazard = new pub_safety_officer();
    }

    for(int i = 0; i < NUM_RA; ++i) {
        map[getRandomEmptyCell(map).first][getRandomEmptyCell(map).second].hazard = new RA();
    }

    for(int i = 0; i < NUM_FRIEND; ++i) {
        map[getRandomEmptyCell(map).first][getRandomEmptyCell(map).second].hazard = new best_friend();    Weapon* weapon = nullptr;
    }

    for(int i = 0; i < NUM_TASERS; ++i) {
        map[getRandomEmptyCell(map).first][getRandomEmptyCell(map).second].weapon = new Taser();
    }

    for(int i = 0; i < NUM_TREASURE; ++i) {
        map[getRandomEmptyCell(map).first][getRandomEmptyCell(map).second].hasTreasure = true;
    }
    return map;
}

void printHelp() {
    cout << "Symbols:\n";
    cout << "\t-Player(you): '+'\n";
    cout << "\t-Taser: '>'\n";
    cout << "\t-Hazards: RA (@), Nosey Student (!), Pub Safety Officer (#)" << endl;
    cout << "\t-Weapons: Taser (>)" << endl;
    cout << "\t-Treasure: Extra Tasers(?)" << endl;

    cout << "Controls:\n ";
    cout << "\t-N/S/E/W: for moving\n";
    cout << "\t-T + direction(n/s/e/w): Tase in those directions\n";
    cout << "\t-M: View the map\n";
    cout << "\t-H: Help\n";
    cout << "\t-Q: Quit\n";

}

void printMap(const vector<vector<Room>>& map) {
    for(int i = 0; i < MAP_ROWS; i++) {
        for(int j = 0; j < MAP_COLS; j++) {
            if(map[i][j].hasPlayer) {
                cout << "+ ";
            } else if(map[i][j].hazard) {
                cout << map[i][j].hazard->symbol() << " ";
            } else if(map[i][j].weapon) {
                cout << map[i][j].weapon->symbol() << " ";
            } else if(map[i][j].hasTreasure) {
                cout << "? ";
            } else {
                cout << ". ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void checkForHazards(Player& player, const vector<vector<Room>>& map) {
    set<string> cluesHeard;
    const int dRow[] = {-1,1,0,0};
    const int dCol[] = {0,0,-1,1};

    for(int i = 0; i < 4; i++) {
        int newRow = player.row + dRow[i];
        int newCol = player.col + dCol[i];

        if(newRow >= 0 && newRow < MAP_ROWS && newCol >= 0 && newCol < MAP_COLS) {
            if(map[newRow][newCol].hazard) {
                string clue = map[newRow][newCol].hazard->clue();
                if(cluesHeard.find(clue) == cluesHeard.end()) {
                    cout << clue << endl;
                    cluesHeard.insert(clue);
                }
            }
        }
    }
    if(!cluesHeard.empty()) {
        cout << endl;
    }
}

bool movePlayer(char direction, Player& player, vector<vector<Room>>& map) {
    int r = player.row, c = player.col;
    if(direction == 'n') --r;
    else if(direction == 's') ++r;
    else if(direction == 'e') ++c;
    else if(direction == 'w') --c;
    else return false;

    if(r < 0 || r >= MAP_ROWS || c < 0 || c >= MAP_COLS) {
        cout << "You can't move this way :(\n";
        return false;
    }

    map[player.row][player.col].hasPlayer = false;
    player.row = r;
    player.col = c;
    map[r][c].hasPlayer = true;

    Room& room = map[r][c];
    if(room.hazard && room.hazard->trigger(map, r, c, player)) return true;
    if(room.weapon) {
        cout << "You picked up a taser!\n";
        player.tase++;
        delete room.weapon;
        room.weapon = nullptr;
    }

    if(room.hasTreasure) {
        cout << "You picked up another taser!\n";
        player.tase++;
        room.hasTreasure = false;
    }
    checkForHazards(player, map);
    return false;
}

void useTaser(Player& player, vector<vector<Room>>& map, char direction) {
    if(player.tase <= 0) {
        cout << "You don't have any tasers left!\n";
        return;
    }

    int targetRow = player.row;
    int targetCol = player.col;
    switch (direction) {
        case 'n': targetRow--; break;
        case 's': targetRow++; break;
        case 'e': targetCol++; break;
        case 'w': targetCol--; break;
        default:
            cout << "Invalid direction for tasing!\n";
        return;
    }

    if(targetRow >= 0 && targetRow < MAP_ROWS && targetCol >= 0 && targetCol < MAP_COLS) {
        Room& room = map[targetRow][targetCol];
        if(room.hazard) {
            if(room.hazard->symbol() == '@') {
                cout << "You tased an RA!\n";
                delete room.hazard;
            } else if(room.hazard->symbol() == '#') {
                cout << "You tased a Public Safety Officer!\n";
                delete room.hazard;
            } else if(room.hazard->symbol() == '!') {
                cout << "You tased your friend, that's not cool...\n";
                delete room.hazard;
            }
            room.hazard = nullptr;
            player.tase--;
        } else {
            cout << "No hazard in that direction\n";
            player.tase--;
        }
    } else {
        cout << "Taser fired into the void\n";
        player.tase--;
    }
}

int main() {
    cout << "Welcome to 'Escape Public Safety!'" << endl;
    cout << "Goal: Avoid the public safety officers and RAs....and don't get caught!" << endl;
    cout << "How to avoid? Tase them with a taser and RUN\n" << endl;

    srand(static_cast<unsigned>(time(nullptr)));
    bool playing = true;

    while(playing) {
        Player player;
        vector<vector<Room>> map = createMap(player);
        char action;
        bool caught = false;

        do {
            cout << "Tasers: " << player.tase << endl;
            cout << "Action: N)orth, S)outh, E)ast, W)est,\n T)aser, H)elp, Q)uit, M)ap: ";
            cin >> action;
            action = tolower(action);

            if(action == 't') {
                char direction;
                cout << "Choose a direction to tase (n/s/e/w): ";
                cin >> direction;
                direction = tolower(direction);
                useTaser(player, map, direction);
                continue;
            }

            switch(action) {
                case 'h': printHelp(); break;
                case 'm': printMap(map); break;
                case 'n':
                case 's':
                case 'e':
                case 'w':
                    caught = movePlayer(action, player, map);
                    break;
                case 'q':
                    cout << "Thanks for playing, bye now!\n";
                    playing = false;
                    break;
                default:
                    cout << "That action is not an option.\n";
                    break;
            }

            if(caught) {
                char again;
                cout << "You got caught! Would you like to play again? (y/n): ";
                cin >> again;
                if(tolower(again) != 'y') {
                    playing = false;
                    cout << "Thanks for playing, bye now!\n";
                }
                break;
            }

            if(map.empty()) {
                cout << "You got everything! Good job and you win!" << endl;
                char again;
                cout << "Would you like to play again? (y/n): ";
                cin >> again;
                if(tolower(again) != 'y') {
                    playing = true;
                } else {
                    playing = false;
                    cout << "Thanks for playing, bye now!\n" << endl;
                }
            }
        } while(!caught && playing);
    }
    return 0;
}