layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main()
{
    gl_Position = gl_in[0].gl_Position + vec4(-0.3, -0.3, 0.0, 0.0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4(0.3, -0.3, 0.0, 0.0);
    EmitVertex();

	 gl_Position = gl_in[0].gl_Position + vec4(-0.3, 0.3, 0.0, 0.0);
    EmitVertex();

	 gl_Position = gl_in[0].gl_Position + vec4(0.3, 0.3, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
