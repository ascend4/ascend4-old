#ifndef ASCXX_REPORTER_H
#define ASCXX_REPORTER_H

#include "config.h"

#ifdef ASCXX_USE_PYTHON
# include <Python.h>
#endif

extern "C"{
#include <ascend/utilities/ascConfig.h>
#include <ascend/utilities/error.h>
}


#ifdef ASCXX_USE_PYTHON
extern "C"{
/**
	This function is a hook function that will convey errors 
	back to Python via the C++ 'Reporter' class.
*/
ASC_EXPORT int reporter_error_python(ERROR_REPORTER_CALLBACK_ARGS);
}
#endif

/**
	This class provides C++ abstraction of the error.h error callback
	interface.

	Initially, it's trying to just handling the conveying of error
	messages back to python, but it could be used to pass back
	all sorts of other 'messages' eventually.

	Maybe raising alerts, notifying of the progress of big tasks, etc.

	The client_data pointer allows callback context to be set. This will
	be used to specify which Python function should be used for error
	reporting, in the case of the Python extension to this class.
*/
class Reporter{
private:
	void *client_data;
	Reporter(); // This class will be a singleton
	~Reporter();
	static Reporter *_instance;
#ifdef ASCXX_USE_PYTHON
	bool is_python;
#endif

public:
	static ASC_EXPORT Reporter * Instance();
	void setErrorCallback(error_reporter_callback_t, void *client_data=NULL);

#ifdef ASCXX_USE_PYTHON
	void setPythonErrorCallback(PyObject *pyfunc);
	void clearPythonErrorCallback();
	int reportErrorPython(ERROR_REPORTER_CALLBACK_ARGS);
#endif

};

Reporter *getReporter();

#endif // ASCXX_REPORTER_H