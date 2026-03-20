-- fisica.sp
-- Módulo: engineering.physics

pub const G:   f64 = 9.81    -- aceleração gravitacional (m/s²)
pub const C:   f64 = 299792458.0  -- velocidade da luz (m/s)

pub fn energia_cinetica(massa f64, velocidade f64) -> f64 {
    return 0.5 * massa * velocidade * velocidade
}

pub fn energia_potencial(massa f64, altura f64) -> f64 {
    return massa * G * altura
}

pub fn forca(massa f64, aceleracao f64) -> f64 {
    return massa * aceleracao
}

pub fn velocidade_final(v0 f64, a f64, t f64) -> f64 {
    return v0 + a * t
}

pub fn distancia(v0 f64, a f64, t f64) -> f64 {
    return v0 * t + 0.5 * a * t * t
}
