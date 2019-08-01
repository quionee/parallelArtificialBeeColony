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
        void realizarSomatorio();
    public:
        Solucao(Grafo *grafo);
        ~Solucao();
        int getQtdElementos();
        std::vector<int> getSolucao();
        std::vector<double> getArestas();
        double getSomatorioTotal();
        void setSolucao(std::vector<int> solucao);
        void setArestas(std::vector<double> arestas);
        void setSomatorioTotal(double somatorioTotal);

        void buscaLocal();
        void imprimirSolucao();
};

#endif
