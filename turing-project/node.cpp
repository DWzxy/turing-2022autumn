#include "node.h"
using namespace std;
// #define DEBUG

extern FILE *p;
extern bool debug;

vector<State *> state_set; // 状态集
bool in_letters[256];      // 输入字符集，bool表示是否在其中
bool all_letters[256];     // 字符集
State *start;              // 初始状态
char space_letter;         // 空格符号
int tape_num;              // 纸带数

Machine *machine;
int step = 0;

// 映射，正数为双，负数为单
// 0 -> 0
//-1 -2 -3 -> 1 3 5
// 1 2 3 -> 2 4 6
void move_right(int i)
{
    machine->point[i] += 1;
    int now = tran_num(machine->point[i]);
    for (int k = machine->tapes[i].size() - 1; k < now; k++)
        machine->tapes[i].push_back(space_letter);
}
void move_left(int i)
{
    machine->point[i] -= 1;
    int now = tran_num(machine->point[i]);
    for (int k = machine->tapes[i].size() - 1; k < now; k++)
        machine->tapes[i].push_back(space_letter);
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

Edge *find_edge()
{
#ifdef DEBUG
    cout << "try to find  ";
    for (int j = 0; j < tape_num; j++)
    {
        cout << machine->tapes[j][tran_num(machine->point[j])];
    }
    cout << endl;
#endif

    Edge *tmp = NULL;
    for (Edge *i = machine->state->edge_head; i; i = i->next)
    {
        bool flag = true;

#ifdef DEBUG
        cout << "now check " << i->new_state->name << "  ";
        for (int j = 0; j < tape_num; j++)
            cout << i->old_letter[j];
        cout << endl;
#endif

        for (int j = 0; j < tape_num; j++)
        {
            if (i->old_letter[j] != '*' && i->old_letter[j] !=
                                               machine->tapes[j][tran_num(machine->point[j])])
                flag = false;
        }
        if (!flag)
            continue;
        if (tmp == NULL)
            tmp = i;
        else
        {
            // 看是否比之前的更精确
            flag = true;
            for (int j = 0; j < tape_num; j++)
            {
                if (i->old_letter[j] == '*' &&
                    tmp->old_letter[j] != '*')
                    flag = false;
            }
            if (flag)
                tmp = i;
        }
    }
#ifdef DEBUG
    if (tmp == NULL)
        return tmp;
    cout << "finally choose " << tmp->new_state->name << "  ";
    for (int j = 0; j < tape_num; j++)
        cout << tmp->old_letter[j];
    cout << endl;
#endif
    return tmp;
}

void print_tape(int x)
{
    int left = 0, right = 0;
    int size = machine->tapes[x].size() - 1;
    if (size == 0)
        left = right = 0;
    else if (size % 2)
        left = retran_num(size), right = retran_num(size - 1);
    else
        left = retran_num(size - 1), right = retran_num(size);

    while (machine->tapes[x][tran_num(left)] == space_letter &&
           left < machine->point[x])
        left++;
    while (machine->tapes[x][tran_num(right)] == space_letter &&
           right > machine->point[x])
        right--;

    printf("Index%-2d: ", x);
    for (int i = left; i <= right; i++)
        cout << abs(i) << " ";

    cout << endl;

    printf("Tape%-3d: ", x);
    for (int i = left; i <= right; i++)
    {
        cout << machine->tapes[x][tran_num(i)];
        if (i < -9 || i > 9)
            cout << "  ";
        else
            cout << " ";
    }
    cout << endl;

    printf("Head%-3d: ", x);
    for (int i = left; i < machine->point[x]; i++)
    {
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
        if (feof(p))
            break;
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
    while (c != '\n')
    {
        fscanf(p, "%c", &c);
        if (feof(p))
            break;
    }
    if (c == '\n')
        fseek(p, -1, SEEK_CUR);
}

void skip_char(char x)
{
    char c;
    bool flag = 0;
    //  cout<<"skip ";
    while (1)
    {
        fscanf(p, "%c", &c);
        if (feof(p))
            error();
        if (c == ' ')
            continue;
        else if (c == ';')
            clear_note();
        else if (c == x)
            flag = 1;
        else if (c != ';' && flag)
        {
            c = x;
            fseek(p, -1, SEEK_CUR);
            break;
        }
    }
    //   cout<<endl;
}

bool try_char(char x)
{
    char c;
    bool flag = 0;
    while (1)
    {
        fscanf(p, "%c", &c);
        if (feof(p))
            return false;
        if (c == ' ')
            continue;
        if (c == ';')
            clear_note();
        else if (c == x)
            flag = 1;
        else if (c != ';' && flag)
        {
            fseek(p, -1, SEEK_CUR);
            break;
        }
    }
    return true;
}

void error()
{
    cout << "syntax error" << endl;
    exit(0);
}

void check_char(char x, bool flag) // 若为true则允许_存在
{
    if (x == ' ' || x == ',' || x == ';' || x == '{' ||
        x == '}' || x == '*')
        error();
    if (!flag && x == '_')
        error();
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

    char c;
    while (1)
    {
        bool flag = try_char('\n');
        if (!flag)
            break;
        // 接下来是转移函数
        // 旧状态
        char tmp[50];
        read_state_name(tmp);
        Edge *edge = new_edge(tmp);
        //       cout<<"666  "<<tmp<<endl;
#ifdef DEBUG
        cout << tmp;
#endif
        // cout<<"888  "<<tmp<<endl;
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

#ifdef DEBUG
    for (int i = 0; i < state_set.size(); i++)
    {
        cout << state_set[i]->name << endl;
        for (Edge *k = state_set[i]->edge_head; k; k = k->next)
        {
            cout << " -> " << k->new_state->name << endl;
        }
    }
#endif
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
    else if (c == 'S')
    {
        skip_char('=');
#ifdef DEBUG
        cout << "in_letters: ";
#endif
        while (fscanf(p, "%c", &c))
        {
            if (c == '}')
                break;
            if (c != '{' && c != ',')
                error();
            fscanf(p, "%c", &c);
            if (c == '}')
                break;
#ifdef DEBUG
            cout << c << " ";
#endif
            check_char(c, false);
            in_letters[c] = true;
        }
#ifdef DEBUG
        cout << endl;
#endif
    }

    // 带符号集
    else if (c == 'G')
    {
        skip_char('=');
#ifdef DEBUG
        cout << "all_letters: ";
#endif
        while (fscanf(p, "%c", &c))
        {
            if (c == '}')
                break;
            if (c != '{' && c != ',')
                error();
            fscanf(p, "%c", &c);
            if (c == '}')
                break;
#ifdef DEBUG
            cout << c << " ";
#endif
            check_char(c, true);
            all_letters[c] = true;
        }
#ifdef DEBUG
        cout << endl;
#endif
    }
    // 初始状态
    else if (c == 'q')
    {
        skip_char('0');
        skip_char('=');
        char t[50];
        read_state_name(t);
        start = find_state(t);
#ifdef DEBUG
        cout << "start state = " << start->name << endl;
#endif
    }

    // 初始符号
    else if (c == 'B')
    {
        skip_char('=');
        fscanf(p, "%c", &c);
        space_letter = c;
#ifdef DEBUG
        cout << "space letter = " << space_letter << endl;
#endif
    }

    // 终结状态
    else if (c == 'F')
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

    else if (c == 'N')
    {
        skip_char('=');
        fscanf(p, "%d", &tape_num);
#ifdef DEBUG
        cout << "num = " << tape_num << endl;
#endif
    }

    else error();
}

void run(char *w)
{
    // 先检查输入串
    if (debug)
        cout << "Input: " << w << endl;
    for (int i = 0; i < strlen(w); i++)
    {
        if (in_letters[w[i]] == false)
        {
            if (debug)
            {
                cout << "==================== ERR ====================" << endl;
                cout << "error: '" << w[i] << "' was not declared in the set of input symbols " << endl;
                cout
                    << "Input: " << w << endl;
                cout << "       ";
                for (int k = 0; k < i; k++)
                    cout << ' ';
                cout << '^' << endl;
                cout << "==================== END ====================" << endl;
                exit(-1);
            }
            else
            {
                cout << "illegal input" << endl;
                exit(-1);
            }
        }
    }
    if (debug)
        cout << "==================== RUN ====================" << endl;

    machine = new Machine();
    machine->state = start;
    for (int i = 0; i < tape_num; i++)
    {
        machine->point[i] = 0;
        machine->tapes[i].push_back(space_letter);
    }
    for (int i = 0; i < strlen(w); i++)
    {
        machine->tapes[0][i << 1] = w[i];
        machine->tapes[0].push_back(space_letter);
        machine->tapes[0].push_back(space_letter);
    }

    /*  for (int i = 1; i <= 10; i++)
          move_right(0);
      for (int i = 1; i <= 20; i++)
          move_left(0);
      for (int i = 1; i <= 10; i++)
          move_right(0);
      */

    while (1)
    {
        //   if (step > 25)
        //       break;
        if (debug)
        {
            printf("Step   : %d\n", step);
            step++;
            printf("State  : %s\n", machine->state->name);
            for (int i = 0; i < tape_num; i++)
                print_tape(i);
            cout << "---------------------------------------------" << endl;
        }
        //    if (machine->state->final == true)
        //      break;
        Edge *edge = find_edge();
        if (edge == NULL)
            break; // 没有转移，停机

        machine->state = edge->new_state; // 改状态
        for (int i = 0; i < tape_num; i++)
        { // 改写带符号
            int num = tran_num(machine->point[i]);
            char letter = edge->new_letter[i];
            if (letter != '*')
                machine->tapes[i][num] = letter;
        }
        for (int i = 0; i < tape_num; i++)
        { // 移动指针
            char letter = edge->new_step[i];
            if (letter == 'l')
                move_left(i);
            else if (letter == 'r')
                move_right(i);
        }
    }

    if (debug)
        cout << "Result: ";

    int left = 0, right = 0;
    int size = machine->tapes[0].size() - 1;
    if (size == 0)
        left = right = 0;
    else if (size % 2)
        left = retran_num(size), right = retran_num(size - 1);
    else
        left = retran_num(size - 1), right = retran_num(size);

    while (machine->tapes[0][tran_num(left)] == space_letter)
        left++;
    while (machine->tapes[0][tran_num(right)] == space_letter)
        right--;
    for (int i = left; i <= right; i++)
        cout << machine->tapes[0][tran_num(i)];
    cout << endl;

    if (debug)
        cout << "==================== END ====================" << endl;
}