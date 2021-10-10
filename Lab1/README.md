# Lab1

Zadanie wprowadzające do C++ oraz algorytmów optymalizacyjnych:

### 2.Napisz funkcję w języku C++, która będzie jako argumenty przyjmowała dwie funkcje oraz liczbę typu naturalnego obliczenia(f1, f2, n):
Funkcja f2 będzie generowała punkty z dziedziny f1 i przyjmowała jako argument liczbę całkowitą
Funkcja f1 będzie przyjmowała punkty typu takiego jak zwracane przez f2 oraz będzie zwracała jedną wartość rzeczywistą

Funkcja obliczenia będzie działała w następujący sposób:
```
obliczenia(f1,f2,n):
    max_x ← f2(0);
    max_y ← f1(max_x);
    powtarzaj dla i ← 1 do n:
        x ← f2(i);
        y ← f1(x);
        jeżeli y > max_y:
            max_x ← x
            max_y ← y;
    zwracaj: [max_x, max_y];
```
### 3.Zaimplementuj przykładowe funkcje f1 oraz f2
```	
f1(x) = 20-abs(x)
f2(i) = (2*(i%2)-1)*i/2
```
