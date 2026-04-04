-- ponto.sp: struct Ponto com campos x, y e função distancia ao quadrado

module io.println

struct Ponto {
    x i32
    y i32
}

fn distancia_quad(a Ponto, b Ponto) -> i32 {
    let dx = a.x - b.x
    let dy = a.y - b.y
    return dx * dx + dy * dy
}

fn main() {
    let p1 = Ponto { x: 0, y: 0 }
    let p2 = Ponto { x: 3, y: 4 }

    println("p1.x:")
    println(p1.x)
    println("p1.y:")
    println(p1.y)
    println("p2.x:")
    println(p2.x)
    println("p2.y:")
    println(p2.y)

    let d2 = distancia_quad(p1, p2)
    println("distancia ao quadrado (3,4):")
    println(d2)

    p2.x = 6
    p2.y = 8
    let d3 = distancia_quad(p1, p2)
    println("distancia ao quadrado (6,8):")
    println(d3)
}
