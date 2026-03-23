-- gradiente.sp
-- Módulo: numerical.optimization
-- Descida de gradiente

pub fn gradiente_descendente(
    f  fn(f64) -> f64,
    df fn(f64) -> f64,
    x0 f64,
    taxa f64,
    iter i32
) -> f64 {
    let x = x0
    let i: i32 = 0
    while i < iter {
        x = x - taxa * df(x)
        i += 1
    }
    return x
}
