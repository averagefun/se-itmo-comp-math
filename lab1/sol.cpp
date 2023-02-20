#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Matrix {
   private:
    int n;
    double acc;

    vector<vector<double>> a;
    vector<double> b;
    vector<double> sol;

    vector<double> sol_acc;
    int sol_iter = 0;
    const int MAX_ITER = 100;

   public:
    Matrix() {}

    void init() {
        cout << "Введите n (размерность матрицы) >> ";
        cin >> n;

        cout << "Введите коэффициенты матрицы построчно, разделяя элементы "
                "пробелом - a11 a12 a13 ... a1n b1:"
             << endl;

        double value;
        for (int i = 0; i < n; i++) {
            vector<double> a_row;
            for (int j = 0; j < n; j++) {
                cin >> value;
                a_row.push_back(value);
            }
            a.push_back(a_row);
            cin >> value;
            b.push_back(value);
        }

        sol.assign(n, 0.);
        sol_acc.assign(n, numeric_limits<double>::max());

        cout << "Введите точность вычислений >> ";
        cin >> acc;
    }

    void init(ifstream* fstream) {
        string line;

        getline(*fstream, line);
        n = stoi(line);

        double value;
        for (int i = 0; i < n; i++) {
            getline(*fstream, line);
            vector<double> a_row;
            istringstream iss(line);
            for (int j = 0; j < n; j++) {
                iss >> value;
                a_row.push_back(value);
            }
            a.push_back(a_row);
            iss >> value;
            b.push_back(value);
        }

        sol.assign(n, 0);
        sol_acc.assign(n, numeric_limits<double>::max());

        getline(*fstream, line);
        acc = stod(line);
    }

   private:
    double sum_sol_row(int i) {
        double sum = 0;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            sum += a[i][j] / a[i][i] * sol[j];
        }
        return sum;
    }

   public:
    bool shuffle() {
        // find biggest elements in each row
        vector<int> biggest(n, -1);
        set<int> biggest_set;
        bool found_strict = false;
        for (int i = 0; i < n; i++) {
            double sum = 0;
            for (double element : a[i]) sum += element;

            for (int j = 0; j < n; j++) {
                if (2 * a[i][j] >= sum) {
                    if (2 * a[i][j] > sum) {
                        found_strict = true;
                    }
                    biggest[i] = j;
                    biggest_set.insert(j);
                    break;
                }
            }

            if (biggest[i] == -1) return false;
        }

        if (!found_strict) return false;
        if (biggest.size() != biggest_set.size()) return false;

        vector<vector<double>> shuffled_a(n);
        vector<double> shuffled_b(n);
        for (int i = 0; i < n; i++) {
            shuffled_a[biggest[i]] = a[i];
            shuffled_b[biggest[i]] = b[i];
        }

        a = shuffled_a;
        b = shuffled_b;
        return true;
    }

    void iterate() {
        vector<double> new_sol(n);
        for (int i = 0; i < n; i++) {
            new_sol[i] = b[i] / a[i][i] - sum_sol_row(i);
            sol_acc[i] = abs(new_sol[i] - sol[i]);
        }
        sol = new_sol;
        sol_iter++;
    }

    void solve() {
        if (!shuffle()) {
            cout << "Невозможно привести к диагональному преобладанию." << endl;
            return;
        }
        while (*max_element(sol_acc.begin(), sol_acc.end()) > acc &&
               sol_iter < MAX_ITER) {
            iterate();
        }
        print_sol();
    }

    void print() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << a[i][j] << " ";
            }
            cout << "| " << b[i] << endl;
        }
    }

    void print_sol() {
        cout << "--- Решение ---" << endl;
        for (int i = 0; i < n; i++) {
            cout << "x" << i + 1 << ": " << sol[i] << " (delta = " << sol_acc[i]
                 << ")" << endl;
        }
        cout << "Найдено за " << sol_iter << " итераций." << endl;
    }
};

// Вариант 2 -> Метод простых итераций
int main() {
    cout << "--- Решение СЛАУ ---" << endl;
    cout << "Введите имя входного файла / нажмите Enter для stdin >> ";

    string input;
    getline(cin, input);

    Matrix matrix;

    if (input.empty()) {
        // stdin
        try {
            matrix.init();
        } catch (const exception) {
            cerr << "Ошибка ввода!" << endl;
            return 2;
        }
    } else {
        // read from file
        ifstream fstream(input);
        if (fstream.fail()) {
            cerr << "Файл не существует!" << endl;
            return 1;
        }
        try {
            matrix.init(&fstream);
        } catch (const exception) {
            cerr << "Ошибка при чтении файла!" << endl;
            return 2;
        }
    }

    matrix.solve();
}
