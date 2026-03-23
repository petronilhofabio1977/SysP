-- lcs.sp
-- Módulo: algorithms.dynamic
-- Longest Common Subsequence

pub fn lcs(a string, b string, m i32, n i32) -> i32 {
    let dp: [100][100]i32 = []

    for i in 0..=m {
        for j in 0..=n {
            if i == 0 || j == 0 {
                dp[i][j] = 0
            } else if a[i-1] == b[j-1] {
                dp[i][j] = dp[i-1][j-1] + 1
            } else {
                if dp[i-1][j] > dp[i][j-1] { dp[i][j] = dp[i-1][j] }
                else                        { dp[i][j] = dp[i][j-1] }
            }
        }
    }
    return dp[m][n]
}
