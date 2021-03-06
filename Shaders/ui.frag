// Input color for text
uniform vec3 textColor;
// Texture to sample from
uniform sampler2D text;

/**
 * Images and text share the same fragment shader, but have different
 * behaviours. This boolean must to set correctly before use to ensure that the
 * correct processing is done for either text or images.
 */
uniform bool isImage;

// Main: just output the color of the fragment
void main()
{
    if (isImage)
    {
        // We do not want to alter the texture colors at each coordinate.
        // since the image already supplies the desired colors.
        vFragColor = texture(text, TexCoords);
    }
    else
    {
        // Make all the text color the same as textColor.
        vFragColor = vec4(textColor, texture(text, TexCoords).r);
		//color = texture(text, TexCoords); // For Debugging
    }
	
	vBrightColor = vec4(0.0, 0.0, 0.0, 0.0);
}
