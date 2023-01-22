# MineradorDeNoticia

índice de Palavras:
  - Lista de documentos em que ela aparece (referenciado por indice de posicao) - OK!
  - Numero de ocorrencias da palavra em cada documento - OK!
  - Valor TF-IDF para cada documento
  - Para cada aparicao, criar um vetor de estruturas contendo indice do doc, frequencia e tfidf
  - Tamanho do vetor de aparicoes nos documentos - OK!

índice de Documentos:
  - Lista de palavras que aparecem nele (referenciado por indice de posicao) - OK!
  - Número de ocorrencias por palavra - OK!
  - Valores TF-IDF por palavra
  - Para cada aparicao, criar um vetor de estruturas contendo indice da palvavra, frequencia e tfidf
  - Tamanho do vetor de aparicoes das palavras - OK!

Organização da leitura das TADs:
  - Primeiro ler documentos e criar indices vazios de documentos e palavras - OK!
  - Organizar o indice de palavras em ordem alfabetica - OK!
  - Fazer uma segunda leitura dos documentos, fazendo busca binária para cada palabra e preenchendo as tads com as frequencias - OK!
  - Fazer uma terceira leitura, das TADs, calculando o TF-IDF 
 
 Para Calcular o TF-IDF:
  - OLHAR PARA O INDICE DE PALAVRAS PARA CALCULAR O TF-IDF, E IR PREENCHENDO O INDICE DE DOCUMENTOS SIMULTANEAMENTE
  - TF: É o atributo de frequência da estrutura que a gente vai ter no vetor dentro do indice de palavra
  - DF: É o tamanho do vetor de aparicoes da palavra nos documentos
