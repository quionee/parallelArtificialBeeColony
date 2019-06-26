#ifndef SOLUCAO_HPP
#define SOLUCAO_HPP

#include <vector>
#include "grafo.hpp"
#include "buscaLocal.hpp"

class Solucao {
    private:
        int qtdElementos;
        double **matrizAdjacencia;
        std::vector<int> elementosForaDaRota;
        std::vector<int> solucao;
        std::vector<double> arestas;
        double somatorioTotal;
        
        void solucaoInicial(Grafo *grafo);
    public:
        Solucao(Grafo *grafo);
        ~Solucao();
        void buscaLocal();
        void imprimeSolucao();
};

#endif
