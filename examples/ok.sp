-- ok.sp

module io.println

fn dividir(a i32, b i32) -> Result<i32, string> {
    if b == 0 {
        return Err("divisão por zero")
    }
    return Ok(a / b)
}

fn main() {
    match dividir(10, 2) {
        Ok(v)  => println(v)
        Err(e) => println(e)
    }

    match dividir(10, 0) {
        Ok(v)  => println(v)
        Err(e) => println(e)
    }
}
