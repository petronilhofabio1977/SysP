-- list.sp
-- Módulo: data_structures.list

pub struct No<T> {
    pub valor   T
    pub proximo *No<T>
}

pub struct Lista<T> {
    pub cabeca   *No<T>
    pub tamanho  i32
}

impl<T> Lista<T> {
    pub fn nova() -> Lista<T> {
        return Lista(cabeca: null, tamanho: 0)
    }

    pub fn inserir(self, valor T) {
        region lista_op {
            let no      = new No(move valor, self.cabeca)
            self.cabeca  = no
            self.tamanho += 1
        }
    }

    pub fn vazia(self) -> bool { return self.cabeca == null }
    pub fn tamanho(self) -> i32 { return self.tamanho }
}
