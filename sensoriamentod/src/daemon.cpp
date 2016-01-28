/*
 * daemon.cpp
 *
 * Leitor de sensores que executa em background
 * no LINUX. Pode ser iniciado e parada a partir
 * da linha de comando.
 *
 *  Created on: 7 de jan de 2016
 *      Author: Luis Paulo
 */
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include "Comunicacao.h"
using namespace std;

#define ARQUIVO_LOG 	"/home/isaac/ifba/temp/sensoriamento.log"
#define ARQUIVO_PIPE 	"/home/isaac/ifba/temp/sensoriamentop"

bool continuar = true;
int sensores = 0;
Comunicacao com = NULL;

int daemonize() {
	int resultado = EXIT_SUCCESS;

	pid_t daemon_id = 0;
	pid_t sid = 0;
	//PASSO 1: Criar um processo clonado a partir do main
	daemon_id = fork();
	if (daemon_id < 0) {
		//se falhar, o id do processo é menor que 0
		cout << "Falha de criação do daemon!" << endl;
		//não faz nada, desiste da execução
		resultado = EXIT_FAILURE;
	} else {
		//se o dameon for criado, finaliza o processo original
		//o teste condicional abaixo garante que somente será finalizado o processo original
		if (daemon_id > 0) {
			cout << "Daemon criado! Saindo do processo principal" << endl;
			exit(0);
		}

		umask(0);
		sid = setsid();
		if (sid < 0) {
			exit(1);
		}
		//desliga os recursos de entrada e saída de dados
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}
	return resultado;
}

bool pipeIniciado() {
	//cria o arquivo de PIPE
	//666 permissao de leitura/escrita no arquivo
	int resultado = mkfifo(ARQUIVO_PIPE, 666);

	return (resultado != -1);
}

int lerSensores() {
	int sensores = -1;
	char ci = 0, cf = 0;
	//ler caractere inicial
	if ((com.ler((char*) &ci, sizeof(ci)) == EXIT_SUCCESS) && (ci == 'I')) {
		int is = 0;//ler buffer de dados
		if (com.ler((char*) &is, sizeof(is)) == EXIT_SUCCESS) {
			//ler carctere final
			if ((com.ler((char*) &cf, sizeof(cf)) == EXIT_SUCCESS) && (cf == 'F') ) {
				sensores = is;
			}
		}
	}
	return sensores;
}

int enviarParaPipe(int sensores) {
	int bytes = 0;
	//abrir o PIPE(abrir o arquivo)
	int apipe = open(ARQUIVO_PIPE, O_RDWR);
	if (apipe != -1) {
		//converter sensores inteiro para string
		//a integração funciona melhor com strings
		stringstream ss;
		ss << sensores;
		string strSensores = ss.str();
		//escrever a string de sensores no PIPE
		bytes = write(apipe, strSensores.c_str(), strSensores.length());
	}
	//fechar o PIPE
	close(apipe);

	return bytes;
}

void tratarSinal(int sinal) {
	//tratamentoo para os sinais de parada do daemon
	//o dameon deve parar
	switch (sinal) {
	case SIGHUP:
		continuar = false;
		break;
	case SIGTERM:
		continuar = false;
		break;
	case SIGQUIT:
		continuar = false;
		break;
	default:
		break;
	}
}

void iniciarTratamentoSinais() {
	//mapeamento entre sinal e funcao de tratamento de sinais
	signal(SIGHUP, tratarSinal);
	signal(SIGTERM, tratarSinal);
	signal(SIGINT, tratarSinal);
	signal(SIGQUIT, tratarSinal);
}

int main(int argc, char* argv[]) {
	//se for possivel criar o daemon
	if (daemonize() == EXIT_SUCCESS) {
		//inicia o tratamento de sinais
		iniciarTratamentoSinais();

		//e o log para registro de operacoes
		ofstream log(ARQUIVO_LOG, ios_base::out | ios_base::app);
		log << "Arquivo de log iniciado!" << endl;

		//comunicacao com o arduino
		com = Comunicacao ((char*)"/dev/ttyUSB0");

		if(com.iniciar() == EXIT_FAILURE){
			log << "Falha de inicializacao com a porta USB.";
			exit(1);
		}

		//PIPE
		log << "Pipe sendo criando..." << endl;
		if (!pipeIniciado()) {
			log << "Falha de inicializacao do PIPE" << endl;

			exit(2);
		}

		//executa em loop interrompivel
		continuar = true;
		while (continuar) {
			// a leitura dos sensores
			int sensores = lerSensores();
			log << "Enviando sensores: " << sensores << endl;
			//envio para o pipe
			enviarParaPipe(sensores);

			sleep(1);
		}

		//remove o arquivo de PIPE apos o uso/finalizacao
		unlink(ARQUIVO_PIPE);

		//finaliza a comunicacao com o arduino
		com.finalizar();

		log << "Daemon de sensoriamento finalizado!" << endl;
	}

	return 0;
}
