# SysP — Arquitetura do Compilador

> Versão: 1.0  
> Última atualização: 2026

---

## Visão geral

O compilador SysP é dividido em três grandes camadas:

```
código fonte (.sp)
        ↓
   [ Frontend ]      ← lê e entende o código
        ↓
   [ Middleend ]     ← analisa segurança e dependências  ← Jarbes + Metatron
        ↓
   [ Backend ]       ← gera código de máquina
        ↓
   binário nativo
```

---

## 1. Frontend

Responsável por ler o código `.sp` e transformar em estrutura interna.

| Etapa | O que faz |
|---|---|
| Lexer | Lê o texto e separa em tokens (palavras, símbolos, números) |
| Parser | Valida a gramática e monta a AST (Árvore Sintática) |
| AST | Representação estruturada do programa em memória |

---

## 2. Middleend — Jarbes Kernel

O coração do compilador. Aqui mora toda a inteligência de análise do SysP.

### Localização no projeto
```
src/middleend/
    jarbes_kernel/
        core/
            metatron_graph.hpp      ← grafo de dependências
            metatron_node.hpp       ← nós do grafo
        analyzers/
            cycle_detector.cpp      ← detecta ciclos
            data_race_detector.cpp  ← detecta corrida de dados
            dependency_checker.cpp  ← verifica dependências
        jarbes_kernel.cpp           ← núcleo principal
        jarbes_kernel.hpp
    metatron_graph/
        metatron_builder.cpp        ← constrói o grafo
        metatron_builder.hpp
        symbol_table.hpp            ← tabela de símbolos
```

### O que o Jarbes faz

O Jarbes constrói um **grafo de dependências** (o Metatron Graph) a partir do código e usa esse grafo para detectar erros antes da execução:

| Analisador | Arquivo | O que detecta |
|---|---|---|
| Dependency Checker | `dependency_checker.cpp` | use-before-definition, use-after-move |
| Cycle Detector | `cycle_detector.cpp` | dependências circulares, double-free |
| Data Race Detector | `data_race_detector.cpp` | corrida de dados em unsafe |

### Fluxo do Jarbes

```
AST
 ↓
MetatronBuilder     ← constrói o grafo a partir da AST
 ↓
MetatronGraph       ← grafo de dependências do programa
 ↓
Analyzers           ← analisam o grafo em busca de erros
 ↓
erros em compilação OU aprovação para o backend
```

---

## 3. Backend

Responsável por gerar o código executável a partir da saída aprovada pelo Jarbes.

---

## 4. Runtime

Camada de suporte em tempo de execução. Mesmo sem garbage collector, o runtime fornece estruturas para execução eficiente.

### Localização no projeto
```
runtime/
    honeycomb/
        honeycell.hpp           ← célula de memória
        honeycomb_grid.hpp      ← grade de memória (estrutura colmeia)
        lcache.hpp              ← cache local
        registers.hpp           ← registradores
    memory/
        arena.hpp               ← alocador de arena (região de memória)
    metatron/
        metatron_graph.hpp      ← grafo em tempo de execução
    scheduler/
        scheduler.hpp           ← escalonador de tarefas
    tree/
        tree_of_life.hpp        ← árvore de vida (estrutura de dados interna)
```

### Honeycomb — modelo de memória

O runtime usa um modelo de memória chamado **Honeycomb** (colmeia). Cada célula (`honeycell`) é uma unidade de memória gerenciada. A grade (`honeycomb_grid`) organiza essas células de forma eficiente, sem garbage collector.

---

## 5. Fluxo completo

```
arquivo.sp
    ↓
  Lexer          → tokens
    ↓
  Parser         → AST
    ↓
  MetatronBuilder → MetatronGraph
    ↓
  Jarbes Kernel
    ├─ DependencyChecker
    ├─ CycleDetector
    └─ DataRaceDetector
    ↓
  (sem erros) → Backend → binário nativo
                              ↓
                           Runtime (Honeycomb)
```

---

*SysP — segurança começa no compilador, não no runtime.*    
