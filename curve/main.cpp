#include <GLFW/glfw3.h>

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        glColor3f(0.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLES);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(5.0f, -1.0f, 2.0f);
        glVertex3f(-2.0f, 3.0f, 3.0f);
        glEnd();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}