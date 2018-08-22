#version 330 core

in vec3 fragcoord;

uniform float time;


void main(){
    vec2 st = fragcoord.xy+.5;
    vec2 ms =vec2(0);
    vec2 pos =vec2(0);
    float radius=1.;
	vec2 z=ms;
    vec2 c=pos+((st*4.)-2.)*radius;
    int ind;
    const int mx=300;
    for(int i=0;i<mx;i++){
        ind=i;
        if(z.x*z.x+z.y*z.y>=400.)break;
        float t=z.x*z.x-z.y*z.y+c.x;
        z.y=2.*z.x*z.y+c.y;z.x=t;
    }
    float zmag=z.x*z.x+z.y*z.y;
    float lvl=log(float(ind))/log(zmag);
    vec3 color = ind>mx-10?
        	vec3(st.x,st.y,abs(sin(time))):
    		vec3(cos(lvl+.2)*2.,
                 1.-cos(lvl+.3),
                 1.-cos(lvl+.6));

    gl_FragColor = vec4(color,1.0);
}