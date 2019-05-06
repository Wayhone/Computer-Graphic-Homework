#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// ǰ������������λ�ã��������������ƴ��ڿ��
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window) {
	// �������˳���Esc���˳���Ⱦ
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void showNewTriangle(const int &shaderProgram, bool &colorSelected, const ImVec4 &newColor);
void showLine(const int &shaderProgram);
void showPoint(const int &shaderProgram);

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

int main() {
	glfwInit();	// ��ʼ��GLFW
	const char * glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// ����GLFW���������汾��Ϊ3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// ���ôΰ汾��Ϊ3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// ʹ�ú���ģʽ core-profile

	// �������ڶ���
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "����GLFW����ʧ��" << std::endl;
		glfwTerminate();
		return -1;
	}
	// ��ʾ����
	glfwMakeContextCurrent(window);
	// ���ûص��������ڴ�С�����󽫵��øûص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GLAD��ʼ��ʧ��" << std::endl;
		return -1;
	}

	// ��ʼ��ImGui����
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

	//-----------------------------------
	
	// ����ѡ������ɫ����
	bool colorSelected = false;
	ImVec4 newColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	while (!glfwWindowShouldClose(window))
	{
		// ��������
		processInput(window);

		// ��Ⱦ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ��ʾ������
		showNewTriangle(shaderProgram, colorSelected, newColor);
		// ��ʾ�ߡ���
		showLine(shaderProgram);
		showPoint(shaderProgram);
		
		// ImGui��ʼ��
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// ����ImGui������ʽ
		ImGui::Begin("Set Color");
		ImGui::Text("Select your color");
		ImGui::ColorEdit3("Triangle color", (float*)&newColor);
		ImGui::End();

		// ImGui��Ⱦ
		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// ���ݴ��ڵĻ����С��ȡ�ߴ�
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		

		// ��������
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
	// ��������Ĭ�϶�������
	float vertices[] = {
		// λ��					��ɫ
		0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f
	};
	float vertices2[] = {
		// λ��					��ɫ
		0.0f,  1.0f, 0.0f,		newColor.x, newColor.y, newColor.z,
		-1.0f, -1.0f, 0.0f,		newColor.x, newColor.y, newColor.z,
		1.0f, -1.0f, 0.0f,		newColor.x, newColor.y, newColor.z
	};

	// �������㻺����� Vertex Buffer Objects, �洢����, ����������GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	if (colorSelected)
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	else {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// ���ƶ������ݵ������ڴ��У����ݲ��ᱻ�ı䣩
		if (newColor.x > 0.0f)
			colorSelected = true;
	}
	// ��������������� Vertex Array Objects, ����
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// �����������ݣ�location = 0�� vec3�� ��������Ϊ�����ͣ� ����Ҫ��׼��(0-1)�� ����Ϊ3����������ƫ����Ϊ0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//-----------------------------------
	glViewport(0, 0, 640, 480);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// ���VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void showLine(const int &shaderProgram) {
	// ��������Ĭ�϶�������
	float vertices[] = {
		// λ��					��ɫ
		1.0f, -1.0f, 0.0f,		1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f,		1.0f, 1.0f, 0.0f
	};

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
	glViewport(300,0,400,400);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 2);

	// ���VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void showPoint(const int &shaderProgram) {
	// ��������Ĭ�϶�������
	float vertices[] = {
		// λ��					��ɫ
		1.0f,  1.0f, 0.0f,		0.0f, 1.0f, 0.0f
	};

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
	glViewport(360, 0, 400, 400);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glPointSize(10.0f); // ������С
	glDrawArrays(GL_POINTS, 0, 1);

	// ���VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
