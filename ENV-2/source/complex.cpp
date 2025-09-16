#include "complex.h"
#include <cmath>

// Constructors
Complex::Complex() : real(0.0), imag(0.0) {}
Complex::Complex(double r, double i) : real(r), imag(i) {}
Complex::Complex(const Complex& other) : real(other.real), imag(other.imag) {}

Complex::~Complex() {}

// = operator
Complex& Complex::operator=(const Complex& other) {
    if (this != &other) {
        real = other.real;
        imag = other.imag;
    }
    return *this;
}

// Getters
double Complex::getReal() const {
    return real;
}

double Complex::getImaginary() const {
    return imag;
}

// Setters
void Complex::setReal(double r) {
    real = r;
}

void Complex::setImaginary(double i) {
    imag = i;
}

// arithmetic
Complex Complex::operator+(const Complex& other) const {
    return Complex(real + other.real, imag + other.imag);
}

Complex Complex::operator-(const Complex& other) const {
    return Complex(real - other.real, imag - other.imag);
}

Complex Complex::operator*(double scalar) const {
    return Complex(real * scalar, imag * scalar);
}

// assignment operators
Complex& Complex::operator+=(const Complex& other) {
    real += other.real;
    imag += other.imag;
    return *this;
}

Complex& Complex::operator-=(const Complex& other) {
    real -= other.real;
    imag -= other.imag;
    return *this;
}

Complex& Complex::operator*=(double scalar) {
    real *= scalar;
    imag *= scalar;
    return *this;
}

// comp operators
bool Complex::operator<(const Complex& other) const {
    return abs() < other.abs();
}

bool Complex::operator>(const Complex& other) const {
    return abs() > other.abs();
}

bool Complex::operator==(const Complex& other) const {
    const double epsilon = 1e-10;
    return (std::abs(real - other.real) < epsilon) && 
           (std::abs(imag - other.imag) < epsilon);
}

bool Complex::operator!=(const Complex& other) const {
    return !(*this == other);
}

bool Complex::operator<=(const Complex& other) const {
    return (*this < other) || (*this == other);
}

bool Complex::operator>=(const Complex& other) const {
    return (*this > other) || (*this == other);
}

// Absolute
double Complex::abs() const {
    return std::sqrt(real * real + imag * imag);
}

// output
std::ostream& operator<<(std::ostream& os, const Complex& c) {

    if (c.imag >= 0) {
        os << c.real << " + " << c.imag << "i";
    }
    else {
        os << c.real << " - " << (-c.imag) << "i";
    }
    return os;
}

// input
std::istream& operator>>(std::istream& is, Complex& c) {
    is >> c.real >> c.imag;
    return is;
}
