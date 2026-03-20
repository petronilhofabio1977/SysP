-- stats.sp
-- Módulo: numerical.statistics

pub fn media(dados []f64, n i32) -> f64 {
    let soma: f64 = 0.0
    for i in 0..n { soma = soma + dados[i] }
    return soma / n as f64
}

pub fn variancia(dados []f64, n i32) -> f64 {
    let m = media(dados, n)
    let soma: f64 = 0.0
    for i in 0..n {
        let diff = dados[i] - m
        soma = soma + diff * diff
    }
    return soma / n as f64
}

pub fn maximo(dados []f64, n i32) -> f64 {
    let m = dados[0]
    for i in 1..n {
        if dados[i] > m { m = dados[i] }
    }
    return m
}

pub fn minimo(dados []f64, n i32) -> f64 {
    let m = dados[0]
    for i in 1..n {
        if dados[i] < m { m = dados[i] }
    }
    return m
}
