-- quicksort.sp
-- Módulo: algorithms.sort.quicksort

pub fn quicksort(arr ref []i32, low i32, high i32) {
    if low < high {
        let pivot = particionar(ref arr, low, high)
        quicksort(ref arr, low, pivot - 1)
        quicksort(ref arr, pivot + 1, high)
    }
}

fn particionar(arr ref []i32, low i32, high i32) -> i32 {
    let pivot = arr[high]
    let i: i32 = low - 1
    let j: i32 = low
    while j < high {
        if arr[j] <= pivot {
            i += 1
            let temp = arr[i]
            arr[i]   = arr[j]
            arr[j]   = temp
        }
        j += 1
    }
    let temp     = arr[i + 1]
    arr[i + 1]   = arr[high]
    arr[high]    = temp
    return i + 1
}
