#include <iostream>
#include "solucao.hpp"

Solucao::Solucao(Grafo *grafo) {
    qtdElementos = grafo->getQtdElementos();
    for (int i = 0; i < qtdElementos; ++i) {
        elementosForaDaRota.push_back(i);
    }
    matrizAdjacencia = grafo->getMatrizAdjacencia();
    somatorioTotal = 0;
    solucaoInicial(grafo);
}

Solucao::~Solucao() {
    elementosForaDaRota.clear();
    solucao.clear();
    arestas.clear();
}

int Solucao::getQtdElementos() {
    return qtdElementos;
}

std::vector<int> Solucao::getSolucao() {
    return solucao;
}

std::vector<double> Solucao::getArestas() {
    return arestas;
}

double Solucao::getSomatorioTotal() {
    return somatorioTotal;
}

void Solucao::setSolucao(std::vector<int> solucao) {
    this->solucao = solucao;
}

void Solucao::setArestas(std::vector<double> arestas) {
    this->arestas = arestas;
}

void Solucao::setSomatorioTotal(double somatorioTotal) {
    this->somatorioTotal = somatorioTotal;
}

// A solução inicial utilizada foi a de Inserção Aleatória
void Solucao::solucaoInicial(Grafo *grafo) {
    int pos = rand() % qtdElementos;

    solucao.push_back(elementosForaDaRota[pos]);
    elementosForaDaRota.erase(elementosForaDaRota.begin() + pos);

    int posDisponiveis = qtdElementos - 1, posAresta = 0;
    while (posDisponiveis > 0) {
        int pos = rand() % posDisponiveis;

        solucao.push_back(elementosForaDaRota[pos]);
        elementosForaDaRota.erase(elementosForaDaRota.begin() + pos);

        --posDisponiveis;

        arestas.push_back(matrizAdjacencia[solucao[posAresta]][solucao[posAresta + 1]]);
        somatorioTotal += arestas[posAresta];
        ++posAresta;
    }
    arestas.push_back(matrizAdjacencia[solucao[posAresta]][solucao[0]]);
    somatorioTotal += arestas[posAresta];
}

void Solucao::buscaLocal() {
    BuscaLocal *buscaLocal = new BuscaLocal(solucao, arestas, matrizAdjacencia);
    buscaLocal->realizarBuscaLocal(solucao, arestas);

    realizarSomatorio();
}

void Solucao::realizarSomatorio() {
    somatorioTotal = 0;
    for (unsigned int i = 0; i < arestas.size(); ++i) {
        somatorioTotal += arestas[i];
    }
}

void Solucao::imprimirSolucao() {
    std::cout << "\n\nRota: ";
    for (unsigned int i = 0; i < solucao.size(); ++i) {
        std::cout << solucao[i] << " ";
    }
    somatorioTotal = 0;
    std::cout << "\n\nArestas: ";
    for (unsigned int i = 0; i < arestas.size(); ++i) {
        std::cout << arestas[i] << " ";
        somatorioTotal += arestas[i];
    }
    std::cout << "\n\nSomatorio total: " << somatorioTotal << "\n\n";
}
