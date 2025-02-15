#include <stdio.h>
#include <stdlib.h>
#include "sqlite3ext.h"
#include <Python.h>

#ifdef COMPILE_SQLITE_EXTENSIONS_AS_LOADABLE_MODULE
#include "sqlite3ext.h"
SQLITE_EXTENSION_INIT1
#else
#include "sqlite3.h"
#endif
SQLITE_EXTENSION_INIT1

// Global variable to track if the model is trained
static int model_trained = 0;

// Function to train the model with data from the specified table and column
static void train_model(sqlite3_context *context, int argc, sqlite3_value **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid Arguements\n");
        sqlite3_result_null(context);
        return;
    }

    const char *table_name = (const char *)sqlite3_value_text(argv[0]);
    const char *column_name = (const char *)sqlite3_value_text(argv[1]);

    if (!table_name || !column_name) {
        sqlite3_result_null(context);
        return;
    }

    sqlite3 *db = sqlite3_context_db_handle(context);
    // Detect the data type by querying the first non-null value in the column
    sqlite3_stmt *stmt;
    char query[256];
    snprintf(query, sizeof(query), "SELECT %s FROM %s WHERE %s IS NOT NULL LIMIT 1;", column_name, table_name, column_name);

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_result_null(context);
        return;
    }

    int data_type = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        data_type = sqlite3_column_type(stmt, 0);  // SQLITE_INTEGER, SQLITE_FLOAT, or SQLITE_TEXT
    }
    sqlite3_finalize(stmt);

    // Exit if the column has no data
    if (data_type == -1) {
        fprintf(stderr, "No data in column\n");
        sqlite3_result_null(context);
        return;
    }
      // Clear any existing model from model_cache
    const char *delete_query = "DELETE FROM model_cache";
    sqlite3_stmt *delete_stmt;
    sqlite3_prepare_v2(db, delete_query, -1, &delete_stmt, NULL);;
    sqlite3_step(delete_stmt);
    sqlite3_finalize(delete_stmt);


    //sqlite3_stmt *stmt;
    //char query[256];
    snprintf(query, sizeof(query), "SELECT %s FROM %s;", column_name, table_name);
    const char *data_type_str = (data_type == SQLITE_TEXT) ? "categorical" : "numeric";
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_result_null(context);
        return;
    }
    fprintf(stderr, "Query Complete\n");
    if (!Py_IsInitialized()) {
    Py_Initialize();
    }
    // Create a Python list to store the column data
    PyObject *data_list = PyList_New(0);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (data_type == SQLITE_TEXT) {
            const char *value = (const char *)sqlite3_column_text(stmt, 0);
            PyObject *py_value = PyUnicode_FromString(value);
            PyList_Append(data_list, py_value);
            Py_DECREF(py_value);
        } else {  // Numeric data (INTEGER or FLOAT)
            double value = sqlite3_column_double(stmt, 0);
            PyObject *py_value = PyFloat_FromDouble(value);
            PyList_Append(data_list, py_value);
            Py_DECREF(py_value);
        }
    }
    sqlite3_finalize(stmt);
    fprintf(stderr, "Query Returned\n");
    PyObject *sysPath = PySys_GetObject("path"); // adds ml_module.py into the local env for C compiling.
    PyObject *path = PyUnicode_DecodeFSDefault(".");
    PyList_Append(sysPath, path);
    Py_DECREF(path); 
    // Call the Python function to train and cache the model
    PyObject *pName = PyUnicode_DecodeFSDefault("ml_module");
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    fprintf(stderr, "Python Loaded\n");
    if (pModule != NULL) {
        PyObject *pFunc = PyObject_GetAttrString(pModule, "train_and_serialize_model");
        if (data_type == SQLITE_TEXT) {
            pFunc = PyObject_GetAttrString(pModule, "train_categorical_outlier_model");  // Function for categorical
            printf("Model Type Categorical\n");
        } else {
            pFunc = PyObject_GetAttrString(pModule, "train_and_serialize_model");  // Function for numeric
            printf("Model Type numeric\n");
        }
        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pArgs = PyTuple_Pack(1, data_list);
            PyObject *pResult = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            Py_DECREF(data_list);

            if (pResult != NULL) {
                const char *model_data = PyBytes_AsString(pResult);
                
                int model_data_size = PyBytes_Size(pResult);
                const char *model_id = "current_model";// default model, only allows 1 column from a table at a time
                sqlite3_stmt *insert_stmt;
                const char *insert_query = "INSERT OR REPLACE INTO model_cache (model_id, model_data, data_type) VALUES (?, ?, ?)";
                sqlite3_prepare_v2(db, insert_query, -1, &insert_stmt, NULL);
                sqlite3_bind_text(insert_stmt, 1, model_id, -1, SQLITE_STATIC);
                sqlite3_bind_blob(insert_stmt, 2, model_data, model_data_size, SQLITE_TRANSIENT);  // Use SQLITE_TRANSIENT to ensure data is copied correctly
                sqlite3_bind_text(insert_stmt, 3, data_type_str, -1, SQLITE_STATIC);  // Bind the data type
                sqlite3_step(insert_stmt);
                sqlite3_finalize(insert_stmt);
                Py_DECREF(pResult);
                sqlite3_result_text(context, "Model trained and cached successfully", -1, SQLITE_TRANSIENT);
                printf("Model data size: %d bytes\n", model_data_size);  // Check if size matches the serialized model size in Python
            } else {
                PyErr_Print();
                sqlite3_result_null(context);
            }
        } else {
            PyErr_Print();
            sqlite3_result_null(context);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    } else {
        PyErr_Print();
        sqlite3_result_null(context);
    }
}


// Function to evaluate if a value is an outlier using the trained model
static void outlier(sqlite3_context *context, int argc, sqlite3_value **argv) {
    double threshold = 0.9; // Default threshold
    if (argc == 2 && sqlite3_value_type(argv[1]) == SQLITE_FLOAT) {
        threshold = sqlite3_value_double(argv[1]);
    }
    if (!Py_IsInitialized()) {
            Py_Initialize();
    }
    PyObject *py_value = NULL;
    const char *data_type = NULL;
    // Determine the type of the argument and convert accordingly
    switch (sqlite3_value_type(argv[0])) {
        case SQLITE_INTEGER: {
            int int_value = sqlite3_value_int(argv[0]);
            py_value = PyLong_FromLong(int_value);
            data_type = "numeric";
            break;
        }
        case SQLITE_FLOAT: {
            double double_value = sqlite3_value_double(argv[0]);
            py_value = PyFloat_FromDouble(double_value);
            data_type = "numeric";
            break;
        }
        case SQLITE_TEXT: {
            const char *text_value = (const char *)sqlite3_value_text(argv[0]);
            py_value = PyUnicode_FromString(text_value);
            data_type = "categorical";
            break;
        }
        case SQLITE_BLOB:
        case SQLITE_NULL:
        default:
            sqlite3_result_null(context);
            return;
    }
    const char *model_id = "current_model";//Only allowing 1 model for now
    sqlite3 *db = sqlite3_context_db_handle(context);
    sqlite3_stmt *stmt;

    const char *query = "SELECT model_data FROM model_cache WHERE model_id = ? and data_type = ?";
    sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, model_id, -1, SQLITE_STATIC);// changes the model reference
    sqlite3_bind_text(stmt, 2, data_type, -1, SQLITE_STATIC);// changes the model data_type
    PyObject *py_data_type = NULL;
    py_data_type =  PyUnicode_FromString(data_type);
    //fprintf(stderr, "Loading Model in C\n");
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const void *model_data = sqlite3_column_blob(stmt, 0);
        int model_data_size = sqlite3_column_bytes(stmt, 0);
        if (model_data == NULL || model_data_size == 0) {
            fprintf(stderr, "Error: Model data is NULL or empty\n");
            sqlite3_result_null(context);
            return;
        }
        
        PyObject *sysPath = PySys_GetObject("path"); // adds outlier_module.py into the local env for C compiling.
        PyObject *path = PyUnicode_DecodeFSDefault(".");
        PyList_Append(sysPath, path);
        Py_DECREF(path); 
        PyObject *pName = PyUnicode_DecodeFSDefault("ml_module"); // PY module
        PyObject *pModule = PyImport_Import(pName);
        Py_DECREF(pName);

        if (pModule != NULL) {
            PyObject *pFunc = PyObject_GetAttrString(pModule, "is_outlier_with_model_data"); // outlier py function
            if (pFunc && PyCallable_Check(pFunc)) {
                PyObject *py_model_data = PyBytes_FromStringAndSize((const char *)model_data, model_data_size);
                PyObject *pArgs = PyTuple_Pack(3,  py_value, py_model_data, py_data_type );
                PyObject *pResult = PyObject_CallObject(pFunc, pArgs);
                Py_DECREF(py_model_data);
                Py_DECREF(pArgs);

                if (pResult != NULL) {
                    //int is_outlier = PyLong_AsLong(pResult);
                    //sqlite3_result_int(context, is_outlier);
                    //Py_DECREF(pResult);
                    double score = PyFloat_AsDouble(pResult);
                    //fprintf(stderr, "Raw outlier score: %f\n", score);  // Log the raw score
                    //fprintf(stderr, "Threshold: %f\n", threshold);
                    if (argc == 2) {
                        sqlite3_result_int(context, score < threshold); // Return 1 (TRUE) if score exceeds threshold
                    } else {
                        sqlite3_result_double(context, score); // Return raw score
                    };
                    //sqlite3_result_double(context, score);  // Return normalized score to SQLite
                    Py_DECREF(pResult);
                } else {
                    PyErr_Print();
                    sqlite3_result_null(context);
                }
            } else {
                PyErr_Print();
                sqlite3_result_null(context);
            }
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
        } else {
            PyErr_Print();
            sqlite3_result_null(context);
        }
    } else {
        sqlite3_result_text(context, "Model not found", -1, SQLITE_TRANSIENT);
    }
    sqlite3_finalize(stmt);
}

static void sentiment(sqlite3_context *context, int argc, sqlite3_value **argv) {
    // Ensure a single SQLITE_TEXT argument was passed
    if (argc != 1 || sqlite3_value_type(argv[0]) != SQLITE_TEXT) {
        sqlite3_result_null(context);
        return;
    }
    if (!Py_IsInitialized()) {
        Py_Initialize();
    }

    // Get the text value from SQLite
    const char *text = (const char *)sqlite3_value_text(argv[0]);
    PyObject *py_text = PyUnicode_FromString(text);

    // Add current directory to Python path to locate sentiment_module
    PyObject *sysPath = PySys_GetObject("path");
    PyObject *path = PyUnicode_DecodeFSDefault(".");
    PyList_Append(sysPath, path);
    Py_DECREF(path);

    // Import the sentiment_module
    PyObject *module_id = PyUnicode_FromString("ml_module");
    PyObject *module = PyImport_Import(module_id);
    Py_DECREF(module_id);

    if (module != NULL) {
        // Access analyze_sentiment function
        PyObject *func = PyObject_GetAttrString(module, "analyze_sentiment");
        if (func && PyCallable_Check(func)) {
            PyObject *args = PyTuple_Pack(1, py_text);
            PyObject *result = PyObject_CallObject(func, args);
            Py_DECREF(args);
            Py_DECREF(py_text);

            if (result != NULL) {
                // Extract label and score
                PyObject *label = PyTuple_GetItem(result, 0);
                PyObject *score = PyTuple_GetItem(result, 1);
                const char *label_str = PyUnicode_AsUTF8(label);
                double score_val = PyFloat_AsDouble(score);
                if (strcmp(label_str, "NEGATIVE") == 0) {
                    score_val = -score_val;
                }
                sqlite3_result_double(context, score_val); // Return the score as a double

                Py_DECREF(result);
            } else {
                PyErr_Print();
                sqlite3_result_null(context);
            }
            Py_XDECREF(func);
        } else {
            PyErr_Print();
            sqlite3_result_null(context);
        }
        Py_DECREF(module);
    } else {
        PyErr_Print();
        sqlite3_result_null(context);
    }
}
#ifdef _WIN32
__declspec(dllexport)
#endif
// Extension initialization function
int sqlite3_extension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
    SQLITE_EXTENSION_INIT2(pApi);
    sqlite3_auto_extension((void(*)(void))sqlite3_extension_init);
    
    // Register the train_model function
    int rc = sqlite3_create_function(db, "train_model", 2, SQLITE_UTF8, NULL, train_model, NULL, NULL);
    if (rc != SQLITE_OK) return rc;

    // Register the outlier function
    rc = sqlite3_create_function(db, "outlier", 1, SQLITE_UTF8, NULL, outlier, NULL, NULL);
    if (rc != SQLITE_OK) return rc;
    // Register the outlier function with two args
    rc = sqlite3_create_function(db, "outlier", 2, SQLITE_UTF8, NULL, outlier, NULL, NULL);
    if (rc != SQLITE_OK) return rc;

    // Register the sentiment function
    rc = sqlite3_create_function(db, "sentiment_analysis", 1, SQLITE_UTF8, NULL, sentiment, NULL, NULL);
    if (rc != SQLITE_OK) return rc;


    return SQLITE_OK;
}