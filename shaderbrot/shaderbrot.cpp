#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


using namespace std;
using namespace glm;

GLFWwindow * initWindow(int w, int h, const char *title);
GLuint loadshaders(const char *vertexfile, const char *fragmentfile);
GLuint LoadGLTexture(const char *filename);

void viewportinit(GLFWwindow *window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}

void callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		vector< unsigned char > buf(w * h * 3);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0]);
		int err = SOIL_save_image("img.bmp", SOIL_SAVE_TYPE_BMP, w, h, 3, &buf[0]);
	}
}

int main(){
	GLFWwindow *window = initWindow(700, 600, "test");
	glfwSetKeyCallback(window,callback);
	if (window == nullptr)return-1;

	GLuint vao, vbo;
	vector<vec3> vertices = {
		vec3( 1, 1,0),
		vec3(-1, 1,0),
		vec3(-1,-1,0),
		vec3( 1,-1,0)
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	GLuint mandelbrotprogram = loadshaders("vertex.vert", "mandelbrot.frag");
	GLuint juliaprogram = loadshaders("vertex.vert", "julia.frag");

	dvec2 pos(0, 0), rel(0, 0);
	double radius = .5;

	mat4 mand = translate(vec3(.5f, 0, 0))*scale(vec3(.5f));
	mat4 jul = translate(vec3(-.5f, 0, 0))*scale(vec3(.5f));

	float ticks = 0;
	double lastFrame = glfwGetTime();
	int frames = 0;
	double lastFPS = glfwGetTime();
	int dfps = 90;
	do {
		glClear(GL_COLOR_BUFFER_BIT);

		double currenttime = glfwGetTime();
		frames++;
		if (currenttime - lastFPS >= 1) {
			printf("%d -- %d\n", frames, dfps);
			glfwSetWindowTitle(window, string(to_string(frames) + " -- " + to_string(dfps)).c_str());
			frames = 0;
			lastFPS += 1.0;
		}
		double delta = currenttime - lastFrame;
		lastFrame = currenttime;

		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
			radius += radius * delta;
		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
			radius -= radius * delta;

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			pos.y += radius * delta;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			pos.y -= radius * delta;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			pos.x += radius * delta;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			pos.x -= radius * delta;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			rel.y += radius * delta;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			rel.y -= radius * delta;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			rel.x += radius * delta;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			rel.x -= radius * delta;




		glUseProgram(mandelbrotprogram);
		glUniform1f(glGetUniformLocation(mandelbrotprogram, "radius"), radius);
		glUniform2f(glGetUniformLocation(mandelbrotprogram, "position"), pos.x, pos.y);
		glUniformMatrix4fv(glGetUniformLocation(mandelbrotprogram,"model"),1,GL_FALSE,&mand[0][0]);
		glDrawArrays(GL_QUADS, 0, 4);

		glUseProgram(juliaprogram);
		glUniform1f(glGetUniformLocation(juliaprogram, "radius"), radius);
		glUniform2f(glGetUniformLocation(juliaprogram, "position"), rel.x, rel.y);
		glUniform2f(glGetUniformLocation(juliaprogram, "relative"), pos.x, pos.y);
		glUniformMatrix4fv(glGetUniformLocation(juliaprogram, "model"), 1, GL_FALSE, &jul[0][0]);
		glDrawArrays(GL_QUADS, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
		Sleep(1000 / dfps);
		ticks += 1 * delta;
		viewportinit(window);
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	glfwTerminate();
	return 0;
}

GLFWwindow * initWindow(int w, int h, const char *title) {
	GLFWwindow *window;
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		getchar();
		exit(-1);
	}

	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (window == nullptr) {
		fprintf(stderr, "Failed to init window\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to load glew\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}
	GLFWimage icons[1];
	icons[0].pixels = SOIL_load_image("img.bmp", &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
	glfwSetWindowIcon(window, 1, icons);
	SOIL_free_image_data(icons[0].pixels);

	return window;
}
string readFile(const char *file) {
	string content;
	ifstream stream(file);
	if (stream.is_open()) {
		stringstream ss;
		ss << stream.rdbuf();
		content = ss.str();
		stream.close();
	} else {
		printf("Failed to open %s\n", file);
		return "";
	}
	return content;
}
bool compileshader(const char* file, GLuint id) {
	GLint result = GL_FALSE;
	int infoLogLength;
	printf("Compiling shader: %s\n", file);
	string content = readFile(file);
	if (content.compare("") == 0) { return NULL; }
	char const * src = content.c_str();
	const GLint length = content.size();
	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &errormessage[0]);
		printf("%s compile error:\n\t%s\n", file, &errormessage[0]);
		return false;
	}
	return true;
}

GLuint loadshaders(const char *vertexfile, const char *fragmentfile) {
	GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_FALSE;
	int infoLogLength;

	if (!compileshader(vertexfile, vertexID)) { return NULL; }
	if (!compileshader(fragmentfile, fragmentID)) { return NULL; }

	printf("linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		printf("link error:\n%s\n", &errormessage[0]);
		return NULL;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	return programID;
}
GLuint LoadGLTexture(const char *filename) {
	printf("loading texture: %s\n", filename);
	glEnable(GL_TEXTURE_2D);
	GLuint texture = 0;
	texture = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
	(
		filename,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, NULL
	);
	if (texture == 0) {
		printf("error loading texture %s!\n", filename);
		getchar();
		exit(-1);
	}
	glBindTexture(GL_TEXTURE_2D, texture); // select our current texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // texture should tile
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}
