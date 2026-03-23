-- merge_sort.sp
-- Módulo: algorithms.sort.merge_sort

pub fn merge_sort(arr ref []i32, low i32, high i32) {
    if low < high {
        let meio = (low + high) / 2
        merge_sort(ref arr, low, meio)
        merge_sort(ref arr, meio + 1, high)
        merge(ref arr, low, meio, high)
    }
}

fn merge(arr ref []i32, low i32, meio i32, high i32) {
    let n1 = meio - low + 1
    let n2 = high - meio
    let esq: [100]i32 = []
    let dir: [100]i32 = []

    for i in 0..n1 { esq[i] = arr[low + i] }
    for j in 0..n2 { dir[j] = arr[meio + 1 + j] }

    let i: i32 = 0
    let j: i32 = 0
    let k: i32 = low

    while i < n1 && j < n2 {
        if esq[i] <= dir[j] {
            arr[k] = esq[i]
            i += 1
        } else {
            arr[k] = dir[j]
            j += 1
        }
        k += 1
    }

    while i < n1 { arr[k] = esq[i]; i += 1; k += 1 }
    while j < n2 { arr[k] = dir[j]; j += 1; k += 1 }
}
