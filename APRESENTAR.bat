
@echo off
cd /d "%~dp0"
title Apresentacao - Compilador Lume
color 0A

cls
echo ==========================================
echo           COMPILADOR LUME
echo ==========================================
echo.
echo Linguagem propria desenvolvida em C++.
echo Traduz programas Lume para codigo Python.
echo.
pause

cls
echo ==========================================
echo TESTE 1 - PROGRAMA BASICO
echo ==========================================
echo.
echo OBJETIVO:
echo Testar declaracao de variaveis e comandos mostrar.
echo.
echo ANALISE LEXICA:
echo Identificando palavras reservadas, numeros e simbolos.
echo.
echo ANALISE SINTATICA:
echo Verificando se os comandos seguem a gramatica.
echo.
echo ANALISE SEMANTICA:
echo Verificando se as variaveis foram declaradas.
echo.
echo GERACAO DE CODIGO:
echo Traduzindo o programa para Python.
echo.
"%~dp0compilador.exe" testes\teste1_basico.lume
echo.
pause

cls
echo ==========================================
echo TESTE 2 - EXPRESSOES ARITMETICAS
echo ==========================================
echo.
echo OBJETIVO:
echo Testar operacoes matematicas e precedencia.
echo.
echo O compilador reconhece expressoes com
echo soma, subtracao e multiplicacao.
echo.
"%~dp0compilador.exe" testes\teste2_expressao.lume
echo.
pause

cls
echo ==========================================
echo TESTE 3 - ESTRUTURA CONDICIONAL
echo ==========================================
echo.
echo OBJETIVO:
echo Testar o comando SE e a comparacao de valores.
echo.
echo O compilador verifica a condicao e traduz
echo a estrutura condicional para Python.
echo.
"%~dp0compilador.exe" testes\teste3_condicional.lume
echo.
pause

cls
echo ==========================================
echo TESTE 4 - ERRO LEXICO
echo ==========================================
echo.
echo OBJETIVO:
echo Identificar simbolos invalidos.
echo.
echo O programa contem o caractere @.
echo O analisador lexico deve identificar
echo que esse simbolo nao pertence a linguagem.
echo.
"%~dp0compilador.exe" testes\teste4_lexico.lume
echo.
pause

cls
echo ==========================================
echo TESTE 5 - ERRO SINTATICO
echo ==========================================
echo.
echo OBJETIVO:
echo Verificar se o programa segue a gramatica.
echo.
echo O programa possui uma atribuicao incompleta.
echo Falta uma expressao depois do sinal de igual.
echo.
"%~dp0compilador.exe" testes\teste5_sintatico.lume
echo.
pause

cls
echo ==========================================
echo TESTE 6 - ERRO SEMANTICO
echo ==========================================
echo.
echo OBJETIVO:
echo Verificar se as variaveis foram declaradas.
echo.
echo O programa utiliza uma variavel que
echo nao foi declarada anteriormente.
echo.
"%~dp0compilador.exe" testes\teste6_semantico.lume
echo.
pause

cls
echo ==========================================
echo       APRESENTACAO FINALIZADA
echo ==========================================
echo.
echo Os seis testes foram executados.
echo Foram demonstrados testes validos e erros
echo lexicos, sintaticos e semanticos.
echo.
pause
