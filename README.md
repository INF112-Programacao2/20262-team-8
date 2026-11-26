Gerenciador de Reservas de Espaços Esportivos:

Sistema para gerenciamento de clientes, quadras e reservas de espaços esportivos,
com cálculo de tarifas conforme a modalidade e adicionais de infraestrutura, além
de operação por CLI e modo headless.

O `SistemaGestao` funciona como fachada: a CLI delega a gestão de clientes, quadras
e reservas aos serviços do domínio. Cada serviço é dono dos objetos que cria;
o cliente mantém apenas referências às reservas associadas.

## Compilar e testar

Com `g++` e `make` instalados:

```sh
make
./reservas
make test
```

`make test` executa testes das regras de reserva sem iniciar a interface de terminal.
Os arquivos gerados ficam em `build/` e o executável em `reservas`. Para removê-los:

```sh
make clean
```