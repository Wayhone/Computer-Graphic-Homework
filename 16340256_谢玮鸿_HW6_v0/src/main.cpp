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
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);	// �������


// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// vision
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// light
glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
float ambientFactor = 0.1f, diffuseFactor = 1.0f, specularFactor = 0.5f;
int shininessFactor = 32;

// ������ɫ�� - ���� - Phong
const char *vertexShaderSource_Phong = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 fragPos;\n"
"out vec3 normal;\n"
"uniform mat4 model, view, projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	fragPos = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));\n"
"	normal = aNormal;\n"
"}\0";

// Ƭ����ɫ�� - ���� - Phong
const char *fragmentShaderSource_Phong = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 normal;\n"
"in vec3 fragPos;\n"
"uniform vec3 viewPos, lightPos;\n"
"uniform vec3 objectColor, lightColor;\n"
"uniform float ambientFactor, diffuseFactor, specularFactor;\n"
"uniform int shininessFactor;\n"
"void main() {\n"
"	vec3 ambient = ambientFactor * lightColor;	// ��������\n"
"\n"
"	vec3 norm = normalize(normal);\n"
"	vec3 lightDir = normalize(lightPos - fragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diffuseFactor * diff * lightColor;	// ���������\n"
"\n"
"	vec3 viewDir = normalize(viewPos - fragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessFactor);\n"
"	vec3 specular = specularFactor * spec * lightColor;		// ������� \n"
"\n"
"	vec3 result = (ambient + diffuse + specular) * objectColor; \n"
"   FragColor = vec4(result, 1.0f);\n"
"}\n\0";

// ������ɫ�� - ���� - Gouraud
const char *vertexShaderSource_Gouraud = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"out vec3 lightingColor;\n"
"uniform mat4 model, view, projection;\n"
"uniform vec3 viewPos, lightPos, lightColor;\n"
"uniform float ambientFactor, diffuseFactor, specularFactor;\n"
"uniform int shininessFactor;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"	vec3 fragPos = vec3(model * vec4(aPos.x, aPos.y, aPos.z, 1.0));\n"
"	vec3 normal = aNormal;\n"
"	vec3 ambient = ambientFactor * lightColor;\n"
"	vec3 norm = normalize(normal);\n"
"	vec3 lightDir = normalize(lightPos - fragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diffuseFactor * diff * lightColor;\n"
"	vec3 viewDir = normalize(viewPos - fragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininessFactor);\n"
"	vec3 specular = specularFactor * spec * lightColor; \n"
"	lightingColor = ambient + diffuse + specular;\n"
"}\0";

// Ƭ����ɫ�� - ���� - Gouraud
const char *fragmentShaderSource_Gouraud = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 lightingColor;\n"
"uniform vec3 objectColor;\n"
"void main() {\n"
"	FragColor = vec4(lightingColor * objectColor, 1.0f);\n"
"}\0";

// ������ɫ�� - ��Դ
const char *vertexShaderSource_Light = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model, view, projection;\n"
"void main() {\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Ƭ����ɫ�� - ��Դ
const char *fragmentShaderSource_Light = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"	FragColor = vec4(1.0f);\n"
"}\0";

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
	unsigned int vertexShader_Phong = glCreateShader(GL_VERTEX_SHADER),
		vertexShader_Gouraud = glCreateShader(GL_VERTEX_SHADER),
		vertexShader_Light = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader_Phong, 1, &vertexShaderSource_Phong, NULL);
	glCompileShader(vertexShader_Phong);
	glShaderSource(vertexShader_Gouraud, 1, &vertexShaderSource_Gouraud, NULL);
	glCompileShader(vertexShader_Gouraud);
	glShaderSource(vertexShader_Light, 1, &vertexShaderSource_Light, NULL);
	glCompileShader(vertexShader_Light);
	// �����ɫ���������
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader_Phong, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader_Phong, 512, NULL, infoLog);
		std::cout << "���붥����ɫ��1Դ����ִ���\n" << infoLog << std::endl;
	}
	glGetShaderiv(vertexShader_Gouraud, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader_Gouraud, 512, NULL, infoLog);
		std::cout << "���붥����ɫ��1Դ����ִ���\n" << infoLog << std::endl;
	}
	glGetShaderiv(vertexShader_Light, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader_Light, 512, NULL, infoLog);
		std::cout << "���붥����ɫ��2Դ����ִ���\n" << infoLog << std::endl;
	}


	// ����Ƭ����ɫ�����󣬱�����ɫ��Դ��
	unsigned int fragmentShader_Phong = glCreateShader(GL_FRAGMENT_SHADER),
		fragmentShader_Gouraud = glCreateShader(GL_FRAGMENT_SHADER),
		fragmentShader_Light = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader_Phong, 1, &fragmentShaderSource_Phong, NULL);
	glCompileShader(fragmentShader_Phong);
	glShaderSource(fragmentShader_Gouraud, 1, &fragmentShaderSource_Gouraud, NULL);
	glCompileShader(fragmentShader_Gouraud);
	glShaderSource(fragmentShader_Light, 1, &fragmentShaderSource_Light, NULL);
	glCompileShader(fragmentShader_Light);
	// �����ɫ���������
	glGetShaderiv(fragmentShader_Phong, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader_Phong, 512, NULL, infoLog);
		std::cout << "����Ƭ����ɫ��1Դ����ִ���\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader_Gouraud, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader_Gouraud, 512, NULL, infoLog);
		std::cout << "����Ƭ����ɫ��1Դ����ִ���\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader_Light, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader_Light, 512, NULL, infoLog);
		std::cout << "����Ƭ����ɫ��2Դ����ִ���\n" << infoLog << std::endl;
	}

	//-----------------------------------
	// ������ɫ�� - ���� - Phong
	unsigned int shaderProgram_Phong;
	shaderProgram_Phong = glCreateProgram();
	glAttachShader(shaderProgram_Phong, vertexShader_Phong);
	glAttachShader(shaderProgram_Phong, fragmentShader_Phong);
	glLinkProgram(shaderProgram_Phong);
	// ���������ɫ���������
	glGetProgramiv(shaderProgram_Phong, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_Phong, 512, NULL, infoLog);
		std::cout << "������ɫ��������ִ���\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader_Phong);
	glDeleteShader(fragmentShader_Phong);

	// ������ɫ�� - ���� - Gouraud
	unsigned int shaderProgram_Gouraud;
	shaderProgram_Gouraud = glCreateProgram();
	glAttachShader(shaderProgram_Gouraud, vertexShader_Gouraud);
	glAttachShader(shaderProgram_Gouraud, fragmentShader_Gouraud);
	glLinkProgram(shaderProgram_Gouraud);
	// ���������ɫ���������
	glGetProgramiv(shaderProgram_Gouraud, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_Gouraud, 512, NULL, infoLog);
		std::cout << "������ɫ��������ִ���\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader_Gouraud);
	glDeleteShader(fragmentShader_Gouraud);

	// ������ɫ�� - ��Դ
	unsigned int shaderProgram_Light;
	shaderProgram_Light = glCreateProgram();
	glAttachShader(shaderProgram_Light, vertexShader_Light);
	glAttachShader(shaderProgram_Light, fragmentShader_Light);
	glLinkProgram(shaderProgram_Light);
	// ���������ɫ���������
	glGetProgramiv(shaderProgram_Light, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram_Light, 512, NULL, infoLog);
		std::cout << "������ɫ��������ִ���\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader_Light);
	glDeleteShader(fragmentShader_Light);


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	// �������㻺����� Vertex Buffer Objects, �洢����, ����������GL_ARRAY_BUFFER
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// ������VAO, ������������ - ��Դ
	unsigned int LightVAO;
	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ������VAO, ��������+���������� - ����
	unsigned int ObjVAO;
	glGenVertexArrays(1, &ObjVAO);
	glBindVertexArray(ObjVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	bool isPhong = true, spinningCamera = false;

	while (!glfwWindowShouldClose(window))
	{
		// ��������
		processInput(window);
		// glfwSetCursorPosCallback(window, mouse_callback);

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
		// ImGui::PushItemWidth(150);
		ImGui::Checkbox("Spinning Camera", &spinningCamera);
		ImGui::Checkbox("Using Phone Shading", &isPhong);
		ImGui::Text("Unclick for using Gouraud Shading");
		ImGui::SliderFloat("ambient", &ambientFactor, 0, 5);
		ImGui::SliderFloat("diffuse", &diffuseFactor, 0, 5);
		ImGui::SliderFloat("specular", &specularFactor, 0, 5);
		ImGui::SliderInt("shininess", &shininessFactor, 0, 256);
		ImGui::End();

		ImGui::Render();
		int s_width, s_height;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &s_width, &s_height);	// ���ݴ��ڵĻ����С��ȡ�ߴ�
		glViewport(0, 0, s_width, s_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//-----------------------------------

		// �任����
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, -1.0f));

		// �۲����
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// ͶӰ����
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		if (isPhong) {
			glUseProgram(shaderProgram_Phong);

			// ģ�;��󡢹۲����ͶӰ�����������������������ɫ��
			unsigned int modelLoc = glGetUniformLocation(shaderProgram_Phong, "model");
			unsigned int viewLoc = glGetUniformLocation(shaderProgram_Phong, "view");
			unsigned int projLoc = glGetUniformLocation(shaderProgram_Phong, "projection");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// �� �۲�㡢��Դ�㡢������ɫ��������ɫ ������ɫ��
			unsigned int vievPosLoc = glGetUniformLocation(shaderProgram_Phong, "viewPos");
			unsigned int lightPosLoc = glGetUniformLocation(shaderProgram_Phong, "lightPos");
			unsigned int objectColorLoc = glGetUniformLocation(shaderProgram_Phong, "objectColor");
			unsigned int lightColorLoc = glGetUniformLocation(shaderProgram_Phong, "lightColor");
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
			glUniform3fv(vievPosLoc, 1, glm::value_ptr(cameraPos));
			glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.8f, 0.5f, 0.6f)));
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

			// �� ����(Ambient)��������(Diffuse)������(Specular)���պͷ����(Shininess) ������ɫ��
			unsigned int ambientLoc = glGetUniformLocation(shaderProgram_Phong, "ambientFactor");
			unsigned int diffuseLoc = glGetUniformLocation(shaderProgram_Phong, "diffuseFactor");
			unsigned int specularLoc = glGetUniformLocation(shaderProgram_Phong, "specularFactor");
			unsigned int shininessLoc = glGetUniformLocation(shaderProgram_Phong, "shininessFactor");
			glUniform1fv(ambientLoc, 1, &ambientFactor);
			glUniform1fv(diffuseLoc, 1, &diffuseFactor);
			glUniform1fv(specularLoc, 1, &specularFactor);
			glUniform1iv(shininessLoc, 1, &shininessFactor);

			glBindVertexArray(ObjVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else {
			glUseProgram(shaderProgram_Gouraud);

			// ģ�;��󡢹۲����ͶӰ�����������������������ɫ��
			unsigned int modelLoc = glGetUniformLocation(shaderProgram_Gouraud, "model");
			unsigned int viewLoc = glGetUniformLocation(shaderProgram_Gouraud, "view");
			unsigned int projLoc = glGetUniformLocation(shaderProgram_Gouraud, "projection");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

			// �� �۲�㡢��Դ�㡢������ɫ��������ɫ ������ɫ��
			unsigned int vievPosLoc = glGetUniformLocation(shaderProgram_Gouraud, "viewPos");
			unsigned int lightPosLoc = glGetUniformLocation(shaderProgram_Gouraud, "lightPos");
			unsigned int objectColorLoc = glGetUniformLocation(shaderProgram_Gouraud, "objectColor");
			unsigned int lightColorLoc = glGetUniformLocation(shaderProgram_Gouraud, "lightColor");
			glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
			glUniform3fv(vievPosLoc, 1, glm::value_ptr(cameraPos));
			glUniform3fv(objectColorLoc, 1, glm::value_ptr(glm::vec3(0.8f, 0.5f, 0.6f)));
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

			// �� ����(Ambient)��������(Diffuse)������(Specular)���պͷ����(Shininess) ������ɫ��
			unsigned int ambientLoc = glGetUniformLocation(shaderProgram_Gouraud, "ambientFactor");
			unsigned int diffuseLoc = glGetUniformLocation(shaderProgram_Gouraud, "diffuseFactor");
			unsigned int specularLoc = glGetUniformLocation(shaderProgram_Gouraud, "specularFactor");
			unsigned int shininessLoc = glGetUniformLocation(shaderProgram_Gouraud, "shininessFactor");
			glUniform1fv(ambientLoc, 1, &ambientFactor);
			glUniform1fv(diffuseLoc, 1, &diffuseFactor);
			glUniform1fv(specularLoc, 1, &specularFactor);
			glUniform1iv(shininessLoc, 1, &shininessFactor);

			glBindVertexArray(ObjVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ��Դ
		if (spinningCamera) {
			lightPos.x = sin(glfwGetTime()) * 1.0f - 1.0f;
			lightPos.y = cos(glfwGetTime()) * 1.0f;
			lightPos.z = 0.0f;
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		glUseProgram(shaderProgram_Light);
		unsigned int modelLoc2 = glGetUniformLocation(shaderProgram_Light, "model");
		unsigned int viewLoc2 = glGetUniformLocation(shaderProgram_Light, "view");
		unsigned int projLoc2 = glGetUniformLocation(shaderProgram_Light, "projection");
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(LightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);


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
