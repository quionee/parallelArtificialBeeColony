#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mpi.h"        // import da biblioteca mpi
#include "solucao.hpp"

using namespace std;

// função que gera o grafo para a instância do arquivo
Grafo *lerArquivo(string nomeArquivo) {
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

bool melhoraSolucao(Solucao* &solucao, double &probabilidade) {
    vector<int> melhorSolucao = solucao->getSolucao();
    vector<double> melhorArestas = solucao->getArestas();
    double melhorSomatorio = solucao->getSomatorioTotal();
    solucao->buscaLocal();

    // verifica se após a busca local a solução não melhorou,
    // caso não tenha, retorna à solução anterior
    if (melhorSomatorio <= solucao->getSomatorioTotal()) { 
        solucao->setSolucao(melhorSolucao);
        solucao->setArestas(melhorArestas);
        solucao->setSomatorioTotal(melhorSomatorio);
        probabilidade = solucao->getSomatorioTotal();
        return false;
    }

    probabilidade = solucao->getSomatorioTotal();
    return true;
}

// função que executa os ciclos da colônia de abelha
void executa(Solucao *solucao, double limitante, Grafo *grafo, vector<int> &melhorRota,
             vector<double> &melhorArestas, double &melhorSomatorio) {
    bool exclui = false; // booleano para verificar se a solução será excluida
    int qtdIteracoes = 0; // quantidade de soluções para limitar a quantidade de ciclos
    do {
        double probabilidade = 0, somatorioProbabilidade;

        // tenta melhorar a solução
        if (!melhoraSolucao(solucao, probabilidade)) { 
            // caso não tenha tido melhora, a solução pode ser excluída
            exclui = true;
        }

        // verifica se a solução atual é melhor que a melhor conhecida, se for,
        // a melhor solução conhecida recebe a solução atual
        if (solucao->getSomatorioTotal() < melhorSomatorio) {
            melhorRota = solucao->getSolucao();
            melhorArestas = solucao->getArestas();
            melhorSomatorio = solucao->getSomatorioTotal();
        }

        // bloqueia até que todos os processos estejam neste trecho do código
        MPI_Barrier(MPI_COMM_WORLD);
        // faz a soma da probabilidade de cada processo em somatorioProbabilidade
        MPI_Reduce(&probabilidade, &somatorioProbabilidade, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        // faz o broadcast de somatorioProbabilidade para todos os processos
        MPI_Bcast(&somatorioProbabilidade, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // tenta melhorar a solução se o cálculo da probabilidade é maior que o limitante
        if ((solucao->getSomatorioTotal() / somatorioProbabilidade) > limitante) {
            melhoraSolucao(solucao, probabilidade);
            // verifica se a solução atual é melhor que a melhor conhecida, se for,
            // a melhor solução conhecida recebe a solução atual
            if (solucao->getSomatorioTotal() < melhorSomatorio) {
                melhorRota = solucao->getSolucao();
                melhorArestas = solucao->getArestas();
                melhorSomatorio = solucao->getSomatorioTotal();
            }
        }
        
        // se exclui == true, a solução não consegue mais melhorar, e é reiniciada
        if (exclui) {
            Solucao *solucaoAux;
            solucaoAux = solucao;
            delete solucaoAux;	
            solucao = new Solucao(grafo);
        }
        ++qtdIteracoes;
    } while (qtdIteracoes < 5);
}


int main(int argc, char **argv) {
    time_t inicio = clock();
    time_t fim;
    srand(time(NULL));

    string pastaInstancias = "instancias/";
    Grafo *grafo = lerArquivo(pastaInstancias + argv[1]);  // gera o grafo com a instância do aquivo

    Solucao *melhorSolucao = new Solucao(grafo);
    vector<int> melhorRota = melhorSolucao->getSolucao();
    vector<double> melhorArestas = melhorSolucao->getArestas();
    double melhorSomatorio = melhorSolucao->getSomatorioTotal();
    delete melhorSolucao;

    double melhorSomatorioGeral = 0;

    double limitante;

    // inicializa o MPI
    MPI::Init(argc, argv);

    int tamanho = MPI::COMM_WORLD.Get_size();  // quantidade de processos criados
    int processo = MPI::COMM_WORLD.Get_rank(); // rank (id) do processo

    if (processo == 0) { // verifica se é o primeiro processo
        limitante = 1.0 / tamanho;	// calcula o limitante 
    }

    MPI_Barrier(MPI_COMM_WORLD); // bloqueia até que todos os processos estejam neste trecho do código
    MPI_Bcast(&limitante, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);// faz o broadcast do limitante para todos os processos

    for (int i = 0; i < tamanho; ++i) { // gera uma solucao inicial para cada processo
        if (processo == i) {
            Solucao *solucao = new Solucao(grafo);
            executa(solucao, limitante, grafo, melhorRota, melhorArestas, melhorSomatorio);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD); // bloqueia até que todos os processos estejam neste trecho do código
   
    // melhorSomatorioGeral recebe o menor dentre o melhorSomatorio de todos os processos
    MPI_Reduce(&melhorSomatorio, &melhorSomatorioGeral, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Bcast(&melhorSomatorioGeral, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD); // faz o broadcast do melhorSomatorioGeral para todos os processos

    if (melhorSomatorio == melhorSomatorioGeral) { // faz broadcast da melhorRota do processo que tem a melhor solução
        MPI_Bcast(&melhorRota, melhorRota.size(), MPI_INT, processo, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD); // bloqueia ate que todos os processos estejam neste trecho do código
    if (processo == 0) {    // verifica se é o primeiro processo
        cout << "\n\nRota: ";
        for (unsigned int i = 0; i < melhorRota.size(); ++i) {
            cout << melhorRota[i] << " ";
        }
        cout << "\n\nSomatorio total: " << melhorSomatorioGeral << "\n\n";

        fim = clock();
        cout << "Tempo de execução em segundos: " << ((float)(fim-inicio)/CLOCKS_PER_SEC) << endl;
    }
    
    MPI::Finalize(); // encerra o mpi
    
    delete grafo;

    return 0;
}
