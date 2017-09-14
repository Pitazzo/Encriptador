#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <ctime>
#include <cmath>

#include "BigIntegerLibrary.hh"
#include "RabinMiller.h"

using namespace std;

int numeroDeBytes(BigInteger n){
	int resultado = 0;
	while(n>0){
		n=n/2;
		resultado++;
	}
	return int(ceil(resultado/8.0));
}

void escribirEntero(ofstream& f, BigInteger n, int bytesTotales){
	int bytesACero = bytesTotales - numeroDeBytes(n);
	if(bytesACero >= 0){
		unsigned char byteGuardado = 0;
		BigUnsigned mascara = 0xFF;
		BigUnsigned salida = n.getMagnitude();
		while(salida > 0){
			byteGuardado = char((salida & mascara).toInt());
			f.write(reinterpret_cast <char *>(&byteGuardado),sizeof(char));
			salida = salida/256;
		}
		byteGuardado = 0;
		for(int i = 0; i < bytesACero; i++){
			f.write(reinterpret_cast <char *>(&byteGuardado),sizeof(char));
		}
	}else{
		cout << "Revisar número de bytes del entero: " << n << endl;
	}
}

BigInteger potencia(int base, int exponente){
	BigInteger resultado = 1;
	for(int i = 0; i < exponente; i++){
		resultado = resultado*base;
	}
	return resultado;
}

BigInteger leerEntero(ifstream& f, int numBytes){
	BigInteger resultado = 0;
	unsigned char temp = 0;
	if(!f.eof()){
		f.read(reinterpret_cast <char *>(&temp), sizeof(char));
		resultado = int(temp);
		for(int i = 0; i < numBytes-1 && !f.eof(); i++){
			f.read(reinterpret_cast <char *>(&temp), sizeof(char));
			resultado = resultado + (BigInteger(int(temp)) * potencia(256,i+1));
		}
	}
	return resultado;
}

void calcularExtension(char extension[], BigInteger cabecera){
	char temp[10];
	int i = 0;
	while(cabecera>0){
		temp[i] = (cabecera%1000).toInt();
		cabecera = cabecera/1000;
		i++;
	}

	for(int j = 0; j < i; j++){
		extension[j] = temp[i-1-j];
	}
	extension[i] = '\0';
}

void leerExtension(char entrada[], char extension[]){
	int longitudHastaExtension = 0, contador = 0;
	while(entrada[longitudHastaExtension]!='.'){
		longitudHastaExtension++;
	}
	longitudHastaExtension++;
	while(entrada[longitudHastaExtension]!='\0'){
		extension[contador] = entrada[longitudHastaExtension];
		contador++;
		longitudHastaExtension++;
	}
	extension[contador] = '\0';
}

void cifrarFichero(ifstream& in, ofstream& out, char extension[], BigInteger n, BigInteger e){
	cout << "Comenzando cifrado..." << endl;
	int longitudExtension = strlen(extension);
	BigInteger cabecera = 0;
	BigInteger temp = 0;
	for(int i = 0; i < longitudExtension; i++){
		cabecera = cabecera + BigInteger(int(extension[i]))*potencia(1000,longitudExtension-1-i);
	}
	
	int bytesClave = numeroDeBytes(n);
	in.seekg (0, in.end);
    int longitud = in.tellg();
    in.seekg (0, in.beg);
    int numBytes = numeroDeBytes(n);
    int bloques = int(ceil(double(longitud)/double(numBytes)));
    int bloquesRecorridos = 1;
	cout << "Comenzando descifrado..." << endl;
	cout << "El fichero original ocupa " 
	<< longitud << " bytes (" << bloques << " bloques de memoria)" <<endl;
	
	cabecera = exponenciacion(cabecera,e,n);
	escribirEntero(out,cabecera,bytesClave);
	

	cout << "Se ha cifrado el " << setprecision(3)
	<< (double(bloquesRecorridos)/bloques)*100 << "% del fichero       " << flush;
	

	while(!in.eof()){
		cout << '\r';
		cout << "Se ha cifrado el " << setprecision(3)
		<< (double(bloquesRecorridos)/bloques)*100 << "% del fichero       " << flush;
		temp = leerEntero(in, bytesClave-1);
		temp = exponenciacion(temp,e,n);
		escribirEntero(out,temp,bytesClave);
		bloquesRecorridos++;
	}
	cout << endl << "Cifrado concluido" << endl;
	
}

void descifrarFichero(ifstream& in, char salida[], BigInteger n, BigInteger d){
    in.seekg (0, in.end);
    int longitud = in.tellg();
    in.seekg (0, in.beg);
    int numBytes = numeroDeBytes(n);
    int bloques = int(ceil(double(longitud)/double(numBytes)));
    int bloquesRecorridos = 0;
	cout << "Comenzando descifrado..." << endl;
	cout << "El fichero cifrado ocupa " 
	<< longitud << " bytes (" << bloques << " bloques de memoria)" <<endl;

	char extension[10];
	BigInteger cabecera = leerEntero(in, numBytes);
	BigInteger temp = 0;
	cabecera = exponenciacion(cabecera,d,n);
	calcularExtension(extension, cabecera);
	bloquesRecorridos++;
	
	cout << "El fichero descifrado es del tipo ." << extension << endl;
	cout << "Se ha descifrado el " << setprecision(3)
	<< (double(bloquesRecorridos)/bloques)*100 << "% del fichero       " << flush;
	strcat(salida, ".");
	strcat(salida, extension);
	ofstream out;
	out.open(salida, ios::binary);
	if(out.is_open()){
	
		temp = leerEntero(in, numBytes);
		temp = exponenciacion(temp, d, n);
		escribirEntero(out, temp, numBytes-1);
		
		while(!in.eof()){
		
			temp = leerEntero(in, numBytes);
			bloquesRecorridos++;
			cout << '\r';
			cout << "Se ha descifrado el " << setprecision(3) 
			<< (double(bloquesRecorridos)/bloques)*100 << "% del fichero       " << flush;
			
			if(temp!=0){
		
				temp = exponenciacion(temp, d, n);
				if(bloques-1 != bloquesRecorridos){
					escribirEntero(out, temp, numBytes-1);
				}else{
					escribirEntero(out, temp, numeroDeBytes(temp));
				}
	
			}
		}
		cout << endl;
		cout << "El descifrado ha concluido" << endl;
	}else{
		cout << "El fichero descifrado no ha podido ser creado"<< endl;
	}
}


void cargarClaves(BigInteger& clave1, BigInteger& clave2){
	clave1 = 0;
	clave2 = 0;
	char c;
	char ruta[100];
	cout << "Introduzca el nombre del fichero de claves: " << endl;
	cout << ">> " << flush;
	cin >> ruta;
	ifstream claves;
	claves.open(ruta);
	if(claves.is_open()){
		claves.get(c);
		while(c!='\n'){
			clave1 = clave1 * 10 + (c-'0');
			claves.get(c);
		}
		claves.get(c);
		while(c!='\n'){
			clave2 = clave2 * 10 + (c-'0');
			claves.get(c);
		}
	}else{
		cout << "No ha sido posible acceder al fichero de claves." << endl;
	}
}

void interaccionCifrar(){
	char entrada[100];
	char salida[100];
	char extension[100];
	ifstream in; 
	ofstream out;
	BigInteger n;
	BigInteger e;
	BigInteger d;
	cout << "Introduzca nombre del fichero de entrada:" << endl;
	cout << ">> ";
	cin >> entrada;
	leerExtension(entrada, extension);
	cout << "El fichero a cifrar es del tipo ."<<extension << endl;
	cargarClaves(n,e);
	in.open(entrada, ios :: binary);
	if(in.is_open()){
		cout << "El fichero " << entrada << " ha sido leído con éxito." << endl;
		cout << "Introduzca nombre del fichero cifrado:" << endl;
		cout << ">> ";
		cin >> salida;
		out.open(salida, ios :: binary);
		cifrarFichero(in, out, extension,n,e);
	}else{
		cout << "El fichero " << entrada << " no ha podido ser abierto." << endl;
	}
}

void interaccionDescifrar(){
	char entrada[100];
	char salida[100];
	char extension[100];
	BigInteger n;
	BigInteger d;
	ifstream in; 
	ofstream out;
	
	cout << "Introduzca nombre del fichero a descifrar:" << endl;
	cout << ">> ";
	cin >> entrada;
	cout << "Introduzca nombre de salida del fichero descifrado:" << endl;
	cout << ">> ";
	cin >> salida;
	
	in.open(entrada, ios :: binary);
	
	if(in.is_open()){
		cout << "El fichero " << entrada << " ha sido leído con éxito." << endl;
		cargarClaves(n,d);
		descifrarFichero(in, salida, n, d);
		cout << "Descifrado con éxito" << endl;
	}else{
		cout << "No ha sido posible leer el fichero " << entrada << endl;
	}
}

BigInteger xGCD(BigInteger a, BigInteger b, BigInteger &x, BigInteger &y) {
    if(b == 0) {
       x = 1;
       y = 0;
       return a;
    }

    BigInteger x1, y1, gcd = xGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

BigInteger euler(BigInteger p, BigInteger q){
	return (p-1)*(q-1);
}

void escribirClaves(BigInteger clave1, BigInteger clave2, char tipo[]){
	cout << "Introduzca el nombre del archivo donde escribir la clave "<< tipo << "." << endl;
	cout << "Debe ser de texto, y en caso de no existir, será creado" << endl;
	cout << ">> " << flush;
	char ruta[100];
	cin >> ruta;
	ofstream claves;
	claves.open(ruta);
	if(claves.is_open()){
		claves << clave1;
		claves << '\n';
		claves << clave2;
		claves << '\n';
		cout << "Clave " << tipo << " guardada con éxito" << endl;
	}else{
		cout << "No ha sido posible escribir el fichero de clave "<< tipo << endl;
	}
}

void generarClaves(){
	int cifras = 100;
	cout << "Introduzca el número de bytes de la clave:" << endl;
	cout << ">> "<< flush;
	cin >> cifras;
	srand(time(NULL));
	BigInteger p = rand() % 10;
	BigInteger q = rand() % 10;

	for(int i = 0; i < cifras; i++){
		if(i!=cifras-1){
			p = p*10 + rand() % 10;
			q = q*10 + rand() % 10;
		}else{
			BigInteger ultimaCifraP = rand() % 10;
			while(ultimaCifraP%2 == 0){
				ultimaCifraP = rand() % 10;
			}
			p = p*10 + ultimaCifraP;
			
			BigInteger ultimaCifraQ = rand() % 10;
			while(ultimaCifraQ%2 == 0){
				ultimaCifraQ = rand() % 10;
			}
			q = q*10 + ultimaCifraQ;
			
		}
	}
	cout << "Calculando clave pública..." << endl;
	p = siguientePrimo(p);
	q = siguientePrimo(q);
	
	BigInteger n = p*q;
	cout << "-----------------------------------" << endl;
	cout << "n:" << endl;
	cout << n << endl;
	cout << "-----------------------------------" << endl;
	cout << "e:" << endl;
	BigInteger e = 65537;
	while(euler(p,q)%e==0){
		e = siguientePrimo(e);
	}
	cout << e << endl;
	cout << "-----------------------------------" << endl;
	cout << "-----------------------------------" << endl;

	escribirClaves(n,e,"publica");

	cout << "Calculando clave privada..." << endl;
	BigInteger d = 0;
	BigInteger f = 0;
	xGCD(e,euler(p,q),d,f);
	cout << "------------------------------------"<< endl;
	while(d<0){
		d = d+euler(p,q); 
	}
	cout << "d:"<< endl;
	cout << d << endl;
	cout << "-----------------------------------" << endl;
	cout << "-----------------------------------" << endl;

	escribirClaves(n,d,"privada");

}

void mostrarMenu(){
	cout << "----------------------------" << endl;
	cout << "Seleccione una opción:" << endl;
	cout << "1 - Encriptar archivos" << endl;
	cout << "2 - Desencriptar archivos" << endl;
	cout << "3 - Generar nuevas claves RSA" << endl;
	cout << "4 - Ayuda" << endl;
	cout << "0 - Salir" << endl;
}

int main(){
	srand (time(NULL));
	int seleccion = 0;
	cout << endl;
	cout << "ENCRIPTADOR DE ARCHIVOS v0.9" << endl;
	mostrarMenu();
	cout << ">> ";
	cin >> seleccion;
	while(seleccion!=0){
		if(seleccion == 1){
			interaccionCifrar();	
		}else if(seleccion == 2){
			interaccionDescifrar();
		}else if(seleccion == 3){
			generarClaves();
		}else if(seleccion == 4){
			mostrarMenu();
		}else{
			cout << "Opción no válida" << endl;
		}
		
		cout << ">> ";
		cin >> seleccion;
	}
	cout << "Pulse cualquier tecla para terminar..."<< endl;;
	
	return 0;
}










