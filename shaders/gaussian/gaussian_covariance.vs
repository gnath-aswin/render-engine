#version 330 core

layout (location = 0) in vec2 aCorner;

layout (location = 1) in vec3 aPos;
layout (location = 2) in float aOpacity;
layout (location = 3) in vec3 aScale;
layout (location = 4) in vec4 aRotation;
layout (location = 5) in vec4 aColor;

out vec2 EllipseCoord;
out vec2 GaussianCoord;
out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float splatExtent;      // try 3.0
uniform float opacityScale;     // try 0.5
uniform float maxScreenRadius;  // try 0.04 in NDC

mat3 quatToMat3(vec4 q)
{
    q = normalize(q);

    float w = q.x;
    float x = q.y;
    float y = q.z;
    float z = q.w;

    return mat3(
        1.0 - 2.0*y*y - 2.0*z*z,
        2.0*x*y + 2.0*w*z,
        2.0*x*z - 2.0*w*y,

        2.0*x*y - 2.0*w*z,
        1.0 - 2.0*x*x - 2.0*z*z,
        2.0*y*z + 2.0*w*x,

        2.0*x*z + 2.0*w*y,
        2.0*y*z - 2.0*w*x,
        1.0 - 2.0*x*x - 2.0*y*y
    );
}

void main()
{
    vec4 centerView4 = view * model * vec4(aPos, 1.0);
    vec3 centerView = centerView4.xyz;

    float depth = max(0.05, -centerView.z);

    // --------------------------------------------------
    // 1. Build 3D covariance in local/object space
    // --------------------------------------------------
    mat3 R = quatToMat3(aRotation);

    mat3 S2 = mat3(
        aScale.x * aScale.x, 0.0, 0.0,
        0.0, aScale.y * aScale.y, 0.0,
        0.0, 0.0, aScale.z * aScale.z
    );

    mat3 SigmaLocal = R * S2 * transpose(R);

    // --------------------------------------------------
    // 2. Transform covariance to view space
    // --------------------------------------------------
    mat3 A = mat3(view * model);
    mat3 SigmaView = A * SigmaLocal * transpose(A);

    // --------------------------------------------------
    // 3. Projection Jacobian
    // --------------------------------------------------
    float fx = projection[0][0];
    float fy = projection[1][1];

    vec3 jx = vec3(
        fx / depth,
        0.0,
        fx * centerView.x / (depth * depth)
    );

    vec3 jy = vec3(
        0.0,
        fy / depth,
        fy * centerView.y / (depth * depth)
    );

    // --------------------------------------------------
    // 4. 2D covariance: Sigma2D = J SigmaView J^T
    // --------------------------------------------------
    float cov00 = dot(jx, SigmaView * jx);
    float cov01 = dot(jx, SigmaView * jy);
    float cov11 = dot(jy, SigmaView * jy);

    // Small stabilizer so tiny splats do not disappear completely.
    cov00 += 1e-8;
    cov11 += 1e-8;

    // --------------------------------------------------
    // 5. Eigen decomposition of 2x2 covariance
    // --------------------------------------------------
    float trace = cov00 + cov11;
    float diff = cov00 - cov11;
    float discr = sqrt(max(0.0, 0.25 * diff * diff + cov01 * cov01));

    float lambda1 = max(0.5 * trace + discr, 1e-10);
    float lambda2 = max(0.5 * trace - discr, 1e-10);

    vec2 axis1;

    if (abs(cov01) > 1e-10) {
        axis1 = normalize(vec2(cov01, lambda1 - cov00));
    } else {
        axis1 = cov00 >= cov11 ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    }

    vec2 axis2 = vec2(-axis1.y, axis1.x);

    float radius1 = splatExtent * sqrt(lambda1);
    float radius2 = splatExtent * sqrt(lambda2);

    radius1 = clamp(radius1, 0.0002, maxScreenRadius);
    radius2 = clamp(radius2, 0.0002, maxScreenRadius);

    // --------------------------------------------------
    // 6. Expand quad in NDC space along ellipse axes
    // --------------------------------------------------
    vec2 offsetNDC =
        axis1 * (aCorner.x * radius1) +
        axis2 * (aCorner.y * radius2);

    vec4 centerClip = projection * centerView4;

    gl_Position = centerClip + vec4(offsetNDC * centerClip.w, 0.0, 0.0);

    EllipseCoord = aCorner;
    GaussianCoord = aCorner * splatExtent;
    Color = vec4(aColor.rgb, aOpacity * opacityScale);
}
