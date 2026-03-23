-- string.sp
-- Módulo: string

pub fn tamanho(s string) -> i32 {
    let n: i32 = 0
    while s[n] != '\0' { n += 1 }
    return n
}

pub fn contem(s string, sub string) -> bool {
    let ns = tamanho(s)
    let nb = tamanho(sub)
    for i in 0..ns - nb {
        let achou: bool = true
        for j in 0..nb {
            if s[i + j] != sub[j] { achou = false }
        }
        if achou { return true }
    }
    return false
}

pub fn vazia(s string) -> bool {
    return tamanho(s) == 0
}
