#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>  
#include <vector>
#include <math.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// 存储点坐标的类
class Point
{
	public:
		float x, y;
		void setxy(float _x, float _y) {
			x = _x;
			y = _y;
		}
};

float xCur, yCur;	// 记录鼠标实时位置
vector<Point> pointSet;
float delta = 0.001;	// Bezier 步长

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

void drawLine(const int &shaderProgram, float x1, float y1, float x2, float y2, ImVec4 color);
void drawPoint(const int &shaderProgram, float x, float y, float pointSize, ImVec4 color);
void drawBezierCurve(const int &shaderProgram, ImVec4 color);
void drawTangent(const int &shaderProgram, vector<Point> points, float t, ImVec4 color);


// 顶点着色器
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main() {\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";
// 片段着色器
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main() {\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

int main()
{
	// glfw初始化和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DrawTriangle", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);		//鼠标移动回调函数
	glfwSetMouseButtonCallback(window, mouse_button_callback);	//鼠标点击回调函数

	// glad: 载入OpenGL函数指针
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return 0;
	}

	// 初始化ImGui环境
	//const char * glsl_version = "#version 130";
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO &io = ImGui::GetIO();
	//(void)io;
	//ImGui::StyleColorsDark();
	//ImGui_ImplGlfw_InitForOpenGL(window, true);	// 绑定渲染器
	//ImGui_ImplOpenGL3_Init(glsl_version);

	//-----------------------------------
	// 创建顶点着色器对象，编译着色器源码
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// 检查着色器编译错误
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "编译顶点着色器源码出现错误\n" << infoLog << std::endl;
	}

	// 创建片段着色器对象，编译着色器源码
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// 检查着色器编译错误
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "编译片段着色器源码出现错误\n" << infoLog << std::endl;
	}

	//-----------------------------------
	// 链接着色器
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram(); // 创建一个着色器程序对象
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// 检查连接着色器程序错误
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "链接着色器程序出现错误\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	// 循环渲染
	// -----------
	ImVec4 line_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 point_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	ImVec4 tangent_color = ImVec4(0.60f, 0.80f, 0.50f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		// 处理输入
		// -----
		processInput(window);

		// 渲染处理
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ImGui初始化 - 设置 - 渲染
		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		//ImGui::NewFrame();

		//ImGui::Begin("Configuration");
		//ImGui::ColorEdit3("Line color", (float*)&line_color);
		//ImGui::End();

		//ImGui::Render();
		//int s_width, s_height;
		//glfwMakeContextCurrent(window);
		//glfwGetFramebufferSize(window, &s_width, &s_height);	// 根据窗口的缓冲大小获取尺寸
		//glViewport(0, 0, s_width, s_height);
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Draw Line & Points
		for (int i = 0; i < pointSet.size(); i++) {
			drawPoint(shaderProgram, pointSet[i].x, pointSet[i].y, 5.0f, point_color);
			if (i > 0)
				drawLine(shaderProgram, pointSet[i-1].x, pointSet[i-1].y, pointSet[i].x, pointSet[i].y, point_color);
		}

		// Draw Bezier Curve  
		if (pointSet.size() > 1)
			drawBezierCurve(shaderProgram, line_color);

		// Show Bonus
		float t = (float)((int)(glfwGetTime() * 10) % 16) / 16;
		if (pointSet.size() > 1)
			drawTangent(shaderProgram, pointSet, t, tangent_color);

		// glfw: 交换缓存，处理事件
		// ------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// glfw: 清除分配的GLFW资源
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

// process mouse cursor 实时记录当前坐标
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos) {
	// xCur = xpos; 
	// yCur = ypos; 
	xCur = (xpos - SCR_WIDTH / 2) / SCR_WIDTH * 2;
	yCur = 0  - (ypos - SCR_HEIGHT / 2) / SCR_HEIGHT * 2;
};

// process mouse button input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// 左键将当前点加入到pointSet
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Point point;
		point.setxy(xCur, yCur);
		pointSet.push_back(point);
	}

	// 右键删除pointSet最后一个元素
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (!pointSet.empty())
			pointSet.pop_back();
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// --------------------------------

void drawLine(const int &shaderProgram, float x1, float y1, float x2, float y2, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) {
	float vertice[12] = { 
		x1, y1, 0, color.x,  color.y, color.z, 
		x2, y2, 0, color.x,  color.y, color.z
	};

	// 创建顶点缓冲对象 Vertex Buffer Objects, 存储顶点, 缓冲类型是GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	// 创建顶点数组对象 Vertex Array Objects, 并绑定
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 解析顶点数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 解析颜色数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(2.0f); // 调整大小
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	// 解绑VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void drawPoint(const int &shaderProgram, float x, float y, float pointSize, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) {

	float vertice[6] = {x, y, 0, color.x,  color.y, color.z};	

	// 创建顶点缓冲对象 Vertex Buffer Objects, 存储顶点, 缓冲类型是GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	// 创建顶点数组对象 Vertex Array Objects, 并绑定
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 解析顶点数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 解析颜色数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(pointSize); // 调整大小
	glDrawArrays(GL_POINTS, 0, 1);

	// 解绑VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void drawBezierCurve(const int &shaderProgram, ImVec4 color) {
	int size = pointSet.size();
	int n = size - 1;	// 0, 1, ... , n 共n+1个点
	float *B = new float[size];	// 多项式的常数系数

	// 求组合数C_n^k，由于数据量不大，采用比较直接的方法求，为了防止数据溢出，采用一边乘一边除的方法
	for (int i = 0; i < size; i++) {
		int k = i, x = 1;
		float c = 1.0;
		if (k > n - k)
			k = n - k;
		for (int j = n; j > n - k; j--, x++)
			c *= (float)j / x;

		B[i] = c;
	}
		
	for (float t = 0.0; t < 1.0; t += delta) {
		float x = 0.0, y = 0.0, tmp;
		for (int i = 0; i <= n; i++) {
			tmp = B[i] * pow(t, i) * pow(1 - t, n - i);
			x += tmp * pointSet[i].x;
			y += tmp * pointSet[i].y;
		}
		drawPoint(shaderProgram, x, y, 2.0f, color);
	}
}

void drawTangent(const int &shaderProgram, vector<Point> points, float t, ImVec4 color)
{
	vector<Point> new_points;
	float x1, y1, x2, y2;
	Point point;

	// 第一个点
	if (points.size() < 2)
		return;
	x2 = points[0].x + t * (points[1].x - points[0].x);
	y2= points[0].y + t * (points[1].y - points[0].y);
	point.setxy(x2, y2);
	new_points.push_back(point);
	drawPoint(shaderProgram, x2, y2, 2.0f, color);

	// 开始画第二个点
	for (int i = 2; i <= points.size() - 1; i++) {
		x1 = x2;
		y1 = y2;
		x2 = points[i - 1].x + t * (points[i].x - points[i - 1].x);
		y2 = points[i - 1].y + t * (points[i].y - points[i - 1].y);
		point.setxy(x2, y2);
		new_points.push_back(point);
		drawPoint(shaderProgram, x2, y2, 2.0f, color);

		drawLine(shaderProgram, x1, y1, x2, y2, color);
	}

	// 递归
	drawTangent(shaderProgram, new_points, t, color);
}