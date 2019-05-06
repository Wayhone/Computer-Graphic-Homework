#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>  

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

const int RANGE = 500;

// draw line
void swap(int &x, int &y);
int drawLine(int x_arr[], int y_arr[], int x1, int y1, int x2, int y2);
void bresenham(int arr[], int p, int i, int length, int dx, int dy);
void fillColor(const int &shaderProgram, ImVec4 tri_color, int x1, int y1, int x2, int y2, int x3, int y3);
// draw circle 
int drawCircle(int x_arr[], int y_arr[], int R, int cx, int cy);

void showPoint(const int &shaderProgram, int length, int x[], int y[], ImVec4 tri_color);

// ������ɫ��
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main() {\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";
// Ƭ����ɫ��
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main()
{
	float x1 = -0.3f, y1 = -0.3f, x2 = 0.3f, y2 = -0.3f, x3 = 0.0f, y3 = 0.3f;

	// glfw��ʼ��������
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// ��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawTriangle", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: ����OpenGL����ָ��
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return 0;
	}

	// ��ʼ��ImGui����
	const char * glsl_version = "#version 130";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);	// ����Ⱦ��
	ImGui_ImplOpenGL3_Init(glsl_version);

	//-----------------------------------
	// ����������ɫ�����󣬱�����ɫ��Դ��
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// �����ɫ���������
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "���붥����ɫ��Դ����ִ���\n" << infoLog << std::endl;
	}

	// ����Ƭ����ɫ�����󣬱�����ɫ��Դ��
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// �����ɫ���������
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "����Ƭ����ɫ��Դ����ִ���\n" << infoLog << std::endl;
	}

	//-----------------------------------
	// ������ɫ��
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); // ����һ����ɫ���������
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// ���������ɫ���������
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "������ɫ��������ִ���\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	// ѭ����Ⱦ
	// -----------
	bool show_circle = false;
	bool show_triangle = true;
	ImVec4 tri_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 cir_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	
	float cx = 0.0f, cy = 0.0f , radius = 0.5f;	// Բ��λ�� �� �뾶

	while (!glfwWindowShouldClose(window))
	{
		// ��������
		// -----
		processInput(window);

		// ��Ⱦ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ImGui��ʼ�� - ���� - ��Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Configuration");
		ImGui::Checkbox("Circle", &show_circle);
		ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
		ImGui::SliderFloat("Centre - X", &cx, -1.0f, 1.0f);
		ImGui::SliderFloat("Centre - Y", &cy, -1.0f, 1.0f);

		ImGui::Checkbox("Triangle", &show_triangle);
		ImGui::Text("Point A");
		ImGui::SliderFloat("A - X", &x1, -1.0f, 1.0f);
		ImGui::SliderFloat("A - Y", &y1, -1.0f, 1.0f);
		ImGui::Text("Point B");
		ImGui::SliderFloat("B - X", &x2, -1.0f, 1.0f);
		ImGui::SliderFloat("B - Y", &y2, -1.0f, 1.0f);
		ImGui::Text("Point C");
		ImGui::SliderFloat("C - X", &x3, -1.0f, 1.0f);
		ImGui::SliderFloat("C - Y", &y3, -1.0f, 1.0f);
		ImGui::ColorEdit3("Triangle color", (float*)&tri_color);
		ImGui::End();

		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// ���ݴ��ڵĻ����С��ȡ�ߴ�
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Draw Line 
		int x_arr[100000], y_arr[100000];
		int length;
		if (show_triangle) {
			int ix1 = (int)(x1 * RANGE),
				iy1 = (int)(y1 * RANGE),
				ix2 = (int)(x2 * RANGE),
				iy2 = (int)(y2 * RANGE),
				ix3 = (int)(x3 * RANGE),
				iy3 = (int)(y3 * RANGE);
			// Line 1
			length = drawLine(x_arr, y_arr, ix1, iy1, ix2, iy2);
			showPoint(shaderProgram, length, x_arr, y_arr, tri_color);

			// Line 2
			length = drawLine(x_arr, y_arr, ix1, iy1, ix3, iy3);
			showPoint(shaderProgram, length, x_arr, y_arr, tri_color);

			// Line 3
			length = drawLine(x_arr, y_arr, ix2, iy2, ix3, iy3);
			showPoint(shaderProgram, length, x_arr, y_arr, tri_color);

			// Fill Color
			fillColor(shaderProgram, tri_color, ix1, iy1, ix2, iy2, ix3, iy3);
		}

		// Circle 
		if (show_circle) {
			length = drawCircle(x_arr, y_arr, (int)(radius * RANGE), int(cx * RANGE), int(cy * RANGE));
			showPoint(shaderProgram, length, x_arr, y_arr, cir_color);
		}

		// glfw: �������棬�����¼�
		// ------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// glfw: ��������GLFW��Դ
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// --------------------------------
// ����������
void swap(int &x, int &y) {
	int temp = x;
	x = y;
	y = temp;
}

// bresenham�㷨���ݹ����
//		pΪ���߲�����iΪ��ǰԪ���±�
void bresenham(int arr[], int p, int i, int length, int dx, int dy) {
	if (i + 1 >= length)
		return;
	int next;
	if (p < 0) {
		arr[i + 1] = arr[i];
		next = p + 2 * dy;
	}
	else {
		arr[i + 1] = arr[i] + 1;
		next = p + 2 * dy - 2 * dx;
	}
	bresenham(arr, next, i + 1, length, dx, dy);
}

int drawLine(int x_arr[], int y_arr[], int x1, int y1, int x2, int y2) {
	int dx, dy, length;

	if (x1 == x2) {
		// #1. б�ʲ�����
		if (y1 > y2)
			swap(y1, y2);
		length = y2 - y1;
		for (int i = 0; i < length; i++) {
			x_arr[i] = x1;
			y_arr[i] = y1 + i;
		}
	}
	else {
		float m = float(y2 - y1) / float(x2 - x1);
		// �������λ�ã��������
		if ((fabs(m) < 1 && x1 > x2) || (fabs(m) > 1 && y1 > y2)) {
			swap(x1, x2);
			swap(y1, y2);
		}
		dx = x2 - x1;
		dy = y2 - y1;

		// #2. б�� |m| <= 1  , �� dx >= dy�� ��ÿ��xȡ��
		if (fabs(m) <= 1) {
			length = x2 - x1 + 1;
			int p = 2 * dy - dx;
			for (int i = 0; i < length; i++)
				x_arr[i] = x1 + i;

			if (m >= 0) {
				y_arr[0] = y1;
				bresenham(y_arr, p, 0, length, dx, dy);
			}
			else {
				p = -p;
				y_arr[0] = -y1;
				bresenham(y_arr, p, 0, length, dx, -dy);
				for (int i = 0; i < length; i++)
					y_arr[i] = -y_arr[i];
			}
		}
		else {
		// #3. б�� |m| > 1  , �� dy > dx�� ��ÿ��yȡ��
			length = y2 - y1 + 1;
			int p = 2 * dx - dy;
			for (int i = 0; i < length; i++)
				y_arr[i] = y1 + i;

			if (m > 0) {
				x_arr[0] = x1;
				bresenham(x_arr, p, 0, length, dy, dx);
			}
			else {
				p = -p;
				x_arr[0] = -x1;
				bresenham(x_arr, p, 0, length, dy, -dx);
				for (int i = 0; i < length; i++)
					x_arr[i] = -x_arr[i];
			}
		}
	}

	return length;
}

void fillColor(const int &shaderProgram, ImVec4 tri_color, int x1, int y1, int x2, int y2, int x3, int y3) {
	int x_min = (x1 < x2 ? x1 : x2) < x3 ? (x1 < x2 ? x1 : x2) : x3,
		x_max = (x1 > x2 ? x1 : x2) > x3 ? (x1 > x2 ? x1 : x2) : x3,
		y_min = (y1 < y2 ? y1 : y2) < y3 ? (y1 < y2 ? y1 : y2) : y3,
		y_max = (y1 > y2 ? y1 : y2) > y3 ? (y1 > y2 ? y1 : y2) : y3;

	int i = 0;
	int x_arr[6 * RANGE], y_arr[6 * RANGE];

	// �������� u = AB, v = AC
	int ux = x2 - x1, uy = y2 - y1,	
		vx = x3 - x1, vy = y3 - y1; 
	float a, b;

	// �����е�����һ������� ����au + bv ����ʾ�� �� a+b <= 1 �� ��õ�����������
	for (int x = x_min; x <= x_max; x++)
		for (int y = y_min; y <= y_max; y++) {
			b = (float)(x * uy - y * ux + y1 * ux - x1 * uy) / (float)(vx * uy - ux * vy);
			a = (float)(x * vy - y * vx + y1 * vx - x1 * vy) / (float)(ux * vy - vx * uy);
			if (a + b <= 1 && a >= 0 && b >= 0) {
				x_arr[i] = x;
				y_arr[i] = y;
				i++;
				if (i == 6 * RANGE) {
					showPoint(shaderProgram, i, x_arr, y_arr, tri_color);
					i = 0;
				}
			}
		}
	showPoint(shaderProgram, i, x_arr, y_arr, tri_color);
}

int drawCircle(int x_arr[], int y_arr[], int R, int cx, int cy) {
	int x = 0, y = R, p = 3 - 2 * R;
	int i = 0;
	
	for (; x <= y; x++, i += 8) {
		x_arr[i] = x_arr[i + 1] = cx + x;
		x_arr[i + 2] = x_arr[i + 3] = cx - x;
		x_arr[i + 4] = x_arr[i + 5] = cx + y;
		x_arr[i + 6] = x_arr[i + 7] = cx - y;

		y_arr[i] = y_arr[i + 2] = cy + y;
		y_arr[i + 1] = y_arr[i + 3] = cy - y;
		y_arr[i + 4] = y_arr[i + 6] = cy + x;
		y_arr[i + 5] = y_arr[i + 7] = cy - x;
		if (p >= 0) {
			p += 4 * (x - y);
			y--;
		}
		else 
			p += 4 * x + 6;
	}
	return i;
}

void showPoint(const int &shaderProgram, int length, int x[], int y[], ImVec4 tri_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) {
	float fx, fy;
	float vertices[6 * 6 * RANGE];
	int index = 0;
	for (int i = 0; i < length; i++) {
		fx = float(x[index]) / RANGE;
		fy = float(y[index]) / RANGE;
		vertices[index * 6] = fx;
		vertices[index * 6 + 1] = fy;
		vertices[index * 6 + 2] = 0.0f;
		vertices[index * 6 + 3] = tri_color.x;
		vertices[index * 6 + 4] = tri_color.y;
		vertices[index * 6 + 5] = tri_color.z;
		index++;
	}

	// �������㻺����� Vertex Buffer Objects, �洢����, ����������GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// ��������������� Vertex Array Objects, ����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// ������������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(2.0f); // ������С
	glDrawArrays(GL_POINTS, 0, length);

	// ���VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}