
# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Paulo Santos do Nascimento Filho**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, 09 de junho de 2025

---

# 1. O que acontece se todas as tarefas tiverem a mesma prioridade?
Quando todas as tarefas têm a mesma prioridade no FreeRTOS, o escalonador utiliza time slicing (fatia de tempo) para dividir o tempo da CPU entre elas de forma justa. Ou seja, cada tarefa recebe uma fatia de tempo para executar, e o FreeRTOS alterna rapidamente entre elas, dando a impressão de execução simultânea. Porém, se uma tarefa nunca cede o controle (por exemplo, não usa vTaskDelay() ou outra função de bloqueio), ela pode monopolizar a CPU, impedindo as outras de rodarem corretamente.

# 2. Qual tarefa consome mais tempo da CPU?
A tarefa que consome mais tempo da CPU é aquela que não realiza nenhum bloqueio ou atraso, ou seja, que roda em um loop contínuo sem chamar vTaskDelay(), vTaskSuspend() ou aguardar eventos. Isso porque ela nunca libera voluntariamente o processador, mantendo a CPU ocupada, e assim reduz a execução das outras tarefas.

# 3. Quais seriam os riscos de usar polling sem prioridades?
Usar polling sem definir prioridades pode causar:

Desperdício de CPU: A tarefa que faz polling contínuo sem atrasos pode consumir toda a CPU, causando alta utilização desnecessária.

Inanição (starvation): Outras tarefas podem não conseguir tempo para executar, prejudicando o funcionamento geral do sistema.

Baixa responsividade: Como o sistema está ocupado verificando repetidamente uma tarefa, eventos importantes de outras tarefas podem ser atrasados.

Dificuldade de escalabilidade: Com o aumento do número de tarefas, a falta de prioridade pode gerar conflitos e falhas no controle do fluxo.

---

## 📜 Licença
GNU GPL-3.0.
