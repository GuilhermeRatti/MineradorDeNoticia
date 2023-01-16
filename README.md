# MineradorDeNoticia

índice de Palavras:
  - Lista de documentos em que ela aparece (referenciado por indice de posicao)
  - Numero de ocorrencias da palavra em cada documento
  - Valor TF-IDF
  - Para cada aparicao, criar um vetor de estruturas contendo indice do doc, frequencia e tfidf

índice de Documentos:
  - Lista de palavras que aparecem nele (referenciado por indice de posicao)
  - Número de ocorrencias por palavra
  - Valores TF-IDF por palavra
  - Para cada aparicao, criar um vetor de estruturas contendo indice da palvavra, frequencia e tfidf

Organização da leitura das TADs:
  - Primeiro ler documentos e criar indices vazios de documentos e palavras
  - Organizar o indice de palavras em ordem alfabetica
  - Fazer uma segunda leitura dos documentos, fazendo busca binária para cada palabra e preenchendo as tads com as frequencias
