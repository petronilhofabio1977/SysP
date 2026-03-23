-- type_mismatch.sp — ERRO esperado: type-mismatch
module io.println
fn main() {
    let x: i32 = 42
    let y: string = "hello"
    let z = x + y
    println(z)
}
