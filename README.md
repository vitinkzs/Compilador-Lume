# Projeto de Compiladores — Linguagem Lume

## 1. Visão geral
Lume é uma linguagem didática criada para demonstrar as etapas de um compilador. O compilador foi escrito em C++17 e traduz programas Lume para Python 3 (transpilação).

## 2. Recursos da linguagem
- Declaração de variáveis numéricas com `numero`.
- Atribuição com `=`.
- Operações `+`, `-` e `*`.
- Parênteses e precedência: `*` antes de `+` e `-`.
- Saída com `mostrar`.
- Condicional `se (...) entao ... fimse`.
- Comparadores: `>`, `<`, `>=`, `<=`, `==`, `!=`.
- Comentários de linha iniciados por `//`.
- Um único escopo; declarações devem aparecer depois de `inicio` e antes dos comandos.

## 3. Palavras reservadas
| Palavra | Significado |
|---|---|
| `inicio` | Inicia o programa |
| `fim` | Encerra o programa |
| `numero` | Declara uma variável numérica |
| `mostrar` | Exibe uma expressão |
| `se` | Inicia uma condição |
| `entao` | Separa a condição do bloco |
| `fimse` | Encerra o bloco condicional |

## 4. Identificadores e tipos
Identificadores começam com letra ou `_` e continuam com letras, números ou `_`. Não podem ser palavras reservadas. O único tipo é `numero`, que aceita números inteiros e decimais. As variáveis precisam ser declaradas antes do uso e receber um valor antes de serem lidas.

## 5. Símbolos
- Aritméticos: `+`, `-`, `*`
- Atribuição: `=`
- Comparação: `>`, `<`, `>=`, `<=`, `==`, `!=`
- Delimitadores: `(`, `)`, `;`
- Comentário: `//`

Todo comando simples termina com `;`. O programa começa com `inicio` e termina com `fim`. Um bloco condicional termina com `fimse`.

## 6. Gramática (EBNF)
```text
programa       = "inicio", { declaracao }, { comando }, "fim" ;
declaracao     = "numero", identificador, ";" ;
comando        = atribuicao | saida | condicional ;
atribuicao     = identificador, "=", expressao, ";" ;
saida          = "mostrar", expressao, ";" ;
condicional    = "se", "(", comparacao, ")", "entao",
                 { comando }, "fimse" ;
comparacao     = expressao, operador_relacional, expressao ;
expressao      = termo, { ("+" | "-"), termo } ;
termo          = unario, { "*", unario } ;
unario         = ("+" | "-"), unario | primario ;
primario       = numero | identificador | "(", expressao, ")" ;
operador_relacional = ">" | "<" | ">=" | "<=" | "==" | "!=" ;
identificador  = (letra | "_"), { letra | digito | "_" } ;
numero         = digito, { digito }, [ ".", digito, { digito } ] ;
```

A gramática usa recursão para parênteses e operadores unários. A multiplicação está em `termo`, por isso tem precedência sobre soma e subtração.

## 7. Etapas implementadas
1. **Análise léxica:** percorre os caracteres e produz tokens com categoria, trecho e linha. Símbolos desconhecidos geram erro léxico.
2. **Análise sintática:** parser descendente recursivo verifica a gramática e constrói uma representação interna do programa. Erros mostram linha e elemento esperado.
3. **Análise semântica:** tabela de símbolos registra nome, tipo e inicialização. Verifica declaração duplicada, uso de variável não declarada e leitura antes da atribuição. A inicialização feita dentro de um `se` não é considerada garantida depois do bloco, pois a condição pode ser falsa.
4. **Geração de código:** somente após as três etapas sem erros, produz um arquivo Python equivalente. Declarações não geram linha Python; atribuições, saídas e condicionais são traduzidas.

## 8. Compilação e execução
É necessário ter um compilador C++ com suporte a C++17 e Python 3 instalado.

### Linux
```bash
g++ -std=c++17 -Wall -Wextra -pedantic compilador.cpp -o compilador
./compilador exemplos/exemplo1.lume
python3 exemplos/exemplo1.py
```

### Windows (MinGW)
```bash
g++ -std=c++17 -Wall -Wextra compilador.cpp -o compilador.exe
compilador.exe exemplos\exemplo1.lume
python exemplos\exemplo1.py
```

Também é possível informar o nome do arquivo Python:
```bash
./compilador exemplos/exemplo1.lume resultado.py
```

Se o segundo argumento for omitido, o compilador usa o nome do arquivo de entrada com extensão `.py`.

## 9. Testes
Os seis arquivos da pasta `testes` cobrem os requisitos mínimos. Os três primeiros são válidos. Os demais contêm, respectivamente, erro léxico, sintático e semântico. Execute cada um com o compilador. Os testes inválidos devem terminar com erro e não devem gerar um novo arquivo Python.

## 10. Limitações
O projeto trabalha com um único tipo numérico e escopo global. Não possui funções, laços, vetores, entrada de dados, divisão ou otimização. A análise semântica não tenta provar condições; por segurança, não assume que atribuições dentro de condicionais serão executadas.
