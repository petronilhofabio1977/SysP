-- bfs.sp
-- Módulo: algorithms.graph.bfs
-- Busca em largura (Breadth-First Search)

pub fn bfs(grafo [][]i32, n i32, inicio i32) {
    let visitado: [100]bool = []
    let fila: [100]i32 = []
    let frente: i32 = 0
    let tras: i32   = 0

    visitado[inicio] = true
    fila[tras] = inicio
    tras += 1

    while frente < tras {
        let vertice = fila[frente]
        frente += 1

        for i in 0..n {
            if grafo[vertice][i] == 1 && !visitado[i] {
                visitado[i] = true
                fila[tras]  = i
                tras += 1
            }
        }
    }
}
