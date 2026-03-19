# SysP — Referência Oficial da Linguagem

> Documento vivo. Toda decisão de design vai aqui primeiro.  
> Versão: 1.1 · Grammar: v5.0 Final

---

## 1. Identidade da linguagem

**SysP** (System Programming Language) é uma linguagem compilada, sem garbage collector e sem máquina virtual.

O diferencial central é o **Jarbes Kernel** — um analisador global que detecta erros de memória e estrutura em **tempo de compilação**, antes do programa executar.

> Você escreve código simples. O compilador garante a segurança.

---

## 2. Decisões oficiais da linguagem

| Decisão | Valor |
|---|---|
| Extensão dos arquivos | `.sp` |
| Primeira linha de todo arquivo | `-- nome_do_arquivo.sp` |
| Ponto de entrada do programa | arquivo chamado `main.sp` |
| Palavra `module` | chama módulos da própria linguagem |
| Módulos da linguagem | `module io.println`, `module algorithms.sort`, etc. |
| Comentário de linha | `--` |
| Comentário de bloco | `(* ... *)` |

---

## 3. Estrutura de um arquivo `.sp`

Todo arquivo `.sp` segue esta ordem:

```
-- main.sp                      ← 1. comentário com o nome do arquivo

module io.println               ← 2. módulos da linguagem que serão usados
module io.print

fn main() {                     ← 3. funções e lógica do programa
    println("Olá, SysP!")
}
```

### Regras
- A **primeira linha** é sempre um comentário com o nome do arquivo
- Os **módulos** vêm logo depois, antes de qualquer função
- O compilador identifica o ponto de entrada pelo arquivo `main.sp`
- Dentro de `main.sp` deve existir uma função `fn main()`

---

## 4. Módulos da linguagem

Módulos são partes da própria linguagem SysP — não são criados pelo usuário.  
São chamados com a palavra `module` no topo do arquivo.

### Módulos disponíveis

| Módulo | O que faz |
|---|---|
| `io.println` | Imprime texto com quebra de linha |
| `io.print` | Imprime texto sem quebra de linha |
| `io.fs` | Leitura e escrita de arquivos |
| `io.string` | Manipulação de strings |
| `algorithms.sort.quicksort` | Ordenação rápida |
| `algorithms.sort.mergesort` | Ordenação estável |
| `algorithms.search.binary` | Busca binária |
| `algorithms.graph.bfs` | Busca em largura |
| `algorithms.graph.dfs` | Busca em profundidade |
| `algorithms.graph.dijkstra` | Caminho mínimo |
| `data_structures.list` | Lista encadeada |
| `data_structures.tree` | Árvore binária |
| `data_structures.hash` | Tabela hash |
| `math.matrix` | Operações com matrizes |
| `math.vector` | Operações com vetores |
| `math.numerical` | Métodos numéricos |
| `geometry` | Operações geométricas |

---

## 5. Como programar em SysP

### 5.1 Variáveis

Existem três formas de declarar variáveis:

```
-- inferência de tipo (mais comum)
let x = 10
let nome = "Ana"

-- tipo explícito
idade: i32 = 25
peso:  f64 = 68.5

-- constante (não muda nunca)
const PI: f64 = 3.14159
pub const MAX: u64 = 1_000_000
```

### 5.2 Tipos primitivos

| Tipo | O que guarda | Exemplo |
|---|---|---|
| `i8` `i16` `i32` `i64` | Inteiro com sinal | `-10`, `0`, `42` |
| `u8` `u16` `u32` `u64` | Inteiro sem sinal (só positivo) | `0`, `255` |
| `f32` `f64` | Número com vírgula | `3.14`, `-0.5` |
| `bool` | Verdadeiro ou falso | `true`, `false` |
| `string` | Texto UTF-8 | `"olá"` |

### 5.3 Literais numéricos

```
let decimal = 1_000_000       -- underscore para legibilidade
let hex     = 0xFF_A0         -- hexadecimal
let bin     = 0b1010_0011     -- binário
let f       = 3.14            -- float
```

### 5.4 Operadores

| Categoria | Operadores |
|---|---|
| Aritmético | `+` `-` `*` `/` `%` |
| Comparação | `==` `!=` `<` `>` `<=` `>=` |
| Lógico | `&&` `\|\|` `!` |
| Atribuição | `=` `+=` `-=` `*=` `/=` `%=` |
| Referência | `&` `*` `ref` `move` |
| Cast | `x as Tipo` |
| Propagação de erro | `expressao?` |
| Range | `0..n` `0..=n` |

### 5.5 Funções

```
-- função simples
fn soma(a i32, b i32) -> i32 {
    return a + b
}

-- função pública (visível fora do arquivo)
pub fn fatorial(n i32) -> i32 {
    if n <= 1 {
        return 1
    }
    return n * fatorial(n - 1)
}
```

### 5.6 Controle de fluxo

```
-- if / else if / else
if x > 0 {
    println("positivo")
} else if x < 0 {
    println("negativo")
} else {
    println("zero")
}

-- while
let i = 0
while i < 10 {
    i += 1
}

-- for com range exclusivo (0 até 9)
for j in 0..10 {
    print(j)
}

-- for com range inclusivo (0 até 10)
for k in 0..=10 {
    print(k)
}

-- loop infinito
loop {
    let entrada = ler()
    if entrada == "sair" { break }
    continue
}
```

---

## 6. Estruturas de dados

### 6.1 Arrays e Slices

```
-- array de tamanho fixo
let arr: [5]i32 = [1, 2, 3, 4, 5]
arr[0] = 10

-- slice — janela sem cópia de memória
let sub  = arr[1..4]     -- elementos 1, 2, 3
let tudo = arr[0..=4]    -- todos inclusive
```

> Slice não copia memória — é uma referência ao array original.  
> O Jarbes verifica os bounds em compilação quando possível.

### 6.2 Struct

```
pub struct Ponto {
    pub x f32
    pub y f32
}
```

### 6.3 Impl — métodos de um tipo

```
impl Ponto {
    fn novo(x f32, y f32) -> Ponto {
        return Ponto(x, y)
    }

    fn distancia(self) -> f32 {
        return (self.x * self.x + self.y * self.y)
    }

    fn mover(self, dx f32, dy f32) {
        self.x += dx
        self.y += dy
    }
}

fn main() {
    let p = Ponto.novo(3.0, 4.0)
    let d = p.distancia()
    p.mover(1.0, 0.0)
}
```

### 6.4 Enum e match

```
enum Forma {
    Circulo(f32),
    Retangulo(f32, f32),
    Triangulo(f32, f32, f32)
}

fn area(f Forma) -> f32 {
    match f {
        Circulo(r)       => 3.14159 * r * r
        Retangulo(l, a)  => l * a
        Triangulo(a,b,c) => {
            let s = (a + b + c) / 2.0
            return (s * (s-a) * (s-b) * (s-c))
        }
        _ => 0.0
    }
}
```

---

## 7. Traits e Generics

### 7.1 Traits

```
trait Imprimivel {
    fn imprimir(self)
}

impl Imprimivel for Ponto {
    fn imprimir(self) {
        print("Ponto(")
        print(self.x)
        print(", ")
        print(self.y)
        println(")")
    }
}
```

### 7.2 Generics

```
struct Pilha<T> {
    dados [100]T
    topo  i32
}

impl<T> Pilha<T> {
    fn empilhar(self, valor T) {
        self.dados[self.topo] = move valor
        self.topo += 1
    }

    fn desempilhar(self) -> Option<T> {
        if self.topo == 0 { return None }
        self.topo -= 1
        return Some(move self.dados[self.topo])
    }
}
```

---

## 8. Modelo de memória

### As três formas

| Mecanismo | Onde vive | Quando é liberado |
|---|---|---|
| Stack (automático) | Bloco atual | Ao sair do bloco |
| `region { }` | Heap com escopo | Ao fechar a chave `}` |
| `unsafe { }` | Heap manual | Você controla |

### Region — heap seguro

```
fn processar() {
    region temp {
        let node = new Node(42)
        let buf  = new [256]u8
        usar(node)
    }
    -- node e buf liberados automaticamente aqui
    usar(node)  -- ERRO: reference escapes region
}
```

### move — transferência de ownership

```
let x = new Node(10)
let y = move x        -- x transferido para y
usar(y)               -- OK
usar(x)               -- ERRO: use of consumed value
```

### unsafe — quando necessário

```
fn acesso_hardware() {
    unsafe {
        -- Jarbes não analisa este bloco
        -- responsabilidade total do programador
        let porta = 0xFF00 as *u8
        *porta = 0x01
    }
}
```

> Use `unsafe` apenas para FFI, drivers ou hardware.  
> Todo bloco unsafe é visível e auditável no código.

---

## 9. Tratamento de erros

### Result<T, E> — erros esperados

```
fn dividir(a i32, b i32) -> Result<i32, string> {
    if b == 0 {
        return Err("divisão por zero")
    }
    return Ok(a / b)
}

match dividir(10, 2) {
    Ok(v)  => println(v)
    Err(e) => println(e)
}
```

### ? — propagação automática

```
fn calcular(a i32, b i32, c i32) -> Result<i32, string> {
    let x = dividir(a, b)?    -- se Err, retorna imediatamente
    let y = dividir(x, c)?
    return Ok(x + y)
}
```

### Option<T> — ausência de valor

```
fn buscar(arr []i32, valor i32) -> Option<i32> {
    for i in 0..arr.length() {
        if arr[i] == valor {
            return Some(i)
        }
    }
    return None
}
```

### panic — erro irrecuperável

```
fn dividir_safe(a i32, b i32) -> i32 {
    if b == 0 {
        panic("divisor não pode ser zero")
    }
    return a / b
}
```

| Quando usar | Mecanismo |
|---|---|
| Falha previsível (IO, parsing, validação) | `Result<T, E>` |
| Valor que pode não existir | `Option<T>` |
| Bug interno, invariante violada | `panic` |

---

## 10. O que o Jarbes detecta

| Detectado em compilação ✔ | Requer unsafe ou runtime ✖ |
|---|---|
| use-before-definition | Erros de lógica de negócio |
| use-after-move | Comportamento de entrada do usuário |
| region escape | Erros de rede / IO |
| dangling pointer | Condições de corrida em unsafe |
| double-free | Erros externos (hardware, SO) |
| buffer overflow (estático) | Bounds dinâmicos em unsafe |

---

## 11. Palavras reservadas

```
module    fn        return    let       const
type      struct    enum      trait     impl
for       pub       self
if        else      while     loop      break
continue  match     in        as        move
region    new       panic     unsafe
true      false
Result    Option    Ok        Err       Some    None
i8  i16   i32  i64  u8  u16   u32  u64
f32 f64   bool      string    ref
```

---

*SysP — segurança começa no compilador, não no runtime.*
