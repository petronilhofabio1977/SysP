-- bubble_sort.sp
-- Módulo: algorithms.sort.bubble_sort

pub fn bubble_sort(arr ref []i32, n i32) {
    for i in 0..n {
        for j in 0..n - i - 1 {
            if arr[j] > arr[j + 1] {
                let temp   = arr[j]
                arr[j]     = arr[j + 1]
                arr[j + 1] = temp
            }
        }
    }
}
