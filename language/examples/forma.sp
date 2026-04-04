-- forma.sp: enum Forma com Circulo(raio) e Retangulo(largura, altura)

module io.println

enum Forma {
    Circulo(i32),
    Retangulo(i32, i32)
}

fn area_circulo(r i32) -> i32 {
    return r * r * 3
}

fn area_retangulo(w i32, h i32) -> i32 {
    return w * h
}

fn main() {
    let c  = Circulo(5)
    let r  = Retangulo(3, 4)
    let r2 = Retangulo(6, 7)

    println("Circulo raio=5, area aprox:")
    match c {
        Circulo(raio) => { println(area_circulo(raio)) }
        Retangulo(w, h) => { println(0) }
    }

    println("Retangulo 3x4, area:")
    match r {
        Circulo(raio) => { println(0) }
        Retangulo(w, h) => { println(area_retangulo(w, h)) }
    }

    println("Retangulo 6x7, area:")
    match r2 {
        Circulo(raio) => { println(0) }
        Retangulo(w, h) => { println(area_retangulo(w, h)) }
    }

    println("c e Circulo?")
    match c {
        Circulo(x) => { println(1) }
        _ => { println(0) }
    }

    println("r e Retangulo?")
    match r {
        Circulo(x) => { println(0) }
        Retangulo(w, h) => { println(1) }
    }
}
