/*
 * Comunicacao.h
 *
 *  Created on: 8 de out de 2015
 *      Author: alunoifba
 */

#ifndef COMUNICACAO_H_
#define COMUNICACAO_H_

#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#define Sleep(x) usleep(x * 1000); //Macro para usar Sleep como usleep no linux
#endif

#if _WIN32 || _WIN64
#include <windows.h>
#endif

class Comunicacao{
private:
	char* porta;

#ifdef __linux__
	int hPorta;
#endif

#if _WIN32 || _WIN64
	HANDLE hPorta;
#endif

public:
	Comunicacao(char* porta);
	//Iniciar a comunicacao com a porta serial, passivel de erro
	//Retorna 0 se funcionar ok e retorna outro inteiro se nao funcionar(codigo do erro)
	int iniciar();

	//Realiza a leitura de um buffer a partir da porta
	int ler(char* buffer, long unsigned int bytesParaLer);

	//Finaliza o uso da porta serial
	int finalizar();
};

#endif //COMUNICACAO_H_
