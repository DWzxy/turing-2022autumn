#include <iostream>
#include <cstdio>
#include <vector>
#include <cstring>
#include <cstdlib>
using namespace std;

struct State;
#define N 501

struct Edge
{
    vector<char> old_letter; // 现在指向的字符

    vector<char> new_letter; // 将要写下的字符
    vector<char> new_step;
    State *new_state;
    Edge *next = NULL;
};

struct State
{
    char name[50];
    Edge *edge_head = NULL; // 链式前向星
    bool final = false;     // 标记是否为终结状态
};

struct Machine
{
    State *state;
    int point[N];          // 指针
    vector<char> tapes[N]; // 纸带
};

void move_right(int x);
void move_left(int x);
int tran_num(int x);
int retran_num(int x);
Edge* find_edge();
void print_tape(int x);

State *find_state(char *name);
State *new_state();
Edge *new_edge(char *from);

char read_state_name(char *name);
void clear_note();
void skip_char(char c);
void error();
void check_in_letters();

void read();
void read_single();
void run(char *x);