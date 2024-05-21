#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <Windows.h>
#include <string>
using namespace std;

#define ESP 27
#define START_POINT_KEY 9 // tab key
#define END_POINT_KEY 13  // ENter key
#define START_SEARCH 32   // space bar

// arrow keys to move
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

// wall keys wasd
#define WALL_UP 119    // w
#define WALL_DOWN 115  // s
#define WALL_LEFT 97   // a
#define WALL_RIGHT 100 // d

// board
#define BOX_X_START 40
#define BOX_X_END 110
#define BOX_Y_START 7
#define BOX_Y_END 32

// design
// #define VERTICAL_LINE 186
// #define HORIZONTAL_LINE 205
// #define LD_CORNER 200
// #define LT_CORNER 201
// #define RT_CORNER 187
// #define RD_CORNER 188
#define WALL 219 // █
// #define VISITED 176  // ░
#define VISITED 177 // ▒
#define CURSOR 254  // ■

// colors
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

typedef struct Coordinate
{
    int x;
    int y;
} Coordinate;
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
    NONE
};

typedef struct ValueSet
{
    Direction direction;
    float value;
} ValueSet;

void game_engine();
void draw_board();
void GotoXY(int x, int y);
void initialize();
void print_wall();
void print_visited();
void print_cursor();
void print_starting_point();
void print_ending_point();
void move_cursor(string directtion);
void make_walls(string direction);
Coordinate pixel_to_index(int pixel_x, int pixel_y);
void update_cursor(string direction);
void set_starting_point();
void set_ending_point();
void start_search();
void clear_cursor();
void algo();
bool is_found();
void trace();
void flip_all();
void show_all();
void side();
void tracing_loop(Coordinate current_index);
Coordinate get_index_by_direction(Coordinate current_index, Direction togo);
void print_path();
void explore(int i, int j);

int down = 0;

const int box_width = BOX_X_END - BOX_X_START + 1;
const int box_height = BOX_Y_END - BOX_Y_START + 1;

class Point
{
public:
    int x, y;
    bool wall = false, visited = false,
         end = false, start = false,
         explore_now = false, explore_later = false;
    float value = box_width * box_height;
    Direction direction = NONE;
    Point()
    {
        x = 0;
        y = 0;
    }
    Point(int a, int b)
    {
        x = a;
        y = b;
    }

    // Copy constructor
    Point(const Point &pt)
    {
        x = pt.x;
        y = pt.y;
        wall = pt.wall;
        visited = pt.visited;
        end = pt.end;
        start = pt.start;
        explore_now = pt.explore_now;
        explore_later = pt.explore_later;
        value = pt.value;
        direction = pt.direction;
    }

    void show()
    {
        cout << "x : " << x << ",   y : " << y << endl;
    }

    void print()
    {
        GotoXY(x, y);
        if (wall)
            print_wall();
        else if (start)
            print_starting_point();
        else if (end)
            print_ending_point();
        else if (visited)
            print_visited();
        // else { cout << "A"; }
    }
};

Point board[box_width][box_height];
Coordinate *get_neighbours(Point central_point);
ValueSet get_valuset_by_index(int i, Point pt);

int start_pt_x = 0;
int start_pt_y = 0;
int end_pt_x = 0;
int end_pt_y = 0;

int cursor_x = (BOX_X_START + BOX_X_END) / 2;
int cursor_y = (BOX_Y_START + BOX_Y_END) / 2;

int main()
{
    initialize();
    draw_board();
    game_engine();
    return 0;
}
void print_visited()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, GREEN); // 12 for red color
    char a = VISITED;
    cout << a;
    // SetConsoleTextAttribute(hConsole, 15); // Reset color to default (white)
}
void print_path()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, WHITE); // 12 for red color
    char a = WALL;
    cout << a;
    // SetConsoleTextAttribute(hConsole, 15); // Reset color to default (white)
}
void print_wall()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, LIGHTRED);
    char a = WALL;
    cout << a;
    // SetConsoleTextAttribute(hConsole, 15); // Reset color to default (white)
}
void print_cursor()
{
    GotoXY(cursor_x, cursor_y);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, CYAN);
    char a = CURSOR;
    cout << a;
}
void print_starting_point()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, YELLOW);
    char a = CURSOR;
    cout << a;
}
void print_ending_point()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, MAGENTA);
    char a = CURSOR;
    cout << a;
}
void clear_cursor()
{
    GotoXY(cursor_x, cursor_y);
    printf(" ");
}
ValueSet get_valuset_by_index(int i, Point pt)
{
    ValueSet set;
    if (i == 0)
        set.direction = DOWN_RIGHT;
    else if (i == 1)
        set.direction = RIGHT;
    else if (i == 2)
        set.direction = UP_RIGHT;
    else if (i == 3)
        set.direction = DOWN;
    else if (i == 4)
        set.direction = UP;
    else if (i == 5)
        set.direction = DOWN_LEFT;
    else if (i == 6)
        set.direction = LEFT;
    else if (i == 7)
        set.direction = UP_LEFT;

    // side();
    // cout<<"cental Value : "<<pt.value;

    if (i == 1 || i == 3 || i == 4 || i == 6)
        set.value = pt.value + 1;
    else
        set.value = pt.value + 1.5;
    // cout<<",  set value : "<<set.value;

    return set;
}

void side()
{
    GotoXY(BOX_X_END + 1, down);
    down++;
}

void initialize()
{
    for (int i = 0; i < box_width; i++)
    {
        for (int j = 0; j < box_height; j++)
        {
            int pt_x = i + BOX_X_START;
            int pt_y = j + BOX_Y_START;
            Point pt(pt_x, pt_y);
            if (i == 0 || i == box_width - 1 || j == 0 || j == box_height - 1)
                pt.wall = true;
            // if(i==j && i!=0) pt.visited = true;
            // if(i==55 && j==9) pt.start = true;
            // if(i==60 && j==10) pt.end = true;
            board[i][j] = pt;
        }
    }
}

void GotoXY(int x, int y)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursorPosition;
    cursorPosition.X = x;
    cursorPosition.Y = y;
    SetConsoleCursorPosition(consoleHandle, cursorPosition);
}

// clear the screen and make borders
void draw_board()
{
    system("cls");

    for (int i = 0; i < box_width; i++)
    {
        for (int j = 0; j < box_height; j++)
        {
            board[i][j].print();
        }
    }
    print_cursor();
}

void move_cursor(string direction)
{
    bool clear_background = true;

    // if curson is over starting point
    if (cursor_x == start_pt_x && cursor_y == start_pt_y)
    {
        clear_background = false;
        GotoXY(cursor_x, cursor_y);
        print_starting_point();
    }

    // if cursor is over ending point
    if (cursor_x == end_pt_x && cursor_y == end_pt_y)
    {
        clear_background = false;
        GotoXY(cursor_x, cursor_y);
        print_ending_point();
    }

    // if curson is over any wall
    Coordinate index = pixel_to_index(cursor_x, cursor_y);
    Point *point = &(board[index.x][index.y]);
    if (point->wall)
    {
        clear_background = false;
        GotoXY(cursor_x, cursor_y);
        print_wall();
    }
    if (clear_background)
        clear_cursor();
    update_cursor(direction);
}

void update_cursor(string direction)
{
    if (direction == "up" && cursor_y > BOX_Y_START + 1)
        cursor_y--;
    if (direction == "down" && cursor_y < BOX_Y_END - 1)
        cursor_y++;
    if (direction == "left" && cursor_x > BOX_X_START + 1)
        cursor_x--;
    if (direction == "right" && cursor_x < BOX_X_END - 1)
        cursor_x++;
    print_cursor();
}

void make_walls(string direction)
{
    Coordinate index = pixel_to_index(cursor_x, cursor_y);
    board[index.x][index.y].wall = true;
    GotoXY(cursor_x, cursor_y);
    print_wall();
    update_cursor(direction);
}

Coordinate pixel_to_index(int pixel_x, int pixel_y)
{
    Coordinate index;
    index.x = pixel_x - BOX_X_START;
    index.y = pixel_y - BOX_Y_START;
    return index;
}

void set_starting_point()
{
    if (start_pt_x > 0)
    { // setting by erasing
        GotoXY(start_pt_x, start_pt_y);
        printf(" ");
    }
    start_pt_x = cursor_x;
    start_pt_y = cursor_y;
    GotoXY(start_pt_x, start_pt_y);
    print_starting_point();
}
void set_ending_point()
{
    if (end_pt_x > 0)
    { // setting by erasing
        GotoXY(end_pt_x, end_pt_y);
        printf(" ");
    }
    end_pt_x = cursor_x;
    end_pt_y = cursor_y;
    GotoXY(end_pt_x, end_pt_y);
    print_ending_point();
}

void game_engine()
{
    while (true)
    {
        if (kbhit())
        {
            char key = getch();
            if (key == ESP)
                exit(1);
            else if (key == UP_ARROW)
                move_cursor("up");
            else if (key == DOWN_ARROW)
                move_cursor("down");
            else if (key == LEFT_ARROW)
                move_cursor("left");
            else if (key == RIGHT_ARROW)
                move_cursor("right");
            else if (key == WALL_UP)
                make_walls("up");
            else if (key == WALL_DOWN)
                make_walls("down");
            else if (key == WALL_LEFT)
                make_walls("left");
            else if (key == WALL_RIGHT)
                make_walls("right");
            else if (key == START_POINT_KEY)
                set_starting_point();
            else if (key == END_POINT_KEY)
                set_ending_point();
            else if (key == START_SEARCH)
            {
                if (start_pt_x > 0 && end_pt_x > 0)
                {
                    break;
                    // start_search();
                }
            }
            GotoXY(0, 0);
        }
        Sleep(5);
    }
    start_search();
}

void start_search()
{
    // set start end blocks.. visited.. explaore
    // and start algo

    // clear cursor
    clear_cursor();

    GotoXY(0, 0);

    Coordinate end_index = pixel_to_index(end_pt_x, end_pt_y);
    board[end_index.x][end_index.y].end = true;

    Coordinate start_index = pixel_to_index(start_pt_x, start_pt_y);
    board[start_index.x][start_index.y].start = true;
    board[start_index.x][start_index.y].value = 0;
    board[start_index.x][start_index.y].direction = NONE;
    board[start_index.x][start_index.y].explore_now = true;

    algo();
}
void algo()
{
    // GotoXY(0, 0);
    for (int i = 0; i < box_width; i++)
    {
        for (int j = 0; j < box_height; j++)
        {
            if (board[i][j].explore_now)
            {
                // cout << "Explore Now => i : " << i << ", j : " << j << endl;
                explore(i, j);
            }
        }
    }
    bool end_reached = is_found();
    if (end_reached)
        trace();
    else
        flip_all();
}

Coordinate *get_neighbours(Point central_point)
{
    Coordinate *neigbhours_indexs_array = new Coordinate[8];

    Coordinate centeral_index = pixel_to_index(central_point.x, central_point.y);

    int left_top_x = centeral_index.x - 1;
    int left_top_y = centeral_index.y - 1;
    int index = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (!(i == 1 && j == 1))
            {
                neigbhours_indexs_array[index].x = left_top_x + i;
                neigbhours_indexs_array[index].y = left_top_y + j;
                index++;
            }
        }
    }
    return neigbhours_indexs_array;
}

void explore(int i, int j)
{
    Coordinate *neighbours_indexes = get_neighbours(board[i][j]);
    for (int iteration = 0; iteration < 8; iteration++)
    {
        Coordinate current_neighbour_index = neighbours_indexes[iteration];
        Point *current_neighbour = &board[current_neighbour_index.x][current_neighbour_index.y];
        // cout << "current_neighbour : " << current_neighbour->x << ", " << current_neighbour->y << " =>  ";
        // cout << "wall : " << current_neighbour->wall << ", visited : " << current_neighbour->visited << ", if(...) : " << (!(current_neighbour->wall || current_neighbour->visited));

        if (!(current_neighbour->wall || current_neighbour->visited))
        {
            // normal pixel
            current_neighbour->explore_later = true;
            ValueSet set = get_valuset_by_index(iteration, board[i][j]);
            // side();
            // cout <<"["<<current_neighbour_index.x<<", "<<current_neighbour_index.y<<"]"<< "  set.value : " << set.value << ",  original : " << current_neighbour->value << endl;
            if (current_neighbour->value > set.value)
            {
                current_neighbour->value = set.value;
                current_neighbour->direction = set.direction;
                // cout << " value : " << set.value << ",  direction : " << set.direction;
            }
        }
        // cout << endl;
    }
}

void flip_all()
{
    // cout << "FLIP __ ALL !!!" << endl;
    for (int i = 0; i < box_width; i++)
    {
        for (int j = 0; j < box_height; j++)
        {
            if (board[i][j].explore_later)
            {
                board[i][j].explore_now = true;
                board[i][j].explore_later = false;
                GotoXY(board[i][j].x, board[i][j].y);
                print_visited();
            }
            else if (board[i][j].explore_now)
            {
                // GotoXY(board[i][j].x,board[i][j].y);
                // print_visited();
                board[i][j].explore_now = false;
                board[i][j].visited = true;
                board[i][j].print();
            }
        }
    }
    Sleep(5);
    algo();
}

void show_all()
{
    GotoXY(0, BOX_Y_END + 2);
    for (int i = 0; i < box_width; i++)
    {
        for (int j = 0; j < box_height; j++)
        {
            cout << "[" << i << ", " << j << "] - value : " << board[i][j].value << "\t";
        }
        cout << "\n\n------------------------------------------------------------------------------------------\n\n";
    }

    char a = getch();
    side();
    cout << "\n\n";
    algo();
}

bool is_found()
{
    Coordinate index = pixel_to_index(end_pt_x, end_pt_y);
    if (board[index.x][index.y].explore_later)
        return true;
    else
        return false;
}

void tracing_loop(Coordinate current_index)
{
    Point *current_point = &board[current_index.x][current_index.y];
    if (current_point->direction == NONE)
    {
        // break the recursion... we reached start
        return;
    }
    else
    {
        // color the path
        GotoXY(current_point->x, current_point->y);
        print_path();

        // tracing algo
        Coordinate next_index = get_index_by_direction(current_index, board[current_index.x][current_index.y].direction);
        tracing_loop(next_index);
        return;
    }
}

Coordinate get_index_by_direction(Coordinate current_index, Direction togo)
{
    int x = current_index.x;
    int y = current_index.y;

    if (togo == RIGHT)
        x++;
    else if (togo == LEFT)
        x--;
    else if (togo == UP)
        y--;
    else if (togo == DOWN)
        y++;
    else if (togo == UP_LEFT)
    {
        y--;
        x--;
    }
    else if (togo == UP_RIGHT)
    {
        y--;
        x++;
    }
    else if (togo == DOWN_LEFT)
    {
        y++;
        x--;
    }
    else if (togo == DOWN_RIGHT)
    {
        y++;
        x++;
    }
    else
    {
        cout << "IMMPOSSIBLE !";
        exit(1);
    }

    Coordinate final;
    final.x = x;
    final.y = y;
    return final;
}

void trace()
{
    Coordinate end_index = pixel_to_index(end_pt_x, end_pt_y);
    Direction direction = board[end_index.x][end_index.y].direction;
    Coordinate next_index = get_index_by_direction(end_index,direction);
    tracing_loop(next_index);
    exit(0);
}