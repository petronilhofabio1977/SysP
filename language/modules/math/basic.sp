-- basic.sp
-- Módulo: math.basic

pub const PI:  f64 = 3.14159265358979
pub const E:   f64 = 2.71828182845904
pub const TAU: f64 = 6.28318530717958

pub fn abs(x i32) -> i32 {
    if x < 0 { return x * -1 }
    return x
}

pub fn max(a i32, b i32) -> i32 {
    if a > b { return a }
    return b
}

pub fn min(a i32, b i32) -> i32 {
    if a < b { return a }
    return b
}

pub fn pow(base i32, exp i32) -> i32 {
    let resultado: i32 = 1
    let i: i32 = 0
    while i < exp {
        resultado = resultado * base
        i += 1
    }
    return resultado
}

pub fn fatorial(n i32) -> i32 {
    if n <= 1 { return 1 }
    return n * fatorial(n - 1)
}

pub fn fibonacci(n i32) -> i32 {
    if n <= 0 { return 0 }
    if n == 1 { return 1 }
    return fibonacci(n - 1) + fibonacci(n - 2)
}
