# SysP — Especificação da Linguagem

> Referência técnica formal  
> Versão: 1.1 · Grammar: v5.0 Final

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

### 5.2 Tipos compostos

| Tipo | Sintaxe | Exemplo |
|---|---|---|
| Array fixo | `[N]T` | `[5]i32` |
| Slice | `[]T` | `[]i32` |
| Ponteiro | `*T` | `*i32` |
| Referência | `ref T` | `ref i32` |
| Genérico | `T<A>` | `Option<i32>` |
| Result | `Result<T, E>` | `Result<i32, string>` |
| Option | `Option<T>` | `Option<i32>` |

---

## 6. Variáveis

```
-- inferência de tipo
let x = 10

-- tipo explícito
x: i32 = 10

-- constante local
const PI: f64 = 3.14159

-- constante pública
pub const MAX: u64 = 1_000_000
```

### Operadores de atribuição

| Operador | Equivalente |
|---|---|
| `x = y` | atribuição simples |
| `x += y` | `x = x + y` |
| `x -= y` | `x = x - y` |
| `x *= y` | `x = x * y` |
| `x /= y` | `x = x / y` |
| `x %= y` | `x = x % y` |

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

-- string
let e = "olá, mundo"

-- bool
let f = true
let g = false
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

---

## 8. Funções

```
-- função privada
fn nome(parametro tipo) -> tipo_retorno {
    return valor
}

-- função pública
pub fn nome(parametro tipo) -> tipo_retorno {
    return valor
}

-- sem retorno
fn nome(parametro tipo) {
    ...
}

-- função com self (método)
fn nome(self) -> tipo {
    return self.campo
}

-- parâmetro por referência
fn nome(parametro ref tipo) {
    *parametro += 1
}
```

---

## 9. Controle de fluxo

### if / else

```
if condicao {
    ...
} else if outra_condicao {
    ...
} else {
    ...
}
```

### while

```
while condicao {
    ...
}
```

### for

```
-- range exclusivo (0 até n-1)
for i in 0..n {
    ...
}

-- range inclusivo (0 até n)
for i in 0..=n {
    ...
}
```

### loop

```
loop {
    if condicao { break }
    continue
}
```

---

## 10. Match

```
match valor {
    padrao1 => expressao
    padrao2 => {
        bloco
    }
    _ => padrao_default
}
```

### Padrões válidos

```
match x {
    0        => println("zero")
    n        => println(n)           -- captura valor
    _        => println("outro")     -- wildcard

    Some(v)  => println(v)
    None     => println("vazio")

    Ok(v)    => println(v)
    Err(e)   => println(e)

    Forma(a, b) => ...               -- enum com dados
}
```

---

## 11. Structs

```
pub struct Nome {
    pub campo1 tipo1
    pub campo2 tipo2
        campo3 tipo3    -- privado (sem pub)
}
```

---

## 12. Enums

```
enum Nome {
    Variante1,
    Variante2(tipo),
    Variante3(tipo1, tipo2)
}
```

---

## 13. Traits

```
trait Nome {
    fn metodo(self) -> tipo
    fn outro(self, param ref tipo) -> tipo
}

impl Nome for Struct {
    fn metodo(self) -> tipo {
        return self.campo
    }
}
```

---

## 14. Generics

```
struct Nome<T> {
    campo T
}

impl<T> Nome<T> {
    fn metodo(self) -> T {
        return self.campo
    }
}
```

---

## 15. Modelo de memória

### Stack

Variáveis locais vivem na stack e são liberadas ao sair do bloco automaticamente.

### Region

```
region nome {
    let x = new Tipo(valor)    -- alocado no heap, pertence à região
}
-- x liberado automaticamente aqui
```

Regras das regiões:
- Cada valor pertence a exatamente uma região
- Ao fechar `}` todos os valores da região são liberados
- Referências a valores de uma região não podem escapar dela

### Move

```
let x = new Tipo(valor)
let y = move x    -- ownership transferido para y
                  -- x está consumed, não pode ser usado
```

### Referências

```
fn funcao(p ref i32) {
    *p += 1    -- modifica o valor original
}

let x = 5
funcao(ref x)
```

### Unsafe

```
unsafe {
    -- Jarbes não analisa este bloco
    -- responsabilidade total do programador
}
```

---

## 16. Tratamento de erros

### Result

```
fn operacao() -> Result<i32, string> {
    return Ok(42)
    return Err("mensagem de erro")
}
```

### Option

```
fn buscar() -> Option<i32> {
    return Some(42)
    return None
}
```

### Propagação com ?

```
fn composta() -> Result<i32, string> {
    let x = operacao()?    -- retorna Err automaticamente se falhar
    return Ok(x + 1)
}
```

### Panic

```
panic("mensagem")    -- encerra o programa imediatamente
```

---

## 17. Operadores

| Categoria | Operadores | Precedência |
|---|---|---|
| Aritmético | `*` `/` `%` | Alta |
| Aritmético | `+` `-` | Média |
| Comparação | `<` `>` `<=` `>=` | Média |
| Comparação | `==` `!=` | Média |
| Lógico | `&&` | Baixa |
| Lógico | `\|\|` | Baixa |
| Unário | `-` `!` `*` `&` | - |
| Cast | `as` | - |
| Erro | `?` | - |

---

## 18. Palavras reservadas

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

*SysP Language Specification — documento técnico formal da linguagem.*
