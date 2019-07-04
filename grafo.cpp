#include <iostream>
#include <cmath>
#include "grafo.hpp"

using namespace std;

Grafo::Grafo(int qtdElementos, double **coordenadas) {
    this->qtdElementos = qtdElementos;
    this->coordenadas = coordenadas;
    qtdArestas = (qtdElementos * (qtdElementos - 1)) / 2;

    arestasElementos = new int*[qtdArestas];
    for (int i = 0; i < qtdArestas; ++i) {
        arestasElementos[i] = new int[2];
    }
    arestasValor = new double[qtdArestas];

    int pos = 0;
    for (int i = 0; i < (qtdElementos - 1); ++i) {
        for (int j = i + 1; j < qtdElementos; ++j) {
            arestasElementos[pos][0] = i;
            arestasElementos[pos][1] = j;
            arestasValor[pos] = sqrt(pow((coordenadas[j][0] - coordenadas[i][0]), 2) + pow((coordenadas[j][1] - coordenadas[i][1]), 2));

            ++pos;
        }
    }

    matrizAdjacencia = new double*[qtdElementos];
    for (int lin = 0; lin < qtdElementos; ++lin) {
        matrizAdjacencia[lin] = new double[qtdElementos];
        matrizAdjacencia[lin][lin] = 0;
    }
    for (int i = 0; i < qtdArestas; ++i) {
        matrizAdjacencia[arestasElementos[i][0]][arestasElementos[i][1]] = arestasValor[i];
        matrizAdjacencia[arestasElementos[i][1]][arestasElementos[i][0]] = arestasValor[i];
    }
}

Grafo::~Grafo() {
    delete[] arestasElementos[0];
    delete[] arestasElementos[1];
    delete[] arestasElementos;

    delete[] arestasValor;

    for (int col = 0; col < qtdElementos; ++col) {
        delete[] matrizAdjacencia[col];
    }
    delete[] matrizAdjacencia;
}

int Grafo::getQtdElementos() {
    return qtdElementos;
}

int Grafo::getQtdArestas() {
    return qtdArestas;
}

int **Grafo::getArestasElementos() {
    return arestasElementos;
    }

double *Grafo::getArestasValor() {
    return arestasValor;
}

double **Grafo::getMatrizAdjacencia() {
    return matrizAdjacencia;
}
