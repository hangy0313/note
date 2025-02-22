---
title: Java C 程序調用
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,program,java,c
description: 簡單介紹下 Java 如何調用 C 程序。
---

簡單介紹下 Java 和 C 程序的相互調用。

<!-- more -->

## Java 調用 C

Java 可以通過 JNI 調用 C 程序，這裡通過一個 `HelloWorld` 的 Java 程序調用 `helloFromC` 函數，一個保存在 ctest 的共享庫中函數。

{% highlight java %}
/* HelloWorld.java */
public class HelloWorld {
    native void helloFromC();        /* (1) */
    static {
        System.loadLibrary("ctest"); /* (2) */
    }
    static public void main(String argv[]) {
        HelloWorld helloWorld = new HelloWorld();
        helloWorld.helloFromC();     /* (3) */
    }
}
{% endhighlight %}

簡單介紹下上述的步驟：

1. 通知 JVM 存在 `helloFromC()` 函數；
2. 加載 ctest 動態庫，該庫中定義了上述的函數；
3. 直接調用上述的定義的函數。

即是現在還沒有實現動態庫，實際上仍可以編譯 Java 程序，因為默認會在加載時可以找到相應的函數；只是在真正執行函數時會報錯。

{% highlight text %}
$ javac HelloWorld.java
$ java HelloWorld
Exception in thread "main" java.lang.UnsatisfiedLinkError: no ctest in java.library.path
        at java.lang.ClassLoader.loadLibrary(ClassLoader.java:1867)
        at java.lang.Runtime.loadLibrary0(Runtime.java:870)
        at java.lang.System.loadLibrary(System.java:1122)
        at HelloWorld.<clinit>(HelloWorld.java:5)

{% endhighlight %}

接下來，就需要通過 C 創建一個 ctest 庫。首先，通過上面生成的 `.class` 文件創建相應的 C 語言頭文件。

{% highlight text %}
$ javah HelloWorld
{% endhighlight %}

該命令會生成 `HelloWorld.h` 頭文件，包含了如下內容：

{% highlight c %}
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class HelloWorld */

#ifndef _Included_HelloWorld
#define _Included_HelloWorld
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     HelloWorld
 * Method:    helloFromC
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_HelloWorld_helloFromC
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
{% endhighlight %}

直接從上述的文件中複製函數的聲明，並生成如下的文件。

{% highlight c %}
/* ctest.c */
#include <jni.h>
#include <stdio.h>

JNIEXPORT void JNICALL Java_HelloWorld_helloFromC
  (JNIEnv * env, jobject jobj)
{
    printf("Hello from C!\n");
}
{% endhighlight %}

接著就是生成動態庫了，不同的平臺對應 jni.h 文件位置有所區別，例如 CentOS 中可以通過如下命令查看頭文件所在路徑，並編譯。

{% highlight text %}
$ rpm -ql `rpm -qa | grep "java.*-openjdk-devel"` | grep 'jni.h'
$ gcc -o libctest.so -shared -I/path/to/jdk/headers ctest.c -lc
{% endhighlight %}

其中在 Mac 中要將 `.so` 文件替換為 `.dylib` ，在 Windows 中替換為 `.dll` 文件，編譯完成後，就可以通過如下方式運行：

{% highlight text %}
$ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD; java HelloWorld
{% endhighlight %}

## C 調用 Java

書寫 C 文件，模擬 JDK 中自帶的 Java 命令。

{% highlight c %}
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{
    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    jobjectArray applicationArgs;
    jstring appArg;

    /* Setting VM arguments */
    vm_args.version = JNI_VERSION_1_2;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    vm_args.nOptions = 0;

     /* Setting classpath */
    char classpath[1024] = "-Djava.class.path=";
    char *env_classpath = getenv("CLASSPATH");

    int mainclass_index = 1;
    if (argc >= 3 && !strcmp("-classpath", argv[1])) {
        options[0].optionString = strcat(classpath, argv[2]);
        vm_args.nOptions++;
        mainclass_index += 2;
    } else if (env_classpath) {
        options[0].optionString = strcat(classpath, env_classpath);
        vm_args.nOptions++;
    }

    if (vm_args.nOptions > 0) {
        vm_args.options = options;
    }

    if (mainclass_index >= argc) {
        printf("Main class not found, please specify it\n");
        return 0;
    }

    jint res = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
    if (res < 0) {
        printf("Create VM error, code = %d\n", res);
        return -1;
    }

    jclass cls = (*env)->FindClass(env, argv[mainclass_index]);
    if (!cls) {
        printf("Class %s not found\n", argv[mainclass_index]);
        return -1;
    }

    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");

    if (!mid) {
        printf("Method %s of Class %s not found\n", "main", argv[mainclass_index]);
        return -1;
    }
    applicationArgs = (*env)->NewObjectArray(env, argc - mainclass_index - 1,
                              (*env)->FindClass(env, "java/lang/String"),
                              NULL);

    int i = 0;
    for (i = mainclass_index + 1; i < argc; i ++) {
        appArg = (*env)->NewStringUTF(env, argv[i]);
        (*env)->SetObjectArrayElement(env, applicationArgs, i - mainclass_index - 1, appArg);
    }

    (*env)->CallStaticVoidMethod(env, cls, mid, applicationArgs);

    printf("before destroy\n");

    /*
     * Destroy the JVM.
     * This is necessary, otherwise if the called method exits,
     * this program will return immediately.
     */
    (*jvm)->DestroyJavaVM(jvm);

    printf("after destroy\n");

    return 0;
 }
{% endhighlight %}

接著需要編寫 Makefile 文件，注意要鏈接 JDK 中所自帶的 `libjvm.so` 庫文件，在 CentOS 中可以通過 `java-N.N.N-openjdk-headless` 包安裝。

設置環境變量 `LD_LIBRARY_PATH`，也即 `libjvm.so` 所在的路徑。

{% highlight text %}
export LD_LIBRARY_PATH=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.131-3.b12.el7_3.x86_64/jre/lib/amd64/server/
{% endhighlight %}

然後運行命令 `./jvm -classpath . HelloWorld` 。


## 參考

詳細代碼可以查看 [Java 調用 C]( {{ site.example_repository }}/java/prgramc/ ) 以及 [C 調用 Java]( {{ site.example_repository }}/java/cprogram/ ) 。

<!--
Calling C code from Java using JNI
http://stuf.ro/calling-c-code-from-java-using-jni
-->


{% highlight text %}
{% endhighlight %}
