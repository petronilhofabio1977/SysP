-- convex_hull.sp
-- Módulo: geometry.computational

pub struct Ponto {
    pub x f64
    pub y f64
}

pub fn orientacao(p Ponto, q Ponto, r Ponto) -> i32 {
    let val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y)
    if val == 0.0 { return 0 }
    if val > 0.0  { return 1 }
    return 2
}
