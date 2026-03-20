-- i32_overflow.sp — ERRO: integer-overflow
module io.println
fn main() {
    let x: i32 = 2147483647 + 1
    println(x)
}
