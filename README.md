### Colônia Artificial de Abelhas para resolver o TSP

O Problema do Caixeiro Viajante (Traveling Salesman Problem - TSP) consiste em, dado um conjunto de *n* cidades com distâncias entre cada par de cidades, sair de um ponto inicial, encontrar o menor caminho para percorrer todas as cidades passando por cada uma apenas uma vez, e retornar para o ponto inicial ao final. A ideia do algoritmo Colônia de Abelhas consiste em gerar fontes de comida, onde cada fonte representa uma solução para o Problema do Caixeiro Viajante, e a partir disso tentar melhorar essa solução por meio de buscas locais. Sempre que uma fonte de comida é esgotada, ou seja, não é mais possível melhorar aquela solução, ela é substituída por uma nova fonte de comida (nova solução). Cada solução encontrada é verificada com a melhor solução já encontrada até então, dessa forma garantimos que a solução final do problema será a melhor de todas as iterações de cada processo. O espaço de busca contém apenas soluções factíveis, para a estrutura da vizinhança foi utilizada uma busca local contendo dois movimentos possíveis, o 2-opt move e a troca de vértices.

O TSP considerado para implementação é o TPS simétrico.

Na pasta *instancias* se encontram as instâncias para testes de execução. As instâncias estão também disponíveis em [TSPLIB](http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsplib.html), mas as utilizadas na execução têm algumas modificações.

O formato de instância aceita é:

		<quantidadeDeVertices>
		<vertice0> <coordenadaX0> <coordenadaY0>
		<vertice1> <coordenadaX1> <coordenadaY1>
		...
e vai até o vértice *quantidadeDeVertices - 1*.

Após o comando *make* no terminal, basta executar o comando *lamboot* (caso já não tenha) e rodar o comando de execução *mpirun -np n ./main i*, onde n = quantidade de processos, e i o nome da instância.

Os resultados são impressos no terminal ao final da execução, e são eles:

	* rota
	* somatório total das arestas da rota
	* tempo de execução em segundos

Copyright (c) 2019 Lorena Kerollen Botelho Tavares