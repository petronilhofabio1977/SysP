-- region_ok.sp
module io.println
fn main() {
    region temp {
        let n: i32 = 100
        println(n)
    }
}
