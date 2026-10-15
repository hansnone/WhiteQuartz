#version 330 core

in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D uWindowTexture;
uniform vec2 uWindowSize;
uniform float uCornerRadius; // Típicamente 11.0
uniform float uPower;        // n = 4.5 para continuidad G2
uniform vec4 uBorderColor;   // Borde perimetral interior 1px (blanco sutil en dark, gris en light)

void main() {
    vec2 pixelPos = vTexCoord * uWindowSize;

    // Localizar el origen de la esquina superelíptica más cercana
    vec2 cornerOrigin = vec2(
        (pixelPos.x < uCornerRadius) ? uCornerRadius : ((pixelPos.x > uWindowSize.x - uCornerRadius) ? uWindowSize.x - uCornerRadius : pixelPos.x),
        (pixelPos.y < uCornerRadius) ? uCornerRadius : ((pixelPos.y > uWindowSize.y - uCornerRadius) ? uWindowSize.y - uCornerRadius : pixelPos.y)
    );

    vec2 d = abs(pixelPos - cornerOrigin);

    // Ecuación de Lamé para superelipse: (|x/r|^n + |y/r|^n)^(1/n) <= 1.0
    float squircleDistance = pow(pow(d.x / uCornerRadius, uPower) + pow(d.y / uCornerRadius, uPower), 1.0 / uPower);

    if (squircleDistance > 1.0) {
        discard; // Descartar fragmentos fuera de la curvatura G2
    }

    vec4 baseColor = texture(uWindowTexture, vTexCoord);

    // Borde perimetral interior subpíxel de 1px
    float borderFactor = smoothstep(1.0 - (1.0 / uCornerRadius), 1.0, squircleDistance);
    fragColor = mix(baseColor, uBorderColor, borderFactor * uBorderColor.a);
}
