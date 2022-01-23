#include <array>
#include <random>
#include <sstream>
#include "windows.h"
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


class Player{
private:
    //2 - marked, 1-uncovered, 0-covered
    unsigned __int8 choice_board[30][30]{0};
    __int16 flags_left = 15;
    __int16 user_row{};//user selected row
    __int16 user_col{};//user ...
    char user_letter{};
public:
    void add_one_flag(){flags_left+=1;};
    void remove_one_flag(){flags_left-=1;};
    bool get_choice_board_stat(int& user_r, int& user_c, int s){
        if(choice_board[user_r][user_c] == s)
            return true;
        else
            return false;
    };
    int get_flags_left() const{return this->flags_left;};
    int get_user_row() const{return this->user_row;};
    int get_user_col() const{return this->user_col;};
    char get_user_letter() const{return this->user_letter;};
    void set_choice_board(int& user_r, int& user_c, __int16 a){ choice_board[user_r][user_c]=a;};

    //to loop
    void get_user_input();
    bool validate_input();
    Player() = default;
    ~Player() = default;
};

void Player::get_user_input() {
    std::string input;
    bool error = false;
    do {
        do {
            if (error)
                std::cout << "Bledne dane! Wprowadz poprawne dane!\n";
            std::cout << "Pozostalo " << flags_left << " flag do wykorzystania\n";
            std::cout << "Wprowadz numer wiersza, numer kolumny i literke(F/Z/P):\n";
            std::getline(std::cin, input);
            if (input[input.size() - 2] == ' ')
                user_letter = input[input.size() - 1];
            std::stringstream u_r(input);
            u_r >> user_row;
            if (user_row < 10)
                input.erase(0, 2);
            else
                input.erase(0, 3);
            std::stringstream u_c(input);
            u_c >> user_col;
            error = true;

        } while (user_letter != 'F' && user_letter != 'Z' && user_letter != 'P' || user_row > 30 || user_row < 1 || user_col > 30 || user_col < 1);
        user_row -= 1;
        user_col -= 1;
    }while(validate_input());
}

bool Player::validate_input() {
    if(user_letter == 'Z')
    {
        if(flags_left >= 1)
        {
            if(choice_board[user_row][user_col] == 0)
                return false;
            else
                return true;
        }
        else
            return true;
    }
    else
    {
        if(user_letter == 'P')
        {
            if(choice_board[user_row][user_col] == 0)
                return false;
            else
                return true;
        }
        else{
            if(choice_board[user_row][user_col] == 2)
                return false;
        }
    }
    return true;
}

class Board : public Player{
    signed __int16 board[30][30]{0};//wartosci wokol min i miny
    std::array<__int32, 15> mines_row{};
    std::array<__int32, 15> mines_col{};
    int game_status = 0;//"-1"lost "0"running "1"win
public:
    //constructor
    void create_mines(std::array<int, 15>& mines_r, std::array<int, 15>& mines_c);
    void count_mines_around_cell();
    void place_mines();

    //other
    void set_game_status(int status);//running0, lost-1, win1
    static bool check_for_same(std::array<int, 15>& mine_x, std::array<int, 15>& mine_y, int& i);
    int get_game_status() const{return game_status;};

    //loop
    void do_user_input();
    void print_game();
    void print_board();
    static void print_win();
    void run_minesweeper();//main game loop
    Board(){
        create_mines(mines_row, mines_col);
        count_mines_around_cell();
        place_mines();
    }
    ~Board() = default;
};

bool Board::check_for_same(std::array<int, 15>& mine_x, std::array<int, 15>& mine_y, int& i) {
    for(int j=0;j<i;j++)
    {
        if(mine_x[i] == mine_x[j] && mine_y[i] == mine_y[j])
            return true;
    }
    return false;
}

void Board::create_mines(std::array<int, 15> &mines_r, std::array<int, 15> &mines_c) {
    std::random_device rd;
    std::uniform_int_distribution<int> d30 (0, 29);
    for(int i=0;i<mines_r.size();i++)
    {
        do{
            mines_r[i] = d30(rd);
            mines_c[i] = d30(rd);
        }while(check_for_same(mines_row, mines_col, i));
    }
}

void Board::count_mines_around_cell() {
    int m_r;
    int m_c;
    for(int i=0;i<mines_row.size();i++)
    {
        for(int r = -1;r<2;r++)
        {
            m_r = mines_row[i]+r;
            if(m_r > -1 && m_r < 30)
                for(int c = -1;c<2;c++)
                {
                    m_c = mines_col[i] + c;
                    if(m_c > -1 && m_c < 30)
                        board[m_r][m_c] += 1;
                }

        }
    }

}

void Board::place_mines() {
    for(int i=0;i<mines_row.size();i++)
        board[mines_row[i]][mines_col[i]] = -1;
}

void Board::set_game_status(int status) {
    if(status == -1)
        game_status = -1;
    else
    if(status == 1)
        game_status = 1;
    else
        game_status = 0;
}

void Board::do_user_input() {
    auto u_row = get_user_row();
    auto u_col = get_user_col();
    auto u_let = get_user_letter();
    __int16 mines_flagged=1;
    switch (u_let) {
        case 'P':
            set_choice_board(u_row, u_col, 1);
            if(board[u_row][u_col] == -1)
                set_game_status(-1);
            break;
        case 'Z':
            if(get_flags_left()==1)
            {
                for(int i=0;i<mines_row.size();i++)
                {
                    if(get_choice_board_stat(mines_row[i], mines_col[i], 2))
                        mines_flagged += 1;
                }
                std::cout << mines_flagged << "\n";
                if(mines_flagged == 15)
                {
                    set_game_status(1);
                    std::cout << "\nWygrales\n";
                }
            }
            remove_one_flag();
            set_choice_board(u_row, u_col, 2);
            break;
        case 'F':
            add_one_flag();
            set_choice_board(u_row, u_col, 0);
            break;
        default:
            std::cout << "User input error\n";
            system("pause");
            exit(-10);
    }
}

void Board::print_game() {
    switch (get_game_status()) {
        case 0:
            print_board();
            std::cout << "\n";
            break;
        case 1:
            print_win();
            break;
        case -1:
            print_board();
            SetConsoleTextAttribute(hConsole, 12);
            std::cout << "Przegrales!";
            SetConsoleTextAttribute(hConsole, 15);
            system("pause");
            break;
    }
}

void Board::print_board() {
    system("cls");
    //first line
    std::cout << char(218);
    for(int i = 1;i < 31;i++)
    {
        std::cout << char(196) << char(196);
        std::cout << char(194);
    }
    std::cout << char(196) << char(196) << char(191) << "\n";
    //second line(column numbers)
    std::cout << char(179) << "  ";
    for(int i=1;i<31;i++)
    {
        if(i<10)
            std::cout << char(179) << ' ' << i;
        else
            std::cout << char(179) << i;
    }
    std::cout << char(179) << "\n";
    //body
    for(int r=0;r<30;r++)
    {
        std::cout << char(195);
        for(int i=0;i<30;i++)
            std::cout << char(196) << char(196) << char(197);
        std::cout << char(196) << char(196) << char(180);
        std::cout << "\n";
        //row numbers
        if(r+1<10)
            std::cout << char(179) << char(32) << r+1;
        else
            std::cout << char(179) << r+1;
        if(get_game_status()==0)//running
        {
        for(int c=0;c<30;c++) {//printing body chars(mines/numbers etc.)
            std::cout << char(179);
            if (get_choice_board_stat(r, c, 0))
                std::cout << "  ";
            else {
                if (get_choice_board_stat(r, c, 1)) {
                    if (board[r][c] == 0) {
                        if (r == get_user_row() && c == get_user_col()) {
                            SetConsoleTextAttribute(hConsole, 10);//green
                            std::cout << " .";
                            SetConsoleTextAttribute(hConsole, 15);//white
                        } else {
                            std::cout << " .";
                        }
                    } else {
                        if (r == get_user_row() && c == get_user_col()) {
                            SetConsoleTextAttribute(hConsole, 10);
                            std::cout << ' ' << board[r][c];
                            SetConsoleTextAttribute(hConsole, 15);
                        } else {
                            if (board[r][c] == 1) {
                                SetConsoleTextAttribute(hConsole, 6);//dark yellow
                                std::cout << ' ' << board[r][c];
                                SetConsoleTextAttribute(hConsole, 15);
                            } else {
                                SetConsoleTextAttribute(hConsole, 4);//dark red
                                std::cout << ' ' << board[r][c];
                                SetConsoleTextAttribute(hConsole, 15);
                            }
                        }
                    }
                }
                if (get_choice_board_stat(r, c, 2)) {
                    if (r == get_user_row() && c == get_user_col()) {
                        SetConsoleTextAttribute(hConsole, 10);
                        std::cout << " X";
                        SetConsoleTextAttribute(hConsole, 15);
                    } else {
                        SetConsoleTextAttribute(hConsole, 14);//red
                        std::cout << " X";
                        SetConsoleTextAttribute(hConsole, 15);
                    }
                }
            }
        }
        }
        if(get_game_status()==-1)
        {
            for(int c=0;c<30;c++)
            {
                std::cout << char(179);
                if(board[r][c] == -1)
                {
                    if(get_choice_board_stat(r, c, 1) || get_choice_board_stat(r, c, 0))
                    {
                        SetConsoleTextAttribute(hConsole, 4);
                        std::cout << ' ' << '*';
                        SetConsoleTextAttribute(hConsole, 15);
                    }
                    else{
                        SetConsoleTextAttribute(hConsole, 10);
                        std::cout << ' ' << "+";
                        SetConsoleTextAttribute(hConsole, 15);
                    }
                }
                else{
                    std::cout << "  ";
                }
            }
        }
        std::cout << char(179);
        std::cout << "\n";
    }
    //last line
    std::cout << char(192);
    for(int i=0;i<30;i++)
        std::cout << char(196) << char(196) << char(193);
    std::cout << char(196) << char(196) << char(217) << "\n";
}

void Board::print_win() {
    system("cls");
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "Wygrales!";
    system("pause");
}

void Board::run_minesweeper() {
    do{
        get_user_input();
        do_user_input();
        print_game();
    }while(get_game_status() == 0);
}
