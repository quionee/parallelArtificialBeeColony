#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
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

bool melhoraSolucao(Solucao* &solucao, double &somatorioProbabilidade) {
    vector<int> melhorSolucao = solucao->getSolucao();
    vector<double> melhorArestas = solucao->getArestas();
    double melhorSomatorio = solucao->getSomatorioTotal();
    solucao->buscaLocal();

    if (melhorSomatorio <= solucao->getSomatorioTotal()) {
        solucao->setSolucao(melhorSolucao);
        solucao->setArestas(melhorArestas);
        solucao->setSomatorioTotal(melhorSomatorio);
        somatorioProbabilidade += solucao->getSomatorioTotal();
        return false;
    }

    somatorioProbabilidade += solucao->getSomatorioTotal();
    return true;
}

void executa(vector<Solucao*> &solucoes, int &qtdSolucoesIniciais, Grafo *grafo, vector<int> &melhorSolucao, vector<double> &melhorArestas, double &melhorSomatorio) {
    vector<int> paraExcluir;
    time_t tempo = 1;
    time_t inicio = time(NULL);
    time_t fim;
    do {
        double somatorioProbabilidade = 0;
        for (int i = 0; i < qtdSolucoesIniciais; ++i) {
            if (!melhoraSolucao(solucoes[i], somatorioProbabilidade)) {
                paraExcluir.push_back(i);
            }
            if (solucoes[i]->getSomatorioTotal() < melhorSomatorio) {
                melhorSolucao = solucoes[i]->getSolucao();
                melhorArestas = solucoes[i]->getArestas();
                melhorSomatorio = solucoes[i]->getSomatorioTotal();
            }
        }
        double aux = 0;
        double limitante = 1.0 / qtdSolucoesIniciais;
        for (int i = 0; i < qtdSolucoesIniciais; ++i) {
            if ((solucoes[i]->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
                melhoraSolucao(solucoes[i], aux);
                if (solucoes[i]->getSomatorioTotal() < melhorSomatorio) {
                    melhorSolucao = solucoes[i]->getSolucao();
                    melhorArestas = solucoes[i]->getArestas();
                    melhorSomatorio = solucoes[i]->getSomatorioTotal();
                }
            }
        }

        if (paraExcluir.size() > 0) {
            int posExcluir = paraExcluir.size() - 1;
            for (int i = qtdSolucoesIniciais - 1; (i >= 0) and (posExcluir >= 0); --i) {
                if (paraExcluir[posExcluir] == i) {
                    solucoes.erase(solucoes.begin() + i);
                    --posExcluir;
                }
            }

            for (unsigned int i = 0; i < paraExcluir.size(); ++i) {
                Solucao *solucao = new Solucao(grafo);
                solucoes.push_back(solucao);
            }
            paraExcluir.clear();
        }
        fim = time(NULL);
    } while (difftime(fim, inicio) < tempo);
}

int main() {
    srand(time(NULL));
    
    string nomeArquivo = "a280.txt";
    int qtdSolucoesIniciais;
    cout << "\nQuantidade de solucoes iniciais: ";
    cin >> qtdSolucoesIniciais;

    Grafo *grafo = leArquivo(nomeArquivo);

    vector<Solucao*> solucoes;
    for (int i = 0; i < qtdSolucoesIniciais; ++i) {
        Solucao *solucao = new Solucao(grafo);
        solucoes.push_back(solucao);
    }

    vector<int> melhorSolucao = solucoes[0]->getSolucao();
    vector<double> melhorArestas = solucoes[0]->getArestas();
    double melhorSomatorio = solucoes[0]->getSomatorioTotal();

    executa(solucoes, qtdSolucoesIniciais, grafo, melhorSolucao, melhorArestas, melhorSomatorio);

    cout << "\n\nRota: ";
    for (unsigned int i = 0; i < melhorSolucao.size(); ++i) {
        cout << melhorSolucao[i] << " ";
    }
    cout << "\n\nArestas: ";
    for (unsigned int i = 0; i < melhorArestas.size(); ++i) {
        std::cout << melhorArestas[i] << " ";
    }
    cout << "\n\nSomatorio total: " << melhorSomatorio << "\n\n";
    
    return 0;
}
