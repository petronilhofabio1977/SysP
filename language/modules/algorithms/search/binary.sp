-- binary.sp
-- Módulo: algorithms.search.binary

pub fn busca_binaria(arr []i32, n i32, alvo i32) -> Option<i32> {
    let esq: i32 = 0
    let dir: i32 = n - 1

    while esq <= dir {
        let meio = (esq + dir) / 2
        if arr[meio] == alvo { return Some(meio) }
        if arr[meio] < alvo  { esq = meio + 1 }
        else                 { dir = meio - 1 }
    }
    return None
}
