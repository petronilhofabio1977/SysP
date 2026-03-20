-- simulacao.sp
-- Módulo: engineering.simulation

pub struct Particula {
    pub x  f64
    pub y  f64
    pub vx f64
    pub vy f64
    pub massa f64
}

impl Particula {
    pub fn nova(x f64, y f64, massa f64) -> Particula {
        return Particula(x: x, y: y, vx: 0.0, vy: 0.0, massa: massa)
    }

    pub fn atualizar(self, dt f64) {
        self.x = self.x + self.vx * dt
        self.y = self.y + self.vy * dt
    }

    pub fn aplicar_forca(self, fx f64, fy f64, dt f64) {
        self.vx = self.vx + (fx / self.massa) * dt
        self.vy = self.vy + (fy / self.massa) * dt
    }
}
