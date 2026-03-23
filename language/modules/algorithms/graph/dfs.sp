-- dfs.sp
-- Módulo: algorithms.graph.dfs
-- Busca em profundidade (Depth-First Search)

pub fn dfs(grafo [][]i32, visitado ref []bool, vertice i32, n i32) {
    visitado[vertice] = true

    for i in 0..n {
        if grafo[vertice][i] == 1 && !visitado[i] {
            dfs(grafo, ref visitado, i, n)
        }
    }
}
