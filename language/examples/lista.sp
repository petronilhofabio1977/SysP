-- lista.sp: lista encadeada com struct No e region para gestao de memoria

module io.println

struct No {
    valor i32
    proximo i32
}

fn soma_lista(a No, b No, c No) -> i32 {
    return a.valor + b.valor + c.valor
}

fn main() {
    let n1 = No { valor: 10, proximo: 0 }
    let n2 = No { valor: 20, proximo: 0 }
    let n3 = No { valor: 30, proximo: 0 }

    println("No 1:")
    println(n1.valor)
    println("No 2:")
    println(n2.valor)
    println("No 3:")
    println(n3.valor)

    let total = soma_lista(n1, n2, n3)
    println("Soma total:")
    println(total)

    println("Media:")
    println(total / 3)

    -- region demonstra escopo de memoria local
    region temp {
        let local = No { valor: 99, proximo: 0 }
        println("No local no region:")
        println(local.valor)
    }

    n2.valor = 50
    println("n2 apos modificacao:")
    println(n2.valor)
}
