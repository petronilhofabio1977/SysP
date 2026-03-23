-- gauss.sp
-- Módulo: numerical.linear_algebra
-- Eliminação de Gauss para sistemas lineares

pub fn gauss(a ref [][]f64, b ref []f64, n i32) -> []f64 {
    for i in 0..n {
        for k in i+1..n {
            let fator = a[k][i] / a[i][i]
            for j in i..n {
                a[k][j] = a[k][j] - fator * a[i][j]
            }
            b[k] = b[k] - fator * b[i]
        }
    }
    let x: [100]f64 = []
    for i in 0..n {
        let idx = n - 1 - i
        x[idx] = b[idx] / a[idx][idx]
        for j in idx+1..n {
            x[idx] = x[idx] - a[idx][j] * x[j] / a[idx][idx]
        }
    }
    return x
}
