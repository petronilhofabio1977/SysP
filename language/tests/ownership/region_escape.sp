-- region_escape.sp — ERRO: region-escape
module io.println
fn main() {
    region temp {
        let n: i32 = 100
    }
    println(n)
}
