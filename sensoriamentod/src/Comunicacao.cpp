/*
 * Comunicacao.cpp
 *
 *  Created on: 9 de dez de 2015
 *      Author: isaac
 */
#include "Comunicacao.h"

#ifdef __linux__
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#endif

Comunicacao::Comunicacao(char* porta){
	hPorta = 0;
	this->porta=porta;
}

int Comunicacao::iniciar(){
	int resultado = EXIT_SUCCESS;
#if _WIN32 || _WIN64
	hPorta = CreateFile(porta, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hPorta == INVALID_HANDLE_VALUE) {
		resultado = GetLastError();
	}
	else {
		//configuracao da porta
		DCB dcb;
		memset(&dcb, 0, sizeof(dcb));
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = CBR_9600;
		//Pesquisar bit paridade
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;
		dcb.ByteSize = 8;

		if(!SetCommState(hPorta, &dcb)) {
			resultado = GetLastError();
		}
	}
#endif

#ifdef __linux__
	hPorta = open(porta, O_RDWR | O_NOCTTY | O_NDELAY);
	if(hPorta == -1){
		resultado = errno;
	} else {
		fcntl(hPorta, F_SETFL, 0);
	}
#endif

	Sleep(2000);

	return resultado;
}

int Comunicacao::ler(char* buffer, long unsigned int bytesParaLer){
	int resultado = EXIT_FAILURE;
	long unsigned int bytesLidos = 0;
#if _WIN32 || _WIN64
	ReadFile(hPorta, buffer, bytesParaLer, &bytesLidos, NULL);//nem sempre ler a qnt de dados pedidos
	resultado = EXIT_SUCCESS;
#endif

#ifdef __linux__
	//Le os bytes que vem da porta serial hPorta e guarda no buffer
	// alem de receber a quantidade de bytes que sera lida(tamanho limite)
	bytesLidos = read(hPorta, (void*)buffer, bytesParaLer);
	if(bytesLidos == bytesParaLer){
		resultado = EXIT_SUCCESS;
	}
#endif
	return resultado;
}

int Comunicacao::finalizar(){
#if _WIN32 || _WIN64
	CloseHandle(hPorta);
#endif

#ifdef __linux__
	close(hPorta);
	//Fechar porta no linux
#endif
	return EXIT_SUCCESS;
}



