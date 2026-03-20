-- non_exhaustive.sp — ERRO: non-exhaustive-match
module io.println
fn verificar(x Option<i32>) {
    match x {
        Some(v) => println(v)
    }
}
fn main() {
    verificar(Some(42))
}
