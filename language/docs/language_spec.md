# SysP — Especificação da Linguagem

> Referência técnica formal  
> Versão: 1.2 · Grammar: v7.0 Final

---

## 1. Arquivos

| Propriedade | Valor |
|---|---|
| Extensão | `.sp` |
| Codificação | UTF-8 |
| Primeira linha | `-- nome_do_arquivo.sp` (obrigatório) |
| Ponto de entrada | arquivo `main.sp` com função `fn main()` |

---

## 2. Estrutura de um arquivo

```
-- nome_do_arquivo.sp          ← linha 1: sempre o nome do arquivo

module io.println              ← módulos da linguagem (opcional)
module algorithms.sort

fn main() {                    ← declarações e funções
    ...
}
```

### Ordem obrigatória

1. Comentário com nome do arquivo
2. Chamadas de módulos (`module`)
3. Declarações (funções, structs, enums, traits, constantes)

---

## 3. Comentários

```
-- comentário de linha

(* comentário
   de bloco *)
```

Ambos são removidos pelo lexer antes do parsing.

---

## 4. Módulos da linguagem

Módulos são partes da própria linguagem SysP. Não são criados pelo usuário.

```
module io.println
module algorithms.sort.quicksort
module math.matrix
```

### Lista de módulos disponíveis

```
io
    io.println
    io.print
    io.fs
    io.string

algorithms
    algorithms.sort.quicksort
    algorithms.sort.mergesort
    algorithms.sort.heapsort
    algorithms.search.binary
    algorithms.search.linear
    algorithms.graph.bfs
    algorithms.graph.dfs
    algorithms.graph.dijkstra

data_structures
    data_structures.list
    data_structures.tree
    data_structures.hash

math
    math.matrix
    math.vector
    math.numerical

geometry
```

---

## 5. Tipos

### 5.1 Tipos primitivos

| Tipo | Tamanho | Descrição |
|---|---|---|
| `i8` | 8 bits | Inteiro com sinal |
| `i16` | 16 bits | Inteiro com sinal |
| `i32` | 32 bits | Inteiro com sinal |
| `i64` | 64 bits | Inteiro com sinal |
| `u8` | 8 bits | Inteiro sem sinal |
| `u16` | 16 bits | Inteiro sem sinal |
| `u32` | 32 bits | Inteiro sem sinal |
| `u64` | 64 bits | Inteiro sem sinal |
| `f32` | 32 bits | Ponto flutuante |
| `f64` | 64 bits | Ponto flutuante |
| `bool` | 1 bit | `true` ou `false` |
| `string` | variável | Texto UTF-8 |

> Todos os tipos primitivos implementam `Copy` automaticamente.

### 5.2 Tipos compostos

| Tipo | Sintaxe | Exemplo |
|---|---|---|
| Array fixo | `[N]T` | `[5]i32` |
| Slice | `[]T` | `[]i32` |
| Tupla | `(T, U)` | `(i32, string)` |
| Ponteiro | `*T` | `*i32` |
| Referência | `ref T` | `ref i32` |
| Result | `Result<T, E>` | `Result<i32, string>` |
| Option | `Option<T>` | `Option<i32>` |
| Task | `Task<T>` | `Task<i32>` |
| Channel | `Channel<T>` | `Channel<string>` |
| Lambda | `fn(T) -> U` | `fn(i32) -> bool` |

---

## 6. Variáveis

```
-- inferência de tipo
let x = 10

-- tipo explícito
x: i32 = 10

-- destructuring de tupla
let (q, r) = dividir(10, 3)

-- constante local
const PI: f64 = 3.14159

-- constante pública
pub const MAX: u64 = 1_000_000
```

### Operadores de atribuição

| Operador | Descrição |
|---|---|
| `=` | atribuição simples |
| `+=`  `-=`  `*=`  `/=`  `%=` | aritmético |
| `&=`  `\|=`  `^=` | bitwise |
| `<<=`  `>>=` | shift |

---

## 7. Literais

```
-- inteiro decimal
let a = 1_000_000

-- hexadecimal
let b = 0xFF_A0

-- binário
let c = 0b1010_0011

-- float
let d = 3.14

-- string simples
let e = "olá, mundo"

-- string interpolada
let nome = "SysP"
let msg  = f"Bem-vindo ao {nome}!"

-- tupla
let par: (i32, i32) = (10, 20)

-- bool
let ok = true
```

### Sequências de escape em strings

| Escape | Caractere |
|---|---|
| `\n` | nova linha |
| `\t` | tabulação |
| `\r` | retorno de carro |
| `\"` | aspas duplas |
| `\\` | barra invertida |
| `\0` | nulo |
| `\{` | chave literal (em strings interpoladas) |

### String interpolada

Prefixo `f` antes das aspas. Qualquer expressão dentro de `{ }` é avaliada e convertida para string via o trait `Display`.

```
let x = 42
let s = f"O valor é {x} e o dobro é {x * 2}"
```

---

## 8. Tuplas

Agrupam valores de tipos diferentes em um único valor.

```
-- declaração
let par: (i32, i32) = (10, 20)

-- inferência
let dados = ("Ana", 25, true)

-- acesso por índice
let nome  = dados.0    -- "Ana"
let idade = dados.1    -- 25

-- destructuring
let (nome, idade, ativo) = dados

-- retorno de função
fn dividir(a i32, b i32) -> (i32, i32) {
    return (a / b, a % b)
}

let (quociente, resto) = dividir(10, 3)
```

---

## 9. Funções

```
-- função privada
fn soma(a i32, b i32) -> i32 {
    return a + b
}

-- função pública
pub fn fatorial(n i32) -> i32 {
    if n <= 1 { return 1 }
    return n * fatorial(n - 1)
}

-- sem retorno
fn imprimir(msg string) {
    println(msg)
}

-- método com self
fn distancia(self) -> f64 {
    return self.x * self.x + self.y * self.y
}

-- parâmetro por referência
fn incrementar(p ref i32) {
    *p += 1
}

-- genérico com bound
fn maior<T: Ord>(a T, b T) -> T {
    if a.cmp(ref b) > 0 { return a }
    return b
}

-- genérico com where
fn processar<T, U>(x T, y U) where T: Display, U: Clone {
    println(x.format())
}

-- lambda (função anônima)
let dobrar = fn(x i32) -> i32 { return x * 2 }
let resultado = dobrar(5)    -- 10
```

---

## 10. Controle de fluxo

### if / else

```
if x > 0 {
    println("positivo")
} else if x < 0 {
    println("negativo")
} else {
    println("zero")
}
```

### while

```
while condicao {
    ...
}
```

### for — itera sobre qualquer Iterator

```
-- range exclusivo (0 até n-1)
for i in 0..n { print(i) }

-- range inclusivo (0 até n)
for i in 0..=n { print(i) }

-- slice ou array
for item in minha_lista { println(item) }

-- canal
for msg in canal { processar(msg) }
```

### loop

```
-- loop simples
loop {
    if condicao { break }
    continue
}

-- loop com valor de retorno
let resultado = loop {
    let v = calcular()
    if v > 100 { break v }
}
```

---

## 11. Match

```
match valor {
    padrao => expressao
    padrao => {
        bloco
    }
    _ => default
}
```

### Padrões disponíveis

```
match x {
    0           => println("zero")          -- literal
    1..=10      => println("entre 1 e 10")  -- range
    n           => println(n)               -- captura
    _           => println("outro")         -- wildcard

    Some(v)     => println(v)               -- Option
    None        => println("vazio")

    Ok(v)       => println(v)               -- Result
    Err(e)      => println(e)

    (a, b)      => println(a)              -- tupla

    Forma(x, y) => ...                     -- enum com dados
}
```

### Guards — condição extra no padrão

```
match x {
    Some(v) if v > 0  => println("positivo")
    Some(v) if v < 0  => println("negativo")
    Some(v)           => println("zero")
    None              => println("vazio")
}
```

---

## 12. Structs

```
pub struct Ponto {
    pub x f32
    pub y f32
}

-- genérica
pub struct Par<T, U> {
    pub primeiro T
    pub segundo  U
}

-- com where clause
pub struct Cache<K, V> where K: Eq {
    chave K
    valor V
}
```

---

## 13. Enums

```
enum Direcao {
    Norte,
    Sul,
    Leste,
    Oeste
}

enum Forma {
    Circulo(f32),
    Retangulo(f32, f32),
    Triangulo(f32, f32, f32)
}

enum Resultado<T, E> {
    Sucesso(T),
    Falha(E)
}
```

---

## 14. Traits

### Declaração

```
trait Display {
    fn format(self) -> string
}

trait Comparavel {
    fn menor(self, outro ref Self) -> bool
    fn maior(self, outro ref Self) -> bool {
        return !self.menor(outro)    -- implementação padrão
    }
}
```

### Implementação

```
impl Display for Ponto {
    fn format(self) -> string {
        return f"Ponto({self.x}, {self.y})"
    }
}
```

### Traits padrão da linguagem

| Trait | Métodos | Descrição |
|---|---|---|
| `Display` | `format(self) -> string` | Conversão para string |
| `Iterator` | `next(self) -> Option<Item>` | Iteração sequencial |
| `Eq` | `eq / ne` | Igualdade |
| `Ord` | `cmp / lt / gt` (lt/gt com default) | Ordenação |
| `Clone` | `clone(self) -> Self` | Cópia explícita |
| `Copy` | marker trait | Cópia implícita (atribuição) |
| `Drop` | `drop(self)` | Destruição ao sair do escopo |

> Tipos primitivos implementam `Copy` automaticamente.

---

## 15. Generics

```
-- struct genérica
struct Pilha<T> {
    dados [100]T
    topo  i32
}

-- impl genérico
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

-- função genérica com bound
fn maior<T: Ord>(a T, b T) -> T {
    if a.cmp(ref b) > 0 { return a }
    return b
}

-- múltiplos bounds
fn imprimir_maior<T: Ord + Display>(a T, b T) {
    let m = maior(a, b)
    println(m.format())
}

-- where clause
fn complexo<T, U>(x T, y U) -> string
    where T: Display + Ord,
          U: Clone
{
    return x.format()
}
```

---

## 16. Modelo de memória

### Stack — automático

```
fn calcular() {
    let x: i32 = 10    -- stack, liberado ao sair da função
    let y: f64 = 3.14
}
```

### Region — heap com escopo

```
region temp {
    let node = new Node(42)    -- heap, pertence à região
    let buf  = new [256]u8
    usar(node)
}
-- node e buf liberados aqui automaticamente
usar(node)  -- ERRO: reference escapes region
```

### Move — transferência de ownership

```
let x = new Node(10)
let y = move x         -- ownership transferido para y
usar(y)                -- OK
usar(x)                -- ERRO: use of consumed value
```

### Drop — liberação explícita antecipada

```
region temp {
    let buf = new [1024]u8
    processar(buf)
    drop(buf)           -- liberado aqui, antes de fechar a região
    -- qualquer uso de buf depois = ERRO
}
```

### Referências

```
fn incrementar(p ref i32) {
    *p += 1
}

let x = 5
incrementar(ref x)    -- x agora é 6
```

### Unsafe

```
unsafe {
    -- Jarbes não analisa este bloco
    -- responsabilidade total do programador
    let porta = 0xFF00 as *u8
    *porta = 0x01
}
```

---

## 17. Tratamento de erros

### Result

```
fn dividir(a i32, b i32) -> Result<i32, string> {
    if b == 0 { return Err("divisão por zero") }
    return Ok(a / b)
}

match dividir(10, 2) {
    Ok(v)  => println(v)
    Err(e) => println(e)
}
```

### Propagação com ?

```
fn calcular(a i32, b i32, c i32) -> Result<i32, string> {
    let x = dividir(a, b)?    -- retorna Err se falhar
    let y = dividir(x, c)?
    return Ok(x + y)
}
```

### Option

```
fn buscar(arr []i32, valor i32) -> Option<i32> {
    for i in 0..arr.length() {
        if arr[i] == valor { return Some(i) }
    }
    return None
}
```

### Panic

```
panic("estado inválido — nunca deveria acontecer")
```

| Quando usar | Mecanismo |
|---|---|
| Falha previsível e recuperável | `Result<T, E>` |
| Valor que pode não existir | `Option<T>` |
| Bug interno, invariante violada | `panic` |

---

## 18. Concorrência

### Criando um canal

```
let ch: Channel<i32> = channel()
```

### Lançando uma tarefa

```
let tarefa = spawn calcular(x, y)    -- executa concorrentemente
```

### Enviando e recebendo

```
send ch <- 42          -- envia valor ao canal
let v = recv ch        -- recebe valor (bloqueia até chegar)
```

### Aguardando resultado

```
let resultado = await tarefa    -- aguarda Task<T> terminar
```

### Iterando sobre canal

```
for msg in canal {
    processar(msg)
}
```

### Select — múltiplos canais

```
select {
    recv ch1 -> v => { processar(v) }
    recv ch2 -> v => { logar(v) }
    default       => { esperar() }
}
```

### Exemplo completo

```
-- concorrencia.sp

module io.println

fn calcular(n i32) -> i32 {
    return n * n
}

fn main() {
    let ch: Channel<i32> = channel()

    let t1 = spawn calcular(10)
    let t2 = spawn calcular(20)

    let r1 = await t1    -- 100
    let r2 = await t2    -- 400

    println(f"Resultados: {r1} e {r2}")
}
```

---

## 19. Operadores

### Precedência (do menor para o maior)

| Nível | Operadores | Categoria |
|---|---|---|
| 1 | `\|\|` | lógico ou |
| 2 | `&&` | lógico e |
| 3 | `\|` | bitwise ou |
| 4 | `^` | bitwise xor |
| 5 | `&` | bitwise e |
| 6 | `==`  `!=` | igualdade |
| 7 | `<`  `>`  `<=`  `>=` | relacional |
| 8 | `<<`  `>>` | shift |
| 9 | `+`  `-` | aditivo |
| 10 | `*`  `/`  `%` | multiplicativo |
| 11 | `-`  `!`  `~`  `*`  `&` | unário |
| 12 | `as` | cast |
| 13 | `()`  `.`  `[]`  `?` | postfix |

### Bitwise

```
let a = 0b1100
let b = 0b1010

let e   = a & b     -- 0b1000  AND
let ou  = a | b     -- 0b1110  OR
let xor = a ^ b     -- 0b0110  XOR
let inv = ~a        -- NOT
let esq = a << 2    -- shift left
let dir = a >> 1    -- shift right

a &= 0xFF           -- atribuição bitwise
```

---

## 20. Palavras reservadas

```
module    fn        return    let       const
type      struct    enum      trait     impl
for       pub       self      where
if        else      while     loop      break
continue  match     in        as        move
region    new       drop      panic     unsafe
spawn     send      recv      await     channel
select    default
true      false
Result    Option    Ok        Err       Some    None
Task      Channel
Display   Iterator  Eq        Ord       Clone   Copy    Drop
i8  i16   i32  i64  u8  u16   u32  u64
f32 f64   bool      string    ref
```

---

*SysP Language Specification v1.2 — documento técnico formal da linguagem.*
