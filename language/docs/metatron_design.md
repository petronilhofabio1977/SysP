# Metatron — Design do Grafo de Dependências

> Componente central do Jarbes Kernel  
> Versão: 1.0

---

## O que é o Metatron?

O **Metatron** é o grafo de dependências que o Jarbes Kernel constrói a partir do código SysP. Cada variável, função e valor do programa vira um **nó** nesse grafo. As relações entre eles (quem usa quem, quem passa valor pra quem) viram **arestas**.

Com esse grafo, o Jarbes consegue ver o programa inteiro de uma vez e detectar erros que um compilador tradicional não detectaria.

---

## Localização no projeto

```
src/middleend/
    jarbes_kernel/
        core/
            metatron_graph.hpp    ← estrutura do grafo
            metatron_node.hpp     ← estrutura de cada nó
    metatron_graph/
        metatron_builder.cpp      ← constrói o grafo a partir da AST
        metatron_builder.hpp
        symbol_table.hpp          ← tabela de símbolos do programa

runtime/
    metatron/
        metatron_graph.hpp        ← versão do grafo em tempo de execução
```

---

## Conceitos fundamentais

### Nó (MetatronNode)

Cada elemento do programa é um nó:

| Tipo de nó | Exemplo em SysP |
|---|---|
| Variável | `let x = 10` |
| Função | `fn soma(a i32, b i32)` |
| Valor alocado | `new Node(42)` |
| Região | `region temp { }` |
| Parâmetro | `a i32` em uma função |

### Estado de um nó

Todo nó tem um estado que o Jarbes acompanha:

| Estado | Significado |
|---|---|
| `defined` | Nó foi declarado |
| `live` | Nó está em uso |
| `consumed` | Nó foi transferido com `move` |
| `freed` | Nó saiu do escopo (region fechou) |

### Aresta (dependência)

Arestas representam relações entre nós:

| Tipo de aresta | Exemplo |
|---|---|
| Uso | `x` é usado em `x + 1` |
| Ownership | `move x` transfere de x para y |
| Referência | `ref x` passa referência |
| Escopo | nó pertence a uma região |

---

## Como o Metatron detecta erros

### use-after-move

```
let x = new Node(10)
let y = move x        ← aresta de ownership: x → y, x vira consumed
usar(x)               ← ERRO: nó x está consumed
```

O Jarbes vê no grafo que `x` está com estado `consumed` e bloqueia o uso.

### use-before-definition

```
println(z)    ← ERRO: nó z não existe no grafo ainda
let z = 5
```

O Jarbes verifica a ordem dos nós no grafo — `z` não existe no momento do uso.

### region escape

```
region temp {
    let node = new Node(42)   ← nó pertence à região temp
}
usar(node)    ← ERRO: nó foi freed quando a região fechou
```

O Jarbes verifica se referências a nós de uma região escapam para fora dela.

### double-free

```
region a {
    let x = new Node(1)
    region b {
        usar(x)
    }   ← x não pertence a b, não pode ser liberado aqui
}   ← x é liberado aqui
```

O Jarbes garante que cada nó tem exatamente um dono e é liberado uma única vez.

### cycle detector

```
let a = b + 1
let b = a + 1   ← ciclo! a depende de b e b depende de a
```

O Jarbes detecta ciclos no grafo e reporta como erro.

---

## MetatronBuilder — construindo o grafo

O `MetatronBuilder` percorre a AST gerada pelo parser e constrói o grafo:

```
AST
 ↓
MetatronBuilder.build()
    ├─ para cada declaração → cria nó
    ├─ para cada uso         → cria aresta
    ├─ para cada move        → muda estado do nó para consumed
    ├─ para cada region      → agrupa nós sob a região
    └─ para cada unsafe      → marca nós como não analisados
 ↓
MetatronGraph (completo)
```

---

## Symbol Table

A `symbol_table.hpp` mantém o mapeamento de nomes para nós do grafo:

```
"x"    → nó #1  (i32, estado: live)
"y"    → nó #2  (i32, estado: consumed)
"node" → nó #3  (Node, estado: freed)
"soma" → nó #4  (fn i32->i32)
```

Quando o Jarbes encontra o nome `x` no código, consulta a tabela para saber em qual nó do grafo ele está e qual é seu estado atual.

---

## Metatron no Runtime

A versão em `runtime/metatron/metatron_graph.hpp` é uma versão simplificada do grafo usada em tempo de execução para suporte a blocos `unsafe` e rastreamento de regiões dinâmicas.

---

*Metatron — o mapa completo do programa, construído antes de executar uma linha.*
