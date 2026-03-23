-- mochila.sp
-- Módulo: algorithms.dynamic
-- Problema da mochila (knapsack)

pub fn mochila(pesos []i32, valores []i32, n i32, capacidade i32) -> i32 {
    let dp: [100][100]i32 = []

    for i in 0..=n {
        for w in 0..=capacidade {
            if i == 0 || w == 0 {
                dp[i][w] = 0
            } else if pesos[i-1] <= w {
                let com    = valores[i-1] + dp[i-1][w - pesos[i-1]]
                let sem    = dp[i-1][w]
                if com > sem { dp[i][w] = com }
                else         { dp[i][w] = sem }
            } else {
                dp[i][w] = dp[i-1][w]
            }
        }
    }
    return dp[n][capacidade]
}
