#ifndef BUSCALOCAL_HPP
#define BUSCALOCAL_HPP

#include <vector>

class BuscaLocal {
    private:
        std::vector<int> solucao;
        std::vector<double> arestas;
        double **matrizAdjacencia;

        void optMove();
        void optMoveSwap(int inicio, int fim);
        void swapVertice();
        void swapVertices(int pos1, int pos2);
        void swapVerticeAux(int pos1, int pos2);
        void swapVerticeAux1(int pos1, int pos2);
        void swapVerticeAux2(int pos1, int pos2);
    public:
        BuscaLocal(std::vector<int> &solucao, std::vector<double> &arestas, double **matrizAdjacencia);
        ~BuscaLocal();
        void realizarBuscaLocal(std::vector<int> &solucao, std::vector<double> &arestas);
};

#endif
