-- uninit.sp — ERRO: uninitialized-use
module io.println
fn main() {
    let x: i32
    println(x)
}
