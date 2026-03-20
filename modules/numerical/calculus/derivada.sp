-- derivada.sp
-- Módulo: numerical.calculus

pub fn derivada(f fn(f64) -> f64, x f64) -> f64 {
    let h: f64 = 0.000001
    return (f(x + h) - f(x)) / h
}

pub fn integral(f fn(f64) -> f64, a f64, b f64, n i32) -> f64 {
    let h = (b - a) / n as f64
    let soma: f64 = 0.0
    let i: i32 = 0
    while i < n {
        soma = soma + f(a + i as f64 * h)
        i += 1
    }
    return soma * h
}
