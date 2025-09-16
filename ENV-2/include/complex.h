#ifndef COMPLEX_H
#define COMPLEX_H

#include <iostream>

class Complex {
    double real;
    double imag;

public:
    Complex();
    Complex(double r, double i = 0.0);
    Complex(const Complex& other);
    
    ~Complex();
    
    Complex& operator=(const Complex& other);
    
    double getReal() const;
    double getImaginary() const;
    void setReal(double r);
    void setImaginary(double i);

    Complex operator+(const Complex& other) const;
    Complex operator-(const Complex& other) const;
    Complex operator*(double scalar) const;
    
    Complex& operator+=(const Complex& other);
    Complex& operator-=(const Complex& other);
    Complex& operator*=(double scalar);
    
    bool operator<(const Complex& other) const;
    bool operator>(const Complex& other) const;
    bool operator==(const Complex& other) const;
    bool operator!=(const Complex& other) const;
    bool operator<=(const Complex& other) const;
    bool operator>=(const Complex& other) const;
    
    double abs() const;
    friend std::ostream& operator<<(std::ostream& os, const Complex& c);
    friend std::istream& operator>>(std::istream& is, Complex& c);
};

#endif // COMPLEX_H