# Casos de teste

Execute cada arquivo com `./compilador caminho/do/arquivo.lume` (ou `compilador.exe` no Windows).

| Teste | Arquivo | Resultado esperado |
|---|---|---|
| 1 — básico | `teste1_basico.lume` | Compila; Python imprime `15` |
| 2 — expressões | `teste2_expressao.lume` | Compila; Python imprime `14` e `20` |
| 3 — condicional | `teste3_condicional.lume` | Compila; Python imprime `10` e `5`; a segunda condição é falsa e não imprime |
| 4 — léxico | `teste4_lexico.lume` | Erro léxico na linha 3 por causa de `@`; não gera Python |
| 5 — sintático | `teste5_sintatico.lume` | Erro sintático na linha 3 por falta de expressão após `=`; não gera Python |
| 6 — semântico | `teste6_semantico.lume` | Erro semântico por uso de `fantasma` não declarada; não gera Python |

Para registrar o resultado obtido, execute os arquivos e anote no relatório ou na apresentação a mensagem exibida pelo programa. Nos testes válidos, execute também os arquivos `.py` gerados com Python 3.
