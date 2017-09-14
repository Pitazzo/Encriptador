#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "BigIntegerLibrary.hh"

using namespace std;

BigInteger exponenciacion(BigInteger base, BigInteger exponente, BigInteger modulo){
    BigInteger x = 1;
    BigInteger y = base;
    while (exponente > 0){
        if (exponente % 2 == 1){
        	x = (x * y) % modulo;
        }    
		y = (y * y) % modulo;
		exponente = exponente / 2;
    }
    return x % modulo;
}

bool RabinMillerTest(BigInteger n, BigInteger d){
	BigInteger aleatorio = rand();
	BigInteger dos = 2;
	BigInteger a = dos + aleatorio % ((n-2) - 1);
	BigInteger x = exponenciacion(a,d,n);
	if(x == 1 || x == n-1){
		return true;
	}
	while (d != n-1){
        x = (x * x) % n;
        d = d+d;
        if (x == 1){
        	return false;
        }      
        if (x == n-1){
        	return true;
        }
    }
    return false;
}

bool RabinMiller(BigInteger n, int k){
	srand(time(NULL));
	if(n<2){
		return false;
	}else if(n%2==0 && n!=2){
		return false;
	}else if(n == 2){
		return true;
	}else{
		BigInteger d = n-1;
		BigInteger r = 0;
		while(d%2 == 0){
			d = d/2;
			r++;
		}
		for(int i = 0; i < k; i++){
			if(!RabinMillerTest(n,d)){
				return false;
			}
		}
		return true;
	}
}

BigInteger siguientePrimo(BigInteger n){
	if(n%2==0){
		n++;
	}
	while(!RabinMiller(n, 10)){
		n = n+2;
	}
	return n;
}
