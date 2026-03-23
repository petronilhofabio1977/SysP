-- kmp.sp
-- Módulo: algorithms.string
-- Knuth-Morris-Pratt pattern matching

pub fn kmp(texto string, padrao string, n i32, m i32) -> Option<i32> {
    let lps: [100]i32 = []

    -- computar lps
    let len: i32 = 0
    let i: i32   = 1
    lps[0] = 0

    while i < m {
        if padrao[i] == padrao[len] {
            len += 1
            lps[i] = len
            i += 1
        } else {
            if len != 0 { len = lps[len - 1] }
            else        { lps[i] = 0; i += 1 }
        }
    }

    -- busca
    let j: i32 = 0
    i = 0
    while i < n {
        if padrao[j] == texto[i] { i += 1; j += 1 }
        if j == m { return Some(i - j) }
        else if i < n && padrao[j] != texto[i] {
            if j != 0 { j = lps[j - 1] }
            else      { i += 1 }
        }
    }
    return None
}
