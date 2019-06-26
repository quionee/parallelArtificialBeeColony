#include <iostream>
#include <fstream>
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

int main() {
    string nomeArquivo = "teste.txt";
    Grafo *grafo = leArquivo(nomeArquivo);

    Solucao *solucao = new Solucao(grafo);
    solucao->buscaLocal();

    return 0;
}
