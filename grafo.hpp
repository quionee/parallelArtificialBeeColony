#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>

class Grafo {
    private:
        int qtdElementos;
        double **coordenadas;
        int qtdArestas;
        int **arestasElementos;
        double *arestasValor;
        double **matrizAdjacencia;
    public:
        Grafo(int qtdElementos, double **coordenadas);
        ~Grafo();
        
        //~ // Desabilita Construtor de Cópia gerado pelo compilador
        //~ Grafo(const Grafo &) = delete;

        //~ // Desabilita Operador de Atribuição gerado pelo compilador
        //~ Grafo &operator=(const Grafo &) = delete;
        
        int getQtdElementos();
        int getQtdArestas();
        int **getArestasElementos();
        double *getArestasValor();
        bool *getInseridos();
        std::vector<int> getElementosSemGrupo();
        double **getMatrizAdjacencia();
};

#endif
