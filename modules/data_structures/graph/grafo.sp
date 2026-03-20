-- grafo.sp
-- Módulo: data_structures.graph

pub struct Grafo {
    pub adj     [100][100]i32
    pub vertices i32
    pub arestas  i32
}

impl Grafo {
    pub fn novo(v i32) -> Grafo {
        return Grafo(adj: [], vertices: v, arestas: 0)
    }

    pub fn adicionar_aresta(self, u i32, v i32) {
        self.adj[u][v] = 1
        self.adj[v][u] = 1
        self.arestas += 1
    }

    pub fn tem_aresta(self, u i32, v i32) -> bool {
        return self.adj[u][v] == 1
    }

    pub fn grau(self, v i32) -> i32 {
        let g: i32 = 0
        for i in 0..self.vertices {
            if self.adj[v][i] == 1 { g += 1 }
        }
        return g
    }
}
