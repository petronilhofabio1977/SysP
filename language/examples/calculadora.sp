-- calculadora.sp: usa math.abs, math.min, math.max, string.length

module io.println

fn fatorial(n i32) -> i32 {
    if n <= 1 { return 1 }
    return n * fatorial(n - 1)
}

fn potencia(base i32, exp i32) -> i32 {
    let resultado: i32 = 1
    let i: i32 = 0
    while i < exp {
        resultado = resultado * base
        i = i + 1
    }
    return resultado
}

fn main() {
    -- operacoes basicas
    println("=== Calculadora SysP ===")

    let a = 10
    let b = -3
    let c = 7

    println("a = 10, b = -3, c = 7")
    println("a + c:")
    println(a + c)

    println("a * c:")
    println(a * c)

    println("a / c:")
    println(a / c)

    println("a % c:")
    println(a % c)

    -- math.abs
    println("abs(b) = abs(-3):")
    println(abs(b))

    println("abs(a) = abs(10):")
    println(abs(a))

    -- math.min / math.max
    println("min(a, c) = min(10, 7):")
    println(min(a, c))

    println("max(a, c) = max(10, 7):")
    println(max(a, c))

    println("min(b, c) = min(-3, 7):")
    println(min(b, c))

    -- fatorial
    println("4! =")
    println(fatorial(4))

    println("6! =")
    println(fatorial(6))

    -- potencia
    println("2^10 =")
    println(potencia(2, 10))

    println("3^5 =")
    println(potencia(3, 5))

    -- string.length
    let msg = "SysP"
    println("len de SysP:")
    println(len(msg))

    -- comparacoes com match
    let x = 42
    println("x = 42, classificacao:")
    match x {
        42 => { println("resposta!") }
        _ => { println("outro") }
    }

    println("=== Fim ===")
}
