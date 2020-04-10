SOPE T02G05
----------

### Project 1

**SIMPLEDU**
A simple version of the LINUX `du` (Disk Usage), a standard Unix/Linux command, used to check the information of disk usage of files and directories on a machine.
[Mais sobre o comando `du`](http://man7.org/linux/man-pages/man1/du.1.html)

#### Report

>-----------------------------------------------------------------------------

>*Quais features estão ok, quais estão mais ou menos e quais não foram implementadas*

##### Requisitos Funcionais - *OK*

A leitura dos argumentos da linha de comandos está implementada, tanto na versão simples, como na versão longa das opções.
Todas as opções de invocação se encontram funcionais e, quando detetadas, são guardadas numa máscara de flags para posterior uso, na apresentação dos resultados.
Procuramos, para além disso, apresentar o tamanho correto, mesmo quando são usadas flags repetidas e/ou em conjunto, mas por ordens diferentes (por exemplo, a conjugação das opções `-b` e `-B <size>`)

##### Apresentação de resultados - *OK*

O formato de saída do `du` foi replicado corretamente. 

##### Geração de registos de execução - *OK*

A criação e terminação de processos, envio e receção de sinais e de dados através de pipes, e os ficheiros e diretórios analisados por cada processo são operações, a todo o momento, catalogadas num ficheiro de *logs*, cujo caminho é guardado na variável de ambiente **LOG_FILENAME**.

##### Interrupção pelo utilizador - *OK*

Os sinais são tratados com a mesma finalidade exposta no enunciado do projeto, estando o mecanismo completamente funcional. Os detalhes de implementação deste mecanismo estão descritos em maior profundidade no último ponto, numa estratégia alternativa para lidar com o envio dos sinais a todos os processos filho.

##### Requisitos Arquiteturais - *OK*

Cada processo analisa apenas um diretório e seus constituintes, criando processos para os seus subdiretórios, que executam, de novo, o mesmo código do programa `simpledu`, para a respetiva pasta.
O processo-pai (primeiro a ser executado) aguarda pela terminação de todos os processos-filho antes de terminar a sua execução, imprimindo a acumulação total e o seu tamanho final.
Estas comunicações entre processos usam sempre pipes (sem nome).

<br>
<br>

>-----------------------------------------------------------------------------

>*Que erros de imprecisão de contas sabem que podem existir (p.ex. diferentes comportamentos entre os computadores que usaram para testar)*

##### Análise do comportamento do comando `du` nos computadores dos diferentes elementos do grupo

Nos computadores de dois dos elementos do grupo, verificamos um comportamento idêntico, no que diz respeito ao resultados apresentados pelo comando `du`.
No entanto, um de nós detetou que o comando `du` apresentava valores diferentes em vários casos:
- Por exemplo, quando todos os ficheiros `.c` de um certo diretório apresentavam tamanho "4", utlizando o comando `du` sem flags, o ficheiro `.h` apresentava tamanho "1". No caso dos restantes elementos do grupo, para o mesmo diretório, os valores dos ficheiros `.c` e `.h` eram todos de "4".
- Também em bytes, o comando `du` apresentava tamanhos distintos para esse elemento do grupo.

Tendo em conta que não se detetou a razão pela qual os valores apresentados nesse computador eram diferentes (que é mais antigo que os dos restantes membros do grupo), optou-se por se basear o cálculo dos valores naquilo que é apresentado pelo `du`, nos computadores da maioria dos elementos do grupo.

##### Cálculo dos valores apresentados

O cálculo do tamanho ocupado pelos ficheiros/subdiretórios em disco utiliza a `struct stat` para obter todos os valores relevantes, entre os quais:
- o tamanho real em bytes (`-b`) de espaço ocupado em disco por um dado ficheiro/subdiretório (disponível no campo `st_size`)
- o tamanho em blocos de 512 bytes ocupado em disco por um dado ficheiro/subdiretório (disponível no campo `st_blocks`)

A partir dos valores anteriores, nos casos em que se pretende obter o tamanho em blocos, em particular, quando é utilizada a flag `-B <size>`, ou quando é utilizado o comando sem quaisquer flags (blocos de 1024 bytes), o cálculo utiliza a seguinte fórmula para obter o número de blocos de tamanho indicado no argumento:

1. O valor em blocos é convertido em bytes 
`bytesSize = stat_buf->st_blocks * 512`
2. No caso de estarmos perante um subdiretório, ou ficheiro de um (sub)diretório, é somado o valor anterior ao tamanho total do subdiretório, pois a conversão do tamanho do seu subdiretório, consoante o tamanho dos blocos a apresentar, só pode ser feita no fim
3. Se a flag `-a` estiver ativa, o tamanho de um ficheiro é apresentado com o nº de blocos especificado através do cálculo do *ceil* da divisão do nº de bytes calculado em 1 pelo `size`, ou por 1024, no caso de não serem especificadas flags, isto é, 

   ```c
   if(bytesSize % blockSize == 0)
      return bytesSize / blockSize;
   else
      return bytesSize / blockSize + 1;
   ```

Para links simbólicos, quando está ativa a flag `-L`, o valor apresentado é obtido usando, para esses casos, a função `int stat(const char *pathname, struct stat *statbuf)`, em vez da utilizada quando `-L` não está ativa: `int lstat(const char *pathname, struct stat *statbuf)`; sendo que, no caso de o tamanho ser pedido em blocos, o link simbólico apresenta, normalmente, tamanho nulo (a não ser que seja pedido, especificamente, o tamanho desse ficheiro em particular, ao invés de ser pedido o tamanho total de um diretório).

<br>
<br>

>-----------------------------------------------------------------------------

>*Qualquer outro detalhe que achem relevante (e.g. como lidam com sinais, pipes, se usam forks + execs ou simplesmente forks + recursividade, etc.).*

##### Requisitos Funcionais

As opções/argumentos da linha de comandos são lidos com o auxílio de uma biblioteca externa da API do Linux - `<getopt.h>`. Toda a implementação foi baseada nos exemplos presentes na [MAN PAGE](https://linux.die.net/man/3/getopt_long) da respetiva biblioteca e encontra-se, programaticamente, na função `int checkArgs(int argc, char * argv[], flagMask * flags)` do ficheiro `utils.c`. 
Nota: ver `utils.h` para mais pormenores, documentação e créditos.

##### Geração de registos de execução

Os registos de *log* são guardados num ficheiro comum, que permite verificar a ordem temporal e o conteúdo das diversas operações realizadas. Para otimizar tal efeito, foi criada uma espécie de API, nos ficheiros `logging.c` e `logging.h`, com as diversas opções/funções de registo. Existe uma variável global que guarda apenas o instante inicial de execução do programa e que é necessária em todos os processos, por forma a calcular o tempo decorrido. O seu valor, por isso mesmo, é partilhado pelo processo pai e inicializado com a criação de cada processo filho.

##### Interrupção pelo utilizador
> *Copiado de https://sopefeup.slack.com/archives/G010D9YNE4V/p1586302695005100*

O mecanismo para lidar com o requisito funcional relacionado com os sinais é como se segue.

Inicialmente, todos os processos que começam o `simpledu` ignoram o SIGINT e estabelecem um handler para um sinal auxiliar, neste caso, o SIGUSR2:

```c
struct sigaction action;
attachSIGHandler(action, SIGUSR2, sigHandler);
attachSIGHandler(action, SIGINT, SIG_IGN);
```

Ao contrário do pai de todos, que muda esses handlers de forma a só ele receber (não ignorar) o SIGINT, mas ignorar o SIGUSR2:

```c
attachSIGHandler(action, SIGUSR2, SIG_IGN);
attachSIGHandler(action, SIGINT, sigHandler);
```

Assim, quando o utilizador mandar o Ctrl+C ou SIGINT, o pai de todos é o único que o recebe e faz o seguinte:
1. Manda um SIGUSR2 a todos os processos do seu grupo - sendo que só ele é que não o vai receber, porque o está a ignorar.
2. Esse SIGUSR2 é recebido por todos os outros processos, seus filhos, e, portanto, serve de "máscara" escondida para cada um desses processos se parar a si próprio, mandando um SIGSTOP;
3. O resultado disto é que todos os processos ficam parados exceto o pai, que entretanto pergunta ao utilizador se quer ou não continuar;
4. Se continuar, manda um SIGCONT pra todos, incluindo ele, o que só tem realmente efeito se os processos estiverem parados;
5. Senão, manda um SIGTERM e termina tudo, incluindo ele também.

```c
void sigHandler(int signo){
   if (signo == SIGINT){ // Ignored by every process except the parent
      logRECV_SIGNAL(SIGINT);
               
      logSEND_SIGNAL(SIGUSR2, -getpgrp());
      kill(-getpgrp(), SIGUSR2); // Sending a SIGUSR2 to all child processess
      
      printf("\nSTOPPING! - %d - %d\n", getpid(), getppid());
      char c;
      printf("Continue? (Y or N) ");
      scanf("%c",&c);
      while ((getchar()) != '\n');
      printf("You entered: %c\n", c);
      
      if (c == 'Y'){
         printf("CONTINUING! - %d - %d\n", getpid(), getppid());
         
         logSEND_SIGNAL(SIGCONT,-getpgrp());
         kill(-getpgrp(),SIGCONT); // Sending a SIGCONT to all processess, including the parent himself
      }
      else{
         printf("TERMINATING! - %d - %d\n", getpid(), getppid());
         
         logSEND_SIGNAL(SIGTERM, -getpgrp());
         logEXIT(SIGTERM);
         kill(-getpgrp(), SIGTERM); // Sending a SIGTERM to all processess, including the parent himself
      }
   }
   if(signo == SIGUSR2){ // Ignored by the parent process only
      logRECV_SIGNAL(SIGUSR2);    
      logSEND_SIGNAL(SIGSTOP, getpid());    
      kill(getpid(), SIGSTOP); // Sending a SIGSTOP to all child processess themselves
   }
}
```

Isto funciona de forma a disfarçar o SIGSTOP de SIGUSR2, para que o pai o envie a todos os filhos sem se parar a ele próprio, dentro do seu grupo de processos. Ou seja, o pai não envia explicitamente um SIGSTOP a todos os filhos, manda sim um SIGUSR2 a todos os filhos e estes param-se a si próprios. O efeito é o mesmo, tendo as vantagens de não precisarmos de guardar registos dos pids dos filhos todos em arrays, ou outras soluções que envolvam mais coisas além dos pids de cada um dos processos e dentro do mesmo grupo de processos.

Assume-se, no entanto e por fim, que este mecanismo apenas não cumpre escrupulosamente a declaração `"o processo-pai deve enviar um sinal SIGSTOP a todos os processos-filho"`, se interpretada na sua literalidade, no único sentido em que o SIGSTOP não é enviado diretamente aos filhos.

##### Bloqueio de sinais para detetar subprocessos
> *Com base em informação obtida em https://www.gnu.org/software/libc/manual/html_node/Process-Signal-Mask.html*

Existem comportamentos que se pretende que sejam exclusivos do primeiro processo de todos, entre os quais:
- a leitura das `flags` e do `path`, que não deve ser efetuada pelos subprocessos (os subprocessos recebem as flags enviadas no pipe, lendo apenas um descritor dos argumentos)
- a forma como lida com a receção dos sinais, em particular o `SIGINT` e, por isso, os `signal handlers` que lhe estão associados são diferentes dos restantes processos
- ...

Por estas questões de exclusividade no comportamento dos diversos processos, optamos por utilizar uma estratégia baseada no bloqueio de um sinal (`SIGUSR1`), enviado pelos subprocessos a eles próprios. 
Evitamos, assim, utilizar mais uma variável ambiente para guardar informação que servisse apenas para identificar o primeiro processo (o seu `pid`, ou, eventualmente, o seu `ppid`).

Inicialmente, utiliza-se a função `blockSIGUSR1` para bloquear o sinal, em todos os processos:

```c
void blockSIGUSR1(){

   sigset_t mask;

   // Temporarily blocking SIGUSR1
   sigemptyset (&mask);
   sigaddset (&mask, SIGUSR1);

   // Set new mask
   sigprocmask(SIG_BLOCK, &mask, NULL);
}
```

Sempre que é criado um novo processo filho, através da chamada `fork()`, o filho envia o sinal `SIGUSR1` a ele próprio, antes do `exec()`: 
`kill(getpid(),SIGUSR1)`.
Assim, ao iniciar um processo, como o sinal é sempre bloqueado, vai existir um sinal pendente `SIGUSR1`, em todos os subprocessos, o que nos permite distinguir o primeiro processo de todos, o único que não terá esse sinal pendente.

A verificação da sua existência é realizada através da seguinte função, que, no caso de existir um sinal `SIGUSR1` pendente, o remove da lista de sinais pendentes:

```c
int pendingSIGUSR1(){

   sigset_t pending_signals;
   int sig;

   // Checking for pending SIGUSR1
   if (sigpending(&pending_signals) == 0 && sigismember (&pending_signals, SIGUSR1)){

      sigwait(&pending_signals,&sig);

      return OK;
   }

   return ERROR;
}
```

##### Outros Aspetos

`** stack smashing detected **` era um problema recorrente, quando analisando diretórios com caminhos/nomes (strings) mais longos que o limite - 256. Para impedir a perda de informação, e, por consequência, a incorreta apresentação da informação final, foi implementado um simples mecanismo de aviso e terminação do programa. Assim que um processo filho deteta um tamanho ilegal, que ultrapasse o limite imposto pelo compilador, envia um sinal ao seu processo pai e este sinal é replicado na direção inversa (*cascade*), até chegar ao processo pai de todos, que notifica o utilizador e termina os cálculos. A implementação detalhada encontra-se na função `main` do programa, no handler associado ao sinal usado e na função `int validPathSize(char * path);` do ficheiro `utils.c`.

#### Authors
>----

Diana Freitas - [up201806230](mailto:up201806230@fe.up.pt)

Eduardo Brito - [up201806271](mailto:up201806271@fe.up.pt)

Maria Baía - [up201704951](mailto:up201704951@fe.up.pt)