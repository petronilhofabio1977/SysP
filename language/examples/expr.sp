-- expr.sp

module io.println

fn main() {
    let a: i32 = 10
    let b: i32 = 3

    let soma      = a + b
    let subtracao = a - b
    let produto   = a * b
    let divisao   = a / b
    let resto     = a % b

    println(soma)
    println(subtracao)
    println(produto)
    println(divisao)
    println(resto)

    println(a > b)
    println(a == b)
    println(a != b)
}
