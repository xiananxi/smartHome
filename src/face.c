#include <Python.h>

#define WGET_CMD "wget http://127.0.0.1:8080/?action=snapshot -O /tmp/SearchFace.jpg"
#define FACE_FILE "/tmp/SearchFace.jpg"

void face_init(void)
{ 
    Py_Initialize();
    PyObject *sys = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyUnicode_FromString("."));
}
void face_final(void)
{
    Py_Finalize();
}
double face_category(void)
{
    double result = 0.0;
    system(WGET_CMD);
    if(0 == access(FACE_FILE,F_OK)){
        return 0.0;
    }
    PyObject *pModule = PyImport_ImportModule("face");
    if (!pModule)
    {
        PyErr_Print();
        printf("Error: failed to load face.py\n");
        goto FAILED_MODULE;
    }
    //解析加载，看alibaba_face这个函数是否存在
    PyObject *pFunc = PyObject_GetAttrString(pModule, "alibaba_face");
    if (!pFunc)
    {
        PyErr_Print();
        printf("Error: failed to load alibaba_face\n");
        goto FAILED_FUNC;
    }
    PyObject *pValue = PyObject_CallObject(pFunc, NULL);
    if (!pValue)
    {
        PyErr_Print();
        printf("Error: function call failed\n");
        goto FAILED_VALUE;
    }
    if (!PyArg_Parse(pValue, "d", &result))
    {//调用alibaba_face函数是否存在，存在的话就调用，解析获取alibaba_face函数的返回值，
    //转换成C语言格式，最后打印出来返回给face_result。
        PyErr_Print();
        printf("Error: parse failed");
        goto FAILED_RESULT;
    }
    printf("result = %0.2lf\n",result);
FAILED_RESULT:
    Py_DECREF(pValue);
FAILED_VALUE:
    Py_DECREF(pFunc);
FAILED_FUNC:
    Py_DECREF(pModule);
FAILED_MODULE:
    return result;
}