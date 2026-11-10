# Relatório breve — Compilador da linguagem Lume

## Objetivo
O projeto implementa um compilador didático em C++17 para a linguagem Lume. O compilador lê um arquivo-fonte, identifica tokens, verifica a estrutura gramatical, realiza verificações semânticas e gera um arquivo Python.

## Decisões de projeto
Foi escolhida uma linguagem pequena para manter o foco nas etapas fundamentais de compilação. O tipo `numero` aceita valores inteiros e decimais. As declarações ficam no início do programa, simplificando a organização da tabela de símbolos. Os comandos terminam em ponto e vírgula, enquanto `fim` e `fimse` delimitam blocos.

A linguagem de destino é Python 3 porque permite demonstrar a tradução sem precisar gerar código de máquina. O parser foi implementado manualmente por descida recursiva, sem geradores de compiladores.

## Funcionamento
Na análise léxica, o código é separado em tokens, cada um com categoria, texto e linha. Na análise sintática, o parser confere a sequência de tokens segundo a gramática EBNF e constrói uma estrutura interna. Na análise semântica, a tabela de símbolos controla as variáveis e sua inicialização. Por fim, o gerador percorre a estrutura interna e escreve o programa Python.

O arquivo de saída só é escrito depois que todas as análises terminam sem erros. Assim, um programa inválido não produz uma nova saída compilada.

## Testes
Foram preparados seis programas: três válidos e três inválidos. Os válidos verificam declaração, atribuição, operações, parênteses, precedência e condição. Os inválidos verificam símbolo desconhecido, comando incompleto e uso de variável não declarada. Os resultados esperados estão registrados em `testes/README_TESTES.md`.

## Dificuldades e limitações
Uma das principais dificuldades é separar a validação da execução: o compilador não executa o programa Lume, mas constrói uma representação interna e gera Python. Outra preocupação é não considerar inicializada, depois de um condicional, uma variável que só recebeu valor dentro do bloco, pois o bloco pode não ser executado.

A linguagem não implementa laços, funções, vetores, entrada de dados, divisão ou múltiplos tipos. Essas limitações foram intencionais para priorizar o funcionamento correto dos requisitos obrigatórios.

## Conclusão
O projeto demonstra as quatro etapas principais solicitadas: análise léxica, sintática, semântica e geração de código. A implementação permite visualizar os tokens e a tabela de símbolos, informa erros com a etapa e a linha, e traduz programas válidos para Python.
