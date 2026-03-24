#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
    // Pobieramy wartość z kanału R i traktujemy ją jako przezroczystość (Alpha)
    float alpha = texture(text, TexCoords).r;
    color = vec4(textColor, alpha);
}