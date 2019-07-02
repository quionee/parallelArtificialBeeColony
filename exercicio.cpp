#include <iostream>
#include <stdio.h>
#include "mpi.h"

using namespace std;

int main(int argc, char **argv) {
    int tamanho, id, rc;
    
    // inicializa o MPI
    MPI_Init(&argc, &argv);
    // define um identificador para cada processo
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    cout << "hello, world, from process" << id << endl;
    
    // finaliza o MPI
    MPI_Finalize();
    
    return 0;
}