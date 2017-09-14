#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "BigIntegerLibrary.hh"

#ifndef PROPIEDADES_H

#define PROPIEDADES_H

BigInteger exponenciacion(BigInteger base, BigInteger exponente, BigInteger modulo);

BigInteger siguientePrimo(BigInteger n);

bool RabinMillerTest(BigInteger n, BigInteger d);

bool RabinMiller(BigInteger n, int k);

#endif
