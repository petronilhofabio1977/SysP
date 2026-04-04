-- resultado.sp: Result<i32, string> com match expression

module io.println

fn divide(a i32, b i32) -> i32 {
    if b == 0 {
        return -1
    }
    return a / b
}

fn classifica(n i32) -> i32 {
    if n > 0 { return 1 }
    if n < 0 { return -1 }
    return 0
}

fn main() {
    -- Ok/Err com match
    let r1 = Ok(42)
    let r2 = Err(0)
    let r3 = Ok(100)

    println("match r1 (Ok 42):")
    match r1 {
        Ok(v) => { println(v) }
        Err(e) => { println("erro") }
    }

    println("match r2 (Err 0):")
    match r2 {
        Ok(v) => { println(v) }
        Err(e) => { println(e) }
    }

    -- Some/None com match
    let s1 = Some(7)
    let s2 = None

    println("match s1 (Some 7):")
    match s1 {
        Some(n) => { println(n) }
        None => { println(0) }
    }

    println("match s2 (None):")
    match s2 {
        Some(n) => { println(n) }
        None => { println(0) }
    }

    -- match em inteiros
    let x = 3
    println("match x (3):")
    match x {
        1 => { println("um") }
        2 => { println("dois") }
        3 => { println("tres") }
        _ => { println("outro") }
    }

    -- divisao com resultado
    println("10 / 2 =")
    println(divide(10, 2))
    println("10 / 0 =")
    println(divide(10, 0))
}
