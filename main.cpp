#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mpi.h"
#include "solucao.hpp"

using namespace std;

Grafo *leArquivo(string nomeArquivo);

string nomeArquivo = "dsj1000.txt";
Grafo *grafo = leArquivo(nomeArquivo);
Solucao *melhorSolucao = new Solucao(grafo);
double limitante;

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

void executa(Solucao *solucao) {
    // cout << "\n\nExecutando\n\n";
    bool exclui = false;
    time_t tempo = 1;
    time_t inicio = time(NULL);
    time_t fim;
    do {
        double probabilidade = 0, somatorioProbabilidade;
        if (!melhoraSolucao(solucao, probabilidade)) {
            exclui = true;
        }
        if (solucao->getSomatorioTotal() < melhorSolucao->getSomatorioTotal()) {
            melhorSolucao->setSolucao(solucao->getSolucao());
            melhorSolucao->setArestas(solucao->getArestas());
            melhorSolucao->setSomatorioTotal(solucao->getSomatorioTotal());
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Reduce(&probabilidade, &somatorioProbabilidade, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Bcast(&somatorioProbabilidade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if ((solucao->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
            melhoraSolucao(solucao, probabilidade);
            if (solucao->getSomatorioTotal() < melhorSolucao->getSomatorioTotal()) {
                melhorSolucao->setSolucao(solucao->getSolucao());
                melhorSolucao->setArestas(solucao->getArestas());
                melhorSolucao->setSomatorioTotal(solucao->getSomatorioTotal());
            }
        }

        if (exclui) {
            Solucao *solucaoAux;
            solucaoAux = solucao;
            delete solucaoAux;
            solucao = new Solucao(grafo);
        }
        fim = time(NULL);
    } while (difftime(fim, inicio) < tempo);
}

int main(int argc, char **argv) {
    srand(time(NULL));

    // inicializa o MPI
    MPI::Init(argc, argv);

    int tamanho = MPI::COMM_WORLD.Get_size();
    int processo = MPI::COMM_WORLD.Get_rank();
    // int intervalo = qtdSolucoesIniciais;

    int qtdSolucoesIniciais;
    if (processo == 0) {
        cout << "\nQuantidade de solucoes iniciais: ";
        cin >> qtdSolucoesIniciais;
        limitante = 1.0 / qtdSolucoesIniciais;
        // cout << "\n\ngrafo: " << grafo->getQtdElementos() << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&qtdSolucoesIniciais, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // cout << "\n\ntamanho: " << tamanho
    //      << "\n\nprocesso: " << processo;

    // if (processo == 2) {
    //     cout << "\n\n\nQTJSAF: " << qtdSolucoesIniciais << endl;
    //     cout << "\n\ngrafo2222: " << grafo->getQtdElementos() << endl;
    // }

    for (int i = 0; i < tamanho; ++i) {
        if (processo == i) {
            Solucao *solucao = new Solucao(grafo);
            cout << "\n\ni " << i << ": " << solucao << endl;
            executa(solucao);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    if (processo == 0) {
        melhorSolucao->imprimeSolucao();
    }
    MPI::Finalize();
    return 0;
}
