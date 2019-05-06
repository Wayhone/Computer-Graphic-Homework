#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// ������ɫ��
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model, view, projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
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


	// �������� �� ��������
	float vertices[] = {
		-2.0f, -2.0f, -2.0f,	0.4f, 1.0f, 0.4f,
		 2.0f, -2.0f, -2.0f,	1.0f, 0.4f, 0.4f,
		 2.0f,  2.0f, -2.0f,	0.4f, 0.4f, 1.0f,
		-2.0f,  2.0f, -2.0f,	0.5f, 0.5f, 0.5f,

		-2.0f, -2.0f,  2.0f,	1.0f, 0.4f, 0.4f,
		 2.0f, -2.0f,  2.0f,	0.4f, 1.0f, 0.4f,
		 2.0f,  2.0f,  2.0f,	0.5f, 0.5f, 0.5f,
		-2.0f,  2.0f,  2.0f,	0.4f, 0.4f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,	// ǰ��-1
		0, 2, 3,	// ǰ��-2
		4, 5, 6,	// ����-1
		4, 6, 7,	// ����-2

		0, 3, 7,	// ����-1
		0, 4, 7,	// ����-2
		1, 2, 6,	// ����-1
		1, 5, 6,	// ����-2

		0, 1, 5,	// ����-1
		0, 4, 5,	// ����-2
		2, 3, 6,	// ����-1
		3, 6, 7		// ����-2
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

	// ����EBO
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// ������������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������ɫ����
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float trans_x = 0.0f, trans_y = 0.0f, scale = 1.0f;
	bool autoRotate = false, autoScale = false, autoTrans = false, revolution = false;
	// �Զ����š�ƽ�Ʋ���
	int scaleArg = 1, transArg = 1;

	while (!glfwWindowShouldClose(window))
	{
		// ��������
		processInput(window);

		// ��Ⱦ����
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);

		//-----------------------------------
		// ImGui��ʼ�� - ���� - ��Ⱦ
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Configuration");
		ImGui::PushItemWidth(150);
		ImGui::Checkbox("AutoScaling    ", &autoScale);
		ImGui::SameLine();
		ImGui::SliderFloat("Scaling", &scale, 0.1f, 2.0f);

		ImGui::PushItemWidth(100);
		ImGui::Checkbox("AutoTranslating", &autoTrans);
		ImGui::SameLine();
		ImGui::SliderFloat("Coord_X", &trans_x, -5.0f, 5.0f);
		ImGui::SameLine();
		ImGui::SliderFloat("Coord - Y", &trans_y, -5.0f, 5.0f);
		ImGui::Checkbox("AutoRotation   ", &autoRotate);
		ImGui::Checkbox("Revolution     ", &revolution);
		ImGui::End();

		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// ���ݴ��ڵĻ����С��ȡ�ߴ�
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//-----------------------------------
		glUseProgram(shaderProgram);

		
		// �۲����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

		// ͶӰ����
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// �任����
		glm::mat4 model = glm::mat4(1.0f);


		// ��ת+��ת
		if (revolution) {
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));	// ��Z����ת
			model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));	// ��������ת
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		}
		else {
			// ƽ��
			if (autoTrans) {
				model = glm::translate(model, glm::vec3(trans_x, trans_y, 0.0f));
				trans_x += transArg * 0.02;
				if (trans_x > 2.0f)
					transArg = -1;
				else if (trans_x < -2.0f)
					transArg = 1;
			}
			else
				model = glm::translate(model, glm::vec3(trans_x, trans_y, 0.0f));
			
			// ��ת
			if (autoRotate)
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));	// ��XoZ����ת

			// ����
			if (autoScale) {
				model = glm::scale(model, glm::vec3(scale, scale, scale));
				scale += scaleArg * 0.01;
				if (scale > 1.2f)
					scaleArg = -1;
				else if (scale < 0.8f)
					scaleArg = 1;
			}
			else
				model = glm::scale(model, glm::vec3(scale, scale, scale));
		}
		
		// ģ�;��󡢹۲����ͶӰ�����������������������ɫ��
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);		// �������� 36


		// glfw: �������棬�����¼�
		// ------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


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
