#include <iostream>
#include "buscaLocal.hpp"

BuscaLocal::BuscaLocal(std::vector<int> &solucao, std::vector<double> &arestas, double **matrizAdjacencia) {
    this->solucao = solucao;
    this->arestas = arestas;
    this->matrizAdjacencia = matrizAdjacencia;
}

BuscaLocal::~BuscaLocal() {
    
}

void BuscaLocal::realizarBuscaLocal(std::vector<int> &solucao, std::vector<double> &arestas) {
    optMove();
    swapVertice();

    solucao = this->solucao;
    arestas = this->arestas;
}

// retira duas arestas não adjacentes, e acrescenta duas arestas ligando os extremos dos vértices
void BuscaLocal::optMove() {
    for (unsigned int i = 0; i < (arestas.size() - 2); ++i) {
        for (unsigned int j = i + 2; j < (arestas.size() - 1); ++j) {
            if ((matrizAdjacencia[solucao[i]][solucao[j]] + matrizAdjacencia[solucao[i + 1]][solucao[j + 1]]) < (arestas[i] + arestas[j])) {
                optMoveSwap(i, j);
            }
        }
        if ((matrizAdjacencia[solucao[i]][solucao[arestas.size() - 1]] + matrizAdjacencia[solucao[i + 1]][solucao[0]]) < (arestas[i] + arestas[arestas.size() - 1])) {
            optMoveSwap(i, arestas.size() - 1);
        }
    }
}

void BuscaLocal::optMoveSwap(int inicio, int fim) {
    int auxPos = 0;
    for (int i = inicio + 1; i <= inicio + ((fim - inicio) / 2); ++i) {
        int aux = solucao[i];
        solucao[i] = solucao[fim - auxPos];
        solucao[fim - auxPos] = aux;
        ++auxPos;
    }
    if (fim == (int(solucao.size()) - 1)) {
        int i = 0;
        for (i = inicio; i < fim; ++i) {
            arestas[i] = matrizAdjacencia[solucao[i]][solucao[i + 1]];
        }
        arestas[arestas.size() - 1] = matrizAdjacencia[solucao[i]][solucao[0]];
    }
    else {
        for (int i = inicio; i <= fim; ++i) {
            arestas[i] = matrizAdjacencia[solucao[i]][solucao[i + 1]];
        }
    }
}

// troca dois vértices de posição
void BuscaLocal::swapVertice() {
    // para tratar a posição 0
    double rotaAtual = arestas[0] + arestas[arestas.size() - 1] + arestas[1];
    double rotaPossivel = matrizAdjacencia[solucao[0]][solucao[2]] + matrizAdjacencia[1][solucao[solucao.size() - 1]]
                        + matrizAdjacencia[solucao[0]][solucao[1]];
    if (rotaPossivel < rotaAtual) {
        swapVerticeAux(0, 1);
    }
    for (unsigned int i = 2; i < (solucao.size() - 1); ++i) {
        rotaAtual = arestas[0] + arestas[arestas.size() - 1] + arestas[i - 1] + arestas[i];
        rotaPossivel = matrizAdjacencia[solucao[0]][solucao[i - 1]] + matrizAdjacencia[solucao[0]][solucao[i + 1]]
                     + matrizAdjacencia[solucao[i]][solucao[1]] + matrizAdjacencia[i][solucao[solucao.size() - 1]];
        if (rotaPossivel < rotaAtual) {
            swapVerticeAux(0, i);
        }
    }
    rotaAtual = arestas[0] + arestas[solucao[solucao.size() - 1]];
    rotaPossivel = matrizAdjacencia[solucao[0]][solucao[solucao.size() - 1]]
                 + matrizAdjacencia[solucao[solucao.size() - 1]][solucao[1]];
    if (rotaPossivel < rotaAtual) {
        int aux = solucao[0];
        solucao[0] = solucao[solucao.size() - 1];
        solucao[solucao.size() - 1] = aux;

        arestas[0] = matrizAdjacencia[solucao[0]][solucao[1]];
    }

    // para tratar posições fora dos extremos
    for (unsigned int i = 1; i < (solucao.size() - 2); ++i) {
        // para quando j == i + 1
        rotaAtual = arestas[i] + arestas[i + 1] + arestas[i - 1];
        rotaPossivel = matrizAdjacencia[solucao[i - 1]][solucao[i + 1]] + matrizAdjacencia[solucao[i]][solucao[i + 1]]
                     + matrizAdjacencia[solucao[i]][solucao[i + 2]];
        if (rotaPossivel < rotaAtual) {
            swapVerticeAux1(i, i + 1);
        }

        // para quando j > i + 1
        for (unsigned int j = i + 2; j < (solucao.size() - 1); ++j) {
            rotaAtual = arestas[i] + arestas[i - 1] + arestas[j] + arestas[j - 1];
            rotaPossivel = matrizAdjacencia[solucao[i - 1]][solucao[j]] + matrizAdjacencia[solucao[j]][solucao[i + 1]]
                         + matrizAdjacencia[solucao[j - 1]][solucao[i]] + matrizAdjacencia[i][solucao[j + 1]];
            if (rotaPossivel < rotaAtual) {
                swapVerticeAux1(i, j);
            }
        }

        // para quando j == solucao.size() - 1
        rotaAtual = arestas[i] + arestas[i - 1] + arestas[solucao[solucao.size() - 1]] + arestas[solucao[solucao.size() - 2]];
        rotaPossivel = matrizAdjacencia[solucao[i - 1]][solucao[solucao.size() - 1]] + matrizAdjacencia[solucao[solucao.size() - 1]][solucao[i + 1]]
                     + matrizAdjacencia[solucao[solucao.size() - 2]][solucao[i]] + matrizAdjacencia[solucao[i]][solucao[0]];

        if (rotaPossivel < rotaAtual) {
            swapVerticeAux2(i, solucao.size() - 1);
        }
    }
}

// método auxiliar utilizado para trocar 2 vértices de posição
void BuscaLocal::swapVertices(int pos1, int pos2) {
    int aux = solucao[pos1];
    solucao[pos1] = solucao[pos2];
    solucao[pos2] = aux;
}

// método auxiliar utilizado para tratar a posição 0 durante swap
void BuscaLocal::swapVerticeAux(int pos1, int pos2) {
    swapVertices(pos1, pos2);
    arestas[pos1] = matrizAdjacencia[solucao[pos1]][solucao[pos1 + 1]];
    arestas[pos2] = matrizAdjacencia[solucao[pos2]][solucao[pos2 + 1]];
    arestas[pos2 - 1] = matrizAdjacencia[solucao[pos2 - 1]][solucao[pos2]];
    arestas[arestas.size() - 1] = matrizAdjacencia[solucao[pos1]][solucao[solucao.size() - 1]];
}

// método auxiliar utilizado para realizar o swap com pos fora dos extremos
void BuscaLocal::swapVerticeAux1(int pos1, int pos2) {
    swapVertices(pos1, pos2);
    arestas[pos1] = matrizAdjacencia[solucao[pos1]][solucao[pos1 + 1]];
    arestas[pos2] = matrizAdjacencia[solucao[pos2]][solucao[pos2 + 1]];
    arestas[pos1 - 1] = matrizAdjacencia[solucao[pos1 - 1]][solucao[pos1]];
    arestas[pos2 - 1] = matrizAdjacencia[solucao[pos2 - 1]][solucao[pos2]];
}

// método auxiliar utilizado para realizar o swap com ultima posição
void BuscaLocal::swapVerticeAux2(int pos1, int pos2) {
    swapVertices(pos1, pos2);
    arestas[pos1] = matrizAdjacencia[solucao[pos1]][solucao[pos1 + 1]];
    arestas[pos2] = matrizAdjacencia[solucao[pos2]][solucao[0]];
    arestas[pos1 - 1] = matrizAdjacencia[solucao[pos1 - 1]][solucao[pos1]];
    arestas[pos2 - 1] = matrizAdjacencia[solucao[pos2 - 1]][solucao[pos2]];
}
