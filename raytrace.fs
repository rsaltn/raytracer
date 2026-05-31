#version 330 core

out vec4 FragColor;

in vec2 uv;

#define MAX_SPHERES 16
#define INF 999999.0

uniform vec2 uResolution;
uniform float uTime;

uniform vec3 uCamPos;
uniform vec3 uCamFront;
uniform vec3 uCamRight;
uniform vec3 uCamUp;

uniform int uSphereCount;
uniform vec3 uSpherePos[MAX_SPHERES];
uniform float uSphereRadius[MAX_SPHERES];
uniform vec3 uSphereColor[MAX_SPHERES];
uniform float uSphereEmission[MAX_SPHERES];

struct Hit
{
    float t;
    int id;
    vec3 pos;
    vec3 normal;
};

float sphereHit(vec3 ro, vec3 rd, vec3 center, float radius)
{
    vec3 oc = ro - center;

    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;

    float d = b * b - 4.0 * a * c;

    if (d < 0.0)
        return -1.0;

    float sq = sqrt(d);

    float t1 = (-b - sq) / (2.0 * a);
    float t2 = (-b + sq) / (2.0 * a);

    if (t1 > 0.001)
        return t1;

    if (t2 > 0.001)
        return t2;

    return -1.0;
}

Hit traceScene(vec3 ro, vec3 rd)
{
    Hit hit;
    hit.t = INF;
    hit.id = -1;

    for (int i = 0; i < MAX_SPHERES; i++)
    {
        if (i >= uSphereCount)
            break;

        float t = sphereHit(ro, rd, uSpherePos[i], uSphereRadius[i]);

        if (t > 0.0 && t < hit.t)
        {
            hit.t = t;
            hit.id = i;
        }
    }

    if (hit.id != -1)
    {
        hit.pos = ro + rd * hit.t;
        hit.normal = normalize(hit.pos - uSpherePos[hit.id]);
    }

    return hit;
}

float shadowAmount(vec3 point, vec3 lightPos)
{
    vec3 toLight = lightPos - point;
    float lightDist = length(toLight);
    vec3 lightDir = normalize(toLight);

    Hit h = traceScene(point + lightDir * 0.03, lightDir);

    if (h.id != -1 && h.t < lightDist)
        return 0.25;

    return 1.0;
}

vec3 skyColor(vec3 rd)
{
    float y = rd.y * 0.5 + 0.5;

    vec3 bottom = vec3(0.02, 0.02, 0.06);
    vec3 top = vec3(0.0, 0.0, 0.015);

    vec3 col = mix(bottom, top, y);

    float n = fract(sin(dot(rd.xy * 600.0, vec2(12.9898, 78.233))) * 43758.5453);
    float star = step(0.9965, n);

    col += star * vec3(0.8);

    return col;
}

vec3 shade(vec3 ro, vec3 rd)
{
    Hit hit = traceScene(ro, rd);

    if (hit.id == -1)
        return skyColor(rd);

    vec3 base = uSphereColor[hit.id];

    if (uSphereEmission[hit.id] > 0.0)
    {
        return base * uSphereEmission[hit.id];
    }

    vec3 lightPos = vec3(0.0, 0.0, 0.0);

    vec3 lightDir = normalize(lightPos - hit.pos);
    float diff = max(dot(hit.normal, lightDir), 0.0);

    float sh = shadowAmount(hit.pos, lightPos);

    vec3 ambient = base * 0.04;
    vec3 diffuse = base * diff * sh;

    vec3 viewDir = normalize(ro - hit.pos);
    vec3 reflectDir = reflect(-lightDir, hit.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 col = ambient + diffuse + vec3(spec) * 0.2;

    return col;
}

void main()
{
    vec2 screen = uv * 2.0 - 1.0;
    screen.x *= uResolution.x / uResolution.y;

    float fov = 1.0;

    vec3 rd = normalize(
        uCamFront +
        screen.x * uCamRight * fov +
        screen.y * uCamUp * fov
    );

    vec3 ro = uCamPos;

    vec3 col = shade(ro, rd);

    col = col / (col + vec3(1.0));
    col = pow(col, vec3(1.0 / 2.2));

    FragColor = vec4(col, 1.0);
}
