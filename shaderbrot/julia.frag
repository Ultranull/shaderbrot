#version 330 core

in vec3 fragcoord;


uniform float time;
uniform vec2 relative;
uniform vec2 position;
uniform float radius;

void main(){
    vec2 st = fragcoord.xy+.5;
	vec2 c=relative;
    vec2 z=position+((st*4.)-2.)*radius;
    int ind;
    const int mx=300;
    for(int i=0;i<mx;i++){
        ind=i;
        if(z.x*z.x+z.y*z.y>=400.)break;
        float t=z.x*z.x-z.y*z.y+c.x;
        z.y=2.*z.x*z.y+c.y;z.x=t;
        //float t=c.x*exp(z.x)*cos(z.y)-c.y*exp(z.x)*sin(z.y);
        //z.y    =c.x*exp(z.x)*sin(z.y)-c.y*exp(z.x)*cos(z.y);z.x=t;
    }
    float zmag=sqrt(z.x*z.x+z.y*z.y);
    float lvl=( float(ind) + 1. - log(log(abs(zmag)) / log(30.)));
	float grad=log(float(ind))/log(zmag);
    vec3 color = ind>mx-10?
			mix(mix(vec3(1,0,0), vec3(0,1,0), fract(grad)),vec3(0,0,1),fract((grad/2))):
    		abs(vec3(cos(.4*lvl),
					 cos(.3*lvl),
					 cos(.6*lvl)));

    gl_FragColor = vec4(color,1.0);
}