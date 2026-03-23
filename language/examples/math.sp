-- math.sp

module io.println

fn fatorial(n i32) -> i32 {
    if n <= 1 { return 1 }
    return n * fatorial(n - 1)
}

fn fibonacci(n i32) -> i32 {
    if n <= 0 { return 0 }
    if n == 1 { return 1 }
    return fibonacci(n - 1) + fibonacci(n - 2)
}

fn soma(a i32, b i32) -> i32 {
    return a + b
}

fn main() {
    println(fatorial(5))    -- 120
    println(fibonacci(10))  -- 55
    println(soma(7, 3))     -- 10
}
