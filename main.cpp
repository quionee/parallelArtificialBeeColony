#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mpi.h"
#include "solucao.hpp"

using namespace std;

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

void executa(Solucao *solucao, double limitante, Grafo *grafo, vector<int> &melhorRota,
             vector<double> &melhorArestas, double &melhorSomatorio) {
    bool exclui = false;
    int qtdIteracoes = 0;
    do {
        double probabilidade = 0, somatorioProbabilidade;
        if (!melhoraSolucao(solucao, probabilidade)) {
            exclui = true;
        }
        if (solucao->getSomatorioTotal() < melhorSomatorio) {
            melhorRota = solucao->getSolucao();
            melhorArestas = solucao->getArestas();
            melhorSomatorio = solucao->getSomatorioTotal();
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Reduce(&probabilidade, &somatorioProbabilidade, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Bcast(&somatorioProbabilidade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if ((solucao->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
            melhoraSolucao(solucao, probabilidade);
            if (solucao->getSomatorioTotal() < melhorSomatorio) {
                melhorRota = solucao->getSolucao();
                melhorArestas = solucao->getArestas();
                melhorSomatorio = solucao->getSomatorioTotal();
            }
        }

        if (exclui) {
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

    string nomeArquivo = "a280.txt";
    Grafo *grafo = leArquivo(nomeArquivo);

    Solucao *melhorSolucao = new Solucao(grafo);
    vector<int> melhorRota = melhorSolucao->getSolucao();
    vector<double> melhorArestas = melhorSolucao->getArestas();
    double melhorSomatorio = melhorSolucao->getSomatorioTotal();
    delete melhorSolucao;

    double melhorSomatorioGeral = 0;

    double limitante;

    // inicializa o MPI
    MPI::Init(argc, argv);

    int tamanho = MPI::COMM_WORLD.Get_size();
    int processo = MPI::COMM_WORLD.Get_rank();

    if (processo == 0) {
        limitante = 1.0 / tamanho;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&limitante, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < tamanho; ++i) {
        if (processo == i) {
            Solucao *solucao = new Solucao(grafo);
            executa(solucao, limitante, grafo, melhorRota, melhorArestas, melhorSomatorio);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&melhorSomatorio, &melhorSomatorioGeral, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Bcast(&melhorSomatorioGeral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (melhorSomatorio == melhorSomatorioGeral) {
        MPI_Bcast(&melhorRota, melhorRota.size(), MPI_INT, processo, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (processo == 0) {
        cout << "\n\nRota: ";
        for (unsigned int i = 0; i < melhorRota.size(); ++i) {
            cout << melhorRota[i] << " ";
        }
        cout << "\n\nSomatorio total: " << melhorSomatorioGeral << "\n\n";

        fim = time(NULL);
        cout << "Tempo de execução em segundos: " << difftime(fim, inicio) << endl;
    }
    
    MPI::Finalize();
    
    delete grafo;

    return 0;
}
