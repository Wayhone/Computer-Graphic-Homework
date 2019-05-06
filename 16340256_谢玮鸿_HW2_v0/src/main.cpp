#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// 前两个参数控制位置，后两个参数控制窗口宽高
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window) {
	// 若按下退出键Esc，退出渲染
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void showNewTriangle(const int &shaderProgram, bool &colorSelected, const ImVec4 &newColor);
void showLine(const int &shaderProgram);
void showPoint(const int &shaderProgram);

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

int main() {
	glfwInit();	// 初始化GLFW
	const char * glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// 配置GLFW，设置主版本号为3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// 设置次版本号为3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 使用核心模式 core-profile

	// 创建窗口对象
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "创建GLFW窗口失败" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 显示窗口
	glfwMakeContextCurrent(window);
	// 设置回调，当窗口大小调整后将调用该回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD初始化失败" << std::endl;
		return -1;
	}

	// 初始化ImGui环境
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);	// 绑定渲染器
	ImGui_ImplOpenGL3_Init(glsl_version);

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

	//-----------------------------------
	
	// 定义选择后的颜色数据
	bool colorSelected = false;
	ImVec4 newColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	while (!glfwWindowShouldClose(window))
	{
		// 处理输入
		processInput(window);

		// 渲染处理
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 显示三角形
		showNewTriangle(shaderProgram, colorSelected, newColor);
		// 显示线、点
		showLine(shaderProgram);
		showPoint(shaderProgram);
		
		// ImGui初始化
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// 设置ImGui窗口样式
		ImGui::Begin("Set Color");
		ImGui::Text("Select your color");
		ImGui::ColorEdit3("Triangle color", (float*)&newColor);
		ImGui::End();

		// ImGui渲染
		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// 根据窗口的缓冲大小获取尺寸
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		

		// 交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	glfwTerminate();
	return 0;
}

void showNewTriangle(const int &shaderProgram, bool &colorSelected, const ImVec4 &newColor) {
	// 定义三个默认顶点数据
	float vertices[] = {
		// 位置					颜色
		0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f
	};
	float vertices2[] = {
		// 位置					颜色
		0.0f,  1.0f, 0.0f,		newColor.x, newColor.y, newColor.z,
		-1.0f, -1.0f, 0.0f,		newColor.x, newColor.y, newColor.z,
		1.0f, -1.0f, 0.0f,		newColor.x, newColor.y, newColor.z
	};

	// 创建顶点缓冲对象 Vertex Buffer Objects, 存储顶点, 缓冲类型是GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (colorSelected)
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	else {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// 复制顶点数据到缓冲内存中（数据不会被改变）
		if (newColor.x > 0.0f)
			colorSelected = true;
	}
	// 创建顶点数组对象 Vertex Array Objects, 并绑定
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 解析顶点数据，location = 0， vec3， 数据类型为浮点型， 不需要标准化(0-1)， 步长为3个浮点数，偏移量为0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 解析颜色数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------
	glViewport(0, 0, 640, 480);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// 解绑VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void showLine(const int &shaderProgram) {
	// 定义三个默认顶点数据
	float vertices[] = {
		// 位置					颜色
		1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,		1.0f, 1.0f, 0.0f
	};

	// 创建顶点缓冲对象 Vertex Buffer Objects, 存储顶点, 缓冲类型是GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	
	
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
	glViewport(300,0,400,400);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	// 解绑VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void showPoint(const int &shaderProgram) {
	// 定义三个默认顶点数据
	float vertices[] = {
		// 位置					颜色
		1.0f,  1.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

	// 创建顶点缓冲对象 Vertex Buffer Objects, 存储顶点, 缓冲类型是GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
	glViewport(360, 0, 400, 400);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(10.0f); // 调整大小
	glDrawArrays(GL_POINTS, 0, 1);

	// 解绑VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
