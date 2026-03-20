-- vector.sp
-- Módulo: geometry.vector

pub struct Vec2 {
    pub x f64
    pub y f64
}

pub struct Vec3 {
    pub x f64
    pub y f64
    pub z f64
}

impl Vec2 {
    pub fn somar(self, outro Vec2) -> Vec2 {
        return Vec2(self.x + outro.x, self.y + outro.y)
    }
    pub fn escalar(self, s f64) -> Vec2 {
        return Vec2(self.x * s, self.y * s)
    }
    pub fn ponto(self, outro Vec2) -> f64 {
        return self.x * outro.x + self.y * outro.y
    }
}

impl Vec3 {
    pub fn somar(self, outro Vec3) -> Vec3 {
        return Vec3(self.x + outro.x, self.y + outro.y, self.z + outro.z)
    }
    pub fn escalar(self, s f64) -> Vec3 {
        return Vec3(self.x * s, self.y * s, self.z * s)
    }
    pub fn ponto(self, outro Vec3) -> f64 {
        return self.x * outro.x + self.y * outro.y + self.z * outro.z
    }
    pub fn cruz(self, outro Vec3) -> Vec3 {
        return Vec3(
            self.y * outro.z - self.z * outro.y,
            self.z * outro.x - self.x * outro.z,
            self.x * outro.y - self.y * outro.x
        )
    }
}
