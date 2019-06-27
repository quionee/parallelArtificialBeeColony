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
    srand(time(NULL));
    string nomeArquivo = "teste.txt";
    int qtdSolucoesIniciais;
    cout << "\nQuantidade de solucoes iniciais: ";
    cin >> qtdSolucoesIniciais;

    Grafo *grafo = leArquivo(nomeArquivo);

    cout << "\n\n----- Solucoes iniciais: -----\n";
    vector<Solucao*> solucoes;
    for (int i = 0; i < qtdSolucoesIniciais; ++i) {
        cout << "\nSolucao: " << i;
        Solucao *solucao = new Solucao(grafo);
        solucoes.push_back(solucao);
    }

    cout << "\n\n----- Solucoes apos busca local: -----\n";
    double somatorioProbabilidade = 0;
    for (int i = 0; i < qtdSolucoesIniciais; ++i) {
        cout << "\nSolucao: " << i;
        solucoes[i]->buscaLocal();
        somatorioProbabilidade += solucoes[i]->getSomatorioTotal();
    }

    double limitante = 1.0 / qtdSolucoesIniciais;
    vector<int> paraExcluir;
    for (int i = 0; i < qtdSolucoesIniciais; ++i) {
        cout << "\nprobabilidade " << i << ": " << solucoes[i]->getSomatorioTotal() / somatorioProbabilidade << endl;
        if ((solucoes[i]->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
            paraExcluir.push_back(i);
        }
    }

    cout << "\n\nparaExcluir: ";
    for (unsigned int i = 0; i < paraExcluir.size(); i++) {
        cout << paraExcluir[i] << " ";
    }
    

    int posExcluir = paraExcluir.size() - 1;
    for (int i = qtdSolucoesIniciais - 1; (i >= 0) and (posExcluir >= 0); --i) {
        if (paraExcluir[posExcluir] == i) {
            solucoes.erase(solucoes.begin() + i);
             --posExcluir;
        }
    }

    for (unsigned int i = 0; i < solucoes.size(); ++i) {
        cout << "\n\nsolucoes finais: " << i << endl;
        solucoes[i]->imprimeSolucao();
    }
    
    
    return 0;
}
