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

class Hazard {
public:
    virtual string clue() = 0;
    virtual void trigger()= 0;
    virtual char symbol() const = 0;
    virtual ~Hazard() = default;
};

class pub_safety_officer : public Hazard {
    string clue() override {
        return "You hear keys jangling";
    }
    void trigger() override {
        cout << "You have been caught!\n";
    }

    char symbol() const override {
        return '#';
    }
};

class RA : public Hazard {
    string clue() override {
        return "You see an RA in the area, but they don't see you...yet";
    }
    void trigger() override {
        cout << "The RA has found you!\n";
    }
    char symbol() const override {
        return '@';
    }
};

class nosey_student : public Hazard {
    string clue() override {
        return "You hear a sniffing noise";
    }

    void trigger() override {
        cout << "The nosey student has caught you!\n";
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
        cout << "You tased someone!\n";
    }
    char symbol() const override {
        return '>';
    }
};

class Room {
public:
    Hazard* hazard = nullptr;
    Weapon* weapon = nullptr;
    bool hasTreasure = false;
    bool hasPlayer = false;
};

class Player {
public:
    int row = 0;
    int col = 0;
    int tase = 0;
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

    auto[r1, c1] = getRandomEmptyCell(map);
    map[r1][c1].hazard = new pub_safety_officer();

    auto[r2, c2] = getRandomEmptyCell(map);
    map[r2][c2].hazard = new RA();

    auto[r3, c3] = getRandomEmptyCell(map);
    map[r3][c3].hazard = new nosey_student();

    auto [rw, cw] = getRandomEmptyCell(map);
    map[rw][cw].weapon = new Taser();

    auto[rt, ct] = getRandomEmptyCell(map);
    map[rt][ct].hasTreasure = true;
    return map;
}

void printHelp() {
    cout << "Welcome to 'Escape Public Safety!'" << endl;
    cout << "Goal: Avoid the public safety officers and don't get caught!" << endl;
    cout << "How to avoid? Tase them with a taser" << endl;
    cout << "Hazards: RA (@), Nosey Student (!), Pub Safety Officer (#)" << endl;
    cout << "Weapons: Taser (>)" << endl;
    cout << "Treasure: Extra (?)" << endl;
    cout << "Controls: N/S/E/W to move, f to blind, m for the map, h for help, q to quit\n" << endl;
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
    int newRow = player.row;
    int newCol = player.col;

    switch(direction) {
        case 'n': newRow--; break;
        case 's': newRow++; break;
        case 'e': newCol++; break;
        case 'w': newCol--; break;
        default: return false;
    }

    if(newRow >= 0 && newRow < MAP_ROWS && newCol >= 0 && newCol < MAP_COLS) {
        map[player.row][player.col].hasPlayer = false;
        player.row = newRow;
        player.col = newCol;
        map[player.row][player.col].hasPlayer = true;

        Room& currRoom = map[player.row][player.col];
        if(currRoom.hazard) {
            currRoom.hazard->trigger();
            return true;
        }
        if(currRoom.weapon) {
            cout << "You found a taser!\n";
            player.tase++;
            delete currRoom.weapon;
            currRoom.weapon = nullptr;
        }
        checkForHazards(player, map);
    } else {
        cout << "You can't move that way\n";
    }
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
            cout << "You tased someone the " << room.hazard->symbol() << "!\n";
            delete room.hazard;
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
    srand(static_cast<unsigned>(time(nullptr)));
    bool playing = true;

    while(playing) {
        Player player;
        vector<vector<Room>> map = createMap(player);
        char action;
        bool caught = false;

        do {
            cout << "Action: N)orth, S)outh, E)ast, W)est, T)aser, H)elp, Q)uit, M)ap: ";
            cin >> action;
            action = tolower(action);

            if(action == 'f') {
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
        } while(!caught && playing);
    }
    return 0;
}