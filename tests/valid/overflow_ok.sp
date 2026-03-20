-- overflow_ok.sp
module io.println
fn main() {
    let x: i32 = 2147483646 + 1
    println(x)
}
