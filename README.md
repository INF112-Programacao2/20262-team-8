Gerenciador de Reservas de Espaços Esportivos:

Sistema para gerenciamento de clientes, quadras e reservas de espaços esportivos,
com cálculo de tarifas conforme a modalidade e adicionais de infraestrutura, além
de operação por CLI e modo headless.

O `SistemaGestao` funciona como fachada: a CLI delega a gestão de clientes, quadras
e reservas aos serviços do domínio. Cada serviço é dono dos objetos que cria;
o cliente mantém apenas referências às reservas associadas.

Reservas têm os estados **Confirmada**, **Concluida**, **Cancelada** e **Falta**.
Só reservas confirmadas ocupam horários. A conclusão e o registro de falta só são
permitidos a partir do início agendado; três faltas bloqueiam novas reservas do cliente.
O sistema usa a data e hora locais do computador, e a fachada permite ajustá-las
para testes determinísticos. Na CLI, as opções **Concluir reserva** e
**Registrar falta** só aceitam reservas cujo horário de início já chegou.
Ao criar uma reserva com objetos externos à fachada, mantenha o cliente e a quadra
vivos até a reserva ser destruída.

As quadras podem ser simples, cobertas (+15%), iluminadas (+R$ 10 por hora a partir
das 18h) ou premium (combina os dois adicionais). Esses adicionais são aplicados
ao valor calculado pela estratégia de preço selecionada.

Cada quadra também tem uma política de cancelamento: **Flexível** devolve 100% a
partir de um dia antes e 50% no mesmo dia; **Moderada** devolve 100% a partir de
três dias, 50% com um ou dois dias e 0% no mesmo dia; **Rígida** devolve 50% a
partir de sete dias e 0% com menos antecedência. O reembolso é calculado sobre o
valor pago pela reserva, e reservas de dias anteriores não podem ser canceladas.

Reservas concluídas podem ser avaliadas uma vez, com nota de 1 a 5. Notas 1 e 2
são reclamações: concedem ao cliente um cupom de 15% para a próxima reserva, e
três reclamações deixam a quadra inativa para revisão. Uma média de pelo menos
4,5 com três ou mais avaliações destaca a quadra.

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