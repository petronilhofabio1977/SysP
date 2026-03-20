-- estrutural.sp
-- Módulo: engineering.structural

pub fn tensao(forca f64, area f64) -> f64 {
    return forca / area
}

pub fn deformacao(delta f64, comprimento f64) -> f64 {
    return delta / comprimento
}

pub fn modulo_elasticidade(tensao f64, deformacao f64) -> f64 {
    return tensao / deformacao
}

pub fn momento_inercia_retangulo(base f64, altura f64) -> f64 {
    return (base * altura * altura * altura) / 12.0
}
