-- bst.sp
-- Módulo: data_structures.tree
-- Árvore Binária de Busca

pub struct NoArvore {
    pub valor    i32
    pub esquerda *NoArvore
    pub direita  *NoArvore
}

pub fn inserir(raiz *NoArvore, valor i32) -> *NoArvore {
    if raiz == null {
        region arvore_op {
            let no = new NoArvore(valor, null, null)
            return no
        }
    }
    if valor < raiz.valor {
        raiz.esquerda = inserir(raiz.esquerda, valor)
    } else {
        raiz.direita = inserir(raiz.direita, valor)
    }
    return raiz
}

pub fn buscar(raiz *NoArvore, valor i32) -> bool {
    if raiz == null   { return false }
    if raiz.valor == valor { return true }
    if valor < raiz.valor  { return buscar(raiz.esquerda, valor) }
    return buscar(raiz.direita, valor)
}
