-- move_ok.sp
module io.println
fn main() {
    let x: i32 = 42
    let y = move x
    println(y)
}
