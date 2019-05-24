// Copyright 2019 Roman Vyunov
#include<tbb/tbb.h>
#include<vector>
#include<iostream>
#include<random>
#include<cstring>
#include<stdlib.h>
#include<time.h>

struct Point {
    static double x0, y0;
    double x;
    double y;
    Point () {
        x0 = 0;
        y0 = 0;
        x = 0;
        y = 0;
    }
    friend bool operator < (Point a, Point c){
        return (x0 - a.x) * (c.y - y0) - (y0 - a.y) * (c.x - x0) > 0;
    }
};

double Point::x0;
double Point::y0;

struct Stack {
    Point *s;
    int size;
    int max_size;
    explicit Stack(int max_size) {
        if (max_size <= 0) {
            std::cerr << "Stack create error: negative size" << std::endl;
        } else {
            this->size = 0;
            this->max_size = max_size;
            this->s = new Point[max_size];
        }
    }

    Stack(const Stack& stack) {
        std::memcpy(s, stack.s, stack.size * sizeof(Point));
        size = stack.size;
        max_size = stack.max_size;
    }
    ~Stack() {
        delete[] s;
    }

    bool push(Point val) {
        if (size < max_size) {
            s[size++] = val;
            return true;
        } else {
            std::cerr << "Stack error: stack overflow" << std::endl;
            return false;
        }
    }

    Point pop() {
        return s[size--];
    }

    Point operator[] (int n) {
        return s[n];
    }
};

inline double rotate(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
}

Stack graham(const std::vector<Point> &a) {
    std::vector<Point> p = a;
    Point p0 = p[0];
    for (auto &pnt : a) {
        if (pnt.y < p0.y || (pnt.y == p0.y && pnt.x < p0.x))
            p0 = pnt;
    }
    Point::x0 = p0.x;
    Point::y0 = p0.y;

    tbb::parallel_sort(p);
    Stack s(p.size());
    s.push(p[0]);
    s.push(p[1]);
    for (size_t i = 2; i < p.size(); i++) {
        while (rotate(s[s.size - 2], s[s.size - 1], p[i]) < 0) {
            s.pop();
        }
        s.push(p[i]);
    }
    return s;
}

void generateVectorOfPoints(std::vector<Point> &p, int N, double field) {
    srand(static_cast<unsigned int>(time(nullptr)));
    unsigned int rd_range = RAND_MAX;
    for (int i = 0; i < N; i++) {
        Point pnt;
        pnt.x = ((static_cast<double>(rand()) - rd_range / 2) * 2) / rd_range * field;
        pnt.y = ((static_cast<double>(rand()) - rd_range / 2) * 2) / rd_range * field;
        p.push_back(pnt);
    }
}

int main(int argc, char *argv[]) {
    bool verbose_out = false;
    std::vector<Point> *p = nullptr;
    unsigned int N = 0;
    if (argc == 1) {
        N = 30;
    } else if (argc > 1) {
        N = atoi(argv[1]);
        if (argc > 2 && std::strcmp(argv[2], "-v") == 0) {
            verbose_out = true;
        }
    } else {
        std::cerr << "Incorrect number of arguments" << std::endl;
        return EXIT_FAILURE;
    }
    p = new std::vector<Point>();
    generateVectorOfPoints(*p, N, 10);
    if (verbose_out) {
        std::cout << "Number of points: " << N << std::endl;
        std::cout << "Input array of points:" << std::endl;
        for (unsigned int i = 0; i < (*p).size(); i++) {
            std::cout << i + 1 << " : " << (*p)[i].x << "\t" << (*p)[i].y << std::endl;
        }
    }
    tbb::tick_count start = tbb::tick_count::now();
    Stack result = graham(*p);
    tbb::tick_count end = tbb::tick_count::now();
    if (verbose_out) {
        std::cout << "Resuts:" << std::endl;
        for (int i = 0; i < result.size; i++) {
            Point tmp = result.pop();
            std::cout << i + 1 << " : " <<  tmp.x << "\t" << tmp.y << std::endl;
        }
    }
    std::cout << "Execution time: " << (end - start).seconds() << std::endl;
    // delete p;
    return EXIT_SUCCESS;
}
