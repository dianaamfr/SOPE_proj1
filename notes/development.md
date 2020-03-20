# Notas

## Estado de desenvolvimento:

Para já, para testar as opções/flags no caso de tentarem colocar um path vai dar erro e mostrar a "usage message"
porque em checkArgs() a função devolve false se não for uma flag (ainda só estão a ser aceites as flags mas o path ainda não)

No caso de as opçes serem válidas o programa deve compilar com ou sem "-l"(questao a ver com o prof) a não ser no caso de atribuírem para --max-depth=N e -B SIZE N ou SIZE inválidos pois estes ainda não estão a ser testados (a função validSize() não foi ainda implementada).

Para a implementar validSize() é necessário perceber que tipo de SIZE podemos aceitar para as opções --max-depth=N e -B SIZE.

-Para --max-depth=N N>=0

-Para -B SIZE,
"The SIZE argument is an integer and optional unit (example: 10K is
10*1024).  Units are K,M,G,T,P,E,Z,Y (powers of 1024) or KB,MB,...
(powers of 1000).  Binary prefixes can be used, too: KiB=K, MiB=M,
and so on."

ou seja, vamos ter que aceitar números e unidades

