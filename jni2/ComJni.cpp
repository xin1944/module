#include <iostream>
#include <stdio.h>
#include <string>
#include <assert.h>
#include "ComJni.h"
#include "services.h"
#include "serviceglobal.h"

#define SERVICE_PORT 60123

JavaVM *mVm;

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::cout << "JNI_OnLoad startup ...\n";
    mVm = vm;
    JNIEnv *env = NULL;
    jint result;
 
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) == JNI_OK) {
        result = JNI_VERSION_1_6;
    }
    else if (vm->GetEnv((void **)&env, JNI_VERSION_1_4) == JNI_OK) {
        result = JNI_VERSION_1_4;
    }
    else {
        result = JNI_VERSION_1_2;
    }
 
    assert(env != NULL);
    return result;
}

static JNIEnv* getJNIEnv(JavaVM* pJavaVM)
{
    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeCallBack";
    lJavaVMAttachArgs.group = NULL;
    JNIEnv* lEnv = NULL;
    if((pJavaVM)->AttachCurrentThread((void**)&lEnv, &lJavaVMAttachArgs) != JNI_OK){
        lEnv = NULL;
    }
    return lEnv;
}

void* mydispatch(char* requestBuffer, int requestlen, char** responseBuffer, int* responselen)
{
    std::cout<< "dispatch in\n";
    char buf[2048];
    bool needDetach = false;
    strcpy(buf, "<200 resultfilename>");
    std::cout << requestBuffer << std::endl;
    *responseBuffer = (char *)malloc(sizeof(buf));
    strcpy(*responseBuffer, buf);
    *responselen = 2048;
    //////////////////////////////////////////////
    JNIEnv *jniEnv;
    if (mVm->GetEnv((void **) (&jniEnv), JNI_VERSION_1_6) != JNI_OK) {
        //c++ thread; need acquire java environment by the function of getJNIEnv
        std::cout << "c++ thread\n";
        jniEnv = getJNIEnv(mVm);
        needDetach = true;
    }
    if (jniEnv == NULL) {
        std::cout << "getJNIEnv failed\n";
        return(void*)1;
    }
    jclass clazz = jniEnv->FindClass("ComJni");
    if(clazz == NULL){
        if(needDetach){
            mVm->DetachCurrentThread();
        }
        return (void*)1;
    }
    jmethodID jmethod = jniEnv->GetStaticMethodID(clazz, "callback","(Ljava/lang/String;I)V");
    if(jmethod == NULL){
        if(needDetach){
            mVm->DetachCurrentThread();
        }
        return (void*)1;
    }
    jstring responseBuf = jniEnv->NewStringUTF("response_buffer");
    jniEnv->CallStaticVoidMethod(clazz, jmethod, responseBuf, 10);
    jniEnv->DeleteLocalRef(clazz);
    // jniEnv->DeleteLocalRef(jmethod);
    jniEnv->DeleteLocalRef(responseBuf);
    if(needDetach){
        mVm->DetachCurrentThread();
    }
    return (void*)1;
}

static void initService()
{
    ServiceInfo si;
    //si.addr = 102000;
    si.port = SERVICE_PORT;
    //si.serv = SERVICE_PORT;
    //strcpy(si.servname, "relay_test");
    //si.state = 1;
    //si.keep_alive = 1;
    //si.reserved = 0;
    int ret = ServiceServerInit(si, DISPATCH);
    std::cout << "ret:" << ret << "\n";
    if(ret == 0) std::cout << "init success\n";
    else std::cout << "init fail\n";
    ret = ServiceRegisterInit("relay_test","realtime","scada_relay",SERVICE_PORT, si.port, 0);
    //ret = ServiceRegisterInit("relay_test",1,102000,si.serv);
    std::cout << "ret:" << ret << "\n";
    if(ret == 0) std::cout << "reg success\n";
    else std::cout << "reg fail\n";
    ret = ServiceDispatch(si, MULTI_THREAD_PRIVATE, mydispatch);
    std::cout << "ret:" << ret << "\n";
    if(ret == 0) std::cout << "disp success\n";
    else std::cout << "disp fail\n";
}

JNIEXPORT void JNICALL Java_ComJni_callJni
  (JNIEnv *env, jclass clazz, jstring requestBuf, jint len)
{
    printf("%s\n", __FUNCTION__);
    const char* str = env->GetStringUTFChars(requestBuf, 0);
    char cap[128];
    strcpy(cap, str);
    env->ReleaseStringUTFChars(requestBuf, 0);
    printf("requestBuf:%s\n", cap);
    printf("requestLen:%d\n", len);
    initService();
    return;
}

