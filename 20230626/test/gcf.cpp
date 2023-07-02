#include <iostream>
#include <vector>
// #include <bits/stdc++.h>
using namespace std;

int gcf(int a, int b){
    if(a==1){
        return 1;
    }
    if(b==1){
        return 1;
    }
    while(a!=b){
        if(a>b){
            a -= b;
        }
        else{
            b -= a;
        }
    }
    return a;
}
