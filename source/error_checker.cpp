#include "main.h"
using namespace std;

void GLClearError()
{
   bool is_error = true;
   while (is_error)
   {
      GLenum error_obj = glGetError();
      if (error_obj == GL_NO_ERROR)
      {
         is_error = false;
         break;
      }
     // std::cout << "[OpenGL ERROR]: -> " << error_obj << std::endl;
   };
}

bool GLCheckError(const char *function, const char *file, int line)
{

   while (GLenum error_obj = glGetError())
   {
      std::cout << "Error : " << function << " in file " << file << " at line " << line << std::endl;
      std::cout << "[OpenGL ERROR]: -> " << error_obj << std::endl;
      part;
      return false;
   }
   return true;
}