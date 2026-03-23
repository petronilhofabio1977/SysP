-- erro.sp
-- Demonstra erros detectados pelo Jarbes

module io.println

fn main() {
    -- use-after-move detectado em compilação:
    -- let x = new Node(10)
    -- let y = move x
    -- println(x)   -- ERRO: use of consumed value

    -- region escape detectado em compilação:
    -- region temp {
    --     let n = new Node(42)
    -- }
    -- println(n)   -- ERRO: reference escapes region

    println("Jarbes detecta erros em compilação!")
}
