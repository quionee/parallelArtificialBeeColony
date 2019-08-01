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
        
        int getQtdElementos();
        int getQtdArestas();
        int **getArestasElementos();
        double *getArestasValor();
        bool *getInseridos();
        std::vector<int> getElementosSemGrupo();
        double **getMatrizAdjacencia();
};

#endif
