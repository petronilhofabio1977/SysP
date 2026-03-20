-- stack.sp
-- Módulo: data_structures.stack

pub struct Pilha<T> {
    pub dados [100]T
    pub topo  i32
}

impl<T> Pilha<T> {
    pub fn nova() -> Pilha<T> {
        return Pilha(dados: [], topo: 0)
    }

    pub fn empilhar(self, valor T) {
        self.dados[self.topo] = move valor
        self.topo += 1
    }

    pub fn desempilhar(self) -> Option<T> {
        if self.topo == 0 { return None }
        self.topo -= 1
        return Some(move self.dados[self.topo])
    }

    pub fn vazia(self) -> bool { return self.topo == 0 }
    pub fn tamanho(self) -> i32 { return self.topo }
}
