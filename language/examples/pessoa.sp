-- pessoa.sp: struct Pessoa com nome (string) e idade (i32)

module io.println

struct Pessoa {
    idade i32
    ativo bool
}

fn eh_maior(p Pessoa) -> bool {
    return p.idade >= 18
}

fn main() {
    let alice = Pessoa { idade: 25, ativo: true }
    let bob   = Pessoa { idade: 15, ativo: false }

    println("Alice idade:")
    println(alice.idade)
    println("Alice ativo:")
    println(alice.ativo)

    println("Bob idade:")
    println(bob.idade)
    println("Bob ativo:")
    println(bob.ativo)

    println("Alice maior de idade:")
    println(eh_maior(alice))
    println("Bob maior de idade:")
    println(eh_maior(bob))

    alice.idade = 30
    println("Alice nova idade:")
    println(alice.idade)
}
