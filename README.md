# SysP — System Programming Language

<p align="center">
  <img src="https://img.shields.io/badge/versão-1.1-1E6B9E?style=for-the-badge" />
  <img src="https://img.shields.io/badge/grammar-v5.0_Final-2A8547?style=for-the-badge" />
  <img src="https://img.shields.io/badge/extensão-.sp-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/licença-MIT-lightgrey?style=for-the-badge" />
</p>

<p align="center">
  <strong>A simplicidade do Python. A velocidade do C. A segurança do Rust.</strong><br/>
  <em>Sem garbage collector · sem máquina virtual · sem surpresas</em>
</p>

---

## A proposta

As linguagens de programação modernas forçam uma escolha impossível:

- Quer **facilidade**? Use Python — mas abra mão de desempenho e controle de memória.
- Quer **velocidade**? Use C — mas gerencie memória manualmente e conviva com bugs difíceis de encontrar.
- Quer **segurança de memória**? Use Rust — mas enfrente uma curva de aprendizado enorme e anotações complexas.

**SysP recusa essa escolha.**

SysP é uma linguagem compilada, sem garbage collector e sem máquina virtual, projetada para ser **simples de escrever** e **segura por padrão** — em qualquer domínio da programação.

O segredo está no **Jarbes Kernel**: um analisador que constrói um grafo do programa inteiro e detecta erros de memória em tempo de compilação, antes de executar uma única linha.

> Você escreve código simples. O compilador garante a segurança.

---

## Comparação

| Linguagem | GC | Segurança de memória | Facilidade |
|---|---|---|---|
| C / C++ | Não | Manual — inseguro | Baixa |
| Java / Go | Sim | Runtime — exceções | Média |
| Rust | Não | Borrow checker — anotações complexas | Baixa |
| Python | Sim | GC — sem controle | Alta |
| **SysP** | **Não** | **Compilação — automático** | **Alta** |

---

## Como SysP é diferente

### Código simples como Python

```
-- main.sp

module io.println

fn main() {
    let nome = "SysP"
    let versao: i32 = 1
    println("Bem-vindo ao " + nome)
}
```

### Segurança de memória sem anotações

```
-- main.sp

module io.println

fn main() {
    region temp {
        let dados = new [1024]u8     -- alocado no heap
        processar(dados)
    }
    -- dados liberados automaticamente aqui
    -- qualquer uso depois = erro em compilação
}
```

### Tratamento de erros explícito e elegante

```
-- parser.sp

module io.println

fn dividir(a i32, b i32) -> Result<i32, string> {
    if b == 0 {
        return Err("divisão por zero")
    }
    return Ok(a / b)
}

fn main() {
    match dividir(10, 2) {
        Ok(v)  => println(v)
        Err(e) => println(e)
    }
}
```

---

## O Jarbes Kernel

O **Jarbes** é o analisador central do compilador SysP. Ele constrói um **grafo de dependências** — chamado de **Metatron Graph** — a partir do código fonte e analisa o programa inteiro de uma vez.

### O que o Jarbes detecta em compilação

| Erro | Como o Jarbes previne |
|---|---|
| `use-before-definition` | Ordem dos nós no grafo |
| `use-after-move` | `move` marca o nó como consumed |
| `double-free` | `region` garante único dono |
| `dangling pointer` | Lifetime do nó vs lifetime da região |
| `buffer overflow` | Bounds inferidos do tipo `array` |
| `data race` | Detector de corrida de dados |
| `ciclo de dependência` | Detector de ciclos no grafo |

### O que o Jarbes não tenta controlar

| Situação | Por quê |
|---|---|
| Erros de lógica de negócio | São responsabilidade do programador |
| Erros de rede / IO | Tratados com `Result<T, E>` |
| Código em `unsafe { }` | Programador assume responsabilidade explícita |

---

## Modelo de memória

SysP oferece três formas de gerenciar memória, cada uma com seu propósito:

### Stack — automático e zero custo
```
fn calcular() {
    let x: i32 = 10    -- stack, liberado ao sair da função
    let y: f64 = 3.14  -- stack, liberado ao sair da função
}
```

### Region — heap com escopo garantido
```
region minha_regiao {
    let node = new Node(42)    -- heap, pertence à região
}
-- node liberado aqui, automaticamente, sem GC
```

### Unsafe — controle total quando necessário
```
unsafe {
    -- Jarbes não analisa este bloco
    -- use apenas para drivers, FFI ou hardware
    let porta = 0xFF00 as *u8
    *porta = 0x01
}
```

---

## Domínios de aplicação

Por ser compilada, sem GC e sem VM, SysP é adequada para praticamente qualquer domínio:

| Domínio | Por que SysP funciona bem |
|---|---|
| Sistemas embarcados / firmware | Sem GC, sem runtime, controle total |
| Compiladores e interpretadores | AST, grafos e análise são estruturas nativas |
| Engines e jogos | Sem pausas de GC, alocação por region |
| Ferramentas de linha de comando | Compilado, rápido, binário único |
| Processamento de dados / parsers | Slices sem cópia, arrays fixos, generics |
| Sistemas distribuídos | Sem runtime pesado, módulos bem definidos |
| Segurança / criptografia | Controle bit a bit, unsafe controlado |
| Algoritmos e estruturas de dados | Sintaxe próxima de pseudocódigo |
| Infraestrutura e backend | Result<T,E> para erros, módulos claros |

---

## Arquitetura do compilador

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
  Backend        → binário nativo
    ↓
  Runtime Honeycomb
```

O **Runtime Honeycomb** é o sistema de memória em tempo de execução — um modelo de células de memória eficiente que suporta as regiões e o escalonamento de tarefas sem garbage collector.

---

## Estrutura do projeto

```
SysP/
    src/
        middleend/
            jarbes_kernel/          ← analisador central
                core/               ← MetatronGraph e MetatronNode
                analyzers/          ← detectores de erro
            metatron_graph/         ← construtor do grafo
    runtime/
        honeycomb/                  ← modelo de memória
        memory/                     ← alocador de arena
        scheduler/                  ← escalonador de tarefas
        tree/                       ← tree of life
    docs/
        LANGUAGE.md                 ← decisões e convenções
        language_spec.md            ← especificação técnica formal
        compiler_architecture.md    ← arquitetura do compilador
        metatron_design.md          ← design do grafo Metatron
    examples/                       ← exemplos de código .sp
```

---

## Documentação

| Documento | Descrição |
|---|---|
| [LANGUAGE.md](docs/LANGUAGE.md) | Decisões e convenções da linguagem |
| [language_spec.md](docs/language_spec.md) | Especificação técnica formal |
| [compiler_architecture.md](docs/compiler_architecture.md) | Arquitetura do compilador |
| [metatron_design.md](docs/metatron_design.md) | Design do grafo Metatron |

---

## Status do projeto

SysP está em desenvolvimento ativo. A gramática formal (v5.0 Final) está definida e o Jarbes Kernel está sendo implementado.

---

## Licença

MIT License — veja [LICENSE](LICENSE) para detalhes.

---

<p align="center">
  <em>SysP — segurança começa no compilador, não no runtime.</em>
</p>
