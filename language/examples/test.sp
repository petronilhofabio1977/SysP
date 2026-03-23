-- test.sp

module io.println

fn maior(a i32, b i32) -> i32 {
    if a > b { return a }
    return b
}

fn esta_entre(x i32, min i32, max i32) -> bool {
    return x >= min && x <= max
}

fn main() {
    println(maior(10, 20))
    println(maior(30, 5))
    println(esta_entre(5, 1, 10))
    println(esta_entre(15, 1, 10))

    let soma: i32 = 0
    for i in 1..=10 {
        soma += i
    }
    println(soma)

    let x = 7
    if x > 5 {
        println("maior que 5")
    } else {
        println("menor ou igual a 5")
    }
}
