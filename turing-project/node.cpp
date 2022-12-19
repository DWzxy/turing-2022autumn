#include "node.h"
using namespace std;
#define DEBUG

extern FILE *p;
extern bool debug;

vector<State *> state_set; // 状态集
bool in_letters[256];      // 输入字符集，bool表示是否在其中
bool all_letters[256];     // 字符集
State *start;              // 初始状态
char space_letter;         // 空格符号
vector<State *> final_set; // 终结状态集
int tape_num;              // 纸带数

Machine *machine;

// 映射，正数为双，负数为单
// 0 -> 0
//-1 -2 -3 -> 1 3 5
// 1 2 3 -> 2 4 6
void move_right(int i)
{
    int x = machine->point[i];
    x += 1;
    if (x > machine->right[i])
    {
        machine->tapes[i].push_back(space_letter);
        machine->tapes[i].push_back(space_letter);
        machine->right[i] = x;
    }
    machine->point[i] = x;
}
void move_left(int i)
{
    int x = machine->point[i];
    x -= 1;
    if (x < machine->left[i])
    {
        machine->tapes[i].push_back(space_letter);
        machine->tapes[i].push_back(space_letter);
        machine->left[i] = x;
    }
    machine->point[i] = x;
}

int tran_num(int x)
{
    // 把下标转为正数
    if (x >= 0)
        return x << 1;
    else
        return ((-x) << 1) - 1;
}

int retran_num(int x)
{
    // 把正数转化为实际的下标
    if (x % 2 == 0)
        return x >> 1;
    else
        return -((x + 1) >> 1);
}

void print_tape(int x)
{
    int left = machine->left[x], right = machine->right[x];
    int tmp = 0;

    printf("Index%-2d: ", x);
    for (int i = left; i <= right; i++)
        cout << abs(i) << " ";

    cout << endl;

    printf("Tape%-3d: ", x);
    for (int i = left; i <= right; i++)
    {
        cout << machine->tapes[x][tran_num(i)];
        tmp = 0;
        if (i < -9 || i > 9)
            cout << "  ";
        else
            cout << " ";
    }
    cout << endl;

    printf("Head%-3d: ", x);
    for (int i = left; i < machine->point[x]; i++)
    {
        tmp = 0;
        if (i < -9 || i > 9)
            cout << "   ";
        else
            cout << "  ";
    }
    cout << '^';
    cout << endl;
}

State *find_state(char *name)
{
    for (int i = 0; i < state_set.size(); i++)
    {
        if (strcmp(name, state_set[i]->name) == 0)
            return state_set[i];
    }
    return NULL;
}

State *new_state()
{
    State *now = new State();
    state_set.push_back(now);
    return now;
}

Edge *new_edge(char *from)
{
    Edge *now = new Edge();
    State *k = find_state(from);
    now->next = k->edge_head;
    k->edge_head = now; // 加边
    return now;
}

char read_state_name(char *name)
{
    char c;
    int i = 0;
    fscanf(p, "%c", &c);
    while (c != ',' && c != '}' && c != '\n' && c != ' ')
    {
        name[i] = c;
        i++;
        fscanf(p, "%c", &c);
    }
    name[i] = 0;
    fseek(p, -1, SEEK_CUR);
    return c;
}

void clear_note()
{
    char c;
    fscanf(p, "%c", &c);
    while (!feof(p) && c != '\n')
    {
        fscanf(p, "%c", &c);
    }
    //   fseek(p, -1, SEEK_CUR);
}

void skip_char(char x)
{
    char c;
    fscanf(p, "%c", &c);
    bool flag = 0;
    //  cout<<"skip ";
    while (!feof(p))
    {
        //      cout<<c;
        if (c == ';')
            clear_note();
        else if (c == x)
            flag = 1;
        else if (c != ';' && flag)
        {
            c = x;
            fseek(p, -1, SEEK_CUR);
            break;
        }
        fscanf(p, "%c", &c);
    }
    //   cout<<endl;
    if (c != x)
        error();
}

void error()
{
    if (!debug)
    {
        cout << "syntax error" << endl;
        exit(0);
    }
}

void check_in_letters()
{
    for (int i = 32; i <= 126; i++)
    {
        if (in_letters[i] && !all_letters[i])
            error();
    }
}

void read()
{
    for (int i = 0; i < 7; i++)
    { // 读七部分
        read_single();
    }
    check_in_letters(); // 检查输入符号是否在带符号里

    skip_char('\n');
    // 接下来是转移函数
    char c;
    while (!feof(p))
    {
        // 旧状态
        char tmp[50];
        read_state_name(tmp);
        Edge *edge = new_edge(tmp);
#ifdef DEBUG
        cout << tmp;
#endif
        fscanf(p, "%c", &c); // 空格
        for (int i = 0; i < tape_num; i++)
        {
            fscanf(p, "%c", &c);
            edge->old_letter.push_back(c);
        }
        fscanf(p, "%c", &c);
        for (int i = 0; i < tape_num; i++)
        {
            fscanf(p, "%c", &c);
            edge->new_letter.push_back(c);
        }
        fscanf(p, "%c", &c);
        for (int i = 0; i < tape_num; i++)
        {
            fscanf(p, "%c", &c);
            edge->new_step.push_back(c);
        }
        fscanf(p, "%c", &c);
        read_state_name(tmp);
        edge->new_state = find_state(tmp);
        skip_char('\n');
#ifdef DEBUG
        cout << " -> " << edge->new_state->name << "   ";

        for (int i = 0; i < edge->old_letter.size(); i++)
            cout << edge->old_letter[i];

        cout << " -> ";
        for (int i = 0; i < edge->new_letter.size(); i++)
            cout << edge->new_letter[i];
        cout << "  move ";
        for (int i = 0; i < edge->new_step.size(); i++)
            cout << edge->new_step[i];
        cout << endl;
#endif
    }
}

void read_single()
{
    char c;
    skip_char('#');
    fscanf(p, "%c", &c);

    // 状态集
    if (c == 'Q')
    {
        skip_char('=');
        skip_char('{');
        while (1)
        {
            State *now = new_state();
            c = read_state_name(now->name);
            if (c == '}')
                break;
            if (c == '\n')
                error();
            fscanf(p, "%c", &c);
        }
#ifdef DEBUG
        cout << "State" << endl;
        for (int i = 0; i < state_set.size(); i++)
            cout << state_set[i]->name << endl;
        cout << endl;
#endif
    }

    // 输入符号集
    if (c == 'S')
    {
        skip_char('=');
        skip_char('{');
#ifdef DEBUG
        cout << "in_letters: ";
#endif
        while (fscanf(p, "%c", &c))
        {
            if (c == '\n')
                error();
            if (c == '}')
                break;
            if (c == ',')
                continue;
#ifdef DEBUG
            cout << c << " ";
#endif
            in_letters[c] = true;
        }
#ifdef DEBUG
        cout << endl;
#endif
    }

    // 带符号集
    if (c == 'G')
    {
        skip_char('=');
        skip_char('{');
#ifdef DEBUG
        cout << "all_letters: ";
#endif
        while (fscanf(p, "%c", &c))
        {
            if (c == '\n')
                error();
            if (c == '}')
                break;
            if (c == ',')
                continue;
#ifdef DEBUG
            cout << c << " ";
#endif
            all_letters[c] = true;
        }
#ifdef DEBUG
        cout << endl;
#endif
    }

    // 初始状态
    if (c == 'q')
    {
        skip_char('0');
        skip_char('=');
        skip_char(' ');
        start = new State();
        read_state_name(start->name);
#ifdef DEBUG
        cout << "start state = " << start->name << endl;
#endif
    }

    // 初始符号
    if (c == 'B')
    {
        skip_char('=');
        fscanf(p, "%c", &c);
        fscanf(p, "%c", &c);
        space_letter = c;
#ifdef DEBUG
        cout << "space letter = " << space_letter << endl;
#endif
    }

    // 终结状态
    if (c == 'F')
    {
        skip_char('=');
        skip_char('{');
        while (1)
        {
            char tmp[50];
            c = read_state_name(tmp);
            State *now = find_state(tmp);
            now->final = true;
            if (c == '}')
                break;
            if (c == '\n')
                error();
            fscanf(p, "%c", &c);
        }
#ifdef DEBUG
        cout << "Final State: ";
        for (int i = 0; i < state_set.size(); i++)
            if (state_set[i]->final == true)
                cout << state_set[i]->name << " ";
        cout << endl;
#endif
    }

    if (c == 'N')
    {
        skip_char('=');
        fscanf(p, "%c", &c);
        fscanf(p, "%d", &tape_num);
#ifdef DEBUG
        cout << "num = " << tape_num << endl;
#endif
    }
}

void run(char *w)
{
    // 先检查输入串
    cout << "Input: " << w << endl;
    for (int i = 0; i < strlen(w); i++)
    {
        if (in_letters[w[i]] == false)
        {
            cout << "==================== ERR ====================" << endl;
            cout << "error: '" << w[i] << "' was not declared in the set of input symbols " << endl;
            cout
                << "Input: " << w << endl;
            cout << "       ";
            for (int k = 0; k < i; k++)
                cout << ' ';
            cout << '^';
            cout << "==================== END ====================" << endl;
            exit(-1);
        }
    }
    cout << "==================== RUN ====================" << endl;

    machine = new Machine();
    machine->state = start;
    for (int i = 0; i < tape_num; i++)
    {
        machine->point[i] = 0;
        machine->left[i] = 1;
        machine->right[i] = -1;
    }
    for (int i = 0; i < strlen(w); i++)
    {
        machine->tapes[0].push_back(w[i]);
        machine->tapes[0].push_back(space_letter);
    }
    machine->left[0] = 0;
    machine->right[0] = strlen(w) - 1;

  /*  for (int i = 1; i <= 10; i++)
        move_right(0);
    for (int i = 1; i <= 20; i++)
        move_left(0);
    for (int i = 1; i <= 10; i++)
        move_right(0);
    print_tape(0);*/

    
}