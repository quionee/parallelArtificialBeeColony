#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mpi.h"		// import da biblioteca mpi
#include "solucao.hpp"

using namespace std;

// Função gera o grafo com as intâncias do aquivo
Grafo *leArquivo(string nomeArquivo) {
    ifstream arquivo;
    arquivo.open(nomeArquivo.c_str());

    int qtdElementos;
    arquivo >> qtdElementos;
    int aux;
    double **coordenadas = new double*[qtdElementos];
    for (int i = 0; i < qtdElementos; ++i) {
        coordenadas[i] = new double[2];
        arquivo >> aux
                >> coordenadas[i][0]
                >> coordenadas[i][1];
    }
    
    arquivo.close();
    
    Grafo *grafo = new Grafo(qtdElementos, coordenadas);
    
    return grafo;
}

bool melhoraSolucao(Solucao* &solucao, double &probabilidade) {
    vector<int> melhorSolucao = solucao->getSolucao();
    vector<double> melhorArestas = solucao->getArestas();
    double melhorSomatorio = solucao->getSomatorioTotal();
    solucao->buscaLocal();
    // verifica se após a busca local a solução melhorou, e atualiza a solução, arestas e somatório
    if (melhorSomatorio <= solucao->getSomatorioTotal()) { 
        solucao->setSolucao(melhorSolucao);
        solucao->setArestas(melhorArestas);
        solucao->setSomatorioTotal(melhorSomatorio);
        probabilidade = solucao->getSomatorioTotal();
        return false;
    }

    probabilidade = solucao->getSomatorioTotal();
    return true;
}
// Função que executa os ciclos
void executa(Solucao *solucao, double limitante, Grafo *grafo, vector<int> &melhorRota,
             vector<double> &melhorArestas, double &melhorSomatorio) {
    bool exclui = false; // booleano para verificar se a solução será excluida
    int qtdIteracoes = 0;
    do {
        double probabilidade = 0, somatorioProbabilidade;
        if (!melhoraSolucao(solucao, probabilidade)) { // Verifica se a solução foi melhorada
            exclui = true;
        }
        // verifica se a solução tem o melhor somatório, e atualiza a solução, arestas e somatório
        if (solucao->getSomatorioTotal() < melhorSomatorio) {
            melhorRota = solucao->getSolucao();
            melhorArestas = solucao->getArestas();
            melhorSomatorio = solucao->getSomatorioTotal();
        }

        MPI_Barrier(MPI_COMM_WORLD);// Bloqueia ate que todos os processos estejam neste trecho do código
        MPI_Reduce(&probabilidade, &somatorioProbabilidade, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);// Faz a soma da probabilidade de cada processo em somatorioProbabilidade
        MPI_Bcast(&somatorioProbabilidade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);// Faz o broadcast de somatorioProbabilidade para todos os processos

        //Tenta melhorar a solução se o cálculo da probabilidade é maior que  o limitante
        if ((solucao->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
            melhoraSolucao(solucao, probabilidade);
            // verifica se a solução tem o melhor somatório, e atualiza a solução, arestas e somatório
            if (solucao->getSomatorioTotal() < melhorSomatorio) {
                melhorRota = solucao->getSolucao();
                melhorArestas = solucao->getArestas();
                melhorSomatorio = solucao->getSomatorioTotal();
            }
        }

        if (exclui) { // se exclui == true a fonte (solucao) se esgotou 
            Solucao *solucaoAux;
            solucaoAux = solucao;
            delete solucaoAux;	
            solucao = new Solucao(grafo);
        }
        ++qtdIteracoes;
    } while (qtdIteracoes < 5);
}


int main(int argc, char **argv) {
    time_t inicio = time(NULL);
    time_t fim;
    srand(time(NULL));

    string nomeArquivo = "a280.txt";		// Entrada do arquivo com as instâncias
    Grafo *grafo = leArquivo(nomeArquivo); // Gera o grafo com as instancias do aquivo

    Solucao *melhorSolucao = new Solucao(grafo);
    vector<int> melhorRota = melhorSolucao->getSolucao();
    vector<double> melhorArestas = melhorSolucao->getArestas();
    double melhorSomatorio = melhorSolucao->getSomatorioTotal();
    delete melhorSolucao;

    double melhorSomatorioGeral = 0;

    double limitante;

    // inicializa o MPI
    MPI::Init(argc, argv);

    int tamanho = MPI::COMM_WORLD.Get_size(); // Quantidade de processos criados
    int processo = MPI::COMM_WORLD.Get_rank(); // rank (id) do processo

    if (processo == 0) { // Verifica se é o primeiro processo
        limitante = 1.0 / tamanho;	// Calcula o limitante 
    }

    MPI_Barrier(MPI_COMM_WORLD); // Bloqueia ate que todos os processos estejam neste trecho do código
    MPI_Bcast(&limitante, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);// Faz o broadcast do limitante para todos os processos

    for (int i = 0; i < tamanho; ++i) { // Gera uma fonte (solucao) para cada processo
        if (processo == i) {
            Solucao *solucao = new Solucao(grafo); 
            executa(solucao, limitante, grafo, melhorRota, melhorArestas, melhorSomatorio);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);// Bloqueia ate que todos os processos estejam neste trecho do código
    // melhorSomatorioGeral recebe o menor dentre o melhorSomatorio de todos os processos
    MPI_Reduce(&melhorSomatorio, &melhorSomatorioGeral, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Bcast(&melhorSomatorioGeral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);// Faz o broadcast do melhorSomatorioGeral para todos os processos

    if (melhorSomatorio == melhorSomatorioGeral) { // Faz broadcast da melhorRota do processo que tem a melhor solução
        MPI_Bcast(&melhorRota, melhorRota.size(), MPI_INT, processo, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD); // Bloqueia ate que todos os processos estejam neste trecho do código
    if (processo == 0) {	// Verifica se é o primeiro processo
        cout << "\n\nRota: ";
        for (unsigned int i = 0; i < melhorRota.size(); ++i) {
            cout << melhorRota[i] << " ";
        }
        cout << "\n\nSomatorio total: " << melhorSomatorioGeral << "\n\n";

        fim = time(NULL);
        cout << "Tempo de execução em segundos: " << difftime(fim, inicio) << endl;
    }
    
    MPI::Finalize();// Encerra o mpi
    
    delete grafo;

    return 0;
}
