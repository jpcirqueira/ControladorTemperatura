# Projeto 1
Repositório do projeto 1 da disciplina Fundamentos de Sistemas Embarcados, UNB.

Enunciado do projeto disponivel em: https://gitlab.com/fse_fga/projetos_2020_2/projeto-1-2020.2

**Nome**: João Pedro Soares Cirqueira

**Matrícula**: 150132344

## Sistema

![](./imagens/Figura_Trabalho_1_v2.png)

## Executando o projeto
```sh
$ git clone https://github.com/jpcirqueira/ControladorTemperatura
$ cd ControladorTemperatura
$ make
$ ./run
```

## Screenshots da execução

![](./imagens/terminal.png)

Imagem do terminal apos a execucao.

## Resultados

### Grafico 1

![](./imagens/imagem1.jpeg)

Grafico com as temperaturas (Ambiente, Interna e Referência (Potenciômetro)) x tempo de execucão.

### Grafico 2

![](./imagens/imagem2.jpeg)

Grafico com o valor do acionamento dos atuadores (Resistor / Ventoinha) x tempo de execucão.

## Observações

Os resultados gerados estão sendo gravados em um arquivo .csv disponivel apos a execucao em: ```grafico/dados.csv```
