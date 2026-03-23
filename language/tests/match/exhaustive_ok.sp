-- exhaustive_ok.sp — deve passar
module io.println
fn verificar(x Option<i32>) {
    match x {
        Some(v) => println(v)
        None    => println("vazio")
    }
}
fn main() {
    verificar(Some(42))
}
