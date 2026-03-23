-- matrix.sp
-- Módulo: geometry.matrix

pub struct Matriz {
    pub dados [100]f64
    pub linhas i32
    pub colunas i32
}

impl Matriz {
    pub fn nova(linhas i32, colunas i32) -> Matriz {
        return Matriz(dados: [], linhas: linhas, colunas: colunas)
    }

    pub fn get(self, i i32, j i32) -> f64 {
        return self.dados[i * self.colunas + j]
    }

    pub fn set(self, i i32, j i32, val f64) {
        self.dados[i * self.colunas + j] = val
    }
}
