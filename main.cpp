#include <iostream>
#include <cstdlib>

enum calcres { CR_ERROR, CR_CORRECT };

using namespace std;
template<class T, T (parse)(const char*, char*&)>
class calc {
public:

    //основной метод
    static T run(const char* s, calcres& st){
        const char* e = NULL;
        int n = 0;
        for(const char* i = s; *i; ++i){
            if(*i == '(')
                ++n;
            else if(*i == ')')
                --n;
        }

        if(!*s || (n != 0)){
            st = CR_ERROR;
            return 0;
        }
        return _calc(s, e, st);
    }

private:

    //метод для вычислений выражений
    static T _calc(const char* s, const char*& e, calcres& st){
        T    a, b;
        char m;
        if((st = _next_num(s, a)) == CR_ERROR)
            return 0;

        if(*s == ')'){
            e = s + 1;
            return a;
        }

        while(*s){
            m = *s++;
            if((st = _next_num(s, b)) == CR_ERROR)
                break;

            switch(m){
            case '+':
            case '-':
                if((*s == '*') || (*s == '/')){
                    const char* p = s;
                    if((st = _mul_div(p, b)) == CR_ERROR)
                        return a;
                    s = p;
                }

                if(m == '+')
                    a += b;
                else
                    a -= b;
                break;
            case '*':
                a *= b;
                break;
            case '/':
                if(b == 0){
                    st = CR_ERROR;
                    return a;
                }
                a /= b;
                break;
            default:
                st = CR_ERROR;
                return b;
            }

            if(*s == ')'){
                e = s + 1;
                break;
            }
        }
        return a;
    }

    //выполнение приоритетных операций умножение и деление
    static calcres _mul_div(const char*& s, T& n){
        T       k;
        char    m;
        calcres st = CR_CORRECT;

        while(*s){
            m  = *s++;
            st = _next_num(s, k);
            if(st == CR_ERROR)
                break;

            if(m == '*')
                n *= k;
            else if(m == '/'){
                if(k == 0){
                    st = CR_ERROR;
                    break;
                }
                n /= k;
            }

            if((*s != '*') && (*s != '/'))
                break;
        }
        return st;
    }

    //парсирование числа
    static calcres _next_num(const char*& s, T& n){
        calcres st;
        const char* p = s;
        while(is_space(*p))
            ++p;

        bool neg = (*p == '-');
        if(neg)
            ++p;

        if(*p == '('){
            s = p + 1;
            n = _calc(s, p, st);
            if(st == CR_ERROR)
                return st;
        } else {
            char* i;
            n = (parse)(p, i);
            if(p == i)
                return CR_ERROR;
            p = i;
        }
        if(neg)
            n = -n;

        while(is_space(*p))
            ++p;
        s = p;
        return CR_CORRECT;
    }

    static bool is_space(int c){
        return (c == ' ' || c == '\t' || c == '\n');
    }
};

//для long
long lparse(const char* s, char*& p){
    return strtol(s, &p, 10);
}

//для double
double dparse(const char* s, char*& p){
    return strtod(s, &p);
}


int main(void){
    char s[10000];
    calcres res;

    //решение с консоли тип double
    do {
        cout << "Enter expr: ";
        cin.getline(s, sizeof(s));
        for(int i=0;i<sizeof(s);i++){
            if(s[i]==','){
                s[i]='.';
                }
        }
        cin.sync();

        double d = calc<double, dparse>::run(s, res);
        if(res == CR_CORRECT)
            cout << "result: " << d;
        else
            cout << "error calculate!";

        cout << endl << endl;
    } while(!cin.fail());
    return 0;
}
