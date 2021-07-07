#include "Sample1.h"
#include <string.h>

JNIEXPORT jint JNICALL Java_Sample1_intMethod
  (JNIEnv *env, jobject obj, jint num)
{
    return num * num;
}

JNIEXPORT jboolean JNICALL Java_Sample1_booleanMethod
  (JNIEnv *env, jobject obj, jboolean boolean)
{
    return !boolean;
}

JNIEXPORT jstring JNICALL Java_Sample1_stringMethod

  (JNIEnv *env, jobject obj, jstring string)
{
    const char* str = (*env)->GetStringUTFChars(env, string, 0);
    char cap[128];
    strcpy(cap, str);
    (*env)->ReleaseStringUTFChars(env, string, 0);
    string = (*env)->NewStringUTF(env, "out put sting");
    jclass cls_gzz = (*env)->FindClass(env, "Sample1");
    if(cls_gzz == NULL){
        //return -1;
    }
    jmethodID jmethod = (*env)->GetStaticMethodID(env, cls_gzz, "setInfo","(Ljava/lang/String;II)V");
    if(jmethod == NULL){
        //return -1;
    }
    jstring responseBuf = (*env)->NewStringUTF(env, "responseBuf");
    (*env)->CallStaticVoidMethod(env, cls_gzz, jmethod, responseBuf, 20, 11);
    return (*env)->NewStringUTF(env, cap);
}

JNIEXPORT jint JNICALL Java_Sample1_intArrayMethod
  (JNIEnv *env, jobject obj, jintArray array)
{
    int i, sum = 0;
    jsize len = (*env)->GetArrayLength(env, array);
    jint *body = (*env)->GetIntArrayElements(env, array, 0);

    for (i = 0; i < len; ++i)
    {
        sum += body[i];
    }
    (*env)->ReleaseIntArrayElements(env, array, body, 0);
    return sum;
}
