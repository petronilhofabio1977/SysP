-- linear.sp
-- Módulo: algorithms.search.linear

pub fn busca_linear(arr []i32, n i32, alvo i32) -> Option<i32> {
    for i in 0..n {
        if arr[i] == alvo { return Some(i) }
    }
    return None
}
