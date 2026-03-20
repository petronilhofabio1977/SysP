-- use_after_move.sp — ERRO: use-after-move
module io.println
fn main() {
    let x: i32 = 42
    let y = move x
    println(y)
    println(x)
}
