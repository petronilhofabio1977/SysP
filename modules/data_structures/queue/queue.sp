-- queue.sp
-- Módulo: data_structures.queue

pub struct Fila<T> {
    pub dados  [100]T
    pub frente i32
    pub tras   i32
}

impl<T> Fila<T> {
    pub fn nova() -> Fila<T> {
        return Fila(dados: [], frente: 0, tras: 0)
    }

    pub fn enfileirar(self, valor T) {
        self.dados[self.tras] = move valor
        self.tras += 1
    }

    pub fn desenfileirar(self) -> Option<T> {
        if self.frente == self.tras { return None }
        let val = move self.dados[self.frente]
        self.frente += 1
        return Some(move val)
    }

    pub fn vazia(self) -> bool { return self.frente == self.tras }
    pub fn tamanho(self) -> i32 { return self.tras - self.frente }
}
