// interpolated colour received from vertex stage

uniform sampler2D mTexture;

// first output is mapped to the framebuffer's colour index by default
//out vec4 FragmentColour;

uniform bool horizontal;
uniform float blurValues[5] = float[] (0.0999673828, 0.114416346, 0.119682685, 0.114416346, 0.0999673828);
uniform int matrixSize = 5;
void main(void)
{
	vec2 tex_offset = 1.0 / textureSize(mTexture, 0); // gets size of single texel
	vec3 result = texture(mTexture, TexCoords).rgb * blurValues[0]; // current fragment's contribution
	if (horizontal)
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(mTexture, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * blurValues[i];
			result += texture(mTexture, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * blurValues[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(mTexture, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * blurValues[i];
			result += texture(mTexture, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * blurValues[i];
		}
	}
	vFragColor = vec4(result, 1.0);
}